/**
  ******************************************************************************
  * @file    stsafea_entity_auth.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for entity authentication (header)
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

#ifndef STSAFE_ENTITY_AUTH_H
#define STSAFE_ENTITY_AUTH_H

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

/*! \defgroup stsafea_entity_auth STSAFE-A Entity authentication
 *  \ingroup stsafea_services
 *  @{
 */

/**
  * \brief       STSAFEA generate challenge service
  * \details     This service format and send/receive the generate challenge command/response
  * \param[in]   p_stse      Pointer to target STSecureElement device
  * \param[in]   challenge_size  Challenge buffer size (expected STSE_EDDSA_CHALLENGE_SIZE)
  * \param[out]    p_challenge     Pointer to challenge buffer
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_generate_challenge(
  stse_handle_t *p_stse,
  PLAT_UI8 challenge_size,
  PLAT_UI8 *p_challenge);

/**
  * \brief       STSAFEA verify entity's signature service
  * \details     This service format and send/receive the verify entity's signature command/response
  * \param[in]   p_stse        Pointer to target STSecureElement device
  * \param[in]     slot_number     Public key slot value
  * \param[in]     key_type      Targeted public key's type stored through STSAFE-A generic public slot
  * \param[out]    p_signature       Pointer to signature buffer
  * \param[out]    p_signature_validity Pointer to signature validity byte
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_verify_entity_signature(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *p_signature,
  PLAT_UI8 *p_signature_validity);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFE_ENTITY_AUTH_H */
