/*!
 ******************************************************************************
 * \file	stsafel_reset.h
 * \brief   Reset service for STSAFE-L
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

#ifndef STSAFEL_RESET_H
#define STSAFEL_RESET_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"


/*! \defgroup stsafel_reset STSAFE-L Reset
 *  \ingroup stsafel_services
 *  @{
 */

/**
 * \brief 		Send reset command to target device
 * \details 	This service format and send the reset command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_reset(stse_Handler_t * pSTSE);

/** \}*/

#endif /* STSAFEL_RESET_H */
