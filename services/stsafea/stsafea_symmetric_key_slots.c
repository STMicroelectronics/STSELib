/**
  ******************************************************************************
  * @file    stsafea_symmetric_key_slots.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for symmetric key slots management (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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
#include <string.h>

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/* Exported functions --------------------------------------------------------*/

/* Query provisioning control policy bytes for one symmetric key slot.
 * The response maps directly into stsafea_symmetric_key_slot_provisioning_ctrl_fields_t.
 * No local transformation is applied beyond frame serialization/deserialization.
 */
stse_ReturnCode_t stsafea_query_symmetric_key_slot_provisioning_ctrl_fields(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *pCtrl_fields)
{
  /* Query command requests one slot provisioning control record. */
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_SLOT_PROVISIONING_CONTROL;
  /* Device response starts with status and then packed control flags. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pCtrl_fields == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Request payload: command header + subject tag + slot index. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response payload: status byte then fixed-size control field structure. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame,
                                   eCtrl_fields,
                                   sizeof(stsafea_symmetric_key_slot_provisioning_ctrl_fields_t),
                                   (PLAT_UI8 *)pCtrl_fields);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

/* Update provisioning control policy bytes for one symmetric key slot.
 * This call writes the control-field payload as provided by the caller.
 * Validation is limited to null checks and transport status.
 */
stse_ReturnCode_t stsafea_put_symmetric_key_slot_provisioning_ctrl_fields(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *pCtrl_fields)
{
  /* Put-attribute command writes one slot provisioning control record. */
  PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_SLOT_PROVISIONING_CONTROL;
  /* Put-attribute response returns only status header for this subject. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pCtrl_fields == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Request contains target slot and the packed control-field payload. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eCtrl_fields,
                                   sizeof(stsafea_symmetric_key_slot_provisioning_ctrl_fields_t),
                                   (PLAT_UI8 *)pCtrl_fields);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

/* Query total number of configured symmetric key slots.
 * Returned value is used by callers to size subsequent table requests.
 * The command uses the symmetric-key-table subject selector.
 */
stse_ReturnCode_t stsafea_query_symmetric_key_slots_count(stse_Handler_t *pSTSE, PLAT_UI8 *pSymmetric_key_slot_count)
{
  /* Query the number of entries present in the symmetric key table. */
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pSymmetric_key_slot_count == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_TABLE;
  /* Query response first byte is the active slot count. */
  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Minimal query frame: command header + key-table subject selector. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSymmetric_key_slot_count, 1, pSymmetric_key_slot_count);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

/* Query and decode the complete symmetric key table records.
 * The function parses variable layout records based on mode_of_operation.
 * CMAC, CCM and GCM records expose different mode-specific parameter fields.
 */
stse_ReturnCode_t stsafea_query_symmetric_key_table(
  stse_Handler_t *pSTSE,
  PLAT_UI8 symmetric_key_slot_count,
  stsafea_symmetric_key_slot_information_t *symmetric_key_table_info)
{
  /* Retrieve and decode the full variable-length symmetric key table. */
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (symmetric_key_table_info == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 slot_count, i;
  PLAT_UI8 *current_record;

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_TABLE;
  PLAT_UI8 rsp_header;
  /* Raw payload size is bounded by caller-provided expected slot count. */
  PLAT_UI16 rsp_raw_length = symmetric_key_slot_count * sizeof(stsafea_symmetric_key_slot_information_t);
  PLAT_UI8 pRsp_raw[rsp_raw_length];

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_raw, rsp_raw_length, pRsp_raw);

  /*- Perform Transfer*/
  ret = stsafea_frame_raw_transfer(pSTSE,
                                   &CmdFrame,
                                   &RspFrame,
                                   stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

  if (ret != STSE_OK)
  {
    /* Transport/status failures are propagated without local decoding. */
    return ret;
  }

  /* First payload byte stores the number of records actually returned. */
  slot_count = pRsp_raw[0];

  /* Caller-provided bound prevents oversized responses from overrunning buffer. */
  if (slot_count > symmetric_key_slot_count)
  {
    /* Guard against malformed/truncated sizing assumptions from caller side. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Skip "Number of slot" */
  current_record = pRsp_raw + 1;

  for (i = 0; i < slot_count; i++)
  {
    /* Clear destination before partially filling optional mode parameters. */
    memset(&symmetric_key_table_info[i], 0, sizeof(stsafea_symmetric_key_slot_information_t));
    if ((current_record[0] & 0x01) == 1)
    {
      /* Parse common part of key information record */
      /* Common record prefix always includes slot identity and operation mode. */
      memcpy(&symmetric_key_table_info[i],
             current_record,
             STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH);

      /* Per-mode parameters immediately follow the common record prefix. */
      /* Parse AES CMAC key information record parameters */
      if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_CMAC)
      {
        /* CMAC uses one extra byte for minimum accepted MAC length policy. */
        symmetric_key_table_info[i].parameters.cmac.minimum_MAC_length =
          current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH];
        current_record += 1;
      }
      else
        /* Parse AES CCM* key information record parameters */
        if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_CCM)
        {
          /* CCM block contains fixed parameter bytes and optional counter. */
          memcpy(&symmetric_key_table_info[i].parameters.ccm,
                 &current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH],
                 STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH);
          /* Optional counter is consumed before advancing to next record start. */
          if (symmetric_key_table_info[i].parameters.ccm.counter_presence == 1)
          {
            /* Counter value is present only when counter_presence bit is set. */
            memcpy(&symmetric_key_table_info[i].parameters.ccm.counter_value,
                   &current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH +
                                   STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH],
                   STSAFEA_COUNTER_VALUE_SIZE);
            current_record += STSAFEA_COUNTER_VALUE_SIZE;
          }
          current_record += STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH;
        }
        else
          /* Parse AES GCM key information record parameters */
          if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_GCM)
          {
            /* GCM currently contributes only authentication-tag length. */
            symmetric_key_table_info[i].parameters.gcm.auth_tag_length =
              current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH];
            current_record += 1;
          }
      /* Advance to next record after mode-specific parameter consumption. */
      current_record += STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH;
    }
    else
    {
      /* Inactive slot records are encoded on one byte and skipped as-is. */
      current_record += 1;
    }
  }

  return ret;
}

