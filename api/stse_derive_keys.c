/*!
 ******************************************************************************
 * \file    stse_derive_keys.c
 * \brief   STSE Derive Keys API Layer (source)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2025 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "api/stse_derive_keys.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stse_derive_key(
    stse_handle_t *p_stse,
    PLAT_UI8 master_slot,
    PLAT_UI8 *p_salt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *p_context,
    PLAT_UI16 context_len,
    PLAT_UI8 *p_output_key,
    PLAT_UI16 key_length) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (p_stse == NULL || p_output_key == NULL || key_length == 0) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = p_salt;

    /* Setup context/info */
    info.length = context_len;
    info.data = p_context;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = p_output_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Extract+Expand */
    return stsafea_derive_keys(
        p_stse,
        &input_key,
        1, 1, /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);
}

stse_return_code_t stse_derive_key_extract(
    stse_handle_t *p_stse,
    PLAT_UI8 master_slot,
    PLAT_UI8 *p_salt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *p_prk_slot) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_output_t output = {0};
    stse_return_code_t ret;

    /* Validate parameters */
    if (p_stse == NULL || p_prk_slot == NULL) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = p_salt;

    /* Perform HKDF Extract only */
    ret = stsafea_derive_keys(
        p_stse,
        &input_key,
        1, 0, /* Extract=1, Expand=0 */
        &salt,
        NULL,
        NULL,
        0,
        &output);

    if (ret == STSE_OK) {
        *p_prk_slot = output.prk_slot;
    }

    return ret;
}

stse_return_code_t stse_derive_key_expand(
    stse_handle_t *p_stse,
    PLAT_UI8 prk_slot,
    PLAT_UI8 *p_context,
    PLAT_UI16 context_len,
    PLAT_UI8 *p_output_key,
    PLAT_UI16 key_length) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (p_stse == NULL || p_output_key == NULL || key_length == 0) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key (PRK from slot) */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = prk_slot;

    /* Setup context/info */
    info.length = context_len;
    info.data = p_context;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = p_output_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Expand only */
    return stsafea_derive_keys(
        p_stse,
        &input_key,
        0, 1, /* Extract=0, Expand=1 */
        NULL,
        &info,
        &okm_map,
        1,
        &output);
}

stse_return_code_t stse_derive_session_keys(
    stse_handle_t *p_stse,
    PLAT_UI8 master_slot,
    PLAT_UI32 session_id,
    PLAT_UI8 *p_enc_key,
    PLAT_UI16 enc_key_len,
    PLAT_UI8 *p_mac_key,
    PLAT_UI16 mac_key_len) {
    stse_return_code_t ret;
    PLAT_UI8 prk_slot;
    PLAT_UI8 salt[4];

    /* Validate parameters */
    if (p_stse == NULL || p_enc_key == NULL || p_mac_key == NULL) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Convert session ID to salt (big-endian) */
    salt[0] = (PLAT_UI8)(session_id >> 24);
    salt[1] = (PLAT_UI8)(session_id >> 16);
    salt[2] = (PLAT_UI8)(session_id >> 8);
    salt[3] = (PLAT_UI8)(session_id);

    /* Step 1: Extract PRK from master key with session ID as salt */
    ret = stse_derive_key_extract(p_stse, master_slot, salt, 4, &prk_slot);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Step 2: Derive encryption key with context "ENC" */
    ret = stse_derive_key_expand(p_stse, prk_slot, (PLAT_UI8 *)"ENC", 3,
                                 p_enc_key, enc_key_len);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Step 3: Derive MAC key with context "MAC" */
    ret = stse_derive_key_expand(p_stse, prk_slot, (PLAT_UI8 *)"MAC", 3,
                                 p_mac_key, mac_key_len);

    return ret;
}

stse_return_code_t stse_derive_key_to_slot(
    stse_handle_t *p_stse,
    PLAT_UI8 master_slot,
    PLAT_UI8 *p_salt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *p_context,
    PLAT_UI16 context_len,
    stsafe_output_key_description_information_t *p_key_info,
    PLAT_UI8 *p_output_slot) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};
    stse_return_code_t ret;

    /* Validate parameters */
    if (p_stse == NULL || p_key_info == NULL || p_output_slot == NULL) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = p_salt;

    /* Setup context/info */
    info.length = context_len;
    info.data = p_context;

    /* Setup output to slot */
    okm_map.destination = STSAFEA_KEY_SOURCE_SYMMKEY;
    okm_map.symmkey.key_info = p_key_info;

    /* Setup output structure */
    output.derived_keys = &derived_key_out;

    /* Perform HKDF to slot */
    ret = stsafea_derive_keys(
        p_stse,
        &input_key,
        1, 1, /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);

    if (ret == STSE_OK) {
        *p_output_slot = output.derived_keys[0].symmkey.slot_number;
    }

    return ret;
}

stse_return_code_t stse_derive_key_expand_multiple(
    stse_handle_t *p_stse,
    PLAT_UI8 prk_slot,
    PLAT_UI8 **p_contexts,
    PLAT_UI16 *p_context_lens,
    PLAT_UI8 **p_output_keys,
    PLAT_UI16 *p_key_lengths,
    PLAT_UI8 num_keys) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map[32];
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_keys_out[32];
    PLAT_UI8 i;

    /* Validate parameters */
    if (p_stse == NULL || p_output_keys == NULL || p_key_lengths == NULL ||
        num_keys == 0 || num_keys > 32) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key (PRK from slot) */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = prk_slot;

    /* Note: HKDF typically uses same info for all keys in one expand operation
     * For different contexts per key, you'd call expand multiple times
     * This function uses the first context for all keys */
    info.length = (p_contexts != NULL && p_context_lens != NULL) ? p_context_lens[0] : 0;
    info.data = (p_contexts != NULL) ? p_contexts[0] : NULL;

    /* Setup OKM maps and output buffers */
    memset(derived_keys_out, 0, sizeof(derived_keys_out));
    memset(okm_map, 0, sizeof(okm_map));

    for (i = 0; i < num_keys; i++) {
        okm_map[i].destination = STSAFEA_KEY_SOURCE_RESPONSE;
        okm_map[i].response.key_length = p_key_lengths[i];

        derived_keys_out[i].response.data = p_output_keys[i];
    }

    /* Setup output structure */
    output.derived_keys = derived_keys_out;

    /* Perform HKDF Expand for multiple keys */
    return stsafea_derive_keys(
        p_stse,
        &input_key,
        0, 1, /* Extract=0, Expand=1 */
        NULL,
        &info,
        okm_map,
        num_keys,
        &output);
}

stse_return_code_t stse_derive_key_from_ikm(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_ikm,
    PLAT_UI16 ikm_length,
    PLAT_UI8 *p_salt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *p_context,
    PLAT_UI16 context_len,
    PLAT_UI8 *p_output_key,
    PLAT_UI16 key_length) {
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (p_stse == NULL || p_ikm == NULL || p_output_key == NULL ||
        ikm_length == 0 || key_length == 0) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* Setup input key from command */
    input_key.source = STSAFEA_KEY_SOURCE_COMMAND;
    input_key.command.mode_of_operation = STSAFEA_KEY_OPERATION_MODE_HKDF;
    input_key.command.length = ikm_length;
    input_key.command.data = p_ikm;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = p_salt;

    /* Setup context/info */
    info.length = context_len;
    info.data = p_context;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = p_output_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Extract+Expand */
    return stsafea_derive_keys(
        p_stse,
        &input_key,
        1, 1, /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
