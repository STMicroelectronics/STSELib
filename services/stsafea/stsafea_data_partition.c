/**
  ******************************************************************************
  * @file    stsafea_data_partition.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for data partition (source)
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

#include "services/stsafea/stsafea_data_partition.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_ZONE_INDEX_SIZE 1U
#define STSAFEA_ZONE_OFFSET_SIZE 2U
#define STSAFEA_ZONE_ACCESS_OPTION_SIZE 1U
#define STSAFEA_INC_DEC_AMOUT_SIZE 4U
#define STSAFEA_ZONE_ACCESS_LENGTH_SIZE 2U

stse_return_code_t stsafea_switch_data_partition_access_protection(stse_handle_t *p_stse, PLAT_UI8 command_code,
                                                                   stse_cmd_protection_t protection)
{
  switch (protection)
  {

    case STSE_HOST_C_MAC_R_MAC:
      stsafea_perso_info_set_cmd_ac(&p_stse->perso_info, command_code, STSE_CMD_AC_HOST);
    case STSE_NO_PROT:
      break;

    case STSE_HOST_C_WRAP:
    case STSE_HOST_R_WRAP:
    case STSE_HOST_C_WRAP_R_WRAP:
    default:
      return STSE_SERVICE_INVALID_PARAMETER;
      break;
  }
  return STSE_OK;
}

stse_return_code_t stsafea_get_total_partition_count(stse_handle_t *p_stse,
                                                     PLAT_UI8 *p_total_partition_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Prepare Query Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, &tag);

  /* Prepare Query RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [TOTAL PARTITION COUNT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_total_partition_count, 1, p_total_partition_count);
  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_get_data_partitions_configuration(stse_handle_t *p_stse,
                                                             PLAT_UI8 total_partitions_count,
                                                             stsafea_data_partition_record_t *p_record_table,
                                                             PLAT_UI16 record_table_size)
{
  stse_return_code_t ret;
  volatile PLAT_UI8 partition_idx;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
  PLAT_UI8 rsp_header;
  PLAT_UI8 raw_data[record_table_size];

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Prepare Query Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, &tag);

  /* Prepare Query RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [RAW DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_raw, record_table_size, raw_data);
  ret = stsafea_frame_raw_transfer(p_stse,
                                   &cmd_frame,
                                   &rsp_frame,
                                   stsafea_cmd_timings[p_stse->device_type][cmd_header]);

  /* - Verify transfer result and build Partition record table */
  if (ret == STSE_OK)
  {
    PLAT_UI16 i = 1;
    for (partition_idx = 0; partition_idx < total_partitions_count; partition_idx++)
    {
      p_record_table->index = raw_data[i++];
      p_record_table->zone_type = raw_data[i++];
      p_record_table->read_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_READ_CR_GET(raw_data[i]);
      p_record_table->read_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_READ_GET(raw_data[i]);
      p_record_table->update_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_UPDATE_CR_GET(raw_data[i]);
      p_record_table->update_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_UPDATE_GET(raw_data[i++]);
      p_record_table->data_segment_length = UI16_B1_SET(raw_data[i++]);
      p_record_table->data_segment_length += UI16_B0_SET(raw_data[i++]);
      if (p_record_table->zone_type == 1)
      {
        p_record_table->counter_value = UI32_B3_SET(raw_data[i++]);
        p_record_table->counter_value += UI32_B2_SET(raw_data[i++]);
        p_record_table->counter_value += UI32_B1_SET(raw_data[i++]);
        p_record_table->counter_value += UI32_B0_SET(raw_data[i++]);
      }
      else
      {
        p_record_table->counter_value = 0;
      }
      p_record_table++;
    }
  }

  return (ret);
}

