/*!
 ******************************************************************************
 * \file	stsafea_entity_auth.c
 * \brief   Entity authentication services for STSAFE-A (header)
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

#ifndef STSAFE_ENTITY_AUTH_H
#define STSAFE_ENTITY_AUTH_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_entity_auth STSAFE-A Entity authentication
 *  \ingroup stsafea_services
 *  @{
 */

/**
 * \brief 			STSAFEA generate challenge service
 * \details 		This service format and send/receive the generate challenge command/response
 * \param[in]		pSTSE 			Pointer to target STSecureElement device
 * \param[in]		challenge_size 	Challenge buffer size (expected STSE_EDDSA_CHALLENGE_SIZE)
 * \param[out]		pChallenge 		Pointer to challenge buffer
 * \return 			\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_generate_challenge(
    stse_Handler_t *pSTSE,
    PLAT_UI8 challenge_size,
    PLAT_UI8 *pChallenge);

/**
 * \brief 			STSAFEA verify entity's signature service
 * \details 		This service format and send/receive the verify entity's signature command/response
 * \param[in]		pSTSE 				Pointer to target STSecureElement device
 * \param[in] 		slot_number 		Public key slot value
 * \param[in] 		key_type 			Targeted public key's type stored through STSAFE-A generic public slot
 * \param[out]		pSignature 			Pointer to signature buffer
 * \param[out]		pSignature_validity Pointer to signature validity byte
 * \return 			\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_verify_entity_signature(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pSignature,
    PLAT_UI8 *pSignature_validity);

/** \}*/

#endif /* STSAFE_ENTITY_AUTH_H */
