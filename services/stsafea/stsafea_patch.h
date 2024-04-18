/*!
 ******************************************************************************
 * \file	stsafea_patch.c
 * \brief   Patch services for STSAFE-A
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

#ifndef STSAFE_PATCH_H
#define STSAFE_PATCH_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_patch Patch management
 *  \ingroup stsafea_services
 *  @{
 */


/**
 * \brief 		STSAFEA Get patch update command counter service
 * \details 	This service format and send/receive the get patch update command/response
 * \param[in] 	pSTSE 	Pointer to STSAFE Handler
 * \param[out] 	pCounter 	Patch update command counter
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_get_patch_update_command_counter(
		stse_Handler_t * pSTSE,
		PLAT_UI8 pCounter
);

/**
 * \brief 		STSAFEA Start patch service
 * \details 	This service format and send/receive the patch start command/response
 * \param[in] 	pSTSE 	Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_patch_start(
		stse_Handler_t * pSTSE
);

/**
 * \brief 		STSAFEA Update Patch service
 * \details 	This service format and send/receive the patch update command/response
 * \param[in] 	pSTSE 	Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_patch_update(
		stse_Handler_t * pSTSE
);

/**
 * \brief 		STSAFEA Finalize Patch service
 * \details 	This service format and send/receive the patch finalize command/response
 * \param[in] 	pSTSE 	Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_patch_finalize(
		stse_Handler_t * pSTSE
);

/** \}*/


#endif /* STSAFE_PATCH_H */
