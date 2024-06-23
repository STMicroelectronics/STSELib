/*!
 ******************************************************************************
 * \file	stse_platform.h
 * \brief   STSAFE-Axxx Middleware Platform Abstraction layer (header)
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
/**
 *  \defgroup stse_platform Platform Abstraction Layer (PAL)
 *  \ingroup 	stse_core
 *  \brief		STSAFE Middleware Platform Abstraction Layer
 *  \details  	The STSAFE Middleware Platform Abstraction Layer gather all functions used for adapting the Platform specific Hardware and Software  . \n
 *  			The function declared in the middleware platform abstraction layer must be adaped by the system developer to fit with target application
 *  			HW/SW specificty . Please refer to PAL function description to get more details on expected function behavior and recommended implementation
 *  @{
 */

#ifndef STSE_PLATFORM_H
#define STSE_PLATFORM_H

#include "core/stse_device.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"

/*--------------------- STSAFE platform HAL functions --------------------------- */

/*!
 * \brief      STSAFE Low level Platform initialization function
 */
stse_ReturnCode_t stse_services_platform_init (void);

stse_ReturnCode_t stse_crypto_platform_init (void);

/*!
 * \brief      Compute a 16-bit crc value on specific 8-bit buffer of buffer length
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_Crc16_Calculate (PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Accumulate an 8-bit buffer of buffer length in crc unit
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_Crc16_Accumulate (PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Perform a delay of "delay_val" ms
 * \param[in]  delay_val length of the delay in milliseconds
 * \return     None
 */
void stse_platform_Delay_ms (PLAT_UI32 delay_val);

/*!
  * \brief      Start a timeout counter of "timeout_val" ms
  * \param[in]  timeout_val length of the timeout in milliseconds
  * \return     None
  */
void stse_platform_timeout_ms_start (PLAT_UI16 timeout_val);

/*!
  * \brief      Return timeout status
  * \return     0 if the timeout counter is still running
  * 			1 if the timeout was excedeed
  */
PLAT_UI8 stse_platform_timeout_ms_get_status (void);
/*!
  * \brief      Write data from input buffer into Non Volatile Memory
  * \param[in]  nvmAddr PLAT_UI32 address in NVM
  * \param[in]  bufferAddr pointer to input buffer
  * \param[in]  bufferLength input buffer length
  * \return     None
  */
stse_ReturnCode_t stse_platform_nvm_write (PLAT_UI32 nvmAddr,
								  PLAT_UI32 bufferAddr,
								  PLAT_UI32 bufferLength);

/*!
 * \brief      Generate a PLAT_UI32 random number
 * \return     32-bit random number
 */
PLAT_UI32 stse_platform_Random (void);


stse_ReturnCode_t stse_platform_aes_keywrap_encrypt(PLAT_UI8 *pPayload, 	PLAT_UI32 payload_length,
										 PLAT_UI8 *pKey,		PLAT_UI8 key_length,
										 PLAT_UI8 *pIV,			PLAT_UI8 iv_length,
										 PLAT_UI8 *pOutput, 	PLAT_UI32 *pOutput_length);


stse_ReturnCode_t stse_platform_hash_compute(stse_hash_algorithm_t hash_algo,
									  PLAT_UI8 *pPayload, PLAT_UI32 payload_length,
									  PLAT_UI8 *pHash, PLAT_UI32 *hash_length);

stse_ReturnCode_t stse_platform_ecc_generate_key_pair(
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPrivKey,
		PLAT_UI8 *pPubKey);

stse_ReturnCode_t stse_platform_ecc_sign(
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPrivKey,
		PLAT_UI8 *pDigest,
		PLAT_UI16 digestLen,
		PLAT_UI8 *pSignature);

stse_ReturnCode_t stse_platform_ecc_verify(
		stse_ecc_key_type_t key_type,
		const PLAT_UI8 *pPubKey,
		PLAT_UI8 *pDigest,
		PLAT_UI16 digestLen,
		PLAT_UI8 *pSignature);

stse_ReturnCode_t stse_platform_ecc_ecdh(
		stse_ecc_key_type_t key_type,
		const PLAT_UI8 *pPubKey,
		const PLAT_UI8 *pPrivKey,
		PLAT_UI8       *pSharedSecret);


