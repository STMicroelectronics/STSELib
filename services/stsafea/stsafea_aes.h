/*!
 ******************************************************************************
 * \file	stsafea_aes.h
 * \brief   STSAFE Middleware services for symmetric key cryptography (header)
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

#ifndef STSAFEA_AES_H
#define STSAFEA_AES_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_sessions.h"

/*! \defgroup stsafea_aes AES
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_MAC_SIZE 										4U
#define STSAFEA_NONCE_SIZE 										13U

/**
 * \brief 		Encrypt payload in AES ECB mode
 * \details 	This service format and send encrypt command in AES ECB mode
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message
 * \param[in]	pPlaintext_message	Plaintext message to encrypt
 * \param[out]	pEncrypted_message	Encrypted message
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_ecb_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message);

/**
 * \brief 		Decrypt payload in AES ECB mode
 * \details 	This service format and send decrypt command in AES ECB mode
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message
 * \param[in]	pEncrypted_message	Encrypted message to decrypt
 * \param[out]	pPlaintext_message	Plaintext message
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_ecb_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pPlaintext_message);

/**
 * \brief 		Encrypt payload in AES CCM* mode
 * \details 	This service format and send encrypt command in AES CCM* mode
 * \param[in] 	pSTSE 					Pointer to STSAFE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	pNonce						Buffer containing the nonce
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	pAssociated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to encrypt
 * \param[in]	pPlaintext_message			Buffer containing the message to encrypt
 * \param[out]	pEncrypted_message			Buffer to store the encrypted message
 * \param[out]	pAuthentication_tag			Buffer to store the authentication tag
 * \param[out]	pCounter_presence			Counter presence flag
 * \param[out]	pCounter					Buffer containing counter value if present
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_ccm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pCounter_presence,
		PLAT_UI32 * pCounter);

/**
 * \brief 		Decrypt payload in AES CCM* mode
 * \details 	This service format and send decrypt command in AES CCM* mode
 * \param[in] 	pSTSE 					Pointer to STSAFE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	pNonce						Buffer containing the nonce
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	pAssociated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to encrypt
 * \param[in]	pEncrypted_message			Buffer containing the message to decrypt
 * \param[in]	pAuthentication_tag			Buffer containing the authentication tag
 * \param[out]	pVerification_result		Verification result flag
 * \param[out]	pPlaintext_message			Buffer to store the decrypted message
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_ccm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message);

/**
 * \brief 		Encrypt payload in AES GCM mode
 * \details 	This service format and send encrypt command in AES GCM mode
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	IV_length					IV buffer length in bytes
 * \param[in]	IV							IV buffer
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	pAssociated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to encrypt
 * \param[in]	pPlaintext_message			Buffer containing the message to encrypt
 * \param[out]	pEncrypted_message			Buffer to store the encrypted message
 * \param[out]	pAuthentication_tag			Buffer to store the authentication tag
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_gcm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag);

/**
 * \brief 		Decrypt payload in AES GCM mode
 * \details 	This service format and send decrypt command in AES GCM mode
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	IV_length					IV buffer length in bytes
 * \param[in]	IV							IV buffer
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	pAssociated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to encrypt
 * \param[in]	pEncrypted_message			Buffer containing the message to decrypt
 * \param[in]	pAuthentication_tag			Buffer containing the authentication tag
 * \param[out]	pVerification_result		Verification result flag
 * \param[out]	pPlaintext_message			Buffer to store the decrypted message
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_aes_gcm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message);

/** \}*/

#endif /*STSAFEA_AES_H */
