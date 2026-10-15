/**
  ******************************************************************************
  * @file    stsafel_reset.h
  * @author  CS Application Team
  * @brief   STSAFE-L services for reset (header)
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

#ifndef STSAFEL_RESET_H
#define STSAFEL_RESET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafel/stsafel_timings.h"

/*! \defgroup stsafel_reset STSAFE-L Reset
 *  \ingroup stsafel_services
 *  @{
 */

/**
  * \brief     Reset STSAFE-L device
  * \details   This service format and send the reset command
  * \param[in]   p_stse        Pointer to STSE Handler
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_reset(stse_handle_t *p_stse);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEL_RESET_H */
