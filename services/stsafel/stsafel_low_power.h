/**
  ******************************************************************************
  * @file    stsafel_low_power.h
  * @author  CS Application Team
  * @brief   STSAFE-L services for low-power modes services (header)
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

#ifndef STSAFEL_LOW_POWER_H
#define STSAFEL_LOW_POWER_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafel/stsafel_timings.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/*! \defgroup stsafel_low_power STSAFE-L Low power
 *  \ingroup stsafel_services
 *  @{
 */

/**
  * \brief     Send hibernate command to target device
  * \details   This service format and send the hibernate command
  * \param[in]   pSTSE         Pointer to STSE Handler
  * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
  */
stse_ReturnCode_t stsafel_hibernate(stse_Handler_t *pSTSE);

/**
  * \brief     Send wakeup command to target device
  * \details   This service format and send the wakeup command
  * \param[in]   pSTSE         Pointer to STSE Handler
  * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
  */
stse_ReturnCode_t stsafel_wakeup(stse_Handler_t *pSTSE);

/** \}*/
#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */


#endif /* STSAFEL_LOW_POWER_H */