/*!
 *  \brief Perform an AES CMAC encryption
 *  \param[in] 		*pPayload 		Pointer to Payload
 *  \param[in] 		payload_length 	Length of the payload in byte
 *  \param[in] 		*pKey 			Pointer to key
 *  \param[in] 		key_length 		Length of the key in byte
 *  \param[in] 		exp_tag_size 	Expected  tag size in byte
 *  \param[out] 	*pTag			Pointer to Tag
 *  \param[out]  	*pTag_length	Pointer to Tag length value output
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_compute(const PLAT_UI8 *pPayload, 	 PLAT_UI16   payload_length,
		const PLAT_UI8 *pKey, 		 PLAT_UI16   key_length,
		PLAT_UI16       exp_tag_size,
		PLAT_UI8       *pTag, 		 PLAT_UI16 *pTag_length);

/*!
 *  \brief Perform an AES CMAC decryption
 *  \param[in] 		*pPayload 		Pointer to Payload
 *  \param[in] 		payload_length 	Length of the payload in byte
 *  \param[in] 		*pKey 			Pointer to key
 *  \param[in] 		key_length 		Length of the key in byte
 *  \param[in]		*pTag			Pointer to Tag
 *  \param[in]  	tag_length		Pointer to Tag length value output
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_verify(const PLAT_UI8 *pPayload, PLAT_UI16 payload_length,
		const PLAT_UI8 *pKey, 	 PLAT_UI16 key_length,
		const PLAT_UI8 *pTag, 	 PLAT_UI16 tag_length);


stse_ReturnCode_t stse_platform_aes_cmac_init (const PLAT_UI8 	*pKey,
		PLAT_UI16 		key_length,
		PLAT_UI16 		exp_tag_size);
stse_ReturnCode_t stse_platform_aes_cmac_append(PLAT_UI8* pInput, PLAT_UI16 lenght);
stse_ReturnCode_t stse_platform_aes_cmac_compute_finish(PLAT_UI8* pTag, PLAT_UI8* pTagLen);
stse_ReturnCode_t stse_platform_aes_cmac_verify_finish(PLAT_UI8* pTag);


/*!
 *  \brief Perform an AES CCM encryption
 *  \param[in]  *pPlaintext 			Pointer to the plaintext data
 *  \param[in]  plaintext_length 		Length of the plaintext data
 *  \param[in]  *pKey 					Pointer to the key
 *  \param[in]  key_length 				Length of the key
 *  \param[in]  *pNonce 				Pointer to Nonce
 *  \param[in]  nonce_length 			Length of the Nonce
 *  \param[in]  *pAssocData 			Pointer to Associated Data
 *  \param[in]	assocData_length 		Length of the Associated Data
 *  \param[out] *pEncryptedtext 		Pointer to the encrypted payload
 *  \param[out] *pEncryptedtext_length 	Length of encrypted payload
 *  \param[out] *pTag 					Pointer to the tag
 *  \param[out] tag_length 				Length of the tag
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_ccm_enc(const PLAT_UI8 *pPlaintext, 	  PLAT_UI16  plaintext_length,
		const PLAT_UI8 *pKey, 		  PLAT_UI16  key_length,
		const PLAT_UI8 *pNonce, 		  PLAT_UI16  nonce_length,
		const PLAT_UI8 *pAssocData, 	  PLAT_UI16  assocData_length,
		PLAT_UI8       *pEncryptedtext, PLAT_UI16 *pEncryptedtext_length,
		PLAT_UI8       *pTag, 		  PLAT_UI16  tag_length);

/*!
 *  \brief
 *  \param[in]  *pEncryptedtext Pointer to the encrypted payload
 *  \param[in]  encryptedtext_length Length of encrypted payload
 *  \param[in]  *pTag Pointer to the tag
 *  \param[in]  tag_length Length of the tag
 *  \param[in]  *pKey pointer to the key
 *  \param[in]  key_length Length of the key
 *  \param[in]  *pNonce pointer to Nonce
 *  \param[in]  nonce_length Length of the Nonce
 *  \param[in]  *pAssocData pointer to Associated Data
 *  \param[in]  assocData_length Length of the Associated Data
 *  \param[out] *pPlaintext pointer to PlainText payload
 *  \param[out] plaintext_length Length of the PlainText payload
 *  \return
 */
stse_ReturnCode_t stse_platform_aes_ccm_dec(const PLAT_UI8 *pEncryptedtext, PLAT_UI16 encryptedtext_length,
		const PLAT_UI8 *pTag, 		  PLAT_UI16 tag_length,
		const PLAT_UI8 *pKey, 		  PLAT_UI16 key_length,
		const PLAT_UI8 *pNonce, 		  PLAT_UI16 nonce_length,
		const PLAT_UI8 *pAssocData, 	  PLAT_UI16 assocData_length,
		const PLAT_UI8 *pPlaintext, 	  PLAT_UI16 plaintext_length);

