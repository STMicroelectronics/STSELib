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
#include "core/stse_frame.h"
#include "core/stse_util.h"
#include "stse_platform_generic.h"

/*--------------------- STSAFE platform HAL functions --------------------------- */

/*!
 * \brief      Platform delay initialization callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_delay_init(void);

/*!
 * \brief      Platform power control initialization callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_power_init(void);

/*!
 * \brief      Platform CRC16 initialization callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_crc16_init(void);

/*!
 * \brief      Platform crypto library initialization callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_crypto_init(void);

/*!
 * \brief      Platform random number generation initialization callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_generate_random_init(void);

/*!
 * \brief      Platform generate random callback function
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
PLAT_UI32 stse_platform_generate_random(void);

/*!
 * \brief      Compute a 16-bit crc value on specific 8-bit buffer of buffer length
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_Crc16_Calculate(PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Accumulate an 8-bit buffer of buffer length in crc unit
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_Crc16_Accumulate(PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Perform a delay of "delay_val" ms
 * \param[in]  delay_val length of the delay in milliseconds
 * \return     None
 */
void stse_platform_Delay_ms(PLAT_UI32 delay_val);

/*!
 * \brief      Start a timeout counter of "timeout_val" ms
 * \param[in]  timeout_val length of the timeout in milliseconds
 * \return     None
 */
void stse_platform_timeout_ms_start(PLAT_UI16 timeout_val);

/*!
 * \brief      Return timeout status
 * \return     0 if the timeout counter is still running; 1 if the timeout was exceeded
 */
PLAT_UI8 stse_platform_timeout_ms_get_status(void);

