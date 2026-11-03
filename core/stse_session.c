/**
  ******************************************************************************
  * @file    stse_session.c
  * @author  CS Application Team
  * @brief   STSELib api for session manager (source)
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

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"

/* Public functions ----------------------------------------------------------*/
void stse_session_erase_context(stse_session_t *p_session)
{
  /* - Check stsafe handler initialization */
  if (p_session == NULL)
  {
    return;
  }

  /*Todo validate correct erase */
  memset(p_session, 0x00, sizeof(stse_session_t));
}

stse_return_code_t stse_set_active_session(stse_handle_t *p_stse, stse_session_t *p_session)
{
  if (p_stse == NULL)
  {
    return STSE_CORE_HANDLER_NOT_INITIALISED;
  }

  if (p_session == NULL)
  {
    return STSE_CORE_SESSION_ERROR;
  }

  p_stse->p_active_host_session = p_session;

  return (STSE_OK);
}