/*!
 *  \brief Perform an AES CBC encryption
 *  \param[in]  pPlaintext 			Pointer to the plaintext data
 *  \param[in]  plaintext_length 		Length of the plaintext data
 *  \param[in]  pInitial_value 			Pointer to encryption IV
 *  \param[in]  pKey 					Pointer to the key
 *  \param[in]  key_length 				Length of the key
 *  \param[out] pEncryptedtext 		Pointer to the encrypted payload
 *  \param[out] pEncryptedtext_length 	Length of encrypted payload
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cbc_enc(const PLAT_UI8 *pPlaintext,
		PLAT_UI16  plaintext_length,
		PLAT_UI8 *pInitial_value,
		const PLAT_UI8 *pKey,
		PLAT_UI16  key_length,
		PLAT_UI8  *pEncryptedtext,
		PLAT_UI16 *pEncryptedtext_length);

/*!
 *  \brief Perform an AES CBC decryption
 *  \param[in]  pEncryptedtext Pointer to the encrypted payload
 *  \param[in]  encryptedtext_length Length of encrypted payload
 *  \param[in]  pInitial_value Pointer to decryption IV
 *  \param[in]  pKey pointer to the key
 *  \param[in]  key_length Length of the key
 *  \param[out] pPlaintext pointer to PlainText payload
 *  \param[out] pPlaintext_length Length of the PlainText payload
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cbc_dec(const PLAT_UI8 *pEncryptedtext,
		PLAT_UI16  encryptedtext_length,
		PLAT_UI8 *pInitial_value,
		const PLAT_UI8 *pKey,
		PLAT_UI16  key_length,
		PLAT_UI8  *pPlaintext,
		PLAT_UI16 *pPlaintext_length);

/*!
 *  \brief Perform an AES ECB encryption
 *  \param[in]  pPlaintext 			Pointer to the plaintext data
 *  \param[in]  plaintext_length 		Length of the plaintext data
 *  \param[in]  pKey 					Pointer to the key
 *  \param[in]  key_length 				Length of the key
 *  \param[out] pEncryptedtext 		Pointer to the encrypted payload
 *  \param[out] pEncryptedtext_length 	Length of encrypted payload
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_ecb_enc(const PLAT_UI8 *pPlaintext, 	  PLAT_UI16  plaintext_length,
		const PLAT_UI8 *pKey, 		  PLAT_UI16  key_length,
		PLAT_UI8       *pEncryptedtext, PLAT_UI16 *pEncryptedtext_length);


/*!
 *  \brief Perform an AES ECB decryption
 *  \param[in]  pEncryptedtext Pointer to the encrypted payload
 *  \param[in]  encryptedtext_length Length of encrypted payload
 *  \param[in]  pKey pointer to the key
 *  \param[in]  key_length Length of the key
 *  \param[out] pPlaintext pointer to PlainText payload
 *  \param[out] pPlaintext_length Length of the PlainText payload
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_ecb_dec(const PLAT_UI8 *pEncryptedtext, PLAT_UI16  encryptedtext_length,
								const PLAT_UI8 	*pKey, 		  	PLAT_UI16  key_length,
								PLAT_UI8 		*pPlaintext, 	PLAT_UI16 *pPlaintext_length);

/*!
 *  \brief Perform a NIST KW (keywrap) encrypt
 *  \param[in]  pPayload 				Pointer to payload
 *  \param[in]  payload_length 			Length of payload
 *  \param[in]  pKey 					Pointer to the key
 *  \param[in]  key_length 				Length of the key
 *  \param[out] pOutput 				Pointer to encrypted output
 *  \param[out] pOutput_length 			Length of the encrypted output
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_nist_kw_encrypt(PLAT_UI8 *pPayload, PLAT_UI32 payload_length,
												PLAT_UI8 *pKey,		PLAT_UI8 key_length,
												PLAT_UI8 *pOutput, 	PLAT_UI32 *pOutput_length);

/*!
 *  \brief Perform an HMAC Key derivation using HMAC extract then HMAC expand functions (cf. RFC 5869)
 *  \param[in] 		pSalt 							HMAC salt
 *  \param[in] 		salt_length 					HMAC salt length
 *  \param[in] 		pInput_keying_material 		Input keying material (IKM), could be ECDH output
 *  \param[in] 		input_keying_material_length 	Input keying material length
 *  \param[in] 		pInfo 							Application specific information
 *  \param[in] 		info_length 					Information length
 *  \param[out] 	pOutput_keying_material 		Output keying material (OKM)
 *  \param[in] 		output_keying_material_length 	Output keying material expected length
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_hmac_sha256_compute(PLAT_UI8 *pSalt,			PLAT_UI16 salt_length,
											 PLAT_UI8 *pInput_keying_material, 	PLAT_UI16 input_keying_material_length,
											 PLAT_UI8 *pInfo, 					PLAT_UI16 info_length,
											 PLAT_UI8 *pOutput_keying_material, PLAT_UI16 output_keying_material_length);

/*!
 *  \brief Perform an HMAC extract using SHA256 (cf. RFC 5869)
 *  \param[in] 		pSalt 								HMAC salt
 *  \param[in] 		salt_length 						HMAC salt length
 *  \param[in] 		pInput_keying_material 				Input keying material (IKM), could be ECDH output
 *  \param[in] 		input_keying_material_length 		Input keying material length
 *  \param[out] 	pPseudorandom_key 					Pseudorandom key (PRK)
 *  \param[in] 		pseudorandom_key_expected_length 	Pseudorandom key length
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_hmac_sha256_extract(PLAT_UI8 *pSalt,			PLAT_UI16 salt_length,
											 PLAT_UI8 *pInput_keying_material, 	PLAT_UI16 input_keying_material_length,
											 PLAT_UI8 *pPseudorandom_key, 		PLAT_UI16 pseudorandom_key_expected_length);

/*!
 *  \brief Perform an HMAC expand using SHA256 (cf. RFC 5869)
 *  \param[in] 		pPseudorandom_key 				Pseudorandom key (PRK)
 *  \param[in] 		pseudorandom_key_length			Pseudorandom key length
 *  \param[in] 		pInfo 							Application specific information
 *  \param[in] 		info_length 					Information length
 *  \param[out] 	pOutput_keying_material 		Output keying material (OKM)
 *  \param[in] 		output_keying_material_length 	Output keying material expected length
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_hmac_sha256_expand(PLAT_UI8  *pPseudorandom_key, 		PLAT_UI16 pseudorandom_key_length,
											PLAT_UI8  *pInfo, 					PLAT_UI16 info_length,
											PLAT_UI8  *pOutput_keying_material, PLAT_UI16 output_keying_material_length);

/*!
 *  \brief Perform an HASH using SHA256
 *  \param[in] 		pPayload 						Payload to hash
 *  \param[in] 		payload_length					Payload length
 *  \param[out] 	pHash 							Hash output buffer
 *  \param[in] 		hash_length						Expected hash length
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_platform_hash_sha256_compute(PLAT_UI8 *pPayload, PLAT_UI32 payload_length,
											 PLAT_UI8 *pHash, PLAT_UI32 hash_length);

/*!
 *  \brief Platform Abstraction function for STSAFE power control initialization
 */