/*!
 * \brief      Verify ECC signature
 * \param[in]  key_type Type of ECC key
 * \param[in]  pPubKey Pointer to the public key
 * \param[in]  pDigest Pointer to the digest
 * \param[in]  digestLen Length of the digest
 * \param[in]  pSignature Pointer to the signature
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_ecc_verify(stse_ecc_key_type_t key_type,
                                           const PLAT_UI8 *pPubKey,
                                           PLAT_UI8 *pDigest,
                                           PLAT_UI16 digestLen,
                                           PLAT_UI8 *pSignature);

/*!
 * \brief      Compute hash using specified algorithm
 * \param[in]  hash_algo Hash algorithm to use
 * \param[in]  pPayload Pointer to the payload
 * \param[in]  payload_length Length of the payload
 * \param[out] pHash Pointer to the hash output buffer
 * \param[out] hash_length Pointer to the length of the hash output buffer
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_hash_compute(stse_hash_algorithm_t hash_algo,
                                             PLAT_UI8 *pPayload, PLAT_UI16 payload_length,
                                             PLAT_UI8 *pHash, PLAT_UI16 *hash_length);

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||                      \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED) ||               \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) ||                 \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) ||          \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/*!
 * \brief      Generate ECC key pair
 * \param[in]  key_type Type of ECC key
 * \param[out] pPrivKey Pointer to the private key buffer
 * \param[out] pPubKey Pointer to the public key buffer
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_ecc_generate_key_pair(stse_ecc_key_type_t key_type,
                                                      PLAT_UI8 *pPrivKey,
                                                      PLAT_UI8 *pPubKey);
#endif

#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/*!
 * \brief      Sign data using ECC
 * \param[in]  key_type Type of ECC key
 * \param[in]  pPrivKey Pointer to the private key
 * \param[in]  pDigest Pointer to the digest
 * \param[in]  digestLen Length of the digest
 * \param[out] pSignature Pointer to the signature buffer
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_ecc_sign(stse_ecc_key_type_t key_type,
                                         PLAT_UI8 *pPrivKey,
                                         PLAT_UI8 *pDigest,
                                         PLAT_UI16 digestLen,
                                         PLAT_UI8 *pSignature);

#endif

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||                      \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED) ||               \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) ||                 \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) ||          \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/*!
 * \brief      Perform ECC ECDH key exchange
 * \param[in]  key_type Type of ECC key
 * \param[in]  pPubKey Pointer to the public key
 * \param[in]  pPrivKey Pointer to the private key
 * \param[out] pSharedSecret Pointer to the shared secret buffer
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_ecc_ecdh(stse_ecc_key_type_t key_type,
                                         const PLAT_UI8 *pPubKey,
                                         const PLAT_UI8 *pPrivKey,
                                         PLAT_UI8 *pSharedSecret);
#endif

#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED) ||               \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) ||          \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/**
 * \brief 		Encrypt data using NIST AES Key Wrap algorithm
 * \details 	This platform function implements the NIST SP 800-38F AES Key Wrap encryption
 * \param[in]	pPayload			Pointer to the payload data to encrypt
 * \param[in]	payload_length		Length of the payload in bytes
 * \param[in]	pKey				Pointer to the encryption key
 * \param[in]	key_length			Length of the key in bytes
 * \param[out]	pOutput				Pointer to the output buffer for encrypted data
 * \param[out]	pOutput_length		Pointer to store the output length
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_nist_kw_encrypt(PLAT_UI8 *pPayload, PLAT_UI32 payload_length,
                                                PLAT_UI8 *pKey, PLAT_UI8 key_length,
                                                PLAT_UI8 *pOutput, PLAT_UI32 *pOutput_length);
#endif

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION)

/*!
 * \brief      Initialize AES CMAC computation
 * \param[in]  pKey Pointer to the key
 * \param[in]  key_length Length of the key
 * \param[in]  exp_tag_size Expected tag size
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_init(const PLAT_UI8 *pKey,
                                              PLAT_UI16 key_length,
                                              PLAT_UI16 exp_tag_size);

/*!
 * \brief      Append data to AES CMAC computation
 * \param[in]  pInput Pointer to the input data
 * \param[in]  length Length of the input data
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_append(PLAT_UI8 *pInput, PLAT_UI16 length);

/*!
 * \brief      Finish AES CMAC computation and get the tag
 * \param[out] pTag Pointer to the tag buffer
 * \param[out] pTagLen Pointer to the tag length
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_compute_finish(PLAT_UI8 *pTag, PLAT_UI8 *pTagLen);

/*!
 * \brief      Finish AES CMAC verification
 * \param[in]  pTag Pointer to the tag
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_verify_finish(PLAT_UI8 *pTag);

/*!
 * \brief      Perform an AES CMAC encryption
 * \param[in]  pPayload Pointer to Payload
 * \param[in]  payload_length Length of the payload in bytes
 * \param[in]  pKey Pointer to key
 * \param[in]  key_length Length of the key in bytes
 * \param[in]  exp_tag_size Expected tag size in bytes
 * \param[out] pTag Pointer to Tag
 * \param[out] pTag_length Pointer to Tag length value output
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_compute(const PLAT_UI8 *pPayload, PLAT_UI16 payload_length,
                                                 const PLAT_UI8 *pKey, PLAT_UI16 key_length,
                                                 PLAT_UI16 exp_tag_size,
                                                 PLAT_UI8 *pTag, PLAT_UI16 *pTag_length);

/*!
 * \brief      Perform an AES CMAC decryption
 * \param[in]  pPayload Pointer to Payload
 * \param[in]  payload_length Length of the payload in bytes
 * \param[in]  pKey Pointer to key
 * \param[in]  key_length Length of the key in bytes
 * \param[in]  pTag Pointer to Tag
 * \param[in]  tag_length Pointer to Tag length value output
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cmac_verify(const PLAT_UI8 *pPayload, PLAT_UI16 payload_length,
                                                const PLAT_UI8 *pKey, PLAT_UI16 key_length,
                                                const PLAT_UI8 *pTag, PLAT_UI16 tag_length);

/*!
 * \brief      Perform an AES CBC encryption
 * \param[in]  pPlaintext Pointer to the plaintext data
 * \param[in]  plaintext_length Length of the plaintext data
 * \param[in]  pInitial_value Pointer to encryption IV
 * \param[in]  pKey Pointer to the key
 * \param[in]  key_length Length of the key
 * \param[out] pEncryptedtext Pointer to the encrypted payload
 * \param[out] pEncryptedtext_length Length of encrypted payload
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cbc_enc(const PLAT_UI8 *pPlaintext, PLAT_UI16 plaintext_length,
                                            PLAT_UI8 *pInitial_value, const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length, PLAT_UI8 *pEncryptedtext,
                                            PLAT_UI16 *pEncryptedtext_length);

/*!
 * \brief      Perform an AES CBC decryption
 * \param[in]  pEncryptedtext Pointer to the encrypted payload
 * \param[in]  encryptedtext_length Length of encrypted payload
 * \param[in]  pInitial_value Pointer to decryption IV
 * \param[in]  pKey Pointer to the key
 * \param[in]  key_length Length of the key
 * \param[out] pPlaintext Pointer to PlainText payload
 * \param[out] pPlaintext_length Length of the PlainText payload
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_cbc_dec(const PLAT_UI8 *pEncryptedtext, PLAT_UI16 encryptedtext_length,
                                            PLAT_UI8 *pInitial_value, const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length, PLAT_UI8 *pPlaintext,
                                            PLAT_UI16 *pPlaintext_length);

/*!
 * \brief      Perform an AES ECB encryption
 * \param[in]  pPlaintext Pointer to the plaintext data
 * \param[in]  plaintext_length Length of the plaintext data
 * \param[in]  pKey Pointer to the key
 * \param[in]  key_length Length of the key
 * \param[out] pEncryptedtext Pointer to the encrypted payload
 * \param[out] pEncryptedtext_length Length of encrypted payload
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_aes_ecb_enc(const PLAT_UI8 *pPlaintext, PLAT_UI16 plaintext_length,
                                            const PLAT_UI8 *pKey, PLAT_UI16 key_length,
                                            PLAT_UI8 *pEncryptedtext, PLAT_UI16 *pEncryptedtext_length);

#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION) */

