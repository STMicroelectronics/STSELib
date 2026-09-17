/**
  ******************************************************************************
  * @file    stsafel_data_partition.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for data partition (source)
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

#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_data_partition.h"
#include "services/stsafel/stsafel_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_ReturnCode_t stsafel_read_data_zone(stse_Handler_t *pSTSE,
                                         PLAT_UI8 zone_index,
                                         stsafel_read_option_t option,
                                         PLAT_UI16 offset,
                                         PLAT_UI8 *pData,
                                         PLAT_UI16 data_length,
                                         stse_cmd_protection_t protection)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_READ;
  PLAT_UI8 rsp_header;

  /* READ returns associated data bytes from the selected zone and offset. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pData == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eRead_option, sizeof(stsafel_read_option_t), (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZone_index, 1, &zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, 2, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData_length, 2, (PLAT_UI8 *)&data_length);

  /*- Create Rsp frame and populate elements */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eData, data_length, pData);

  /* Multi-byte protocol fields are serialized in big-endian on the wire. */
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eData_length);

  /*- Perform Transfer*/
  return stsafel_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafel_update_data_zone(stse_Handler_t *pSTSE,
                                           PLAT_UI8 zone_index,
                                           stsafel_update_option_t option,
                                           PLAT_UI16 offset,
                                           PLAT_UI8 *pData,
                                           PLAT_UI16 data_length,
                                           stse_cmd_protection_t protection)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_UPDATE;
  PLAT_UI8 rsp_header;

  /* UPDATE writes caller-provided bytes at offset; response contains status only. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pData == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eUpdate_option, sizeof(stsafel_update_option_t), (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZone_index, 1, &zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, 2, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData, data_length, pData);

  /*- Create Rsp frame and populate elements */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);

  /* Offset is transmitted MSB-first per STSAFE-L frame encoding rules. */
  stse_frame_element_swap_byte_order(&eOffset);

  /* Update command has no data payload in the response frame, only status. */
  return stsafel_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafel_read_counter_zone(stse_Handler_t *pSTSE,
                                            PLAT_UI8 zone_index,
                                            stsafel_read_option_t option,
                                            PLAT_UI16 offset,
                                            PLAT_UI8 *pData,
                                            PLAT_UI16 data_length,
                                            PLAT_UI32 *pCounter_value,
                                            stse_cmd_protection_t protection)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEL_CMD_READ;
  PLAT_UI8 rsp_header;
  PLAT_UI8 temp_counter[STSAFEL_COUNTER_VALUE_SIZE];

  /* Counter-zone READ returns both counter snapshot and optional associated data. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pData == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eRead_option, sizeof(stsafel_read_option_t), (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZone_index, 1, &zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, 2, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData_length, 2, (PLAT_UI8 *)&data_length);

  /*- Create Rsp frame and populate elements */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounter, STSAFEL_COUNTER_VALUE_SIZE, temp_counter);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eData, data_length, pData);

  /* Request parameters using 16-bit integers are converted to wire byte order. */
  stse_frame_element_swap_byte_order(&eOffset);
  stse_frame_element_swap_byte_order(&eData_length);

  /*- Perform Transfer*/
  ret = stsafel_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (ret == STSE_OK)
  {
    /* Counter value is returned as 24-bit big-endian and expanded to 32-bit host type. */
    *pCounter_value = ((temp_counter[2]) | (temp_counter[1] << 8) | (temp_counter[0] << 16));
  }

  return (ret);
}

stse_ReturnCode_t stsafel_decrement_counter_zone(stse_Handler_t *pSTSE,
                                                 PLAT_UI8 zone_index,
                                                 stsafel_decrement_option_t option,
                                                 PLAT_UI32 amount,
                                                 PLAT_UI16 offset,
                                                 PLAT_UI8 *pData,
                                                 PLAT_UI16 data_length,
                                                 PLAT_UI32 *pNew_counter_value,
                                                 stse_cmd_protection_t protection)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEL_CMD_DECREMENT;
  PLAT_UI8 rsp_header;
  PLAT_UI8 decrement_amount[STSAFEL_COUNTER_VALUE_SIZE] =
  {
    ((amount & 0xFF0000) >> 16),
    ((amount & 0xFF00) >> 8),
    (amount & 0xFF)
  };
  PLAT_UI8 temp_counter[STSAFEL_COUNTER_VALUE_SIZE];

  /* Decrement amount is encoded as a 24-bit value to match STSAFE-L counter width. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pData == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eDecrement_option,
                                   sizeof(stsafel_decrement_option_t),
                                   (PLAT_UI8 *)&option);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eZone_index, 1, &zone_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eOffset, 2, (PLAT_UI8 *)&offset);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAmount, STSAFEL_COUNTER_VALUE_SIZE, decrement_amount);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eData, data_length, pData);

  /*- Create Rsp frame and populate elements */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounter, STSAFEL_COUNTER_VALUE_SIZE, temp_counter);

  /* Only offset requires endian swap in decrement request; amount is pre-built as bytes. */
  stse_frame_element_swap_byte_order(&eOffset);

  /*- Perform Transfer*/
  ret = stsafel_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (ret == STSE_OK)
  {
    /* Response counter carries post-decrement value using the same 24-bit encoding. */
    *pNew_counter_value = ((temp_counter[2]) | (temp_counter[1] << 8) | (temp_counter[0] << 16));
  }

  return (ret);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
