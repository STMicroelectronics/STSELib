/**
  ******************************************************************************
  * @file    stsafea_low_power.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for low-power modes services (source)
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
#include "services/stsafea/stsafea_low_power.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/**
  * \brief     Put STSAFE-A device in hibernate mode
  * \details   This function call hibernate service to put the device in hibernate
  * \param[in]   p_stse        Pointer to STSE Handler
  * \param[in] wake_up_mode    Event to wake up from (only significant with STSAFE-A devices),
  *                  listed in type \ref stse_hibernate_wake_up_mode_t
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_hibernate(stse_handle_t *p_stse,
                                     stse_hibernate_wake_up_mode_t wake_up_mode)

{
  PLAT_UI8 cmd_header = STSAFEA_CMD_HIBERNATE;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Prepare HIBERNATE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [WAKE UP MODE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_wake_up_mode, 1, (PLAT_UI8 *)&wake_up_mode);

  /* Prepare HIBERNATE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);

  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
