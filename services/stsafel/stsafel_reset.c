/**
  ******************************************************************************
  * @file    stsafel_reset.c
  * @author  CS Application Team
  * @brief   STSAFE-L services for reset (source)
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
#include "services/stsafel/stsafel_frame_transfer.h"
#include "services/stsafel/stsafel_reset.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_ReturnCode_t stsafel_reset(stse_Handler_t *pSTSE)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_RESET;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);

  /*- Perform Transfer*/
  return stsafel_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
