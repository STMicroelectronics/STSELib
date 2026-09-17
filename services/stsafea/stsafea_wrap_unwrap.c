/**
  ******************************************************************************
  * @file    stsafea_wrap_unwrap.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for wrap & unwrap (source)
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
#include "services/stsafea/stsafea_wrap_unwrap.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_WRAP_MAX_PLAINTEXT_PAYLOAD_SIZE 480U
#define STSAFEA_WRAP_OVERHEAD_SIZE 8U
#define STSAFEA_WRAP_MAX_WRAPPED_PAYLOAD_SIZE (STSAFEA_WRAP_MAX_PLAINTEXT_PAYLOAD_SIZE + STSAFEA_WRAP_OVERHEAD_SIZE)

stse_ReturnCode_t stsafea_wrap_payload(stse_Handler_t *pSTSE,
                                       PLAT_UI8 wrap_key_slot,
                                       PLAT_UI8 *pPayload,
                                       PLAT_UI16 payload_size,
                                       PLAT_UI8 *pWrapped_Payload,
                                       PLAT_UI16 wrapped_payload_size)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_WRAP_LOCAL_ENVELOPE;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pPayload == NULL) || (pWrapped_Payload == NULL)
      || (payload_size > STSAFEA_WRAP_MAX_PLAINTEXT_PAYLOAD_SIZE) || (payload_size == 0)
      || (wrapped_payload_size != (payload_size + STSAFEA_WRAP_OVERHEAD_SIZE)))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &wrap_key_slot);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePayload, payload_size, pPayload);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eWrapped, wrapped_payload_size, pWrapped_Payload);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_unwrap_payload(stse_Handler_t *pSTSE,
                                         PLAT_UI8 wrap_key_slot,
                                         PLAT_UI8 *pWrapped_Payload,
                                         PLAT_UI16 wrapped_payload_size,
                                         PLAT_UI8 *pPayload,
                                         PLAT_UI16 payload_size)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pPayload == NULL) || (pWrapped_Payload == NULL)
      || (wrapped_payload_size > STSAFEA_WRAP_MAX_WRAPPED_PAYLOAD_SIZE)
      || (wrapped_payload_size < STSAFEA_WRAP_OVERHEAD_SIZE)
      || (wrapped_payload_size != (payload_size + STSAFEA_WRAP_OVERHEAD_SIZE)))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &wrap_key_slot);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePayload, wrapped_payload_size, pWrapped_Payload);

  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eWrapped, payload_size, pPayload);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
