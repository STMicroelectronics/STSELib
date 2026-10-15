/*!
 ******************************************************************************
 * \file	stsafea_aes.h
 * \brief   STSAFE-A services for symmetric key cryptography (header)
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

/*! \defgroup stsafea_aes STSAFE AES services
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_MAC_SIZE 4U
#define STSAFEA_NONCE_SIZE 13U

/**
 * \brief 		Encrypt payload in AES ECB mode
 * \details 	This service format and send encrypt command in AES ECB mode
 * \param[in] 	p_stse 			Pointer to STSE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 5
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 5
 * \param[in]	p_plaintext_message	Plaintext message to encrypt
 * \param[out]	p_encrypted_message	Encrypted message
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ecb_encrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_plaintext_message,
    PLAT_UI8 *p_encrypted_message);

/**
 * \brief 		Decrypt payload in AES ECB mode
 * \details 	This service format and send decrypt command in AES ECB mode
 * \param[in] 	p_stse 			Pointer to STSE Handler
 * \param[in] 	slot_number 		Key slot in symmetric key table to be used
 * \param[in] 	message_length 		Length of the message<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 5
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 5
 * \param[in]	p_encrypted_message	Encrypted message to decrypt
 * \param[out]	p_plaintext_message	Plaintext message
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ecb_decrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_encrypted_message,
    PLAT_UI8 *p_plaintext_message);

/**
 * \brief 		Encrypt payload in AES CCM* mode
 * \details 	This service format and send encrypt command in AES CCM* mode
 * \param[in] 	p_stse 						Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 		Expected length for the authentication tag
 * \param[in]	p_nonce							Buffer containing the nonce
 * \param[in]	associated_data_length			Length of the associated data
 * \param[in]	p_associated_data				Buffer containing associated data
 * \param[in]	message_length					Length of the message to encrypt<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 22 - associated_data_length
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 22 - associated_data_length
 * \param[in]	p_plaintext_message				Buffer containing the message to encrypt
 * \param[out]	p_encrypted_message				Buffer to store the encrypted message
 * \param[out]	p_encrypted_authentication_tag	Buffer to store the authentication tag
 * \param[out]	counter_presence				Counter presence flag
 * \param[out]	p_counter						Buffer containing counter value if present
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_encrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI8 *p_nonce,
    PLAT_UI16 associated_data_length,
    PLAT_UI8 *p_associated_data,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_plaintext_message,
    PLAT_UI8 *p_encrypted_message,
    PLAT_UI8 *p_encrypted_authentication_tag,
    PLAT_UI8 counter_presence,
    PLAT_UI32 *p_counter);

/**
 * \brief 		Start chunk encryption in AES CCM* mode
 * \details 	This service start chunk encryption in AES CCM* mode using the specified key from STSAFE symmetric key table
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in]	Nonce_length					Nonce buffer length in bytes
 * \param[in]	p_nonce							Nonce buffer
 * \param[in]	total_associated_data_length	Length of the total amount of associated data
 * \param[in]	total_message_length			Length of the complete message to be encrypted by chunks
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to encrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 17 - Nonce_length - associated_data_chunk_length
 * \param[in]	p_plaintext_message_chunk		Buffer containing 1st piece of plaintext message chunk to encrypt
 * \param[out]	p_encrypted_message_chunk		Buffer to store the encrypted message chunk
 * \param[out]	counter_presence				Counter presence flag
 * \param[out]	p_counter						Buffer containing counter value if present
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_encrypt_start(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 Nonce_length,
    PLAT_UI8 *p_nonce,
    PLAT_UI16 total_associated_data_length,
    PLAT_UI32 total_message_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *counter_presence,
    PLAT_UI32 *p_counter);

/**
 * \brief 		Process chunk encryption in AES CCM* mode
 * \details 	This service process additional chunk encryption in AES CCM* mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to encrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in]	p_plaintext_message_chunk		Buffer containing the message chunk to encrypt
 * \param[out]	p_encrypted_message_chunk		Buffer to store the encrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_encrypt_process(
    stse_handle_t *p_stse,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk);

/**
 * \brief 		Finish chunk encryption in AES CCM* mode
 * \details 	This service finish chunk encryption in AES CCM* mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	authentication_tag_length		Length of the output authentication tag
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to encrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in]	p_plaintext_message_chunk		Buffer containing the message chunk to encrypt
 * \param[out]	p_encrypted_message_chunk		Buffer to store the encrypted message chunk
 * \param[out] 	p_encrypted_authentication_tag	Encrypted authentication tag
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_encrypt_finish(
    stse_handle_t *p_stse,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_encrypted_authentication_tag);

/**
 * \brief 		Decrypt payload in AES CCM* mode
 * \details 	This service format and send decrypt command in AES CCM* mode
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 		Expected length for the authentication tag
 * \param[in]	p_nonce							Buffer containing the nonce
 * \param[in]	associated_data_length			Length of the associated data
 * \param[in]	p_associated_data				Buffer containing associated data
 * \param[in]	message_length					Length of the message to encrypt<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A110 : STSAFEA_MAX_FRAME_LENGTH_A110 - 22 - authentication_tag_length - associated_data_length
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 22 - authentication_tag_length - associated_data_length
 * \param[in]	p_encrypted_message				Buffer containing the message to decrypt
 * \param[in]	p_authentication_tag				Buffer containing the authentication tag
 * \param[out]	p_verification_result			Verification result flag
 * \param[out]	p_plaintext_message				Buffer to store the decrypted message
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_decrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI8 *p_nonce,
    PLAT_UI16 associated_data_length,
    PLAT_UI8 *p_associated_data,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_encrypted_message,
    PLAT_UI8 *p_authentication_tag,
    PLAT_UI8 *p_verification_result,
    PLAT_UI8 *p_plaintext_message);

/**
 * \brief 		Start chunk decryption in AES CCM* mode
 * \details 	This service start chunk decryption in AES CCM* mode using the specified key from STSAFE symmetric key table
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in]	Nonce_length					Nonce buffer length in bytes
 * \param[in]	p_nonce							Nonce buffer
 * \param[in]	total_associated_data_length	Length of the associated data
 * \param[in]	total_ciphertext_length			Length of the complete ciphertext
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 17 - Nonce_length - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_decrypt_start(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 Nonce_length,
    PLAT_UI8 *p_nonce,
    PLAT_UI16 total_associated_data_length,
    PLAT_UI32 total_ciphertext_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_plaintext_message_chunk);

/**
 * \brief 		Process chunk decryption in AES CCM* mode
 * \details 	This service process additional chunk decryption in AES CCM* mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_decrypt_process(
    stse_handle_t *p_stse,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_plaintext_message_chunk);

/**
 * \brief 		Finish chunk decryption in AES CCM* mode
 * \details 	This service finish chunk encryption in AES CCM* mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	authentication_tag_length		Length of the output authentication tag
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - authentication_tag_length - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[in] 	p_authentication_tag 			Authentication tag
 * \param[out] 	p_verification_result 			Verification result flag
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_ccm_decrypt_finish(
    stse_handle_t *p_stse,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_authentication_tag,
    PLAT_UI8 *p_verification_result,
    PLAT_UI8 *p_plaintext_message_chunk);

/**
 * \brief 		Encrypt payload in AES GCM mode
 * \details 	This service format and send encrypt command in AES GCM mode
 * \param[in] 	p_stse 						Pointer to STSE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	IV_length					IV buffer length in bytes
 * \param[in]	p_iv							IV buffer
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	p_associated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to encrypt<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - IV_length - associated_data_length
 * \param[in]	p_plaintext_message			Buffer containing the message to encrypt
 * \param[out]	p_encrypted_message			Buffer to store the encrypted message
 * \param[out]	p_authentication_tag			Buffer to store the authentication tag
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_encrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 IV_length,
    PLAT_UI8 *p_iv,
    PLAT_UI16 associated_data_length,
    PLAT_UI8 *p_associated_data,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_plaintext_message,
    PLAT_UI8 *p_encrypted_message,
    PLAT_UI8 *p_authentication_tag);

/**
 * \brief 		Start chunk encryption in AES GCM mode
 * \details 	This service start chunk encryption in AES GCM mode using the specified key from STSAFE symmetric key table
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in]	IV_length						IV buffer length in bytes
 * \param[in]	p_iv								IV buffer
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to encrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - IV_length - associated_data_chunk_length
 * \param[in]	p_plaintext_message_chunk		Buffer containing the message chunk to encrypt
 * \param[out]	p_encrypted_message_chunk		Buffer to store the encrypted message
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_encrypt_start(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 IV_length,
    PLAT_UI8 *p_iv,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk);

/**
 * \brief 		Process chunk encryption in AES GCM mode
 * \details 	This service process additional chunk encryption in AES GCM mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to encrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in]	p_plaintext_message_chunk		Buffer containing the message chunk to encrypt
 * \param[out]	p_encrypted_message_chunk		Buffer to store the encrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_encrypt_process(
    stse_handle_t *p_stse,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk);

/**
 * \brief 		Finish chunk encryption in AES GCM mode
 * \details 	This service finish chunk encryption in AES GCM mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	authentication_tag_length		Length of the output authentication tag
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in] 	message_chunk_length 			Length of the message chunk<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in] 	p_plaintext_message_chunk		Buffer containing the message chunk to encrypt
 * \param[out] 	p_encrypted_message_chunk		Buffer to store the encrypted message chunk
 * \param[out] 	p_authentication_tag 			Authentication tag
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_encrypt_finish(
    stse_handle_t *p_stse,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_plaintext_message_chunk,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_authentication_tag);

/**
 * \brief 		Decrypt payload in AES GCM mode
 * \details 	This service format and send decrypt command in AES GCM mode
 * \param[in] 	p_stse 						Pointer to STSE Handler
 * \param[in] 	slot_number 				Key slot in symmetric key table to be used
 * \param[in] 	authentication_tag_length 	Expected length for the authentication tag
 * \param[in]	IV_length					IV buffer length in bytes
 * \param[in]	p_iv							IV buffer
 * \param[in]	associated_data_length		Length of the associated data
 * \param[in]	p_associated_data			Buffer containing associated data
 * \param[in]	message_length				Length of the message to decrypt<br>
 * message_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - authentication_tag_length - IV_length - associated_data_length
 * \param[in]	p_encrypted_message			Buffer containing the message to decrypt
 * \param[in]	p_authentication_tag			Buffer containing the authentication tag
 * \param[out]	p_verification_result		Verification result flag
 * \param[out]	p_plaintext_message			Buffer to store the decrypted message
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_decrypt(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 IV_length,
    PLAT_UI8 *p_iv,
    PLAT_UI16 associated_data_length,
    PLAT_UI8 *p_associated_data,
    PLAT_UI16 message_length,
    PLAT_UI8 *p_encrypted_message,
    PLAT_UI8 *p_authentication_tag,
    PLAT_UI8 *p_verification_result,
    PLAT_UI8 *p_plaintext_message);

/**
 * \brief 		Start chunk decryption in AES GCM mode
 * \details 	This service start chunk decryption in AES GCM mode using the specified key from STSAFE symmetric key table
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	slot_number 					Key slot in symmetric key table to be used
 * \param[in]	IV_length						IV buffer length in bytes
 * \param[in]	p_iv								IV buffer
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - IV_length - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_decrypt_start(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    PLAT_UI16 IV_length,
    PLAT_UI8 *p_iv,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_plaintext_message_chunk);

/**
 * \brief 		Process chunk decryption in AES GCM mode
 * \details 	This service process additional chunk decryption in AES GCM mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 8 - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_decrypt_process(
    stse_handle_t *p_stse,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_plaintext_message_chunk);

/**
 * \brief 		Finish chunk decryption in AES GCM mode
 * \details 	This service finish chunk encryption in AES GCM mode using the key specified in start command
 * \param[in] 	p_stse 							Pointer to STSE Handler
 * \param[in] 	authentication_tag_length		Length of the output authentication tag
 * \param[in]	associated_data_chunk_length	Length of the associated data chunk
 * \param[in]	p_associated_data_chunk			Buffer containing associated data chunk
 * \param[in]	message_chunk_length			Length of the message chunk to decrypt<br>
 * message_chunk_length maximum value (in bytes):
 * - STSAFE-A120 : STSAFEA_MAX_FRAME_LENGTH_A120 - 11 - authentication_tag_length - associated_data_chunk_length
 * \param[in]	p_encrypted_message_chunk		Buffer containing the message chunk to decrypt
 * \param[in] 	p_authentication_tag 			Authentication tag
 * \param[out] 	p_verification_result 			Verification result flag
 * \param[out]	p_plaintext_message_chunk		Buffer to store the decrypted message chunk
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafea_aes_gcm_decrypt_finish(
    stse_handle_t *p_stse,
    PLAT_UI8 authentication_tag_length,
    PLAT_UI16 associated_data_chunk_length,
    PLAT_UI8 *p_associated_data_chunk,
    PLAT_UI16 message_chunk_length,
    PLAT_UI8 *p_encrypted_message_chunk,
    PLAT_UI8 *p_authentication_tag,
    PLAT_UI8 *p_verification_result,
    PLAT_UI8 *p_plaintext_message_chunk);

/** \}*/

#endif /*STSAFEA_AES_H */
