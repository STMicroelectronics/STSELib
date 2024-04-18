/*!
 ******************************************************************************
 * \file	stsafea_echo.h
 * \brief   Echo services for STSAFE-A
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

#ifndef STSAFEA_ECHO_H
#define STSAFEA_ECHO_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_echo Echo
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 		Send an echo to target device
 * \details 	This service format and send the echo command
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in]	message				Message buffer to send the echo command
 * \param[in]	echoed_message		Message buffer to receive the echo response
 * \param[in]	message_size		Size of the echo message
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_echo(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * message,
		PLAT_UI8 * echoed_message,
		PLAT_UI16 message_size
);

/** \}*/

#endif /*STSAFEA_ECHO_H*/
