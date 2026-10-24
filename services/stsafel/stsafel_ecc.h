/**
  ******************************************************************************
  * @file    stsafel_ecc.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for ECC (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSAFEL_ECC_H
#define STSAFEL_ECC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafel/stsafel_timings.h"

/*! \defgroup stsafel_ecc STSAFE-L Elliptic Curves Cryptography (ECC) services
 *  \ingroup stsafel_services
 *  @{
 */

#define STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH 16U
#define STSAFEL_ECC_SIGNATURE_SUBJECT_TRACEABILITY 0xFE
#define STSAFEL_ECC_SIGNATURE_SUBJECT_NONE 0xFF

/**
  * \brief     Generate ECC signature over a challenge
  * \details   This service format and send generate signature command
  * \param[in]   p_stse        Pointer to STSE Handler
  * \param[in]   key_type      \ref stse_ecc_key_type_t key type to use
  * \param[in]   challenge_length  Challenge length
  * \param[in]   p_challenge       Challenge to be signed
  * \param[out]  p_signature     Signature output buffer
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_ecc_generate_signature(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *p_challenge,
  PLAT_UI16 challenge_length,
  PLAT_UI8 *p_signature);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEL_ECC_H */
