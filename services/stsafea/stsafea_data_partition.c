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

stse_ReturnCode_t stsafea_switch_data_partition_access_protection(stse_Handler_t *pSTSE, PLAT_UI8 command_code,
                                                                  stse_cmd_protection_t protection)
{
  /* Data-partition services support only plaintext or host MAC protection
   * policy at this middleware layer; wrapping modes are rejected here. */
  switch (protection)
  {

    case STSE_HOST_C_MAC_R_MAC:
      stsafea_perso_info_set_cmd_AC(&pSTSE->perso_info, command_code, STSE_CMD_AC_HOST);
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

stse_ReturnCode_t stsafea_get_total_partition_count(stse_Handler_t *pSTSE,
                                                    PLAT_UI8 *pTotal_partition_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* QUERY over data-partition configuration returns total number of ZIR entries. */

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &tag);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eTotal_partition_count, 1, pTotal_partition_count);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_get_data_partitions_configuration(stse_Handler_t *pSTSE,
                                                            PLAT_UI8 total_partitions_count,
                                                            stsafea_data_partition_record_t *pRecord_table,
                                                            PLAT_UI16 record_table_size)
{
  stse_ReturnCode_t ret;
  volatile PLAT_UI8 partition_idx;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
  PLAT_UI8 rsp_header;
  PLAT_UI8 raw_data[record_table_size];

  /* Raw configuration payload is parsed manually because each zone can carry
   * optional counter bytes depending on its declared zone type. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &tag);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRaw, record_table_size, raw_data);

  /*- Perform Transfer*/
  ret = stsafea_frame_raw_transfer(pSTSE,
                                   &CmdFrame,
                                   &RspFrame,
                                   stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

  /* - Verify transfer result and build Partition record table */
  if (ret == STSE_OK)
  {
    PLAT_UI16 i = 1;
    /* Byte 0 of the raw payload is record-count metadata; parsing starts at 1. */
    for (partition_idx = 0; partition_idx < total_partitions_count; partition_idx++)
    {
      /* Access-condition nibbles are unpacked through dedicated helper macros
       * to keep bit-position handling centralized in one place. */
      pRecord_table->index = raw_data[i++];
      pRecord_table->zone_type = raw_data[i++];
      pRecord_table->read_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_READ_CR_GET(raw_data[i]);
      pRecord_table->read_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_READ_GET(raw_data[i]);
      pRecord_table->update_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_UPDATE_CR_GET(raw_data[i]);
      pRecord_table->update_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_UPDATE_GET(raw_data[i++]);
      pRecord_table->data_segment_length = UI16_B1_SET(raw_data[i++]);
      pRecord_table->data_segment_length += UI16_B0_SET(raw_data[i++]);
      if (pRecord_table->zone_type == 1)
      {
        /* Counter zones append a 32-bit counter value in big-endian order. */
        pRecord_table->counter_value = UI32_B3_SET(raw_data[i++]);
        pRecord_table->counter_value += UI32_B2_SET(raw_data[i++]);
        pRecord_table->counter_value += UI32_B1_SET(raw_data[i++]);
        pRecord_table->counter_value += UI32_B0_SET(raw_data[i++]);
      }
      else
      {
        /* Simple data zones do not carry counter state in configuration records. */
        pRecord_table->counter_value = 0;
      }
      pRecord_table++;
    }
  }

  return (ret);
}

stse_ReturnCode_t stsafea_decrement_counter_zone(stse_Handler_t *pSTSE,
                                                 PLAT_UI8 zone_index,
                                                 stsafea_decrement_option_t option,
                                                 PLAT_UI32 amount,
                                                 PLAT_UI16 offset,
                                                 PLAT_UI8 *pData,
                                                 PLAT_UI8 data_length,
                                                 PLAT_UI32 *pNew_counter_value,
                                                 stse_cmd_protection_t protection)
{

  volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECREMENT;
  PLAT_UI8 rsp_header;

  /* Decrement operation can atomically update counter and associated data. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if ((pData == NULL) || (pNew_counter_value == NULL) || (amount == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = pSTSE->perso_info;
  /* Temporarily override command access policy according to requested protection. */
  ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, &zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAmount, STSAFEA_INC_DEC_AMOUT_SIZE, (PLAT_UI8 *)&amount);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData, data_length, pData);

  /* Frame-size guard prevents oversized payload before transfer function. */
  if (data_length >= stsafea_maximum_frame_length[pSTSE->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame,
                                   eNewCounterVal,
                                   STSAFEA_COUNTER_VALUE_SIZE,
                                   (PLAT_UI8 *)pNew_counter_value);

  /*- Swap Elements byte order before sending*/
  /* Numeric fields are serialized big-endian for transport. */
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eAmount);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  /*- unSwap Elements bytes from Command frame*/
  /* Restore host-order locals to avoid side effects on caller-visible variables. */
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eAmount);

  /*- Swap New Counter value byte order before sending*/
  stse_frame_element_swap_byte_order(&eNewCounterVal);

