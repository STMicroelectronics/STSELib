/**
  ******************************************************************************
  * @file    stsafel_echo.c
  * @author  CS Application Team
  * @brief   STSAFE-L services for echo (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_return_code_t stsafel_echo(stse_handle_t *p_stse,
                                PLAT_UI8 *p_message,
                                PLAT_UI8 *p_echoed_message,
                                PLAT_UI16 message_length)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_ECHO;
  PLAT_UI8 rsp_header;

  /* Check stsafel handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_message == NULL) ||
      (p_echoed_message == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  if (message_length == 0)
  {
    return STSE_OK;
  }

  /* Prepare ECHO Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  /* [MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message, message_length, p_message);

  /* Prepare ECHO RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  /* [ECHOED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_echoed_message, message_length, p_echoed_message);

  return stsafel_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
