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
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_AES_H
#define STSE_AES_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"

/** \defgroup 	stse_aes 	STSE AES cryptography
 *  \ingroup 	stse_api
 *  \brief		STSE data storage API set
 *  \details  	The data storage API set provides high level functions to manage
 * Access to the Secure User-NVM of STSE devices.
 *  \{
 */

/**
 * \brief 		Encrypt payload in AES ECB mode
 * \details 	This API encrypt payload in AES ECB mode using the specified key
 * from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	slot_number 		Key slot in
 * symmetric key table to be used \param[in] 	message_length
 * Length of the message \param[in]	pPlaintext_message	Plaintext
 * message to encrypt \param[out]	pEncrypted_message	Encrypted
 * message \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise \details 	\include{doc} stse_aes_ecb_encrypt.dox
 */
stse_ReturnCode_t stse_aes_ecb_encrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI16 message_length, PLAT_UI8 *pPlaintext_message,
				       PLAT_UI8 *pEncrypted_message);

/**
 * \brief 		Decrypt payload in AES ECB mode
 * \details 	This API decrypt payload in AES ECB mode using the specified key
 * from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	slot_number 		Key slot in
 * symmetric key table to be used \param[in] 	message_length
 * Length of the message \param[in]	pEncrypted_message	Encrypted
 * message to decrypt \param[out]	pPlaintext_message	Plaintext
 * message \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise \details 	\include{doc} stse_aes_ecb_decrypt.dox
 */
stse_ReturnCode_t stse_aes_ecb_decrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI16 message_length, PLAT_UI8 *pEncrypted_message,
				       PLAT_UI8 *pPlaintext_message);

/**
 * \brief 		Encrypt payload in AES CCM* mode
 * \details 	This API Encrypt payload in AES CCM* mode using the specified
 * key from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	slot_number
 * Key slot in symmetric key table to be used \param[in]
 * authentication_tag_length 		Expected length for the authentication
 * tag
 * \param[in]	pNonce							Buffer
 * containing
 * the nonce \param[in]	associated_data_length			Length of the
 * associated data
 * \param[in]	pAssociated_data				Buffer
 * containing associated data
 * \param[in]	message_length					Length of the
 * message to encrypt
 * \param[in]	pPlaintext_message				Buffer
 * containing the message to encrypt \param[out]	pEncrypted_message
 * Buffer to store the encrypted message \param[out]
 * pEncrypted_authentication_tag	Buffer to store the authentication tag
 * \param[out]	pCounter_presence				Counter presence
 * flag \param[out]	pCounter
 * Buffer containing counter value if present \return \ref STSE_OK on success ;
 * \ref stse_ReturnCode_t error code otherwise \details 	\include{doc}
 * stse_aes_ccm_encrypt.dox
 */
stse_ReturnCode_t stse_aes_ccm_encrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI8 authentication_tag_length, PLAT_UI8 *pNonce,
				       PLAT_UI16 associated_data_length, PLAT_UI8 *pAssociated_data,
				       PLAT_UI16 message_length, PLAT_UI8 *pPlaintext_message,
				       PLAT_UI8 *pEncrypted_message,
				       PLAT_UI8 *pEncrypted_authentication_tag,
				       PLAT_UI8 *pCounter_presence, PLAT_UI32 *pCounter);

