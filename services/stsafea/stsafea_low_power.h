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
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSAFEA_LOW_POWER_H
#define STSAFEA_LOW_POWER_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_low_power Low power management
 *  \ingroup stsafea_services
 *  @{
 */

typedef enum {
    STSAFEA_HIBERNATE_WAKEUP_I2C_OR_RESET    = 0x01, /*!< Wake up from hibernate after I2C start condition or after reset. */
	STSAFEA_HIBERNATE_WAKEUP_RESET_ONLY 	 = 0x02  /*!< Wake up from hibernate after reset only. */
} stsafea_hibernate_wake_up_mode_t;


/**
 * \brief 		Put target device in hibernate mode
 * \details 	This function format and send hibernate command to target device
 * \param[in] 	pSTSAFE 			Pointer to STSAFE Handler
 * \param[in]	wake_up_mode 		Event to wake up from,
 * 									listed in enum \ref stsafea_hibernate_wake_up_mode_t
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_hibernate(stse_Handler_t *pSTSAFE,
									 stsafea_hibernate_wake_up_mode_t wake_up_mode);

/** \}*/

#endif /*STSAFEA_LOW_POWER_H*/
