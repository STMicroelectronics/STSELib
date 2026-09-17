/**
  ******************************************************************************
  * @file    stsafel_low_power.c
  * @author  CS Application Team
  * @brief   STSAFE-L services for low-power modes services (source)
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

#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_echo.h"
#include "services/stsafel/stsafel_frame_transfer.h"
#include "services/stsafel/stsafel_low_power.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_ReturnCode_t stsafel_hibernate(stse_Handler_t *pSTSE)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_HIBERNATE;
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

stse_ReturnCode_t stsafel_wakeup(stse_Handler_t *pSTSE)
{
  stse_ReturnCode_t ret;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* When device wakeup callback is available, call it */
  if (pSTSE->io.BusWake != NULL)
  {
    ret = pSTSE->io.BusWake(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed);
  }
#ifdef STSE_CONF_USE_I2C
  /* When wakeup callback is not available but bus type is I2C, send a small echo command. */
  else if (pSTSE->io.BusType == STSE_BUS_TYPE_I2C)
  {
    PLAT_UI8 echo_message[1] = {0x00};
    ret = stsafel_echo(pSTSE, echo_message, echo_message, 1);
  }
#endif /* STSE_CONF_USE_I2C */
  /* If wakeup callback is not available and bus type is not I2C, return an error. */
  else
  {
    ret = STSE_SERVICE_INVALID_PARAMETER;
  }

  return ret;
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
