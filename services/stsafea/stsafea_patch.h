/*!
 ******************************************************************************
 * \file	stsafea_patch.c
 * \brief   STSAFE-A services for patch (header)
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

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_patch STSAFE-A patch management
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 		STSAFEA Get patch update command counter service
 * \details 	This service format and send/receive the get patch update command/response
 * \param[in] 	p_stse 	Pointer to STSE Handler
 * \param[out] 	p_counter 	Patch update command counter
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_get_patch_update_command_counter(
    stse_handle_t *p_stse,
    PLAT_UI8 p_counter);

/**
 * \brief 		STSAFEA Start patch service
 * \details 	This service format and send/receive the patch start command/response
 * \param[in] 	p_stse 	Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_patch_start(
    stse_handle_t *p_stse);

/**
 * \brief 		STSAFEA Update Patch service
 * \details 	This service format and send/receive the patch update command/response
 * \param[in] 	p_stse 	Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_patch_update(
    stse_handle_t *p_stse);

/**
 * \brief 		STSAFEA Finalize Patch service
 * \details 	This service format and send/receive the patch finalize command/response
 * \param[in] 	p_stse 	Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_patch_finalize(
    stse_handle_t *p_stse);

/** \}*/

#endif /* STSAFE_PATCH_H */
