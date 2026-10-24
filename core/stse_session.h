/**
  ******************************************************************************
  * @file    stse_session.h
  * @author  CS Application Team
  * @brief   STSELib api for session manager (header)
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

#ifndef STSE_SESSION_H
#define STSE_SESSION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_generic_typedef.h"
#include "core/stse_return_codes.h"

/*!
 * \brief Erase the session context.
 * \param[in] p_session Pointer to the session context to erase.
 * \return None.
 */
void stse_session_erase_context(stse_session_t *p_session);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_SESSION_H */
