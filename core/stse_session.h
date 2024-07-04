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

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_generic_typedef.h"

/*!
 * \brief 		This Core function Erase the session context from STSAFE handler
 * \param[in] 	*pSTSE 	Pointer to target STSAFE handler
 * \return \ref stsafe_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_erase_context.dox
 */
void stse_session_erase_context (stse_session_t *pSession);


#endif /* STSE_SESSION_MANAGER_H */
