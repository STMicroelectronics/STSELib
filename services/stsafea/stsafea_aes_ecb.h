/**
  ******************************************************************************
  * @file    stsafea_aes_ecb.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for AES ECB symmetric key cryptography (header)
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

#ifndef STSAFEA_AES_ECB_H
#define STSAFEA_AES_ECB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_aes_ecb STSAFE AES ECB services
 *  \ingroup stsafea_services
 *  @{
 */

/**
  * \brief     Encrypt payload in AES ECB mode
  * \details   This service format and send encrypt command in AES ECB mode
  * \param[in]   p_stse      Pointer to STSE Handler
  * \param[in]   slot_number     Key slot in symmetric key table to be used
  * \param[in]   message_length    Length of the message<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 5
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 5
  * \param[in] p_plaintext_message Plaintext message to encrypt
  * \param[out]  p_encrypted_message Encrypted message
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_aes_ecb_encrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_plaintext_message,
  PLAT_UI8 *p_encrypted_message);

/**
  * \brief     Decrypt payload in AES ECB mode
  * \details   This service format and send decrypt command in AES ECB mode
  * \param[in]   p_stse      Pointer to STSE Handler
  * \param[in]   slot_number     Key slot in symmetric key table to be used
  * \param[in]   message_length    Length of the message<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 5
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 5
  * \param[in] p_encrypted_message Encrypted message to decrypt
  * \param[out]  p_plaintext_message Plaintext message
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_aes_ecb_decrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_encrypted_message,
  PLAT_UI8 *p_plaintext_message);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEA_AES_ECB_H */

