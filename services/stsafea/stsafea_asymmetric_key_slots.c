/**
  ******************************************************************************
  * @file    stsafea_asymmetric_key_slots.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for asymmetric key slots management (source)
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
#include <string.h>

#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_query_private_key_slots_count(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pPrivate_key_slot_count)
{
  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPrivate_key_slot_count == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_TABLE;
  PLAT_UI8 rsp_header;

  /* Subject tag selects private-key-table namespace in generic QUERY command. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* QUERY private key table count uses minimal payload: command header + subject tag. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response returns status and one-byte slot count. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePrivate_key_slot_count, 1, pPrivate_key_slot_count);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_query_private_key_table(
  stse_Handler_t *pSTSE,
  PLAT_UI8 private_key_slot_count,
  PLAT_UI16 *pGlobal_usage_limit,
  stsafea_private_key_slot_information_t *private_key_table_info)
{
  stse_ReturnCode_t ret;
  /* Callers provide expected slot count to bound local receive buffer usage. */
  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (private_key_table_info == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 slot_count, i;
  PLAT_UI8 *current_record;
  PLAT_UI8 filler;

  /* Device can return a smaller slot_count than requested depending on family SKU. */

  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_TABLE;
  PLAT_UI8 rsp_header;
  PLAT_UI16 raw_table_length = private_key_slot_count * sizeof(stsafea_private_key_slot_information_t);
  PLAT_UI8 pTable_raw[raw_table_length];

  /* Raw table is a compact byte stream parsed locally into typed records. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Raw table response layout: status, count, filler, global limit, then packed records. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSlot_count, 1, &slot_count);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, efiller, 1, &filler);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eGlobal_usage_limit, 2, (PLAT_UI8 *)pGlobal_usage_limit);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eTable_raw, raw_table_length, pTable_raw);

  /*- Perform Transfer*/
  ret = stsafea_frame_raw_transfer(pSTSE,
                                   &CmdFrame,
                                   &RspFrame,
                                   stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Global usage limit is received in big-endian form on the wire. */
  stse_frame_element_swap_byte_order(&eGlobal_usage_limit);

  if (private_key_slot_count > slot_count)
  {
    /* Protect caller from reading uninitialized records when buffer is oversized. */
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  current_record = pTable_raw;

  for (i = 0; i < slot_count; i++)
  {
    /* current_record advances over variable-size records as each slot is decoded. */
    /* Copy fixed metadata first, then conditionally parse the curve-id TLV
     * only for populated slots. */
    memset(&private_key_table_info[i], 0, sizeof(stsafea_private_key_slot_information_t));
    memcpy(&private_key_table_info[i],
           current_record,
           STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH);
    current_record += STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH;
    if (private_key_table_info[i].presence_flag == 1)
    {
      /* Length check prevents copying malformed curve-id values beyond struct capacity. */
      if (ARRAY_2B_SWAP_TO_UI16(current_record) > STSE_ECC_CURVE_ID_VALUE_MAX_SIZE)
      {
        return STSE_UNEXPECTED_ERROR;
      }
      /* Curve ID is encoded as [len(2B), value(len)] and preserved as-is. */
      memcpy(&private_key_table_info[i].curve_id,
             current_record,
             ARRAY_2B_SWAP_TO_UI16(current_record) + STSAFEA_GENERIC_LENGTH_SIZE);
      current_record += ARRAY_2B_SWAP_TO_UI16(current_record) + STSAFEA_GENERIC_LENGTH_SIZE;
    }
  }

  return ret;
}

stse_ReturnCode_t stsafea_generate_ecc_key_pair(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI16 usage_limit,
  PLAT_UI8 *pPublic_key)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_KEY;
  PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_SLOT;
  PLAT_UI8 pFiller[2] = {0};

  /* Generation command can return either one-coordinate or two-coordinate public key
   * payload depending on selected curve family. */

  PLAT_UI8 rsp_header;
  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pPublic_key == NULL) || (key_type >= STSE_ECC_KT_INVALID))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

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

  /* Length elements are reused to decode coordinate TLVs in response frame. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command payload binds slot id, usage quota, and target curve identifier. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAttribute_tag, 1, &attribute_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eUsage_limit, 2, (PLAT_UI8 *)&usage_limit);
  /* Usage limit is encoded big-endian in transport payload. */
  stse_frame_element_swap_byte_order(&eUsage_limit);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eFiller, 2, pFiller);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response shape is selected at runtime to match curve-specific public key layout. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

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
    /* Curve25519/Ed25519 public keys are single-coordinate in this response layout. */
    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* NIST/Brainpool keys are returned as point representation + X and Y coordinates. */
    stse_frame_push_element(&RspFrame, &ePoint_representation_id);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
  }

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_generate_ECDHE_key_pair(
  stse_Handler_t *pSTSE,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *pPublic_key)
{
  /* ECDHE generation is ephemeral: command returns only the public half. */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_GENERATE_ECDHE};

  /* Extended command header identifies ECDHE flow in firmware command dispatcher. */

  PLAT_UI8 rsp_header;
  PLAT_UI8 pCurve_id_rsp[stse_ecc_info_table[key_type].curve_id_total_length];
  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  /* Returned curve id is echoed by firmware and can be cross-checked by caller if needed. */

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

  /* Empty data pointers are later rebound to caller output buffers before push. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Extended ECDHE command body carries only requested curve-id TLV. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id_cmd,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Firmware echoes curve-id before returning ephemeral public point material. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCurve_id_rsp,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   pCurve_id_rsp);

#ifdef STSE_CONF_ECC_CURVE_25519
  if (key_type == STSE_ECC_KT_CURVE25519)
  {
    /* ECDHE over Curve25519 returns only one coordinate block. */
    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) */
  {
    /* Standard Weierstrass curves return uncompressed point layout (X then Y). */
    stse_frame_push_element(&RspFrame, &ePoint_representation_id);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
  }

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