#ifdef STSE_CONF_USE_HOST_SESSION
  pSTSE->perso_info = perso_info_backup;
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  return ret;
}

stse_ReturnCode_t stsafea_read_counter_zone(stse_Handler_t *pSTSE,
                                            PLAT_UI32 zone_index,
                                            stsafea_read_option_t option,
                                            PLAT_UI16 offset,
                                            PLAT_UI8 *pAssociated_data,
                                            PLAT_UI16 Associated_data_length,
                                            PLAT_UI32 *pCounter_value,
                                            stse_cmd_protection_t protection)
{

  volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if ((pCounter_value == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Counter read can optionally return a trailing associated-data segment. */

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = pSTSE->perso_info;
  ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eLength, STSAFEA_ZONE_ACCESS_LENGTH_SIZE,
                                   (PLAT_UI8 *)&Associated_data_length);

  /* Requested associated-data length must fit transport frame capabilities. */
  if (Associated_data_length >= stsafea_maximum_frame_length[pSTSE->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounterVal, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)pCounter_value);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eAssociatedData, Associated_data_length, pAssociated_data);

  /*- Swap Elements bytes from Command frame*/
  /* Command integer fields are encoded big-endian on the wire. */
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eLength);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  /*- Swap Counter value*/
  /* Returned counter bytes are converted back to host endianness. */
  stse_frame_element_swap_byte_order(&eCounterVal);

  /*- Un-Swap Elements bytes from Command frame*/
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eLength);

#ifdef STSE_CONF_USE_HOST_SESSION
  pSTSE->perso_info = perso_info_backup;
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  return (ret);
}

stse_ReturnCode_t stsafea_read_data_zone(stse_Handler_t *pSTSE,
                                         PLAT_UI32 zone_index,
                                         stsafea_read_option_t option,
                                         PLAT_UI16 offset,
                                         PLAT_UI8 *pReadBuffer,
                                         PLAT_UI16 read_length,
                                         stse_cmd_protection_t protection)
{
  volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* If change access condition indicator is set to STSE_AC_CHANGE, allow zero length and no read buffer. */
  if (option.change_ac_indicator != STSE_AC_CHANGE && (pReadBuffer == NULL || read_length == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Special case: AC-change request allows zero data length and null read buffer. */

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = pSTSE->perso_info;
  ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eLength, STSAFEA_ZONE_ACCESS_LENGTH_SIZE, (PLAT_UI8 *)&read_length);

  /* Ensure requested read payload can be transported in one device frame. */
  if (read_length >= stsafea_maximum_frame_length[pSTSE->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  if (read_length != 0)
  {
    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eData, read_length, (PLAT_UI8 *)pReadBuffer);
  }

  /*- Swap Elements byte order before sending*/
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eLength);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  /*- UnSwap Elements bytes from Command frame*/
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eLength);

#ifdef STSE_CONF_USE_HOST_SESSION
  pSTSE->perso_info = perso_info_backup;
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  return ret;
}

stse_ReturnCode_t stsafea_update_data_zone(stse_Handler_t *pSTSE,
                                           PLAT_UI32 zone_index,
                                           stsafea_update_option_t option,
                                           PLAT_UI16 offset,
                                           PLAT_UI8 *pData,
                                           PLAT_UI32 data_length,
                                           stse_cmd_protection_t protection)
{

  volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 cmd_header = STSAFEA_CMD_UPDATE;
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if ((pData == NULL) || (data_length == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Update operation always requires a concrete payload buffer and length. */

#ifdef STSE_CONF_USE_HOST_SESSION
  stse_perso_info_t perso_info_backup = pSTSE->perso_info;
  ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
  if (ret != STSE_OK)
  {
    return ret;
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData, data_length, pData);

  /* Guard against command payload larger than device frame capacity. */
  if (data_length >= stsafea_maximum_frame_length[pSTSE->device_type])
  {
    return STSE_SERVICE_FRAME_SIZE_ERROR;
  }

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  /*- Swap Elements byte order before sending*/
  stse_frame_element_swap_byte_order(&eOffset);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

#ifdef STSE_CONF_USE_HOST_SESSION
  pSTSE->perso_info = perso_info_backup;
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */

  return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
