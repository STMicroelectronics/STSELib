/*!
 ******************************************************************************
 * \file	stsafel_low_power.h
 * \brief   STSAFE-L services for low-power modes services (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
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

/*! \defgroup stsafel_low_power STSAFE-L Low power
 *  \ingroup stsafel_services
 *  @{
 */

/**
 * \brief 		Send hibernate command to target device
 * \details 	This service format and send the hibernate command
 * \param[in] 	p_stse 				Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafel_hibernate(stse_handle_t *p_stse);

/**
 * \brief 		Send wakeup command to target device
 * \details 	This service format and send the wakeup command
 * \param[in] 	p_stse 				Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafel_wakeup(stse_handle_t *p_stse);

/** \}*/

#endif /* STSAFEL_LOW_POWER_H */
