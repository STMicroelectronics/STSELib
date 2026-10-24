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

stse_return_code_t stsafea_query_generic_public_key_slots_count(
  stse_handle_t *p_stse,
  PLAT_UI8 *p_generic_public_key_slot_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_generic_public_key_slot_count == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_TABLE;
  PLAT_UI8 rsp_header;

  /* Prepare Query Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SUBJECT TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_subject_tag, 1, &subject_tag);

  /* Prepare Query RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [GENERIC PUBLIC KEY SLOT COUNT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_symmetric_key_slot_count, 1, p_generic_public_key_slot_count);

  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_generic_public_key_slot_info(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 *p_presence_flag,
  stsafea_generic_public_key_configuration_flags_t *p_configuration_flags,
  stse_ecc_key_type_t *p_key_type)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_presence_flag == NULL || p_configuration_flags == NULL || p_key_type == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
  stsafea_ecc_curve_id_t curve_id;
  PLAT_UI8 rsp_header;

  /* Prepare Query Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SUBJECT TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_subject_tag, 1, &subject_tag);
  /* [SLOT NUMBER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  /* Prepare Query RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PRESENCE FLAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_presence_flag, 1, p_presence_flag);
  /* [CONFIGURATION FLAGS] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_configuration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)p_configuration_flags);
  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_curve_id, sizeof(stsafea_ecc_curve_id_t), (PLAT_UI8 *)&curve_id);

  ret = stsafea_frame_raw_transfer(p_stse,
                                   &cmd_frame,
                                   &rsp_frame,
                                   stsafea_cmd_timings[p_stse->device_type][cmd_header]);

  if (ret != STSE_OK)
  {
    return ret;
  }

  if (*p_presence_flag == 1)
  {
    stse_ecc_key_type_t curve_id_index;
    PLAT_UI8 curve_id_total_length;
    *p_key_type = STSE_ECC_KT_INVALID;
    /*extract curve id length */
    curve_id_total_length = (*(e_curve_id.p_data) << 8);
    curve_id_total_length += *(e_curve_id.p_data + 1) + STSE_ECC_CURVE_ID_LENGTH_SIZE;
    /* Compare slot curve ID against each known curve ID to set the key type */
    for (curve_id_index = (stse_ecc_key_type_t)0;
         (PLAT_I8)curve_id_index < (PLAT_I8)STSE_ECC_KT_INVALID;
         curve_id_index++)
    {
      /* First check of the ID length to speed-up the loop */
      if (curve_id_total_length == stse_ecc_info_table[curve_id_index].curve_id_total_length)
      {
        PLAT_I32 diff;
        diff = memcmp((PLAT_UI8 *)&stse_ecc_info_table[curve_id_index].curve_id,
                      (PLAT_UI8 *)&curve_id,
                      stse_ecc_info_table[curve_id_index].curve_id_total_length);
        if (diff == 0)
        {
          *p_key_type = curve_id_index;
          break;
        }
      }
    }
    /* If the comparison loop reach the end and p_key_type is always as initialized return error */
    if ((curve_id_index) >= STSE_ECC_KT_INVALID || (PLAT_I8)*p_key_type >= (PLAT_I8)STSE_ECC_KT_INVALID)
    {
      return STSE_UNEXPECTED_ERROR;
    }
  }

  return ret;
}

stse_return_code_t stsafea_query_generic_public_key_slot_value(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *p_public_key)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (key_type >= STSE_ECC_KT_INVALID || p_public_key == NULL)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
  PLAT_UI8 rsp_header;
  PLAT_UI8 presence_flag;
  stsafea_generic_public_key_configuration_flags_t configuration_flags;
  PLAT_UI8 p_curve_id[stse_ecc_info_table[key_type].curve_id_total_length];

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);

  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size)
  };
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  /* Prepare Query Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SUBJECT TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_subject_tag, 1, &subject_tag);
  /* [SLOT NUMBER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  /* Prepare Query RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PRESENCE FLAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_presence_flag, 1, &presence_flag);
  /* [CONFIGURATION FLAGS] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_configuration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)&configuration_flags);
  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_curve_id, stse_ecc_info_table[key_type].curve_id_total_length,
                                   p_curve_id);

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&rsp_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&rsp_frame, &e_public_key_first_element);
  }
  else
#endif /* STSE_CONF_ECC_EDWARD_25519 */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&rsp_frame, &e_point_representation_id);

    /* [PUBLIC KEY FIRST ELEMENT LENGTH] */
    stse_frame_push_element(&rsp_frame, &e_public_key_length_first_element);

    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY FIRST ELEMENT] */
    stse_frame_push_element(&rsp_frame, &e_public_key_first_element);

    /* [PUBLIC KEY SECOND ELEMENT LENGTH] */
    stse_frame_push_element(&rsp_frame, &e_public_key_length_second_element);

    e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = p_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY SECOND ELEMENT] */
    stse_frame_push_element(&rsp_frame, &e_public_key_second_element);
  }

  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_write_generic_ecc_public_key(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *p_public_key)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_WRITE_PUBLIC_KEY
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_public_key == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);
  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  PLAT_UI8 rsp_header;

  /* Prepare Write Public Key Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [SLOT NUMBER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_curve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#if defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519)
  uint8_t is_supported_key = 0;
#ifdef STSE_CONF_ECC_CURVE_25519
  is_supported_key |= (key_type == STSE_ECC_KT_CURVE25519);
#endif /* STSE_CONF_ECC_CURVE_25519 */
#ifdef STSE_CONF_ECC_EDWARD_25519
  is_supported_key |= (key_type == STSE_ECC_KT_ED25519);
#endif /* STSE_CONF_ECC_EDWARD_25519 */

  if (is_supported_key)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&cmd_frame, &e_point_representation_id);

    /* [PUBLIC KEY FIRST ELEMENT LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY FIRST ELEMENT] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

    /* [PUBLIC KEY SECOND ELEMENT LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
    e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = p_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY SECOND ELEMENT] */
    stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
  }

  /* Prepare Write Public Key RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_set_generic_public_slot_configuration_flag(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stsafea_generic_public_key_configuration_flags_t configuration_flags)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_CONFIGURATION_FLAGS;

  /* Prepare Put Attribute Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [ATTRIBUTE TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_attribute_tag, 1, &attribute_tag);
  /* [SLOT NUMBER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  /* [CONFIGURATION FLAGS] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_configuration_flags,
                                   sizeof(stsafea_generic_public_key_configuration_flags_t),
                                   (PLAT_UI8 *)&configuration_flags);

  PLAT_UI8 rsp_header;
  /* Prepare Put Attribute RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
