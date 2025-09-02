/*!
 ******************************************************************************
 * \file    stse_device_authentication.c
 * \brief   STSE uthentication API (sources)
 * \author  STMicroelectronics - CS application team
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

#include <api/stse_device_authentication.h>
#include <api/stse_ecc.h>

#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_crypto.h"
#include "certificate/stse_certificate_subparsing.h"

#define STSE_CERTIFICATE_SIZE_OFFSET_BYTES 2U
#define STSE_CERTIFICATE_SIZE_LENGTH 2U
#define STSE_CERTIFICATE_OFFSET_BYTES 0U
#define STSE_CERTIFICATE_DEVICE_ID_OFFSET 15U
#define STSE_CERTIFICATE_DEVICE_ID_SIZE 11U

/* Static functions declaration ----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_get_device_id(stse_Handler_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI8 *pDevice_id) {
    volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    ret = stse_data_storage_read_data_zone(
        pSTSE,                             /* STSE handler */
        certificate_zone,                  /* Certificate zone */
        STSE_CERTIFICATE_DEVICE_ID_OFFSET, /* X bytes offset */
        pDevice_id,                        /* Returned certificate size */
        STSE_CERTIFICATE_DEVICE_ID_SIZE,   /* Certificate size length */
        0,                                 /* No maximum chunk size (No chunk at all) */
        STSE_NO_PROT);                     /* No protection */

    return ret;
}

stse_ReturnCode_t stse_get_device_certificate_size(stse_Handler_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI16 *pCertificate_size) {
    volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
    PLAT_UI8 certificate_size_ui8[2];

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (pCertificate_size == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    ret = stse_data_storage_read_data_zone(
        pSTSE,                              /* STSE handler */
        certificate_zone,                   /* Certificate zone */
        STSE_CERTIFICATE_SIZE_OFFSET_BYTES, /* 2 bytes offset */
        certificate_size_ui8,               /* Returned certificate size */
        STSE_CERTIFICATE_SIZE_LENGTH,       /* Certificate size length 2 bytes */
        0,                                  /* No maximum chunk size (No chunk at all) */
        STSE_NO_PROT);                      /* No protection */

    if (ret != STSE_OK) {
        return ret;
    }

    *pCertificate_size = (((uint16_t)certificate_size_ui8[0]) << 8) + ((uint16_t)certificate_size_ui8[1]) + 4U;

    return STSE_OK;
}

stse_ReturnCode_t stse_get_device_certificate(stse_Handler_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI16 certificate_size, PLAT_UI8 *pCertificate) {
    volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (pCertificate == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    ret = stse_data_storage_read_data_zone(
        pSTSE,                         /* STSE handler */
        certificate_zone,              /* Certificate zone */
        STSE_CERTIFICATE_OFFSET_BYTES, /* 0 bytes offset */
        pCertificate,                  /* Returned certificate size */
        certificate_size,              /* Certificate size length */
        253,                           /* Above, it throw a STSE_CORE_FRAME_CRC_ERROR */
        STSE_NO_PROT);                 /* No protection */

    if (ret != STSE_OK) {
        return ret;
    }

    return (STSE_OK);
}

stse_ReturnCode_t stse_device_authenticate(
    stse_Handler_t *pSTSE,
    const PLAT_UI8 *pRoot_CA_certificate,
    PLAT_UI8 certificate_zone,
    PLAT_UI8 priv_key_slot_number) {
    stse_ReturnCode_t ret;
    stse_certificate_t leaf_certificate;
    PLAT_UI16 certificate_size;
    PLAT_UI8 certificate_size_ui8[2];

    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (pRoot_CA_certificate == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    ret = stse_data_storage_read_data_zone(
        pSTSE,
        certificate_zone,
        STSE_CERTIFICATE_SIZE_OFFSET_BYTES, /* 2 bytes offset */
        certificate_size_ui8,               /* Returned certificate size */
        STSE_CERTIFICATE_SIZE_LENGTH,       /* Certificate size length 2 bytes */
        0,                                  /* No maximum chunk size (No chunk at all) */
        STSE_NO_PROT);                      /* No protection */

    if (ret != STSE_OK) {
        return ret;
    }

    certificate_size = (((uint16_t)certificate_size_ui8[0]) << 8) + ((uint16_t)certificate_size_ui8[1]) + 4U;

    PLAT_UI8 certificate[certificate_size];

    ret = stse_get_device_certificate(pSTSE, certificate_zone, certificate_size, certificate);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Parse certificate chain (including Root CA certificate) */
    ret = stse_certificate_parse_chain(
        (uint8_t *)pRoot_CA_certificate,
        certificate,
        certificate_size,
        &leaf_certificate);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Get ECC key type from leaf certificate */
    stse_ecc_key_type_t key_type = stse_certificate_get_key_type(&leaf_certificate);
    if (key_type == STSE_ECC_KT_INVALID) {
        return (STSE_UNEXPECTED_ERROR);
    }

    PLAT_UI16 signature_size = stse_ecc_info_table[key_type].signature_size;
    PLAT_UI8 signature[signature_size];

    PLAT_UI16 challenge_size;
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    if (pSTSE->device_type == STSAFE_L010) {
        challenge_size = STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH;
    } else {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
        challenge_size = stse_ecc_info_table[key_type].private_key_size;
#ifdef STSE_CONF_ECC_NIST_P_521
        if (key_type == STSE_ECC_KT_NIST_P_521) {
            challenge_size -= 2;
        }
#endif /* STSE_CONF_ECC_NIST_P_521 */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    PLAT_UI8 challenge[challenge_size];

    /* - Generate random challenge */
    for (int i = 0; i < challenge_size; i++) {
        challenge[i] = (PLAT_UI8)(stse_platform_generate_random() & 0xFF);
    }

    /* - Get target SE challenge signature */
    ret = stse_ecc_generate_signature(
        pSTSE,                /* STSE handler */
        priv_key_slot_number, /* Slot number */
        key_type,             /* Key type */
        challenge,            /* Random number to sign */
        challenge_size,       /* random number size in bytes */
        signature);           /* returned signature */
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Verify SE Challenge over leaf certificate public key*/
    ret = stse_certificate_verify_signature(
        &leaf_certificate,
        challenge,
        challenge_size,
        signature, (signature_size >> 1),
        &signature[signature_size >> 1], (signature_size >> 1));
    if (ret != STSE_OK) {
        return ret;
    }

    return STSE_OK;
}