stse_return_code_t stsafea_decrement_counter_zone(stse_handle_t *p_stse,
                                                  PLAT_UI8 zone_index,
                                                  stsafea_decrement_option_t option,
                                                  PLAT_UI32 amount,
                                                  PLAT_UI16 offset,
                                                  PLAT_UI8 *p_data,
                                                  PLAT_UI8 data_length,
                                                  PLAT_UI32 *p_new_counter_value,
                                                  stse_cmd_protection_t protection)
{

  volatile stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECREMENT;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_data == NULL) || (p_new_counter_value == NULL) || (amount == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = p_stse->perso_info;
  ret = stsafea_switch_data_partition_access_protection(p_stse, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* STSE_CONF_USE_HOST_SESSION */

  /* Prepare Decrement Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [OPTION] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_option, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  /* [ZONE INDEX] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_zone_index, STSAFEA_ZONE_INDEX_SIZE, &zone_index);
  /* [OFFSET] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_offset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  /* [AMOUNT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_amount, STSAFEA_INC_DEC_AMOUT_SIZE, (PLAT_UI8 *)&amount);
  /* [DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_data, data_length, p_data);

  if (data_length >= stsafea_maximum_frame_length[p_stse->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /* Prepare Decrement RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [NEW COUNTER VALUE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_new_counter_val, STSAFEA_COUNTER_VALUE_SIZE,
                                   (PLAT_UI8 *)p_new_counter_value);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_amount);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_amount);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_new_counter_val);

#ifdef STSE_CONF_USE_HOST_SESSION
  p_stse->perso_info = perso_info_backup;
#endif /* STSE_CONF_USE_HOST_SESSION */

  return ret;
}

stse_return_code_t stsafea_read_counter_zone(stse_handle_t *p_stse,
                                             PLAT_UI32 zone_index,
                                             stsafea_read_option_t option,
                                             PLAT_UI16 offset,
                                             PLAT_UI8 *p_associated_data,
                                             PLAT_UI16 Associated_data_length,
                                             PLAT_UI32 *p_counter_value,
                                             stse_cmd_protection_t protection)
{

  volatile stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_counter_value == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = p_stse->perso_info;
  ret = stsafea_switch_data_partition_access_protection(p_stse, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* STSE_CONF_USE_HOST_SESSION */

  /* Prepare Read Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [OPTION] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_option, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  /* [ZONE INDEX] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_zone_index, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  /* [OFFSET] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_offset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  /* [LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_length, STSAFEA_ZONE_ACCESS_LENGTH_SIZE,
                                   (PLAT_UI8 *)&Associated_data_length);

  if (Associated_data_length >= stsafea_maximum_frame_length[p_stse->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /* Prepare Read RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [COUNTER VALUE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_counter_val, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)p_counter_value);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_associated_data, Associated_data_length, p_associated_data);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_length);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_counter_val);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_length);

#ifdef STSE_CONF_USE_HOST_SESSION
  p_stse->perso_info = perso_info_backup;
#endif /* STSE_CONF_USE_HOST_SESSION */

  return (ret);
}

stse_return_code_t stsafea_read_data_zone(stse_handle_t *p_stse,
                                          PLAT_UI32 zone_index,
                                          stsafea_read_option_t option,
                                          PLAT_UI16 offset,
                                          PLAT_UI8 *p_read_buffer,
                                          PLAT_UI16 read_length,
                                          stse_cmd_protection_t protection)
{
  volatile stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  /* If change access condition indicator is set to STSE_AC_CHANGE then we allow zero length
   * and no need of a read buffer */
  if (option.change_ac_indicator != STSE_AC_CHANGE && (p_read_buffer == NULL || read_length == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = p_stse->perso_info;
  ret = stsafea_switch_data_partition_access_protection(p_stse, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* STSE_CONF_USE_HOST_SESSION */

  /* Prepare Read Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [OPTION] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_option, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  /* [ZONE INDEX] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_zone_index, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  /* [OFFSET] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_offset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  /* [LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_length, STSAFEA_ZONE_ACCESS_LENGTH_SIZE, (PLAT_UI8 *)&read_length);

  if (read_length >= stsafea_maximum_frame_length[p_stse->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /* Prepare Read RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  if (read_length != 0)
  {
    /* [DATA] */
    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_data, read_length, (PLAT_UI8 *)p_read_buffer);
  }

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_length);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_length);

#ifdef STSE_CONF_USE_HOST_SESSION
  p_stse->perso_info = perso_info_backup;
#endif /* STSE_CONF_USE_HOST_SESSION */

  return ret;
}

stse_return_code_t stsafea_update_data_zone(stse_handle_t *p_stse,
                                            PLAT_UI32 zone_index,
                                            stsafea_update_option_t option,
                                            PLAT_UI16 offset,
                                            const PLAT_UI8 *p_data,
                                            PLAT_UI32 data_length,
                                            stse_cmd_protection_t protection)
{

  volatile stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_UPDATE;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_data == NULL) || (data_length == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = p_stse->perso_info;
  ret = stsafea_switch_data_partition_access_protection(p_stse, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* STSE_CONF_USE_HOST_SESSION */

  /* Prepare Update Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [OPTION] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_option, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  /* [ZONE INDEX] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_zone_index, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  /* [OFFSET] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_offset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  /* [DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_data, data_length, (PLAT_UI8 *)p_data);

  if (data_length >= stsafea_maximum_frame_length[p_stse->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /* Prepare Update RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_offset);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

#ifdef STSE_CONF_USE_HOST_SESSION
  p_stse->perso_info = perso_info_backup;
#endif /* STSE_CONF_USE_HOST_SESSION */

  return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