stse_ReturnCode_t stse_platform_power_ctrl_init (void);

/*!
 *  \brief Platform Abstraction function for target STSAFE power-on
 *  \param[in]  busID Target STSAFE bus ID
 *  \param[in]  devAddr Target STSAFE device address
 */
stse_ReturnCode_t stse_platform_power_on (PLAT_UI8 busID , PLAT_UI8 devAddr);

/*!
 *  \brief Platform Abstraction function for target STSAFE power-off
 *  \param[in]  busID Target STSAFE bus ID
 *  \param[in]  devAddr Target STSAFE device address
 */
stse_ReturnCode_t stse_platform_power_off (PLAT_UI8 busID , PLAT_UI8 devAddr);

stse_ReturnCode_t stse_platform_i2c_init (PLAT_UI8 busID);

stse_ReturnCode_t stse_platform_i2c_send (PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pFrame,
		PLAT_UI16 FrameLength);

stse_ReturnCode_t stse_platform_i2c_receive (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pFrame_header,
		PLAT_UI8* pFrame_payload,
		PLAT_UI16* pFrame_payload_Length);

stse_ReturnCode_t stse_platform_i2c_wake (PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed);


stse_ReturnCode_t stse_platform_i2c_send_start (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI16 FrameLength);

stse_ReturnCode_t stse_platform_i2c_send_continue (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pElement,
		PLAT_UI16 element_size);

stse_ReturnCode_t stse_platform_i2c_send_stop (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pElement,
		PLAT_UI16 element_size);

stse_ReturnCode_t stse_platform_i2c_receive_start (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI16 *pFrame_Length);

stse_ReturnCode_t stse_platform_i2c_receive_continue (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pElement,
		PLAT_UI16 element_size);

stse_ReturnCode_t stse_platform_i2c_receive_stop (
		PLAT_UI8 busID,
		PLAT_UI8 devAddr,
		PLAT_UI16 speed,
		PLAT_UI8* pElement,
		PLAT_UI16 element_size);


/** @}*/


/**
 *  defgroup stsafe_port How to port the middleware
 *  ingroup 	stse
 *  brief  	The Platform Abstraction Layer gathers all functions intended for adapting the Middleware with the target platform specific Hardware and Software. \n
 *  			The function declared in the Middleware platform abstraction layer must be adapted by the system developer to fit with target application
 *  			HW/SW specificities . Please refer to PAL function description to get more details on expected function behavior and recommended implementation
 *  details
 *  include{doc} stsafe_port.dox
 */

#endif /*STSAFE_PLATFORM_H*/