/*!
 *  \brief Perform a NIST KW (keywrap) encrypt
 *  \param[in]  pPayload 				Pointer to payload
 *  \param[in]  payload_length 			Length of payload
 *  \param[in]  pKey 					Pointer to the key
 *  \param[in]  key_length 				Length of the key
 *  \param[out] pOutput 				Pointer to encrypted output
 *  \param[out] pOutput_length 			Length of the encrypted output
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_nist_kw_encrypt(PLAT_UI8 *pPayload, PLAT_UI32 payload_length,
                                                PLAT_UI8 *pKey, PLAT_UI8 key_length,
                                                PLAT_UI8 *pOutput, PLAT_UI32 *pOutput_length);

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
* \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
*/
stse_ReturnCode_t stse_platform_hmac_sha256_compute(PLAT_UI8 *pSalt, PLAT_UI16 salt_length,
                                                    PLAT_UI8 *pInput_keying_material, PLAT_UI16 input_keying_material_length,
                                                    PLAT_UI8 *pInfo, PLAT_UI16 info_length,
                                                    PLAT_UI8 *pOutput_keying_material, PLAT_UI16 output_keying_material_length);

/*!
*  \brief Perform an HMAC extract using SHA256 (cf. RFC 5869)
*  \param[in] 		pSalt 								HMAC salt
*  \param[in] 		salt_length 						HMAC salt length
*  \param[in] 		pInput_keying_material 				Input keying material (IKM), could be ECDH output
*  \param[in] 		input_keying_material_length 		Input keying material length
*  \param[out] 	pPseudorandom_key 					Pseudorandom key (PRK)
*  \param[in] 		pseudorandom_key_expected_length 	Pseudorandom key length
* \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
*/
stse_ReturnCode_t stse_platform_hmac_sha256_extract(PLAT_UI8 *pSalt, PLAT_UI16 salt_length,
                                                    PLAT_UI8 *pInput_keying_material, PLAT_UI16 input_keying_material_length,
                                                    PLAT_UI8 *pPseudorandom_key, PLAT_UI16 pseudorandom_key_expected_length);

/*!
*  \brief Perform an HMAC expand using SHA256 (cf. RFC 5869)
*  \param[in] 		pPseudorandom_key 				Pseudorandom key (PRK)
*  \param[in] 		pseudorandom_key_length			Pseudorandom key length
*  \param[in] 		pInfo 							Application specific information
*  \param[in] 		info_length 					Information length
*  \param[out] 	pOutput_keying_material 		Output keying material (OKM)
*  \param[in] 		output_keying_material_length 	Output keying material expected length
* \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
*/
stse_ReturnCode_t stse_platform_hmac_sha256_expand(PLAT_UI8 *pPseudorandom_key, PLAT_UI16 pseudorandom_key_length,
                                                   PLAT_UI8 *pInfo, PLAT_UI16 info_length,
                                                   PLAT_UI8 *pOutput_keying_material, PLAT_UI16 output_keying_material_length);

