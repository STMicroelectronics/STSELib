/**
  ******************************************************************************
  * @file    stse_device_authentication.c
  * @author  CS Application Team
  * @brief   STSE uthentication API (sources)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

#include "api/stse_device_authentication.h"

#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_crypto.h"
#include "certificate/stse_certificate_subparsing.h"
#include "certificate/stse_certificate_types.h"

#define STSE_CERTIFICATE_OFFSET_BYTES 0U
#define STSE_CERTIFICATE_DEVICE_ID_OFFSET 15U
#define STSE_CERTIFICATE_DEVICE_ID_SIZE 11U

/* Static functions declaration ----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

stse_return_code_t stse_get_device_id(stse_handle_t *p_stse, PLAT_UI8 certificate_zone, PLAT_UI8 *p_device_id)
{
  volatile stse_return_code_t ret = STSE_API_INVALID_PARAMETER;

  /* - Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  ret = stse_data_storage_read_data_zone(
          p_stse,                             /* STSE handler */
          certificate_zone,                  /* Certificate zone */
          STSE_CERTIFICATE_DEVICE_ID_OFFSET, /* X bytes offset */
          p_device_id,                        /* Returned certificate size */
          STSE_CERTIFICATE_DEVICE_ID_SIZE,   /* Certificate size length */
          0,                                 /* No maximum chunk size (No chunk at all) */
          STSE_NO_PROT);                     /* No protection */

  return ret;
}

stse_return_code_t stse_get_device_certificate_size(stse_handle_t *p_stse, PLAT_UI8 certificate_zone,
                                                    PLAT_UI16 *p_certificate_size)
{
  volatile stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  PLAT_UI8 tlv_header[4];

  /* - Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  if (p_certificate_size == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  ret = stse_data_storage_read_data_zone(
          p_stse,              /* STSE handler */
          certificate_zone,   /* Certificate zone */
          0,                  /* No offset */
          tlv_header,         /* Returned TLV header (tag + length bytes) */
          sizeof(tlv_header), /* TLV header length 4 bytes */
          0,                  /* No maximum chunk size (No chunk at all) */
          STSE_NO_PROT);      /* No protection */

  if (ret != STSE_OK)
  {
    return ret;
  }

  if (tlv_header[0] != TAG_SEQUENCE)
  {
    /* Invalid X.509 DER certificate: the TAG is not SEQUENCE (0x30) */
    return STSE_CERT_UNEXPECTED_SEQUENCE;
  }

  if (tlv_header[1] == 0x80)
  {
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
  switch (len_of_len)
  {
    case 0:
      *p_certificate_size = tlv_header[1];
      min_size = 1; /*  1 byte */
      break;
    case 1:
      *p_certificate_size = tlv_header[2];
      min_size = 0x80; /*  128 bytes */
      break;
    case 2:
      *p_certificate_size = (tlv_header[2] << 8) | tlv_header[3];
      min_size = 0x100; /*  256 bytes */
      break;
    default:
      /* Unsupported X.509 DER certificate: a length field longer than 2 bytes means a
           * certificate larger than 65535 bytes, beyond what *p_certificate_size can represent */
      return STSE_CERT_INVALID_CERTIFICATE;
  }

  if (*p_certificate_size < min_size)
  {
    /* Invalid X.509 DER certificate: the length is zero or not minimally encoded */
    return STSE_CERT_INVALID_CERTIFICATE;
  }

  /* Size of the Tag+Length field: tag byte + first length byte + extra length bytes.
   * The parsed length covers the Value only, so add this to get the whole size */
  PLAT_UI8 tl_len = 2 + len_of_len;
  if (*p_certificate_size > UINT16_MAX - tl_len)
  {
    /* Invalid X.509 DER certificate: the total size overflows 16 bits */
    return STSE_CERT_INVALID_CERTIFICATE;
  }
  *p_certificate_size += tl_len;

  return STSE_OK;
}

