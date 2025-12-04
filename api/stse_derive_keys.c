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

#include "api/stse_derive_keys.h"

stse_ReturnCode_t stse_derive_key(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (pSTSE == NULL || pOutput_key == NULL || key_length == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = pSalt;

    /* Setup context/info */
    info.length = context_len;
    info.data = pContext;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = pOutput_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Extract+Expand */
    return stsafea_derive_keys(
        pSTSE,
        &input_key,
        1, 1,           /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);
}

stse_ReturnCode_t stse_derive_key_simple(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length)
{
    /* Directly call the main function with NULL salt */
    return stse_derive_key(
        pSTSE,
        master_slot,
        NULL,           /* No salt */
        0,              /* Salt length 0 */
        pContext,
        context_len,
        pOutput_key,
        key_length);
}

stse_ReturnCode_t stse_derive_key_extract(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pPrk_slot)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_output_t output = {0};
    stse_ReturnCode_t ret;

    /* Validate parameters */
    if (pSTSE == NULL || pPrk_slot == NULL) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = pSalt;

    /* Perform HKDF Extract only */
    ret = stsafea_derive_keys(
        pSTSE,
        &input_key,
        1, 0,           /* Extract=1, Expand=0 */
        &salt,
        NULL,
        NULL,
        0,
        &output);

    if (ret == STSE_OK) {
        *pPrk_slot = output.prk_slot;
    }

    return ret;
}

stse_ReturnCode_t stse_derive_key_expand(
    stse_Handler_t *pSTSE,
    PLAT_UI8 prk_slot,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (pSTSE == NULL || pOutput_key == NULL || key_length == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key (PRK from slot) */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = prk_slot;

    /* Setup context/info */
    info.length = context_len;
    info.data = pContext;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = pOutput_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Expand only */
    return stsafea_derive_keys(
        pSTSE,
        &input_key,
        0, 1,           /* Extract=0, Expand=1 */
        NULL,
        &info,
        &okm_map,
        1,
        &output);
}

stse_ReturnCode_t stse_derive_session_keys(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI32 session_id,
    PLAT_UI8 *pEnc_key,
    PLAT_UI16 enc_key_len,
    PLAT_UI8 *pMac_key,
    PLAT_UI16 mac_key_len)
{
    stse_ReturnCode_t ret;
    PLAT_UI8 prk_slot;
    PLAT_UI8 salt[4];
    
    /* Validate parameters */
    if (pSTSE == NULL || pEnc_key == NULL || pMac_key == NULL) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Convert session ID to salt (big-endian) */
    salt[0] = (PLAT_UI8)(session_id >> 24);
    salt[1] = (PLAT_UI8)(session_id >> 16);
    salt[2] = (PLAT_UI8)(session_id >> 8);
    salt[3] = (PLAT_UI8)(session_id);

    /* Step 1: Extract PRK from master key with session ID as salt */
    ret = stse_derive_key_extract(pSTSE, master_slot, salt, 4, &prk_slot);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Step 2: Derive encryption key with context "ENC" */
    ret = stse_derive_key_expand(pSTSE, prk_slot, (PLAT_UI8*)"ENC", 3, 
                              pEnc_key, enc_key_len);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Step 3: Derive MAC key with context "MAC" */
    ret = stse_derive_key_expand(pSTSE, prk_slot, (PLAT_UI8*)"MAC", 3, 
                              pMac_key, mac_key_len);
    
    return ret;
}

stse_ReturnCode_t stse_derive_key_to_slot(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    stsafe_output_key_description_information_t *pKey_info,
    PLAT_UI8 *pOutput_slot)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};
    stse_ReturnCode_t ret;

    /* Validate parameters */
    if (pSTSE == NULL || pKey_info == NULL || pOutput_slot == NULL) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key from slot */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = master_slot;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = pSalt;

    /* Setup context/info */
    info.length = context_len;
    info.data = pContext;

    /* Setup output to slot */
    okm_map.destination = STSAFEA_KEY_SOURCE_SYMMKEY;
    okm_map.symmkey.key_info = pKey_info;

    /* Setup output structure */
    output.derived_keys = &derived_key_out;

    /* Perform HKDF to slot */
    ret = stsafea_derive_keys(
        pSTSE,
        &input_key,
        1, 1,           /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);

    if (ret == STSE_OK) {
        *pOutput_slot = output.derived_keys[0].symmkey.slot_number;
    }

    return ret;
}