/*!
*  \brief Platform Abstraction function for STSAFE power control initialization
*/
stse_ReturnCode_t stse_platform_power_ctrl_init(void);

/*!
*  \brief Platform Abstraction function for target STSAFE power-on
*  \param[in]  busID Target STSAFE bus ID
*  \param[in]  devAddr Target STSAFE device address
*/
stse_ReturnCode_t stse_platform_power_on(PLAT_UI8 busID, PLAT_UI8 devAddr);

/*!
 *  \brief Platform Abstraction function for target STSAFE power-off
 *  \param[in]  busID Target STSAFE bus ID
 *  \param[in]  devAddr Target STSAFE device address
 */
stse_ReturnCode_t stse_platform_power_off(PLAT_UI8 busID, PLAT_UI8 devAddr);

/*!
 * \brief      Initialize I2C communication
 * \param[in]  busID I2C bus ID
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_init(PLAT_UI8 busID);

/*!
 * \brief      Send data over I2C
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pFrame Pointer to the data frame
 * \param[in]  FrameLength Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_send(PLAT_UI8 busID,
                                         PLAT_UI8 devAddr,
                                         PLAT_UI16 speed,
                                         PLAT_UI8 *pFrame,
                                         PLAT_UI16 FrameLength);

/*!
 * \brief      Receive data over I2C
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pFrame_header Pointer to the frame header
 * \param[out] pFrame_payload Pointer to the frame payload
 * \param[out] pFrame_payload_Length Pointer to the length of the frame payload
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_receive(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pFrame_header,
    PLAT_UI8 *pFrame_payload,
    PLAT_UI16 *pFrame_payload_Length);

/*!
 * \brief      Wake up I2C device
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_wake(PLAT_UI8 busID,
                                         PLAT_UI8 devAddr,
                                         PLAT_UI16 speed);

/*!
 * \brief      Start I2C send operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  FrameLength Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_send_start(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI16 FrameLength);

/*!
 * \brief      Continue I2C send operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pElement Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_send_continue(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pElement,
    PLAT_UI16 element_size);

/*!
 * \brief      Stop I2C send operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pElement Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_send_stop(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pElement,
    PLAT_UI16 element_size);

/*!
 * \brief      Start I2C receive operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  frame_Length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_receive_start(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI16 frame_Length);

/*!
 * \brief      Continue I2C receive operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pElement Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_receive_continue(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pElement,
    PLAT_UI16 element_size);

/*!
 * \brief      Stop I2C receive operation
 * \param[in]  busID I2C bus ID
 * \param[in]  devAddr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  pElement Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_i2c_receive_stop(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pElement,
    PLAT_UI16 element_size);

/*!
 * \brief      Initialize 1-wire communication
 * \param[in]  busID 1-wire bus ID
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_init(PLAT_UI8 busID);

/*!
 * \brief      Wake up 1-wire device
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_wake(PLAT_UI8 busID,
                                             PLAT_UI8 devAddr,
                                             PLAT_UI16 speed);

/*!
 * \brief      Start 1-wire send operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  FrameLength Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_send_start(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI16 FrameLength);

/*!
 * \brief      Continue 1-wire send operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  pData Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_send_continue(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pData,
    PLAT_UI16 data_size);

/*!
 * \brief      Stop 1-wire send operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  pData Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_send_stop(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pData,
    PLAT_UI16 data_size);

/*!
 * \brief      Start 1-wire receive operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  frameLength Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_receive_start(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI16 frameLength);

/*!
 * \brief      Continue 1-wire receive operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  pData Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_receive_continue(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pData,
    PLAT_UI16 data_size);

/*!
 * \brief      Stop 1-wire receive operation
 * \param[in]  busID 1-wire bus ID
 * \param[in]  devAddr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  pData Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_platform_st1wire_receive_stop(
    PLAT_UI8 busID,
    PLAT_UI8 devAddr,
    PLAT_UI16 speed,
    PLAT_UI8 *pData,
    PLAT_UI16 data_size);

/** @}*/

#endif /*STSE_PLATFORM_H*/
