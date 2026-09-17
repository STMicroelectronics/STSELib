/**
  ******************************************************************************
  * @file    stsafea_public_key_slots.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for for generic public slots (source)
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_public_key_slots.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_query_generic_public_key_slots_count(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pGeneric_public_key_slot_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;


  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pGeneric_public_key_slot_count == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_TABLE;
  PLAT_UI8 rsp_header;

  /* Subject tag selects generic-public-key table namespace for QUERY command. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command frame is deliberately compact: command code and subject selector only. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response payload is a single count byte after command status header. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSymmetric_key_slot_count, 1, pGeneric_public_key_slot_count);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_query_generic_public_key_slot_info(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 *pPresence_flag,
  stsafea_generic_public_key_configuration_flags_t *pConfiguration_flags,
  stse_ecc_key_type_t *pKey_type)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Slot-info query returns presence, mutable flags, and slot curve identifier. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPresence_flag == NULL || pConfiguration_flags == NULL || pKey_type == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
  stsafea_ecc_curve_id_t curve_id;
  PLAT_UI8 rsp_header;

  /* Returned curve_id is parsed to infer abstract stse_ecc_key_type_t value. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Slot-info query scope is narrowed by explicit slot_number element. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Expected response ordering mirrors command subject-specific attribute layout. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePresence_flag, 1, pPresence_flag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eConfiguration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)pConfiguration_flags);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCurve_id, sizeof(stsafea_ecc_curve_id_t), (PLAT_UI8 *)&curve_id);

  /*- Perform Transfer*/
  ret = stsafea_frame_raw_transfer(pSTSE,
                                   &CmdFrame,
                                   &RspFrame,
                                   stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

  if (ret != STSE_OK)
  {
    return ret;
  }

  if (*pPresence_flag == 1)
  {
    stse_ecc_key_type_t curve_id_index;
    PLAT_UI8 curve_id_total_length;
    *pKey_type = STSE_ECC_KT_INVALID;

    /* Presence-flag gate avoids decoding uninitialized curve-id data for empty slots. */
    /*extract curve id length */
    curve_id_total_length = (*(eCurve_id.pData) << 8);
    curve_id_total_length += *(eCurve_id.pData + 1) + STSE_ECC_CURVE_ID_LENGTH_SIZE;
    /* Compare slot curve ID against each known curve ID to set the key type */
    for (curve_id_index = (stse_ecc_key_type_t)0;
         (PLAT_I8)curve_id_index < (PLAT_I8)STSE_ECC_KT_INVALID;
         curve_id_index++)
    {
      /* First check of the ID length to speed-up the loop */
      if (curve_id_total_length == stse_ecc_info_table[curve_id_index].curve_id_total_length)
      {
        PLAT_I32 diff;
        /* Compare full TLV bytes, not just value bytes, to include size consistency. */
        diff = memcmp((PLAT_UI8 *)&stse_ecc_info_table[curve_id_index].curve_id,
                      (PLAT_UI8 *)&curve_id,
                      stse_ecc_info_table[curve_id_index].curve_id_total_length);
        if (diff == 0)
        {
          *pKey_type = curve_id_index;
          break;
        }
      }
    }
    /* If the comparison loop reach the end and pKey_type is always as initialized return error */
    if ((curve_id_index) >= STSE_ECC_KT_INVALID || (PLAT_I8)*pKey_type >= (PLAT_I8)STSE_ECC_KT_INVALID)
    {
      return STSE_UNEXPECTED_ERROR;
    }
  }

  return ret;
}

stse_ReturnCode_t stsafea_query_generic_public_key_slot_value(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *pPublic_key)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Value query returns full public key material in curve-dependent framing. */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (key_type >= STSE_ECC_KT_INVALID || pPublic_key == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Allocate elements and buffers*/
  /* Response carries presence/config metadata before curve-id and key payload. */
  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
  PLAT_UI8 rsp_header;
  PLAT_UI8 presence_flag;
  stsafea_generic_public_key_configuration_flags_t configuration_flags;
  PLAT_UI8 pCurve_id[stse_ecc_info_table[key_type].curve_id_total_length];

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(ePoint_representation_id, 1, &point_representation_id);

  PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size)
  };
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_first_element,
                              STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              pPublic_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(ePublic_key_second_element, 0, NULL);

  /* Data pointers for key elements are bound after frame-shape decision. */

  /* Construct Cmd & Rsp frames */
  /* Command and response element trees are built symmetrically for transfer helper reuse. */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSubject_tag, 1, &subject_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Metadata fields are always present even when slot has no populated key material. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePresence_flag, 1, &presence_flag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eConfiguration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)&configuration_flags);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame,
                                   eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   pCurve_id);

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* Ed25519 response uses one key block with associated 2-byte length field. */
    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
    /* Public key bytes are written directly into caller-provided destination buffer. */
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Weierstrass/Brainpool responses encode point representation then X and Y blocks. */
    stse_frame_push_element(&RspFrame, &ePoint_representation_id);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

    /* First coordinate segment starts at buffer base address. */
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

    stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

    /* Second coordinate is placed immediately after first coordinate bytes. */
    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
  }

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_write_generic_ecc_public_key(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *pPublic_key)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_WRITE_PUBLIC_KEY
  };

  /* Extended write command provisions slot curve-id and point bytes atomically. */

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pPublic_key == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Public key elements */
  /* Point representation is meaningful only for two-coordinate curve families. */
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

  /* Length elements are reused as transport TLV prefixes for key coordinates. */

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Command always embeds target slot and curve-id prior to key payload bytes. */
  /* Extended header selects write-public-key command dispatcher in firmware. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCurve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

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
    /* Curve25519/Ed25519 families serialize as single fixed-size key field. */
    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    /* One-block encoding avoids point-representation byte for these curve families. */
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* Other families serialize as uncompressed point with X then Y coordinates. */
    stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
    /* X coordinate starts at pPublic_key base pointer. */
    ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_first_element.pData = pPublic_key;
    stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

    stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
    /* Y coordinate follows X in contiguous caller-provided key buffer. */
    ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
    stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
  }

  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response payload contains only status for write operation acknowledgement. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_set_generic_public_slot_configuration_flag(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stsafea_generic_public_key_configuration_flags_t configuration_flags)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

  /* Configuration flags control slot behavior (for example usage restrictions). */

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_CONFIGURATION_FLAGS;

  /* PUT_ATTRIBUTE updates only selected slot flag byte; key material is untouched. */

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAttribute_tag, 1, &attribute_tag);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eConfiguration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)&configuration_flags);

  PLAT_UI8 rsp_header;
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
