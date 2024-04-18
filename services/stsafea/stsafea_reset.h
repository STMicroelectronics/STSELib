/*!
 ******************************************************************************
 * \file	stsafea_random.h
 * \brief   Random services for STSAFE
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

#ifndef STSAFEA_RESET_H
#define STSAFEA_RESET_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_reset Reset
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 		Reset target device
 * \details 	This service format and send the reset command
 * \param[in] 	pSTSAFE 	Pointer to STSAFE Handler
 * \return 		\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_reset( stse_Handler_t * pSTSAFE );

/** \}*/

#endif /* STSAFEA_RESET_H */