/* Establish device symmetric keys from host ECDHE public key material.
 * Frame layout differs between Curve25519 and Weierstrass curves.
 * The command carries curve identifier, host point data and algorithm selector.
 */
stse_ReturnCode_t stsafea_establish_symmetric_key(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *host_ecdhe_public_key)
{
  /* Extended command performs ECDHE-based symmetric key establishment. */
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (host_ecdhe_public_key == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY;
  PLAT_UI8 rsp_header;

  /* Uncompressed point representation is required for NIST/Brainpool curves. */
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
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* Curve identifier is serialized in the command before point coordinates. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
  if (key_type == STSE_ECC_KT_CURVE25519)
  {
    /* Curve25519 carries a single key element rather than X/Y coordinates. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Other curves send point format plus two fixed-size coordinates. */
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
  /* Algorithm id binds established secret to symmetric-key-derivation flow. */

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

/* Establish device symmetric keys with additional host signature proof.
 * The authenticated flow appends signer slot, optional hash id, and R/S values.
 * Signature length is derived from selected signature key type.
 */
stse_ReturnCode_t stsafea_establish_symmetric_key_authenticated(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *host_ecdhe_public_key,
  stse_hash_algorithm_t hash_algo,
  PLAT_UI8 signature_public_key_slot_number,
  stse_ecc_key_type_t signature_key_type,
  PLAT_UI8 *pSignature)
{
  /* Authenticated variant appends signer slot/hash/signature evidence. */
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (host_ecdhe_public_key == NULL || pSignature == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY;
  PLAT_UI8 rsp_header;

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
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  PLAT_UI8 filler_1_byte = 0;

  /* Hash-algorithm field can be omitted for signature schemes embedding hash. */
  PLAT_UI8 pEmpty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
  STSE_FRAME_ELEMENT_ALLOCATE(eHash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, pEmpty_hash_algo_id);

  /* Divide Signature length By 2 to get R or S length */
  PLAT_UI16 signature_R_S_length = stse_ecc_info_table[signature_key_type].signature_size >> 1;
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(signature_R_S_length),
    UI16_B0(signature_R_S_length),
  };

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
  if (key_type == STSE_ECC_KT_CURVE25519)
  {
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = host_ecdhe_public_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
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
  /* Reserved filler maintains compatibility with command payload revision. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eFiller, 1, &filler_1_byte);
  /* Signature verifier key slot selects trusted signer in device storage. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_public_key_slot_number, 1, &signature_public_key_slot_number);

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (signature_key_type != STSE_ECC_KT_ED25519)
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Non-Ed25519 signatures require explicit hash identifier selection. */
    eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
    eHash_algo_id.pData = (PLAT_UI8 *)&stsafea_hash_info_table[hash_algo].id;
  }
  stse_frame_push_element(&CmdFrame, &eHash_algo_id);

  /* Signature is serialized as two equal-size big-endian integers R then S. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R, signature_R_S_length, pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S, signature_R_S_length, pSignature + signature_R_S_length);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#if defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED)

/* Confirm previously established/provisioned keys using CMAC evidence.
 * CMAC input is the concatenation of key-information records as serialized.
 * Temporary length byte swap is reverted before returning to caller.
 */
stse_ReturnCode_t stsafea_confirm_symmetric_key(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pMac_confirmation_key,
  PLAT_UI8 key_count,
  stsafea_generic_key_information_t *pKey_information_list)
{
  /* Confirmation command binds provisioned key metadata to caller MAC key. */
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_CONFIRM_SYMMETRIC_KEYS
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pMac_confirmation_key == NULL) || (pKey_information_list == NULL) || (key_count == 0))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 pConfirmation_mac[STSE_KEY_CONFIRMATION_MAC_SIZE];
  /* Variable-length element array tracks each serialized key info record. */
  stse_frame_element_t eKey_information_list[key_count];
  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* First payload field is the confirmation MAC computed over all key infos. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eConfirmation_mac, STSE_KEY_CONFIRMATION_MAC_SIZE, pConfirmation_mac);

  ret = stse_platform_aes_cmac_init(
          pMac_confirmation_key,
          STSAFEA_AES_256_KEY_SIZE,
          STSE_KEY_CONFIRMATION_MAC_SIZE);
  if (ret != STSE_OK)
  {
    return (ret);
  }

  for (PLAT_UI8 i = 0; i < key_count; i++)
  {
    PLAT_UI8 temp_buffer;

    /* Serialized key-information records are length-prefixed in big-endian order. */
    eKey_information_list[i].length = pKey_information_list[i].info_length + STSAFEA_GENERIC_LENGTH_SIZE;
    eKey_information_list[i].pData = (PLAT_UI8 *)&pKey_information_list[i];

    /* Swap 2 bytes of length */
    temp_buffer = eKey_information_list[i].pData[0];
    eKey_information_list[i].pData[0] = eKey_information_list[i].pData[1];
    eKey_information_list[i].pData[1] = temp_buffer;

    ret = stse_platform_aes_cmac_append(
            eKey_information_list[i].pData,
            eKey_information_list[i].length);
    if (ret != STSE_OK)
    {
      return (ret);
    }

    stse_frame_push_element(&CmdFrame, &eKey_information_list[i]);
  }

  /* MAC output is written in-place into command frame confirmation field. */
  ret = stse_platform_aes_cmac_compute_finish(pConfirmation_mac, NULL);
  if (ret != STSE_OK)
  {
    return (ret);
  }

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  for (PLAT_UI8 i = 0; i < key_count; i++)
  {
    /* Restore caller-owned structures after temporary byte-order conversion. */
    PLAT_UI8 temp_buffer = eKey_information_list[i].pData[0];
    eKey_information_list[i].pData[0] = eKey_information_list[i].pData[1];
    eKey_information_list[i].pData[1] = temp_buffer;
  }

  return ret;
}

#endif /* defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || \ */

#if defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/* Provision one symmetric key using an encrypted/wrapped envelope blob.
 * Envelope content is opaque at this layer and not parsed locally.
 * Caller controls envelope length and ownership of the source buffer.
 */
stse_ReturnCode_t stsafea_write_symmetric_key_wrapped(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pSymmetric_key_envelope,
  PLAT_UI8 symmetric_key_envelope_length)
{
  /* Wrapped provisioning passes opaque encrypted envelope to the device. */
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_WRAPPED
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pSymmetric_key_envelope == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSymmetric_key_envelope, symmetric_key_envelope_length,
                                   pSymmetric_key_envelope);
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) || \ */