stse_ReturnCode_t stse_derive_key_expand_multiple(
    stse_Handler_t *pSTSE,
    PLAT_UI8 prk_slot,
    PLAT_UI8 **pContexts,
    PLAT_UI16 *pContext_lens,
    PLAT_UI8 **pOutput_keys,
    PLAT_UI16 *pKey_lengths,
    PLAT_UI8 num_keys)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map[32]; 
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_keys_out[32];
    PLAT_UI8 i;

    /* Validate parameters */
    if (pSTSE == NULL || pOutput_keys == NULL || pKey_lengths == NULL || 
        num_keys == 0 || num_keys > 32) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key (PRK from slot) */
    input_key.source = STSAFEA_KEY_SOURCE_SYMMKEY;
    input_key.symmkey.slot_number = prk_slot;

    /* Note: HKDF typically uses same info for all keys in one expand operation
     * For different contexts per key, you'd call expand multiple times
     * This function uses the first context for all keys */
    info.length = (pContexts != NULL && pContext_lens != NULL) ? pContext_lens[0] : 0;
    info.data = (pContexts != NULL) ? pContexts[0] : NULL;

    /* Setup OKM maps and output buffers */
    memset(derived_keys_out, 0, sizeof(derived_keys_out));
    memset(okm_map, 0, sizeof(okm_map));

    for (i = 0; i < num_keys; i++) {
        okm_map[i].destination = STSAFEA_KEY_SOURCE_RESPONSE;
        okm_map[i].response.key_length = pKey_lengths[i];
        
        derived_keys_out[i].response.data = pOutput_keys[i];
    }

    /* Setup output structure */
    output.derived_keys = derived_keys_out;

    /* Perform HKDF Expand for multiple keys */
    return stsafea_derive_keys(
        pSTSE,
        &input_key,
        0, 1,           /* Extract=0, Expand=1 */
        NULL,
        &info,
        okm_map,
        num_keys,
        &output);
}

stse_ReturnCode_t stse_derive_key_from_ikm(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pIkm,
    PLAT_UI16 ikm_length,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length)
{
    stsafea_hkdf_input_key_t input_key = {0};
    stsafea_hkdf_salt_t salt = {0};
    stsafea_hkdf_info_t info = {0};
    stsafea_hkdf_okm_description_t okm_map = {0};
    stsafea_hkdf_output_t output = {0};
    stsafea_hkdf_derived_key_output_t derived_key_out = {0};

    /* Validate parameters */
    if (pSTSE == NULL || pIkm == NULL || pOutput_key == NULL || 
        ikm_length == 0 || key_length == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* Setup input key from command */
    input_key.source = STSAFEA_KEY_SOURCE_COMMAND;
    input_key.command.mode_of_operation = STSAFEA_KEY_OPERATION_MODE_HKDF;
    input_key.command.length = ikm_length;
    input_key.command.data = pIkm;

    /* Setup salt */
    salt.source = STSAFEA_KEY_SOURCE_COMMAND;
    salt.command.length = salt_length;
    salt.command.data = pSalt;

    /* Setup context/info */
    info.length = context_len;
    info.data = pContext;

    /* Setup output to response */
    okm_map.destination = STSAFEA_KEY_SOURCE_RESPONSE;
    okm_map.response.key_length = key_length;

    /* Pre-allocate output buffer */
    derived_key_out.response.data = pOutput_key;
    output.derived_keys = &derived_key_out;

    /* Perform HKDF Extract+Expand */
    return stsafea_derive_keys(
        pSTSE,
        &input_key,
        1, 1,           /* Extract=1, Expand=1 */
        &salt,
        &info,
        &okm_map,
        1,
        &output);
}