stse_return_code_t stse_get_device_certificate(stse_handle_t *p_stse, PLAT_UI8 certificate_zone,
                                               PLAT_UI16 certificate_size, PLAT_UI8 *p_certificate)
{
  volatile stse_return_code_t ret = STSE_API_INVALID_PARAMETER;

  /* - Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  if (p_certificate == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  ret = stse_data_storage_read_data_zone(
          p_stse,                         /* STSE handler */
          certificate_zone,              /* Certificate zone */
          STSE_CERTIFICATE_OFFSET_BYTES, /* 0 bytes offset */
          p_certificate,                  /* Returned certificate size */
          certificate_size,              /* Certificate size length */
          253,                           /* Above, it throw a STSE_CORE_FRAME_CRC_ERROR */
          STSE_NO_PROT);                 /* No protection */

  if (ret != STSE_OK)
  {
    return ret;
  }

  return (STSE_OK);
}

stse_return_code_t stse_device_authenticate(
  stse_handle_t *p_stse,
  const PLAT_UI8 *p_root_ca_certificate,
  PLAT_UI8 certificate_zone,
  PLAT_UI8 priv_key_slot_number)
{
  stse_return_code_t ret;
  stse_certificate_t leaf_certificate;
  PLAT_UI16 certificate_size;

  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  if (p_root_ca_certificate == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  ret = stse_get_device_certificate_size(p_stse, certificate_zone, &certificate_size);

  if (ret != STSE_OK)
  {
    return ret;
  }

  PLAT_UI8 certificate[certificate_size];

  ret = stse_get_device_certificate(p_stse, certificate_zone, certificate_size, certificate);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Parse certificate chain (including Root CA certificate) */
  ret = stse_certificate_parse_chain(
          (uint8_t *)p_root_ca_certificate,
          certificate,
          certificate_size,
          &leaf_certificate);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Get ECC key type from leaf certificate */
  stse_ecc_key_type_t key_type = stse_certificate_get_key_type(&leaf_certificate);
  if (key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_UNEXPECTED_ERROR);
  }

  PLAT_UI16 signature_size = stse_ecc_info_table[key_type].signature_size;
  PLAT_UI8 signature[signature_size];

  PLAT_UI16 challenge_size;
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type == STSAFE_L010)
  {
    challenge_size = STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH;
  }
  else
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    challenge_size = stse_ecc_info_table[key_type].private_key_size;
#ifdef STSE_CONF_ECC_NIST_P_521
    if (key_type == STSE_ECC_KT_NIST_P_521)
    {
      challenge_size -= 2;
    }
#endif /* STSE_CONF_ECC_NIST_P_521 */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
  PLAT_UI8 challenge[challenge_size];

  /* - Generate random challenge */
  for (PLAT_I32 i = 0; i < challenge_size; i++)
  {
    challenge[i] = (PLAT_UI8)(stse_platform_generate_random() & 0xFF);
  }

  /* - Get target SE challenge signature */
  ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type >= STSAFE_L010 && p_stse->device_type < STSAFE_L010 + STSAFEL_PRODUCT_COUNT)
  {
    ret = stsafel_ecc_generate_signature(p_stse, key_type, challenge, challenge_size, signature);
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  if (p_stse->device_type >= STSAFE_A100 && p_stse->device_type < STSAFE_A100 + STSAFEA_PRODUCT_COUNT)
  {
    ret = stsafea_ecc_generate_signature(p_stse, priv_key_slot_number, key_type, challenge, challenge_size, signature);
  }
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Verify SE Challenge over leaf certificate public key*/
  ret = stse_certificate_verify_signature(
          &leaf_certificate,
          challenge,
          challenge_size,
          signature, (signature_size >> 1),
          &signature[signature_size >> 1], (signature_size >> 1));
  if (ret != STSE_OK)
  {
    return ret;
  }

  return STSE_OK;
}
