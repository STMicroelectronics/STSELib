/*!
 ******************************************************************************
 * \file	stse_asymmetric_keys_management.c
 * \brief   STSE Asymmetric keys management API (sources)
 * \author  STMicroelectronics - SMD application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "api/stse_asymmetric_keys_management.h"

stse_return_code_t stse_get_ecc_key_slots_count(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_private_key_slot_count) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    stse_return_code_t ret;

    if (p_stse == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (p_private_key_slot_count == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    ret = stsafea_query_private_key_slots_count(p_stse, p_private_key_slot_count);

    return ret;
#else
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_get_ecc_key_slot_info(
    stse_handle_t *p_stse,
    PLAT_UI8 private_key_slot_number,
    PLAT_UI16 *p_global_usage_limit,
    stsafea_private_key_slot_information_t *private_key_slot_info) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    stse_return_code_t ret;

    if (p_stse == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (private_key_slot_info == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    PLAT_UI8 private_key_slot_count;
    PLAT_UI8 slot_info_index;

    ret = stsafea_query_private_key_slots_count(p_stse, &private_key_slot_count);

    stsafea_private_key_slot_information_t private_key_table_info[private_key_slot_count];

    ret = stsafea_query_private_key_table(p_stse, private_key_slot_count, p_global_usage_limit, private_key_table_info);

    if (ret != STSE_OK) {
        return (ret);
    }

    for (slot_info_index = 0; slot_info_index < private_key_slot_count; slot_info_index++) {
        if (private_key_table_info[slot_info_index].slot_number == private_key_slot_number) {
            memcpy(private_key_slot_info, &private_key_table_info[slot_info_index], sizeof(stsafea_private_key_slot_information_t));
            break;
        }
    }

    if (slot_info_index == private_key_slot_count) {
        return (STSE_API_KEY_NOT_FOUND);
    }

    return ret;
#else
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_sign_for_generic_public_key_slot(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t private_key_type,
    PLAT_UI8 *p_private_key,
    stse_hash_algorithm_t hash_algo,
    PLAT_UI16 payload_length,
    PLAT_UI8 *p_payload,
    PLAT_UI8 *p_signature) {
    (void)p_stse;
#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

    stse_return_code_t ret;
    PLAT_UI16 hash_length = stsafea_hash_info_table[hash_algo].length;
    PLAT_UI8 hash_data[hash_length];

    if (p_private_key == NULL || p_payload == NULL || p_signature == NULL ||
        private_key_type >= STSE_ECC_KT_INVALID || hash_algo >= STSE_SHA_INVALID) {
        return STSE_API_INVALID_PARAMETER;
    }

#ifdef STSE_CONF_ECC_EDWARD_25519
    if (private_key_type != STSE_ECC_KT_ED25519) {
#endif /* STSE_CONF_ECC_EDWARD_25519 */
        /* - Hash the payload */
        ret = stse_platform_hash_compute(
            hash_algo,
            p_payload, payload_length,
            hash_data, &hash_length);

        if (ret != STSE_OK) {
            return (ret);
        }
#ifdef STSE_CONF_ECC_EDWARD_25519
    }
#endif /* STSE_CONF_ECC_EDWARD_25519 */

    /* - sign the hash of concatenation of pub keys */
    ret = stse_platform_ecc_sign(
        private_key_type,
        p_private_key,
#ifdef STSE_CONF_ECC_EDWARD_25519
        (private_key_type == STSE_ECC_KT_ED25519) ? p_payload : hash_data,
        (private_key_type == STSE_ECC_KT_ED25519) ? payload_length : hash_length,
#else
        hash_data,
        hash_length,
#endif
        p_signature);

    return (ret);
#else
    return STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED ||
          STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED ||
          STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED */
}
