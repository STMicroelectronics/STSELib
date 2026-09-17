/**
  ******************************************************************************
  * @file    stsafea_host_key_slot.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for host key slot management (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_host_key_slot.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stsafea_query_host_key_provisioning_ctrl_fields(
  stse_Handler_t *pSTSE,
  stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Control fields describe provisioning policy state for host-key slots. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pCtrl_fields == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
  PLAT_UI8 rsp_header;

  /* QUERY payload is [header, subject tag], response is [status, control fields]. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command frame allocation keeps element ordering explicit for protocol traceability. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response header carries STSAFE status code consumed by transfer helper. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCtrl_fields, sizeof(stsafea_host_key_provisioning_ctrl_fields_t),
                                   (PLAT_UI8 *)pCtrl_fields);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_put_host_key_provisioning_ctrl_fields(
  stse_Handler_t *pSTSE,
  stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

  /* PUT_ATTRIBUTE updates provisioning flags without touching key material. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pCtrl_fields == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
  PLAT_UI8 rsp_header;

  /* Control-field struct is serialized as-is in command payload. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* PUT_ATTRIBUTE request embeds updated control bytes directly after subject tag. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCtrl_fields, sizeof(stsafea_host_key_provisioning_ctrl_fields_t),
                                   (PLAT_UI8 *)pCtrl_fields);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Attribute update expects status-only response payload. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_query_host_key(
  stse_Handler_t *pSTSE,
  stsafea_host_key_slot_t *pHost_key_slot)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* V1 host-key slot query returns legacy key-slot layout. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pHost_key_slot == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
  PLAT_UI8 rsp_header;

  /* Subject tag differentiates V1 and V2 host-key record formats. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* V1 subject query has no selector argument besides subject tag itself. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Returned key-slot structure is copied in transport order field-by-field. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eHost_key_slot, sizeof(stsafea_host_key_slot_t),
                                   (PLAT_UI8 *)pHost_key_slot);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_query_host_key_v2(
  stse_Handler_t *pSTSE,
  stsafea_host_key_slot_v2_t *pHost_key_slot_v2)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* V2 query supports expanded host-key slot representation. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pHost_key_slot_v2 == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V2;
  PLAT_UI8 rsp_header;

  /* Returned data is copied directly into caller-provided v2 structure. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* V2 query reuses identical command framing with a different subject selector. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eHost_key_slot, sizeof(stsafea_host_key_slot_v2_t),
                                   (PLAT_UI8 *)pHost_key_slot_v2);

  /*- Perform Transfer*/
  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_put_attribute_host_key(
  stse_Handler_t *pSTSE,
  stsafea_aes_128_host_keys_t *host_keys)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

  /* Legacy V1 host-key write path supports AES-128 key bundle format. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (host_keys == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
  PLAT_UI8 rsp_header;

  /* Key bundle payload follows attribute subject selector in command frame. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Legacy host key PUT_ATTRIBUTE path does not include key-type discriminator byte. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHost_keys, sizeof(stsafea_aes_128_host_keys_t), (PLAT_UI8 *)host_keys);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_host_key_provisioning(
  stse_Handler_t *pSTSE,
  stsafea_host_key_type_t key_type,
  stsafea_host_keys_t *host_keys)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT
  };

  /* Extended plaintext provisioning writes host keys in clear inside secure channel
   * context selected by caller/session policy. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (key_type == STSAFEA_AES_INVALID_HOST_KEY || host_keys == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 host_keys_length = (key_type == STSAFEA_AES_128_HOST_KEY)
                              ? STSAFEA_HOST_AES_128_KEYS_SIZE
                              : STSAFEA_HOST_AES_256_KEYS_SIZE;

  /* Key payload length is derived from selected key_type to avoid hardcoded sizes. */

  PLAT_UI8 pPadding[3] = {0};
  PLAT_UI8 rsp_header;

  /* Three-byte padding aligns payload with command specification reserved bytes. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Extended provisioning command body layout: padding, key type, key bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePadding, 3, pPadding);
  /* Padding keeps backward compatibility with reserved command fields. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eKey_type, 1, (PLAT_UI8 *)&key_type);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHost_keys, host_keys_length, (PLAT_UI8 *)host_keys);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_host_key_provisioning_wrapped(
  stse_Handler_t *pSTSE,
  stsafea_host_key_type_t key_type,
  PLAT_UI8 *pHost_key_envelope)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED
  };

  /* Wrapped provisioning sends encrypted/authenticated key envelope to device. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (key_type == STSAFEA_AES_INVALID_HOST_KEY || pHost_key_envelope == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 host_keys_envelope_length = (key_type == STSAFEA_AES_128_HOST_KEY)
                                       ? STSAFEA_HOST_AES_128_KEYS_ENVELOPE_SIZE
                                       : STSAFEA_HOST_AES_256_KEYS_ENVELOPE_SIZE;

  /* Envelope size includes wrapped key bytes and associated authenticity data. */

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Wrapped provisioning omits plaintext key-type padding triplet and sends envelope only. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHost_keys, host_keys_envelope_length, (PLAT_UI8 *)pHost_key_envelope);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_establish_host_key(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t host_ecdh_public_key_type,
  PLAT_UI8 *pPublic_key,
  stsafea_host_key_type_t host_keys_type)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2
  };

  /* Establish flow derives host keys from ECDH public input and selected host key type. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((host_ecdh_public_key_type >= STSE_ECC_KT_INVALID) || (pPublic_key == NULL)
      || (host_keys_type >= STSAFEA_AES_INVALID_HOST_KEY))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  /* Point representation byte is used only for two-coordinate curve families. */

  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  /* Both length elements hold identical coordinate size for X/Y pair encoding. */

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element,
                              stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, pPublic_key);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element,
                              stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, NULL);

  /* First coordinate points at input buffer base; second coordinate is offset later. */

  /* Second coordinate pointer is bound later for non-Curve25519 curves. */

  /* command frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[host_ecdh_public_key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[host_ecdh_public_key_type].curve_id);

  /* Curve identifier precedes public key bytes to disambiguate coordinate size. */

