/*!
 ******************************************************************************
 * \file	stsafea_password.h
 * \brief   password services for STSAFE-A
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

#ifndef STSAFEA_PASSWORD_H
#define STSAFEA_PASSWORD_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


#define STSAFEA_PASSWORD_LENGTH				16U
#define STSAFEA_DELETE_TAG_PASSWORD			0x09

/*! \defgroup stsafea_password Password management
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 			STSAFEA verify password service
 * \details 		This service format and send/receive the generate random command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \param[in]	 	pPassword_buffer 		Pointer to password buffer
 * \param[in]	 	password_length 		Password length in bytes
 * \param[out]	 	pVerification_status 	Pointer to verification status
 * \param[out]	 	pRemaining_tries 		Pointer to remaining tries
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_verify_password(stse_Handler_t *pSTSE,
			PLAT_UI8   *pPassword_buffer,
			PLAT_UI8 	password_length,
			PLAT_UI8   *pVerification_status,
			PLAT_UI8   *pRemaining_tries);

/**
 * \brief 			STSAFEA delete password service
 * \details 		This service format and send/receive the generate random command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_delete_password(stse_Handler_t * pSTSE);

/** \}*/

#endif /*STSAFEA_PASSWORD_H*/
