/*!
 ******************************************************************************
 * \file	stsafel_echo.h
 * \brief   Echo services for STSAFE-L
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

#ifndef STSAFEL_ECHO_H
#define STSAFEL_ECHO_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "stse_platform_generic.h"
#include "services/stsafel/stsafel_timings.h"

/*! \defgroup stsafel_echo STSAFE-L Echo
 *  \ingroup stsafel_services
 *  @{
 */

/**
 * \brief 		Send an echo to target device
 * \details 	This service format and send the echo command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in]	message				Message buffer to send the echo command
 * \param[in]	echoed_message		Message buffer to receive the echo response
 * \param[in]	message_size		Size of the echo message
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_echo(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *message,
    PLAT_UI8 *echoed_message,
    PLAT_UI16 message_size);

/** \}*/

#endif /* STSAFEL_ECHO_H */
