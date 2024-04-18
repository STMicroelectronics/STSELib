/*!
 ******************************************************************************
 * \file	stse_aes.h
 * \brief   STSE AES API set (header)
 * \author  STMicroelectronics - CS application team
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

#ifndef STSE_AES_H
#define STSE_AES_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_aes.h"

/** \defgroup 	stse_aes 	STSE AES cryptography
 *  \ingroup 	stse_api
 *  \brief		STSE data storage API set
 *  \details  	The data storage API set provides high level functions to manage Access to the Secure User NVM of STSAFE devices.
 *  \{
 */


/**
 * \brief 		Encrypt payload in AES ECB mode
 * \details 	This API encrypt payload in AES ECB mode using the specified key from STSAFE symmetric key table
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message
 * \param[in]	pPlaintext_message	Plaintext message to encrypt
 * \param[out]	pEncrypted_message	Encrypted message
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_aes_ecb_encrypt.dox
 */
stse_ReturnCode_t stse_aes_ecb_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message);

/**
 * \brief 		Decrypt payload in AES ECB mode
 * \details 	This API decrypt payload in AES ECB mode using the specified key from STSAFE symmetric key table
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message
 * \param[in]	pEncrypted_message	Encrypted message to decrypt
 * \param[out]	pPlaintext_message	Plaintext message
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_aes_ecb_decrypt.dox
 */
stse_ReturnCode_t stse_aes_ecb_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pPlaintext_message);

/**
 * \brief 		Encrypt payload in AES CCM* mode
 * \details 	This API Encrypt payload in AES CCM* mode using the specified key from STSAFE symmetric key table
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
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_aes_ccm_encrypt.dox
 */
stse_ReturnCode_t stse_aes_ccm_encrypt(
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
 * \details 	This API Decrypt payload in AES CCM* mode using the specified key from STSAFE symmetric key table
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
 * \return \ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_aes_ccm_decrypt.dox
 */
stse_ReturnCode_t stse_aes_ccm_decrypt(
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
 * \brief 		Encrypt payload in AES CCM mode
 * \details 	This API Encrypt payload in AES GCM mode using the specified key from STSAFE symmetric key table
 * \param[in] 	pSTSE 						Pointer to STSE Handler
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
 * \details 	\include{doc} stse_aes_gcm_encrypt.dox
 */
stse_ReturnCode_t stse_aes_gcm_encrypt(
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
 * \details 	This API Decrypt payload in AES GCM mode using the specified key from STSAFE symmetric key table
 * \param[in] 	pSTSE 						Pointer to STSE Handler
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
 * \details 	\include{doc} stse_aes_gcm_decrypt.dox
 */
stse_ReturnCode_t stse_aes_gcm_decrypt(
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

/** @}*/

#endif /*STSE_AES_H*/

