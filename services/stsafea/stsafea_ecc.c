/**
  ******************************************************************************
  * @file    stsafea_ecc.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for ECC (source)
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

#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_start_volatile_KEK_session(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *host_ecdhe_public_key)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION
  };

  /* Start-volatile-KEK opens a transient key-encryption context derived from
   * host ECDHE input and selected curve metadata. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (host_ecdhe_public_key == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
  PLAT_UI8 rsp_header;

  /* Algorithm selector tells firmware that session key will be used for KEK unwrap. */

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  /* Length element value matches coordinate_or_key_size from selected curve entry. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  /* Coordinate element lengths are initialized from curve table once and reused. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command frame layout: ext header, curve-id, host pubkey, algorithm-id. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);
  /* Curve-id bytes are consumed by firmware to validate public key formatting. */

#ifdef STSE_CONF_ECC_CURVE_25519
  if (key_type == STSE_ECC_KT_CURVE25519)
  {
    /* Curve25519 uses a single coordinate/key block representation. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Weierstrass/Brainpool families use point-representation + X and Y coordinates. */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = host_ecdhe_public_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAlgorithm_id, 1, &algorithm_id);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response is status-only for session start command family. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_start_volatile_KEK_session_authenticated(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t ecdhe_key_type,
  PLAT_UI8 *host_ecdhe_public_key,
  stse_hash_algorithm_t hash_algo,
  PLAT_UI8 signature_public_key_slot_number,
  stse_ecc_key_type_t signature_key_type,
  PLAT_UI8 *pSignature)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION
  };

  /* Authenticated variant appends signer metadata and signature over ECDHE data. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (host_ecdhe_public_key == NULL || pSignature == NULL
      || ecdhe_key_type >= STSE_ECC_KT_INVALID || signature_key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  /* Public key length fields are repeated for deterministic two-coordinate packing. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  PLAT_UI8 kdf_algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
  PLAT_UI8 filler_1_byte = 0;

  /* filler_1_byte occupies reserved command field for version-compatible framing. */

  PLAT_UI8 pEmpty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
  STSE_FRAME_ELEMENT_ALLOCATE(eHash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, pEmpty_hash_algo_id);

  /* Hash algo element defaults to empty for Ed25519-style flows when applicable. */

  /* Divide Signature length By 2 to get R or S length */
  PLAT_UI16 signature_R_S_length = stse_ecc_info_table[signature_key_type].signature_size >> 1;
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(signature_R_S_length),
    UI16_B0(signature_R_S_length),
  };

  PLAT_UI8 rsp_header = 0;

  /* FRAME : . */
  STSE_FRAME_ALLOCATE(CmdFrame);

  /* FRAME : [HEADER] [EXT HEADER] */
  /* Extended header selects volatile KEK session opcode in secure-element parser. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* FRAME : [HEADER] [EXT HEADER] [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[ecdhe_key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[ecdhe_key_type].curve_id);

  /* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] */
#ifdef STSE_CONF_ECC_CURVE_25519
  if (ecdhe_key_type == STSE_ECC_KT_CURVE25519)
  {
    /* Curve25519 authenticated flow serializes one public key field only. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Other curves serialize point representation and both affine coordinates. */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

    ePublic_key_first_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

    ePublic_key_second_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = host_ecdhe_public_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  /* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER] [SIGNATURE KEY SLOT] */
  /* Signature public key slot references verification key preloaded in device storage. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eKdf_algorithm_id, 1, &kdf_algorithm_id);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eFiller, 1, &filler_1_byte);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_public_key_slot_number, 1, &signature_public_key_slot_number);

  /* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER] [SIGNATURE KEY SLOT] [HASH ALGO] */
#ifdef STSE_CONF_ECC_EDWARD_25519
  if (signature_key_type != STSE_ECC_KT_ED25519)
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Non-EdDSA signatures explicitly carry hash algorithm identifier byte. */
    eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
    eHash_algo_id.pData = (PLAT_UI8 *)&stsafea_hash_info_table[hash_algo].id;
  }
  stse_frame_push_element(&CmdFrame, &eHash_algo_id);

  /* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER]
   * [SIGNATURE KEY SLOT] [HASH ALGO] [SIGNATURE]
   */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  /* Signature is split as R then S halves, each preceded by 2-byte length field. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R, signature_R_S_length, pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S, signature_R_S_length, pSignature + signature_R_S_length);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Authenticated start command also returns only status byte in payload. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_stop_volatile_KEK_session(
  stse_Handler_t *pSTSE)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION
  };

  /* Stop command closes transient KEK state and associated volatile context. */

  /* Stop command closes transient KEK state and associated volatile context. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Stop command carries only the extended header. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Status response confirms session teardown result. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_verify_signature(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *pPublic_key,
  const PLAT_UI8 *pSignature,
  const PLAT_UI8 *pMessage,
  PLAT_UI16 message_length,
  PLAT_UI8 eddsa_variant,
  PLAT_UI8 *pSignature_validity)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_SIGNATURE;

  /* Verify operation takes full public key, signature and message payload in one frame. */

  PLAT_UI8 subject = 0x00;

  /* subject is currently fixed per command specification for generic verify path. */

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };

  /* Signature elements */
  /* R/S expected size is derived from key_type-specific signature width. */
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPublic_key == NULL || pSignature == NULL
      || pMessage == NULL || pSignature_validity == NULL
      || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Public key elements */
  /* Public key layout depends on curve family: one-block or point representation. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  /* Verify command uses curve-derived public key size to parse input key bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  /* Hash elements*/