/**
 * \brief 		Start chunk encryption in AES CCM* mode
 * \details 	This service start chunk encryption in AES CCM* mode using the
 * specified key from STSE symmetric key table
 * \param[in] 	pSTSE 							Pointer
 * to STSE Handler \param[in] 	slot_number Key slot in symmetric key table to
 * be used \param[in]	Nonce_length Nonce buffer length in bytes \param[in]
 * pNonce Nonce buffer \param[in]	total_associated_data_length	Length
 * of the total amount of associated data \param[in]	total_message_length
 * Length of the complete message to be encrypted by chunks \param[in]
 * associated_data_chunk_length	Length of the associated data chunk \param[in]
 * pAssociated_data_chunk			Buffer containing associated
 * data chunk \param[in]	message_chunk_length Length of the message chunk
 * to encrypt
 * \param[in]	pPlaintext_message_chunk		Buffer containing 1st
 * piece of plaintext message chunk to encrypt \param[out]
 * pEncrypted_message_chunk Buffer to store the encrypted message chunk
 * \param[out]	pCounter_presence				Counter presence
 * flag \param[out]	pCounter
 * Buffer containing counter value if present \return \ref STSE_OK on success ;
 * \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stse_aes_ccm_encrypt_start(stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI16 Nonce_length,
			   PLAT_UI8 *pNonce, PLAT_UI16 total_associated_data_length,
			   PLAT_UI32 total_message_length, PLAT_UI16 associated_data_chunk_length,
			   PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			   PLAT_UI8 *pPlaintext_message_chunk, PLAT_UI8 *pEncrypted_message_chunk,
			   PLAT_UI8 *pCounter_presence, PLAT_UI32 *pCounter);

/**
 * \brief 		Process chunk encryption in AES CCM* mode
 * \details 	This service process additional chunk encryption in AES CCM*
 * mode using the key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in]	associated_data_chunk_length
 * Length of the associated data chunk \param[in]	pAssociated_data_chunk
 * Buffer containing associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to encrypt \param[in] pPlaintext_message_chunk
 * Buffer containing the message chunk to encrypt \param[out]
 * pEncrypted_message_chunk		Buffer to store the encrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t stse_aes_ccm_encrypt_process(stse_Handler_t *pSTSE,
					       PLAT_UI16 associated_data_chunk_length,
					       PLAT_UI8 *pAssociated_data_chunk,
					       PLAT_UI16 message_chunk_length,
					       PLAT_UI8 *pPlaintext_message_chunk,
					       PLAT_UI8 *pEncrypted_message_chunk);

/**
 * \brief 		Finish chunk encryption in AES CCM* mode
 * \details 	This service finish chunk encryption in AES CCM* mode using the
 * key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	authentication_tag_length
 * Length of the output authentication tag \param[in]
 * associated_data_chunk_length	Length of the associated data chunk \param[in]
 * pAssociated_data_chunk			Buffer containing associated
 * data chunk \param[in]	message_chunk_length Length of the message chunk
 * to encrypt
 * \param[in]	pPlaintext_message_chunk		Buffer containing the
 * message chunk to encrypt \param[out]	pEncrypted_message_chunk
 * Buffer to store the encrypted message chunk \param[out]
 * pEncrypted_authentication_tag	Encrypted authentication tag \return
 * \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stse_aes_ccm_encrypt_finish(stse_Handler_t *pSTSE, PLAT_UI8 authentication_tag_length,
			    PLAT_UI16 associated_data_chunk_length,
			    PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			    PLAT_UI8 *pPlaintext_message_chunk, PLAT_UI8 *pEncrypted_message_chunk,
			    PLAT_UI8 *pEncrypted_authentication_tag);

/**
 * \brief 		Decrypt payload in AES CCM* mode
 * \details 	This API Decrypt payload in AES CCM* mode using the specified
 * key from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	slot_number
 * Key slot in symmetric key table to be used \param[in]
 * authentication_tag_length 		Expected length for the authentication
 * tag
 * \param[in]	pNonce							Buffer
 * containing
 * the nonce \param[in]	associated_data_length			Length of the
 * associated data
 * \param[in]	pAssociated_data				Buffer
 * containing associated data
 * \param[in]	message_length					Length of the
 * message to encrypt
 * \param[in]	pEncrypted_message				Buffer
 * containing the message to decrypt \param[in]	pEncrypted_authentication_tag
 * Buffer containing the encrypted authentication tag \param[out]
 * pVerification_result			Verification result flag \param[out]
 * pPlaintext_message				Buffer to store the decrypted
 * message \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise \details 	\include{doc} stse_aes_ccm_decrypt.dox
 */
stse_ReturnCode_t stse_aes_ccm_decrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI8 authentication_tag_length, PLAT_UI8 *pNonce,
				       PLAT_UI16 associated_data_length, PLAT_UI8 *pAssociated_data,
				       PLAT_UI16 message_length, PLAT_UI8 *pEncrypted_message,
				       PLAT_UI8 *pEncrypted_authentication_tag,
				       PLAT_UI8 *pVerification_result,
				       PLAT_UI8 *pPlaintext_message);

