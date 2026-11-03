/**
  ******************************************************************************
  * @file    stsafea_reset.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for reset (header)
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

#ifndef STSAFEA_RESET_H
#define STSAFEA_RESET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_reset STSAFE-A Reset
 *  \ingroup stsafea_services
 *  @{
 */

/**
  * \brief     Reset STSAFE-A device
  * \details   This service format and send the reset command
  * \param[in]   p_stsafe  Pointer to STSE Handler
  * \return    \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_reset(stse_handle_t *p_stsafe);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFEA_RESET_H */
