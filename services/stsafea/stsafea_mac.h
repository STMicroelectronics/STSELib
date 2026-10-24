/**
  ******************************************************************************
  * @file    stsafea_mac.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for message Authentication Code - MAC (header)
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

#ifndef STSAFEA_MAC_H
#define STSAFEA_MAC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_mac STSAFE-A Message Authentication Code (MAC)
 *  \ingroup stsafea_services
 *  @{
 */

/**
  * \brief     Generate a CMAC
  * \details   This service format and send Generate CMAC command
  * \param[in]   p_stse    Pointer to STSE Handler
  * \param[in]   slot_number   Key slot in symmetric key table to be used
  * \param[in] p_message   Plaintext message
  * \param[in] message_length  Plaintext message length<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 6
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 6
  * \param[out]  p_mac       Buffer to store the MAC
  * \param[out]  mac_length    MAC length (CMAC:2,4,8,16 / HMAC:16-32)
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_cmac_hmac_compute(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 *p_message,
  PLAT_UI8 message_length,
  PLAT_UI8 *p_mac,
  PLAT_UI8 mac_length);

/**
  * \brief     Verify a CMAC
  * \details   This service format and send Verify CMAC command
  * \param[in]   p_stse        Pointer to STSE Handler
  * \param[in]   slot_number       Key slot in symmetric key table to be used
  * \param[in]   p_mac           Buffer containing the MAC
  * \param[in] mac_length        MAC length (CMAC:2,4,8,16 / HMAC:16-32)
  * \param[in] p_message       Plaintext message
  * \param[in] message_length      Plaintext message length<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 6 - mac_length
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 6 - mac_length
  * \param[out]  verification_result   Verification result flag
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_cmac_hmac_verify(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 *p_mac,
  PLAT_UI8 mac_length,
  PLAT_UI8 *p_message,
  PLAT_UI8 message_length,
  PLAT_UI8 *verification_result);

/**
  * \brief     Generate a GMAC
  * \details   This service format and send Generate GMAC command
  * \param[in]   p_stse            Pointer to STSE Handler
  * \param[in]   slot_number         Key slot in symmetric key table to be used
  * \param[in] IV_length         IV buffer length in bytes
  * \param[in] p_iv              IV buffer
  * \param[in] associated_data_length    Length of the associated data<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - IV_length
  * \param[in] p_associated_data     Buffer containing associated data
  * \param[in]   authentication_tag_length   Expected length for the authentication tag
  * \param[out]  p_authentication_tag      Buffer to store the authentication tag
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_aes_gmac_compute(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *p_authentication_tag);

/**
  * \brief     Verify a GMAC
  * \details   This service format and send Verify GMAC command
  * \param[in]   p_stse            Pointer to STSE Handler
  * \param[in]   slot_number         Key slot in symmetric key table to be used
  * \param[in] IV_length         IV buffer length in bytes
  * \param[in] p_iv              IV buffer
  * \param[in] associated_data_length    Length of the associated data<br>
  * message_length maximum value (in bytes):
  * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - IV_length - authentication_tag_length
  * \param[in] p_associated_data     Buffer containing associated data
  * \param[in]   authentication_tag_length   Expected length for the authentication tag
  * \param[in] p_authentication_tag      Buffer containing the authentication tag
  * \param[out]  p_verification_result   Verification result flag
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_aes_gmac_verify(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *p_authentication_tag,
  PLAT_UI8 *p_verification_result);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFEA_MAC_H */