/**
 * \brief 		Start chunk decryption in AES CCM* mode
 * \details 	This service start chunk decryption in AES CCM* mode using the
 * specified key from STSE symmetric key table
 * \param[in] 	pSTSE 							Pointer
 * to STSE Handler \param[in] 	slot_number Key slot in symmetric key table to
 * be used \param[in]	Nonce_length Nonce buffer length in bytes \param[in]
 * pNonce Nonce buffer \param[in]	total_associated_data_length	Length
 * of the associated data \param[in]	total_ciphertext_length
 * Length of the complete ciphertext \param[in]	associated_data_chunk_length
 * Length of the associated data chunk \param[in]	pAssociated_data_chunk
 * Buffer containing associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to decrypt \param[in] pEncrypted_message_chunk
 * Buffer containing the message chunk to decrypt \param[out]
 * pPlaintext_message_chunk		Buffer to store the decrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t
stse_aes_ccm_decrypt_start(stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI16 Nonce_length,
			   PLAT_UI8 *pNonce, PLAT_UI16 total_associated_data_length,
			   PLAT_UI16 total_ciphertext_length,
			   PLAT_UI16 associated_data_chunk_length, PLAT_UI8 *pAssociated_data_chunk,
			   PLAT_UI16 message_chunk_length, PLAT_UI8 *pEncrypted_message_chunk,
			   PLAT_UI8 *pPlaintext_message_chunk);

/**
 * \brief 		Process chunk decryption in AES CCM* mode
 * \details 	This service process additional chunk decryption in AES CCM*
 * mode using the key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in]	associated_data_chunk_length
 * Length of the associated data chunk \param[in]	pAssociated_data_chunk
 * Buffer containing associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to decrypt \param[in] pEncrypted_message_chunk
 * Buffer containing the message chunk to decrypt \param[out]
 * pPlaintext_message_chunk		Buffer to store the decrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t stse_aes_ccm_decrypt_process(stse_Handler_t *pSTSE,
					       PLAT_UI16 associated_data_chunk_length,
					       PLAT_UI8 *pAssociated_data_chunk,
					       PLAT_UI16 message_chunk_length,
					       PLAT_UI8 *pEncrypted_message_chunk,
					       PLAT_UI8 *pPlaintext_message_chunk);

/**
 * \brief 		Finish chunk decryption in AES CCM* mode
 * \details 	This service finish chunk encryption in AES CCM* mode using the
 * key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	authentication_tag_length
 * Length of the output authentication tag \param[in]
 * associated_data_chunk_length	Length of the associated data chunk \param[in]
 * pAssociated_data_chunk			Buffer containing associated
 * data chunk \param[in]	message_chunk_length Length of the message chunk
 * to decrypt
 * \param[in]	pEncrypted_message_chunk		Buffer containing the
 * message chunk to decrypt \param[in] 	pAuthentication_tag
 * Authentication tag
 * \param[out] 	pVerification_result 			Verification result flag
 * \param[out]	pPlaintext_message_chunk		Buffer to store the
 * decrypted message chunk \return \ref STSE_OK on success ; \ref
 * stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stse_aes_ccm_decrypt_finish(stse_Handler_t *pSTSE, PLAT_UI8 authentication_tag_length,
			    PLAT_UI16 associated_data_chunk_length,
			    PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			    PLAT_UI8 *pEncrypted_message_chunk, PLAT_UI8 *pAuthentication_tag,
			    PLAT_UI8 *pVerification_result, PLAT_UI8 *pPlaintext_message_chunk);

/**
 * \brief 		Encrypt payload in AES CCM mode
 * \details 	This API Encrypt payload in AES GCM mode using the specified key
 * from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler
 * \param[in] 	slot_number 				Key slot in symmetric
 * key table to be used \param[in] 	authentication_tag_length 	Expected
 * length for the authentication tag
 * \param[in]	IV_length					IV buffer length
 * in bytes \param[in]	pIV IV
 * buffer \param[in]	associated_data_length		Length of the associated
 * data \param[in]	pAssociated_data			Buffer
 * containing associated data
 * \param[in]	message_length				Length of the message to
 * encrypt
 * \param[in]	pPlaintext_message			Buffer containing the
 * message to encrypt
 * \param[out]	pEncrypted_message			Buffer to store the
 * encrypted message
 * \param[out]	pAuthentication_tag			Buffer to store the
 * authentication tag \return \ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t stse_aes_gcm_encrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI8 authentication_tag_length, PLAT_UI16 IV_length,
				       PLAT_UI8 *pIV, PLAT_UI16 associated_data_length,
				       PLAT_UI8 *pAssociated_data, PLAT_UI16 message_length,
				       PLAT_UI8 *pPlaintext_message, PLAT_UI8 *pEncrypted_message,
				       PLAT_UI8 *pAuthentication_tag);

/**
 * \brief 		Start chunk encryption in AES GCM mode
 * \details 	This service start chunk encryption in AES GCM mode using the
 * specified key from STSE symmetric key table
 * \param[in] 	pSTSE 							Pointer
 * to STSE Handler \param[in] 	slot_number Key slot in symmetric key table to
 * be used \param[in]	IV_length IV buffer length in bytes \param[in]	pIV IV
 * buffer \param[in]	associated_data_chunk_length	Length of the associated
 * data chunk
 * \param[in]	pAssociated_data_chunk			Buffer containing
 * associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to encrypt \param[in] pPlaintext_message_chunk
 * Buffer containing the message chunk to encrypt \param[out]
 * pEncrypted_message_chunk		Buffer to store the encrypted message
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stse_aes_gcm_encrypt_start(stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI16 IV_length,
			   PLAT_UI8 *pIV, PLAT_UI16 associated_data_chunk_length,
			   PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			   PLAT_UI8 *pPlaintext_message_chunk, PLAT_UI8 *pEncrypted_message_chunk);

/**
 * \brief 		Process chunk encryption in AES GCM mode
 * \details 	This service process additional chunk encryption in AES GCM mode
 * using the key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in]	associated_data_chunk_length
 * Length of the associated data chunk \param[in]	pAssociated_data_chunk
 * Buffer containing associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to encrypt \param[in] pPlaintext_message_chunk
 * Buffer containing the message chunk to encrypt \param[out]
 * pEncrypted_message_chunk		Buffer to store the encrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t stse_aes_gcm_encrypt_process(stse_Handler_t *pSTSE,
					       PLAT_UI16 associated_data_chunk_length,
					       PLAT_UI8 *pAssociated_data_chunk,
					       PLAT_UI16 message_chunk_length,
					       PLAT_UI8 *pPlaintext_message_chunk,
					       PLAT_UI8 *pEncrypted_message_chunk);

/**
 * \brief 		Finish chunk encryption in AES GCM mode
 * \details 	This service finish chunk encryption in AES GCM mode using the
 * key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	authentication_tag_length
 * Length of the output authentication tag \param[in]
 * associated_data_chunk_length	Length of the associated data chunk \param[in]
 * pAssociated_data_chunk			Buffer containing associated
 * data chunk \param[in] 	message_chunk_length Length of the message chunk
 * \param[in] 	pPlaintext_message_chunk		Buffer containing the
 * message chunk to encrypt \param[out] pEncrypted_message_chunk Buffer to store
 * the encrypted message chunk \param[out] 	pAuthentication_tag
 * Authentication tag \return \ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t
stse_aes_gcm_encrypt_finish(stse_Handler_t *pSTSE, PLAT_UI8 authentication_tag_length,
			    PLAT_UI16 associated_data_chunk_length,
			    PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			    PLAT_UI8 *pPlaintext_message_chunk, PLAT_UI8 *pEncrypted_message_chunk,
			    PLAT_UI8 *pAuthentication_tag);

/**
 * \brief 		Decrypt payload in AES GCM mode
 * \details 	This API Decrypt payload in AES GCM mode using the specified key
 * from STSE symmetric key table \param[in] 	pSTSE
 * Pointer to STSE Handler
 * \param[in] 	slot_number 				Key slot in symmetric
 * key table to be used \param[in] 	authentication_tag_length 	Expected
 * length for the authentication tag
 * \param[in]	IV_length					IV buffer length
 * in bytes \param[in]	pIV IV
 * buffer \param[in]	associated_data_length		Length of the associated
 * data \param[in]	pAssociated_data			Buffer
 * containing associated data
 * \param[in]	message_length				Length of the message to
 * encrypt
 * \param[in]	pEncrypted_message			Buffer containing the
 * message to decrypt
 * \param[in]	pAuthentication_tag			Buffer containing the
 * authentication tag
 * \param[out]	pVerification_result		Verification result flag
 * \param[out]	pPlaintext_message			Buffer to store the
 * decrypted message \return \ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t stse_aes_gcm_decrypt(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
				       PLAT_UI8 authentication_tag_length, PLAT_UI16 IV_length,
				       PLAT_UI8 *pIV, PLAT_UI16 associated_data_length,
				       PLAT_UI8 *pAssociated_data, PLAT_UI16 message_length,
				       PLAT_UI8 *pEncrypted_message, PLAT_UI8 *pAuthentication_tag,
				       PLAT_UI8 *pVerification_result,
				       PLAT_UI8 *pPlaintext_message);

/**
 * \brief 		Start chunk decryption in AES GCM mode
 * \details 	This service start chunk decryption in AES GCM mode using the
 * specified key from STSE symmetric key table
 * \param[in] 	pSTSE 							Pointer
 * to STSE Handler \param[in] 	slot_number Key slot in symmetric key table to
 * be used \param[in]	IV_length IV buffer length in bytes \param[in]	pIV IV
 * buffer \param[in]	associated_data_chunk_length	Length of the associated
 * data chunk
 * \param[in]	pAssociated_data_chunk			Buffer containing
 * associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to decrypt \param[in] pEncrypted_message_chunk
 * Buffer containing the message chunk to decrypt \param[out]
 * pPlaintext_message_chunk		Buffer to store the decrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t
stse_aes_gcm_decrypt_start(stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI16 IV_length,
			   PLAT_UI8 *pIV, PLAT_UI16 associated_data_chunk_length,
			   PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			   PLAT_UI8 *pEncrypted_message_chunk, PLAT_UI8 *pPlaintext_message_chunk);

/**
 * \brief 		Process chunk decryption in AES GCM mode
 * \details 	This service process additional chunk decryption in AES GCM mode
 * using the key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in]	associated_data_chunk_length
 * Length of the associated data chunk \param[in]	pAssociated_data_chunk
 * Buffer containing associated data chunk \param[in]	message_chunk_length
 * Length of the message chunk to decrypt \param[in] pEncrypted_message_chunk
 * Buffer containing the message chunk to decrypt \param[out]
 * pPlaintext_message_chunk		Buffer to store the decrypted message
 * chunk \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code
 * otherwise
 */