#ifdef STSE_CONF_ECC_CURVE_25519
  if (host_ecdh_public_key_type == STSE_ECC_KT_CURVE25519)
  {
    /* Curve25519 input is serialized as a single key block. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Other curves carry point-format byte and two coordinates (X,Y). */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  PLAT_UI8 algorithm_id = 0x03;
  /* Algorithm ID 0x03 corresponds to ECDH-based host-key establishment variant. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAlgorithm_id, 1, &algorithm_id);

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHost_keys_type, 1, (PLAT_UI8 *)&host_keys_type);

  /* Host key type selects AES-128 vs AES-256 derived key storage format. */

  /* response frame */
  PLAT_UI8 rsp_header;
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_establish_host_key_authenticated(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t host_ecdh_public_key_type,
  PLAT_UI8 *pPublic_key,
  stsafea_host_key_type_t host_keys_type,
  PLAT_UI8 signature_public_key_slot,
  stse_ecc_key_type_t signature_public_key_type,
  stse_hash_algorithm_t signature_hash_algo,
  PLAT_UI8 *pSignature)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2
  };

  /* Authenticated establish extends basic ECDH establish with signer metadata and signature. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((host_ecdh_public_key_type >= STSE_ECC_KT_INVALID) || (pPublic_key == NULL)
      || (host_keys_type >= STSAFEA_AES_INVALID_HOST_KEY) || (signature_public_key_type >= STSE_ECC_KT_INVALID)
      || (signature_hash_algo >= STSE_SHA_INVALID) || (pSignature == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  /* Same public-key encoding strategy as non-authenticated establish flow. */

  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  /* Authenticated variant reuses ECDH key serialization from basic establish flow. */

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element,
                              stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, pPublic_key);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element,
                              stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, NULL);

  /* Signature metadata appended later authenticates the ECDH public key origin. */

  /* Signature verification key and hash algorithm are appended after ECDH key material. */

  /* command frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* Host ECDH curve ID of public key */
  /* Curve-id TLV allows device parser to validate coordinate lengths. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[host_ecdh_public_key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[host_ecdh_public_key_type].curve_id);

  /* Host ECDH public key */
#ifdef STSE_CONF_ECC_CURVE_25519
  if (host_ecdh_public_key_type == STSE_ECC_KT_CURVE25519)
  {
    /* Single-coordinate encoding path. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Uncompressed point encoding path (representation + X + Y). */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  /* Algorithm ID */
  PLAT_UI8 algorithm_id = 0x03;
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAlgorithm_id, 1, &algorithm_id);

  /* Host key type */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHost_keys_type, 1, (PLAT_UI8 *)&host_keys_type);

  /* Filler */
  PLAT_UI8 filler = 0x00;
  /* Reserved filler keeps backward-compatible command layout. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eFiller, 1, &filler);

  /* Public key slot number */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_public_key_slot, 1, &signature_public_key_slot);

  /* Signature public key slot identifies verification key stored in generic public slots. */

  /* Hash algo ID */
  STSE_FRAME_ELEMENT_ALLOCATE(eHash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, NULL);
#ifdef STSE_CONF_ECC_EDWARD_25519
  PLAT_UI8 padding_hash[STSAFEA_HASH_ALGO_ID_LENGTH_SIZE] = {0};
  if (signature_public_key_type == STSE_ECC_KT_ED25519)
  {
    /* Ed25519 path uses padding bytes instead of explicit hash-id field. */
    eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_LENGTH_SIZE;
    eHash_algo_id.pData = padding_hash;
  }
  else
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Non-EdDSA path provides one-byte hash algorithm identifier. */
    eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
    eHash_algo_id.pData = (PLAT_UI8 *)&stsafea_hash_info_table[signature_hash_algo].id;
  }
  stse_frame_push_element(&CmdFrame, &eHash_algo_id);

  /* Signature elements */
  /* Signature is transferred as two equal-size integers R and S with 2-byte lengths. */
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[signature_public_key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[signature_public_key_type].signature_size >> 1),
  };

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  /* R and S halves are transmitted sequentially with explicit lengths for parser safety. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R,
                                   (stse_ecc_info_table[signature_public_key_type].signature_size >> 1), pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S,
                                   (stse_ecc_info_table[signature_public_key_type].signature_size >> 1),
                                   pSignature + (stse_ecc_info_table[signature_public_key_type].signature_size >> 1));

  /* response frame */
  PLAT_UI8 rsp_header;
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
