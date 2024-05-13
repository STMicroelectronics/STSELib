/*!
 ******************************************************************************
 * \file	stsafea_hash.h
 * \brief   Hash services for STSAFE-A
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

#ifndef STSAFEA_WRAP_UNWRAP_H
#define STSAFEA_WRAP_UNWRAP_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_sessions.h"

/*! \defgroup stsafea_wrap Wrap/Un-wrap
 *  \ingroup stsafea_services
 *  \{
 */

/**
 * \brief 		STSAFEA wrap service
 * \details 	This service format and send STSAFEA wrap command/response to target STSE
 * \param[in]	pSTSE					Pointer to target SE handler
 * \param[in]	wrap_key_slot			Wrap key slot
 * \param[in] 	pPayload				Pointer to the payload buffer to be wrapped
 * \param[in]	payload_size			size of the payload buffer
 * \param[out] 	pWrapped_Payload		Pointer to the wrapped payload buffer
 * \param[in]	wrapped_payload_size	size of the wrapped payload buffer
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_wrap_payload(
		stse_Handler_t 	*pSTSE,
		PLAT_UI8 		wrap_key_slot,
		PLAT_UI8 		*pPayload,
		PLAT_UI16 		payload_size,
		PLAT_UI8 		*pWrapped_Payload,
		PLAT_UI16 		wrapped_payload_size
);

/**
 * \brief 		STSAFEA un-wrap service
 * \details 	This service format and send STSAFEA un-wrap command/response to target STSE
 * \param[in]	pSTSE					Pointer to target SE handler
 * \param[in]	wrap_key_slot			wrap key slot
 * \param[in]	pWrapped_Payload		Pointer to the wrapped payload buffer to be un-wrapped
 * \param[in]	wrapped_payload_size	Size of the wrapped payload buffer
 * \param[out]	pPayload				Pointer to the plain text payload buffer
 * \param[in]	payload_size			Size of the payload buffer
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_unwrap_payload(
		stse_Handler_t 	*pSTSE,
		PLAT_UI8 		wrap_key_slot,
		PLAT_UI8 		*pWrapped_Payload,
		PLAT_UI16 		wrapped_payload_size,
		PLAT_UI8 		*pPayload,
		PLAT_UI16 		payload_size
);

/** \}*/

#endif /*STSAFEA_WRAP_UNWRAP_H*/