/* Provision one symmetric key using plaintext key bytes and metadata.
 * Key length is inferred from the selected key type or generic-secret length.
 * The function serializes key info first, followed by key value bytes.
 */
stse_ReturnCode_t stsafea_write_symmetric_key_plaintext(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pSymmetric_key_value,
  stsafea_generic_key_information_t *pSymmetric_key_info)
{
  /* Plaintext provisioning sends metadata + raw key bytes in one command. */
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_PLAINTEXT
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pSymmetric_key_info == NULL || pSymmetric_key_value == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  PLAT_UI16 key_value_length = 0;

  /* Key material length is derived from key type or generic-secret parameter. */
  switch (pSymmetric_key_info->type)
  {
    case STSAFEA_SYMMETRIC_KEY_TYPE_AES_128:
      key_value_length = STSAFEA_AES_128_KEY_SIZE;
      break;
    case STSAFEA_SYMMETRIC_KEY_TYPE_AES_256:
      key_value_length = STSAFEA_AES_256_KEY_SIZE;
      break;
    case STSAFEA_SYMMETRIC_KEY_TYPE_GENERIC_SECRET:
      key_value_length = pSymmetric_key_info->HMAC.generic_secret_key_length; /* HMAC or HKDF */
      break;
    default:
      return (STSE_SERVICE_INVALID_PARAMETER);
  }

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSymmetric_key_info, pSymmetric_key_info->info_length,
                                   (PLAT_UI8 *)&pSymmetric_key_info->lock_indicator);
  /* Key value is appended exactly after metadata to match device parser order. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSymmetric_key_value, key_value_length, pSymmetric_key_value);
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

/* Generate local envelope (wrap/unwrap) key in a selected slot.
 * The key-length selector maps AES-128 to 0x00 and AES-256 to 0x01.
 * Device-side key material generation occurs internally after command accept.
 */
stse_ReturnCode_t stsafea_generate_wrap_unwrap_key(stse_Handler_t *pSTSE,
                                                   PLAT_UI8 wrap_key_slot,
                                                   stse_aes_key_type_t key_type)
{
  /* Generate local envelope key in target slot for wrap/unwrap operations. */
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_KEY;

  PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_LOCAL_ENVELOPE_KEY_TABLE;
  /* Device encodes AES-128/AES-256 selection as one-byte length selector. */
  PLAT_UI8 key_length = (key_type == STSE_AES_128_KT) ? 0x00 : 0x01;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &attribute_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &wrap_key_slot);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eKeyLEnght, 1, &key_length);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

/* Erase a symmetric key slot and invalidate its key material on device.
 * Erase operation is addressed by slot index and returns status header only.
 * No local state is cached by this service after erase completion.
 */
stse_ReturnCode_t stsafea_erase_symmetric_key_slot(
  stse_Handler_t *pSTSE,
  PLAT_UI8 symmetric_key_slot_number)
{
  /* Extended erase command invalidates one symmetric key slot by index. */
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_ERASE_SYMMETRIC_KEY_SLOT
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &symmetric_key_slot_number);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