stse_ReturnCode_t stse_aes_gcm_decrypt_process(stse_Handler_t *pSTSE,
					       PLAT_UI16 associated_data_chunk_length,
					       PLAT_UI8 *pAssociated_data_chunk,
					       PLAT_UI16 message_chunk_length,
					       PLAT_UI8 *pEncrypted_message_chunk,
					       PLAT_UI8 *pPlaintext_message_chunk);

/**
 * \brief 		Finish chunk decryption in AES GCM mode
 * \details 	This service finish chunk encryption in AES GCM mode using the
 * key specified in start command \param[in] 	pSTSE
 * Pointer to STSE Handler \param[in] 	authentication_tag_length
 * Length of the output authentication tag \param[in]
 * associated_data_chunk_length	Length of the associated data chunk \param[in]
 * pAssociated_data_chunk			Buffer containing associated
 * data chunk \param[in]	message_chunk_length Length of the message chunk
 * to decrypt
 * \param[in]	pEncrypted_message_chunk		Buffer containing the
 * message chunk to decrypt \param[in] 	pAuthentication_tag
 * Authentication tag
 * \param[out] 	pVerification_result 			Verification result flag
 * \param[out]	pPlaintext_message_chunk		Buffer to store the
 * decrypted message chunk \return \ref STSE_OK on success ; \ref
 * stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stse_aes_gcm_decrypt_finish(stse_Handler_t *pSTSE, PLAT_UI8 authentication_tag_length,
			    PLAT_UI16 associated_data_chunk_length,
			    PLAT_UI8 *pAssociated_data_chunk, PLAT_UI16 message_chunk_length,
			    PLAT_UI8 *pEncrypted_message_chunk, PLAT_UI8 *pAuthentication_tag,
			    PLAT_UI8 *pVerification_result, PLAT_UI8 *pPlaintext_message_chunk);

/** @}*/

#endif /*STSE_AES_H*/
