/**
  ******************************************************************************
  * @file    stsafea_low_power.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for low-power modes services (source)
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
#include "services/stsafea/stsafea_low_power.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_hibernate(stse_Handler_t *pSTSE,
                                    stse_hibernate_wake_up_mode_t wake_up_mode)

{
  PLAT_UI8 cmd_header = STSAFEA_CMD_HIBERNATE;
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, e_wake_up_mode, 1, (PLAT_UI8 *)&wake_up_mode);

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
