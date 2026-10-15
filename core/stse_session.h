/*!
 ******************************************************************************
 * \file	stse_session.h
 * \brief   STSELib api for session manager (header)
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

#ifndef STSE_SESSION_H
#define STSE_SESSION_H

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

#endif /* STSE_SESSION_H */
