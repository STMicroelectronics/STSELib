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
#include <stddef.h>
#include <stdint.h>

#include "api/stse_device_authentication.h"
#include "api/stse_ecc.h"

#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_crypto.h"
#include "certificate/stse_certificate_subparsing.h"
#include "certificate/stse_certificate_types.h"

#define STSE_CERTIFICATE_SIZE_OFFSET_BYTES 2U
#define STSE_CERTIFICATE_SIZE_LENGTH 2U
#define STSE_CERTIFICATE_OFFSET_BYTES 0U
#define STSE_CERTIFICATE_DEVICE_ID_OFFSET 15U
#define STSE_CERTIFICATE_DEVICE_ID_SIZE 11U

/* Static functions declaration ----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_get_device_id(stse_Handle_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI8 *pDevice_id) {
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

stse_ReturnCode_t stse_get_device_certificate_size(stse_Handle_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI16 *pCertificate_size) {
    volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
    PLAT_UI8 tlv_header[4];

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (pCertificate_size == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    ret = stse_data_storage_read_data_zone(
        pSTSE,              /* STSE handler */
        certificate_zone,   /* Certificate zone */
        0,                  /* No offset */
        tlv_header,         /* Returned TLV header (tag + length bytes) */
        sizeof(tlv_header), /* TLV header length 4 bytes */
        0,                  /* No maximum chunk size (No chunk at all) */
        STSE_NO_PROT);      /* No protection */

    if (ret != STSE_OK) {
        return ret;
    }

    if (tlv_header[0] != TAG_SEQUENCE) {
        /* Invalid X.509 DER certificate: the TAG is not SEQUENCE (0x30) */
        return STSE_CERT_UNEXPECTED_SEQUENCE;
    }

    if (tlv_header[1] == 0x80) {
        /* Invalid X.509 DER certificate: the indefinite length is not allowed in DER (it would also
         * decode to len_of_len == 0, clashing with the short-form case below) */
        return STSE_CERT_INVALID_CERTIFICATE;
    }

    /* The second byte encodes the Value length: below 0x80 it is the length itself (short form, no
     * extra length bytes); otherwise bit 7 flags the long form and the low 7 bits tell how many of
     * the following bytes encode the length (e.g. 0x82 means the next 2) */
    PLAT_UI8 len_of_len = (tlv_header[1] < 0x80) ? 0 : (tlv_header[1] & ~0x80);

    /* Decode the Value length and the smallest length the used encoding is allowed to carry:
     * a certificate cannot be empty and DER requires the minimal length encoding */
    PLAT_UI16 min_size;
    switch (len_of_len) {
        case 0:
            *pCertificate_size = tlv_header[1];
            min_size = 1;     // 1 byte
            break;
        case 1:
            *pCertificate_size = tlv_header[2];
            min_size = 0x80;  // 128 bytes
            break;
        case 2:
            *pCertificate_size = (tlv_header[2] << 8) | tlv_header[3];
            min_size = 0x100; // 256 bytes
            break;
        default:
            /* Unsupported X.509 DER certificate: a length field longer than 2 bytes means a
             * certificate larger than 65535 bytes, beyond what *pCertificate_size can represent */
            return STSE_CERT_INVALID_CERTIFICATE;
    }

    if (*pCertificate_size < min_size) {
        /* Invalid X.509 DER certificate: the length is zero or not minimally encoded */
        return STSE_CERT_INVALID_CERTIFICATE;
    }

    /* Size of the Tag+Length field: tag byte + first length byte + extra length bytes.
     * The parsed length covers the Value only, so add this to get the whole size */
    PLAT_UI8 tl_len = 2 + len_of_len;
    if (*pCertificate_size > UINT16_MAX - tl_len) {
        /* Invalid X.509 DER certificate: the total size overflows 16 bits */
        return STSE_CERT_INVALID_CERTIFICATE;
    }
    *pCertificate_size += tl_len;

    return STSE_OK;
}

stse_ReturnCode_t stse_get_device_certificate(stse_Handle_t *pSTSE, PLAT_UI8 certificate_zone, PLAT_UI16 certificate_size, PLAT_UI8 *pCertificate) {
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
    stse_Handle_t *pSTSE,
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
    if (key_type >= STSE_ECC_KT_INVALID) {
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
