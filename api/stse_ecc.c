/*!
 ******************************************************************************
 * \file    stse_ecc.c
 * \brief   STSE ECC API set (sources)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

#include "api/stse_ecc.h"

stse_ReturnCode_t stse_ecc_verify_signature(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pSignature,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 eddsa_variant,
    PLAT_UI8 *pSignature_validity) {
    stse_ReturnCode_t ret;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    /* - Check device type */
    if (pSTSE->device_type == STSAFE_L010) {
        return (STSE_API_INCOMPATIBLE_DEVICE_TYPE);
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

    if (pPublic_key == NULL || pSignature == NULL || pMessage == NULL || pSignature_validity == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    ret = stsafea_ecc_verify_signature(pSTSE, key_type, pPublic_key, pSignature, pMessage, message_length, eddsa_variant, pSignature_validity);

    return ret;
}

stse_ReturnCode_t stse_ecc_generate_signature(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 *pSignature) {
    stse_ReturnCode_t ret;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (pMessage == NULL || pSignature == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    switch (pSTSE->device_type) {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
        ret = stsafel_ecc_generate_signature(pSTSE, key_type, pMessage, message_length, pSignature);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
    case STSAFE_A200:
        ret = stsafea_ecc_generate_signature(pSTSE, slot_number, key_type, pMessage, message_length, pSignature);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }

    return ret;
}

stse_ReturnCode_t stse_ecc_establish_shared_secret(
    stse_Handler_t *pSTSE,
    PLAT_UI8 private_key_slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pShared_secret) {
    stse_ReturnCode_t ret;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    /* - Check device type */
    if (pSTSE->device_type == STSAFE_L010) {
        return (STSE_API_INCOMPATIBLE_DEVICE_TYPE);
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

    if (pPublic_key == NULL || pShared_secret == NULL
#ifdef STSE_CONF_ECC_CURVE_25519
        || key_type == STSE_ECC_KT_CURVE25519
#endif /* STSE_CONF_ECC_CURVE_25519 */
    ) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    ret = stsafea_ecc_establish_shared_secret(pSTSE, private_key_slot_number, key_type, pPublic_key, pShared_secret);

    return ret;
}

stse_ReturnCode_t stse_ecc_decompress_public_key(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 point_representation_id,
    PLAT_UI8 *pPublic_key_X,
    PLAT_UI8 *pPublic_key_Y) {
    stse_ReturnCode_t ret;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    /* - Check device type */
    if (pSTSE->device_type == STSAFE_L010) {
        return (STSE_API_INCOMPATIBLE_DEVICE_TYPE);
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

    if (pPublic_key_X == NULL || pPublic_key_Y == NULL
#ifdef STSE_CONF_ECC_CURVE_25519
        || key_type == STSE_ECC_KT_CURVE25519
#endif /* STSE_CONF_ECC_CURVE_25519 */
#ifdef STSE_CONF_ECC_EDWARD_25519
        || key_type == STSE_ECC_KT_ED25519
#endif /* STSE_CONF_ECC_EDWARD_25519 */
    ) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    ret = stsafea_ecc_decompress_public_key(pSTSE, key_type, point_representation_id, pPublic_key_X, pPublic_key_Y);

    return ret;
}
