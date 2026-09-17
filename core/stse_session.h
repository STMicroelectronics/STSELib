/**
  ******************************************************************************
  * @file    stse_session.h
  * @author  CS Application Team
  * @brief   STSELib api for session manager (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSE_SESSION_H
#define STSE_SESSION_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_generic_typedef.h"
#include "core/stse_return_codes.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/*!
 * \brief     This Core function Erase the session context from STSAFE handler
 * \param[in]   *pSTSE  Pointer to target STSAFE handler
 * \return \ref stsafe_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details   \include{doc} stsafe_erase_context.dox
 */
void stse_session_erase_context(stse_session_t *pSession);
#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */


#endif /* STSE_SESSION_H */
