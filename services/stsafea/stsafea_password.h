/**
  ******************************************************************************
  * @file    stsafea_password.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for password (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSAFEA_PASSWORD_H
#define STSAFEA_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"

#define STSAFEA_PASSWORD_LENGTH 16U
#define STSAFEA_DELETE_TAG_PASSWORD 0x09

/*! \defgroup stsafea_password STSAFE-A Password management
 *  \ingroup stsafea_services
 *  @{
 */

/**
  * \brief       STSAFEA verify password service
  * \details     This service format and send/receive the generate random command/response
  * \param[in]   p_stse          Pointer to target SE handler
  * \param[in]   p_password_buffer     Pointer to password buffer
  * \param[in]   password_length     Password length in bytes
  * \param[out]    p_verification_status   Pointer to verification status
  * \param[out]    p_remaining_tries     Pointer to remaining tries
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_verify_password(stse_handle_t *p_stse,
                                           PLAT_UI8 *p_password_buffer,
                                           PLAT_UI8 password_length,
                                           PLAT_UI8 *p_verification_status,
                                           PLAT_UI8 *p_remaining_tries);

/**
  * \brief       STSAFEA delete password service
  * \details     This service format and send/receive the generate random command/response
  * \param[in]   p_stse          Pointer to target SE handler
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_delete_password(stse_handle_t *p_stse);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*STSAFEA_PASSWORD_H*/
