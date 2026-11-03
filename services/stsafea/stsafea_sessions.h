/**
  ******************************************************************************
  * @file    stsafea_sessions.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for sessions (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSE_SESSION_MANAGER_H
#define STSE_SESSION_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_generic_typedef.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"

/*!
 * \brief       This Core function creates a session context and associates it to STSAFE handler
 *              using keys stored in platform secure storage
 * \param[in]   *p_stse                  Pointer to STSE handler
 * \param[in]   *p_session               \ref stse_session_t Pointer to session
 * \param[in]   host_mac_key_idx        Index of the MAC key in platform secure storage to be used under the session
 * \param[in]   host_cipher_key_idx     Index of the cipher key in platform secure storage to be used under the session
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 * \details     \include{doc} stsafe_erase_context.dox
 * \details     \include{doc} stse_platform_store_aes_key.dox
 */
stse_return_code_t stsafea_open_host_session_from_idx(stse_handle_t *p_stse,
                                                      stse_session_t *p_session,
                                                      PLAT_UI32 host_mac_key_idx,
                                                      PLAT_UI32 host_cipher_key_idx);

/*!
 * \brief     This Core function Close an existing host session context
 * \param[in]   *p_session      \ref stse_session_t Pointer to session
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
void stsafea_close_host_session(stse_session_t *p_session);

/*!
 * \brief     This Core function Erase the session context from STSAFE handler
 * \param[in]   *p_stse   Pointer to target STSAFE handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 * \details   \include{doc} stsafe_erase_context.dox
 */
void stsafea_session_clear_context(stse_session_t *p_session);

/**
  * \brief     Perform encrypted session transfer
  * \details   This service performs a command/response transfer with encryption
  * \param[in]   p_session       Pointer to session structure
  * \param[in]   p_cmd_frame       Pointer to command frame
  * \param[out]  p_rsp_frame       Pointer to response frame
  * \param[in]   cmd_encryption_flag   Command encryption flag
  * \param[in]   rsp_encryption_flag   Response encryption flag
  * \param[in]   cmd_ac_info       Command access conditions
  * \param[in]   processing_time     Expected processing time
  * \return    \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_session_encrypted_transfer(stse_session_t *p_session,
                                                      stse_frame_t *p_cmd_frame,
                                                      stse_frame_t *p_rsp_frame,
                                                      PLAT_UI8 cmd_encryption_flag,
                                                      PLAT_UI8 rsp_encryption_flag,
                                                      stse_cmd_access_conditions_t cmd_ac_info,
                                                      PLAT_UI16 processing_time);

/**
  * \brief     Perform authenticated session transfer
  * \details   This service performs a command/response transfer with authentication
  * \param[in]   p_session     Pointer to session structure
  * \param[in]   p_cmd_frame     Pointer to command frame
  * \param[out]  p_rsp_frame     Pointer to response frame
  * \param[in]   cmd_ac_info     Command access conditions
  * \param[in]   processing_time   Expected processing time
  * \return    \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_session_authenticated_transfer(stse_session_t *p_session,
                                                          stse_frame_t *p_cmd_frame,
                                                          stse_frame_t *p_rsp_frame,
                                                          stse_cmd_access_conditions_t cmd_ac_info,
                                                          PLAT_UI16 processing_time);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_SESSION_MANAGER_H */
