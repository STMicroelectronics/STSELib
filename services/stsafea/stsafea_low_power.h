/*!
 ******************************************************************************
 * \file	stsafea_low_power.h
 * \brief   low-power modes services for STSAFE-A
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSAFEA_LOW_POWER_H
#define STSAFEA_LOW_POWER_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_low_power STSAFE-A low power management
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 		Put target device in hibernate mode
 * \details 	This function format and send hibernate command to target device
 * \param[in] 	pSTSAFE 			Pointer to STSE Handler
 * \param[in]	wake_up_mode 		Event to wake up from (STSAFE-A110 only)
 * 									listed
 * in enum \ref stse_hibernate_wake_up_mode_t \return \ref STSE_OK on success ;
 * \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_hibernate(stse_Handler_t *pSTSAFE,
				    stse_hibernate_wake_up_mode_t wake_up_mode);

/** \}*/

#endif /*STSAFEA_LOW_POWER_H*/
