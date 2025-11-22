/*!
 ******************************************************************************
 * \file	stse_session.h
 * \brief   STSE session manager (header)
 * \author  STMicroelectronics - SMD application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_SESSION_MANAGER_H
#define STSE_SESSION_MANAGER_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_generic_typedef.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"

/*!
 * \brief 		This Core function Create a session context and associate it to STSAFE handler
 * \param[in] 	*pSession 			\ref stse_session_t Pointer to session
 * \param[in] 	*pHost_MAC_key 		Pointer to MAC key buffer to be used under the session
 * \param[in] 	*pHost_cypher_key 	Pointer to cypher key buffer to be used under the session
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stsafe_erase_context.dox
 */
stse_ReturnCode_t stsafea_open_host_session(stse_Handler_t *pSTSE,
                                            stse_session_t *pSession,
                                            PLAT_UI8 *pHost_MAC_key,
                                            PLAT_UI8 *pHost_cypher_key);

/*!
 * \brief 		This Core function Close an existing host session context
 * \param[in] 	*pSession 			\ref stse_session_t Pointer to session
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
void stsafea_close_host_session(stse_session_t *pSession);

/*!
 * \brief 		This Core function Erase the session context from STSAFE handler
 * \param[in] 	*pSTSE 	Pointer to target STSAFE handler
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stsafe_erase_context.dox
 */
void stsafea_session_clear_context(stse_session_t *pSession);

/**
 * \brief 		Perform encrypted session transfer
 * \details 	This service performs a command/response transfer with encryption
 * \param[in] 	pSession				Pointer to session structure
 * \param[in] 	pCmdFrame				Pointer to command frame
 * \param[out] 	pRspFrame				Pointer to response frame
 * \param[in] 	cmd_encryption_flag		Command encryption flag
 * \param[in] 	rsp_encryption_flag		Response encryption flag
 * \param[in] 	cmd_ac_info				Command access conditions
 * \param[in] 	processing_time			Expected processing time
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_session_encrypted_transfer(stse_session_t *pSession,
                                                     stse_frame_t *pCmdFrame,
                                                     stse_frame_t *pRspFrame,
                                                     PLAT_UI8 cmd_encryption_flag,
                                                     PLAT_UI8 rsp_encryption_flag,
                                                     stse_cmd_access_conditions_t cmd_ac_info,
                                                     PLAT_UI16 processing_time);

/**
 * \brief 		Perform authenticated session transfer
 * \details 	This service performs a command/response transfer with authentication
 * \param[in] 	pSession			Pointer to session structure
 * \param[in] 	pCmdFrame			Pointer to command frame
 * \param[out] 	pRspFrame			Pointer to response frame
 * \param[in] 	cmd_ac_info			Command access conditions
 * \param[in] 	processing_time		Expected processing time
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_session_authenticated_transfer(stse_session_t *pSession,
                                                         stse_frame_t *pCmdFrame,
                                                         stse_frame_t *pRspFrame,
                                                         stse_cmd_access_conditions_t cmd_ac_info,
                                                         PLAT_UI16 processing_time);

#endif /* STSE_SESSION_MANAGER_H */
