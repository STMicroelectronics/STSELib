/**
  ******************************************************************************
  * @file    stsafea_password.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for password (source)
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_password.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_verify_password(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pPassword_buffer,
  PLAT_UI8 password_length,
  PLAT_UI8 *pVerification_status,
  PLAT_UI8 *pRemaining_tries)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_PASSWORD;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((password_length != STSAFEA_PASSWORD_LENGTH))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePassword, password_length, pPassword_buffer);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eVerStat, 1, pVerification_status);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRemTri, 1, pRemaining_tries);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_delete_password(stse_Handler_t *pSTSE)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_DELETE;
  PLAT_UI8 tag = STSAFEA_DELETE_TAG_PASSWORD;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &tag);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