#ifdef STSE_CONF_ECC_EDWARD_25519
  STSE_FRAME_ELEMENT_ALLOCATE(eEdDSA_variant, 1, &eddsa_variant);
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject, 1, &subject);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);
  /* Curve id enables firmware-side domain separation across supported ECC sets. */

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* Ed25519 verify path uses single key block representation. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = (PLAT_UI8 *)pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* NIST/Brainpool verify path uses explicit point format + X/Y blocks. */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = (PLAT_UI8 *)pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = (PLAT_UI8 *)pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  /* Signature format is strict pair of fixed-size components R/S. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   (PLAT_UI8 *)pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   (PLAT_UI8 *)pSignature + (stse_ecc_info_table[key_type].signature_size >> 1));

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* EdDSA variant byte selects pure/ph modes according to firmware support. */
    stse_frame_push_element(&CmdFrame, &eEdDSA_variant);
  }
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eMessage_length,
                                   STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* Message length is serialized big-endian before transfer. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_length, (PLAT_UI8 *)pMessage);
  stse_frame_element_swap_byte_order(&eMessage_length);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* verify-signature response returns status and one-byte validity indicator. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_validity, 1, pSignature_validity);

  /* - Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (ret != STSE_OK)
  {
    /* Any transfer/command failure forces validity output to false for safety. */
    *pSignature_validity = STSAFEA_FALSE;
  }

  return ret;
}

stse_ReturnCode_t stsafea_ecc_generate_signature(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *pMessage,
  PLAT_UI16 message_length,
  PLAT_UI8 *pSignature)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_SIGNATURE;

  /* Signature generation uses private key slot and raw message bytes as input. */

  PLAT_UI8 rsp_header;
  /* Signature elements */
  /* Output signature size is inferred from key type configured in slot metadata. */
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pMessage == NULL || pSignature == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command payload is [header, slot, message_len, message]. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eMessage_length,
                                   STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* Message length field is serialized in big-endian order before transfer. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_length, (PLAT_UI8 *)pMessage);
  stse_frame_element_swap_byte_order(&eMessage_length);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response returns R/S halves with explicit lengths for parser robustness. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_R, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   pSignature + (stse_ecc_info_table[key_type].signature_size >> 1));

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_establish_shared_secret(
  stse_Handler_t *pSTSE,
  PLAT_UI8 private_key_slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *pPublic_key,
  PLAT_UI8 *pShared_secret)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_ESTABLISH_KEY;

  /* Shared-secret establishment combines local private slot and peer public key. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPublic_key == NULL || pShared_secret == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Public key elements */
  /* Peer public key encoding mirrors key-family-specific transport representation. */
  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);
  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  /* Peer public key length descriptors are set from selected curve descriptor table. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  PLAT_UI8 rsp_header;
  PLAT_UI8 pShared_secret_length[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].shared_secret_size),
    UI16_B0(stse_ecc_info_table[key_type].shared_secret_size),
  };

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePrivate_key_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
                                   &private_key_slot_number);
  /* private_key_slot_number identifies which on-chip private key participates in ECDH. */

#if defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519)
  uint8_t is_supported_key = 0;
#ifdef STSE_CONF_ECC_CURVE_25519
  is_supported_key |= (key_type == STSE_ECC_KT_CURVE25519);
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
#ifdef STSE_CONF_ECC_EDWARD_25519
  is_supported_key |= (key_type == STSE_ECC_KT_ED25519);
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */

  if (is_supported_key)
  {
    /* Curve25519/Ed25519 path uses one key block. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Other curves include point-representation byte and X/Y coordinates. */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response layout: status + shared secret length + shared secret bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame,
                                   eShared_secret_length,
                                   STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pShared_secret_length);
  /* Returned secret bytes length is predetermined by curve table metadata. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eShared_secret, stse_ecc_info_table[key_type].shared_secret_size,
                                   pShared_secret);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_decompress_public_key(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 point_representation_id,
  PLAT_UI8 *pPublic_key_X,
  PLAT_UI8 *pPublic_key_Y)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY
  };

  /* Decompression reconstructs Y coordinate from curve id, point sign byte and X. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPublic_key_X == NULL || pPublic_key_Y == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command payload carries curve-id, point representation selector and X coordinate. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_ID,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePoint_representation_id, 1, &point_representation_id);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePublic_key_X_coordinate,
                                   stse_ecc_info_table[key_type].coordinate_or_key_size, pPublic_key_X);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response returns only reconstructed Y coordinate after status byte. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePublic_key_Y_coordinate,
                                   stse_ecc_info_table[key_type].coordinate_or_key_size, pPublic_key_Y);

  /* Output buffer pPublic_key_Y receives reconstructed coordinate bytes directly. */

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
