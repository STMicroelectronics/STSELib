/**
  ******************************************************************************
  * @file    stse_platform.h
  * @author  CS Application Team
  * @brief   STSELib callback functions declaration for platform Abstraction layer (header)
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

/**
  *  \defgroup stse_platform Platform Abstraction Layer (PAL)
  *  \ingroup  stse_core
  *  \brief    STSAFE Middleware Platform Abstraction Layer
  *  \details    The STSAFE Middleware Platform Abstraction Layer gather all functions used for adapting the
  *        Platform specific Hardware and Software  . \n
  *        The function declared in the middleware platform abstraction layer must be adapted by the system
  *        developer to fit with target application HW/SW specificty . Please refer to PAL function description to
  *        get more details on expected function behavior and recommended implementation
  *  @{
  */

#ifndef STSE_PLATFORM_H
#define STSE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_util.h"
#include "stse_platform_generic.h"

/*--------------------- STSAFE platform HAL functions --------------------------- */

/*!
 * \brief      Platform delay initialization callback function
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_delay_init(void *p_arg);

/*!
 * \brief      Platform power control initialization callback function
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_power_init(void *p_arg);

/*!
 * \brief      Platform CRC16 initialization callback function
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_crc16_init(void *p_arg);

/*!
 * \brief      Platform crypto library initialization callback function
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_crypto_init(void *p_arg);

/*!
 * \brief      Platform random number generation initialization callback function
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_generate_random_init(void *p_arg);

/*!
 * \brief      Platform log abstraction
 */
void stse_platform_printf(const char *format, ...);

/*!
 * \brief      Platform generate random callback function
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
PLAT_UI32 stse_platform_generate_random(void);

/*!
 * \brief      Compute a 16-bit crc value on specific 8-bit buffer of buffer length
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_crc16_calculate(PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Accumulate an 8-bit buffer of buffer length in crc unit
 * \param[in]  pbuffer pointer to crc input buffer
 * \param[in]  length  input buffer length
 * \return     16-bit CRC value
 */
PLAT_UI16 stse_platform_crc16_accumulate(PLAT_UI8 *pbuffer, PLAT_UI16 length);

/*!
 * \brief      Perform a delay of "delay_val" ms
 * \param[in]  delay_val length of the delay in milliseconds
 */
void stse_platform_delay_ms(PLAT_UI16 delay_val);

/*!
 * \brief      Verify ECC signature
 * \param[in]  key_type Type of ECC key
 * \param[in]  p_pub_key Pointer to the public key
 * \param[in]  p_digest Pointer to the digest
 * \param[in]  digest_len Length of the digest
 * \param[in]  p_signature Pointer to the signature
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_ecc_verify(stse_ecc_key_type_t key_type,
                                            const PLAT_UI8 *p_pub_key,
                                            PLAT_UI8 *p_digest,
                                            PLAT_UI16 digest_len,
                                            PLAT_UI8 *p_signature);

/*!
 * \brief      Compute hash using specified algorithm
 * \param[in]  hash_algo Hash algorithm to use
 * \param[in]  p_payload Pointer to the payload
 * \param[in]  payload_length Length of the payload
 * \param[out] p_hash Pointer to the hash output buffer
 * \param[out] hash_length Pointer to the length of the hash output buffer
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_hash_compute(stse_hash_algorithm_t hash_algo,
                                              PLAT_UI8 *p_payload, PLAT_UI16 payload_length,
                                              PLAT_UI8 *p_hash, PLAT_UI16 *hash_length);

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
 * \param[out] p_priv_key Pointer to the private key buffer
 * \param[out] p_pub_key Pointer to the public key buffer
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_ecc_generate_key_pair(stse_ecc_key_type_t key_type,
                                                       PLAT_UI8 *p_priv_key,
                                                       PLAT_UI8 *p_pub_key);
#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||                      \ */

#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/*!
 * \brief      sign data using ECC
 * \param[in]  key_type Type of ECC key
 * \param[in]  p_priv_key Pointer to the private key
 * \param[in]  p_digest Pointer to the digest
 * \param[in]  digest_len Length of the digest
 * \param[out] p_signature Pointer to the signature buffer
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 * \warning Few specific cryptographic library required to have public key concatenated to private key
 *          for EdDSA mechanism. In such case, p_priv_key pointer shall reference concatenated key pair
 *          buffer's address.
 */
stse_return_code_t stse_platform_ecc_sign(stse_ecc_key_type_t key_type,
                                          PLAT_UI8 *p_priv_key,
                                          PLAT_UI8 *p_digest,
                                          PLAT_UI16 digest_len,
                                          PLAT_UI8 *p_signature);

#endif /* defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \ */

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
 * \param[in]  p_pub_key Pointer to the public key
 * \param[in]  p_priv_key Pointer to the private key
 * \param[out] p_shared_secret Pointer to the shared secret buffer
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_ecc_ecdh(stse_ecc_key_type_t key_type,
                                          const PLAT_UI8 *p_pub_key,
                                          const PLAT_UI8 *p_priv_key,
                                          PLAT_UI8 *p_shared_secret);
#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||                      \ */

#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED) ||               \
    defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) ||          \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

/**
  * \brief     Encrypt data using NIST AES Key Wrap algorithm
  * \details   This platform function implements the NIST SP 800-38F AES Key Wrap encryption
  * \param[in] p_payload     Pointer to the payload data to encrypt
  * \param[in] payload_length    Length of the payload in bytes
  * \param[in] p_key       Pointer to the encryption key
  * \param[in] key_length      Length of the key in bytes
  * \param[out]  p_output        Pointer to the output buffer for encrypted data
  * \param[out]  p_output_length   Pointer to store the output length
  * \return    \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stse_platform_nist_kw_encrypt(PLAT_UI8 *p_payload, PLAT_UI32 payload_length,
                                                 PLAT_UI8 *p_key, PLAT_UI8 key_length,
                                                 PLAT_UI8 *p_output, PLAT_UI32 *p_output_length);
#endif /* defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED) ||               \ */

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||      \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || \
    defined(STSE_CONF_USE_HOST_SESSION)

/*!
 * \brief      Store aes key in platform secure storage
 * \param[in]  p_key       Pointer to the aes key
 * \param[in]  key_length Length of the key
 * \param[in]  usage      Key usage type, used by the platform to set the appropriate key attributes in secure storage
 * \param[out] p_key_idx   Pointer to the index assigned to the stored key in secure storage
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_store_aes_key(PLAT_UI8 *p_key, PLAT_UI16 key_length, stse_aes_key_usage_t usage,
                                               PLAT_UI32 *p_key_idx);

/*!
 * \brief      Delete aes key from platform secure storage
 * \param[in]  key_idx Index of the key to delete from secure storage
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_delete_aes_key(PLAT_UI32 key_idx);

/*!
 * \brief      Initialize AES CMAC computation
 * \param[in]  key_idx Index of the key to use for AES CMAC computation in secure storage
 * \param[in]  exp_tag_size Expected tag size
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_init(const PLAT_UI32 key_idx,
                                               PLAT_UI16 exp_tag_size);

/*!
 * \brief      Append data to AES CMAC computation
 * \param[in]  p_input Pointer to the input data
 * \param[in]  length Length of the input data
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_append(PLAT_UI8 *p_input, PLAT_UI16 length);

/*!
 * \brief      Finish AES CMAC computation and get the tag
 * \param[out] p_tag Pointer to the tag buffer
 * \param[out] p_tag_len Pointer to the tag length
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_compute_finish(PLAT_UI8 *p_tag, PLAT_UI8 *p_tag_len);

/*!
 * \brief      Finish AES CMAC verification
 * \param[in]  p_tag Pointer to the tag
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_verify_finish(PLAT_UI8 *p_tag);

/*!
 * \brief      Perform an AES CMAC encryption
 * \param[in]  p_payload Pointer to Payload
 * \param[in]  payload_length Length of the payload in bytes
 * \param[in]  key_idx Index of the key to use for AES CMAC computation in secure storage
 * \param[in]  exp_tag_size Expected tag size in bytes
 * \param[out] p_tag Pointer to Tag
 * \param[out] p_tag_length Pointer to Tag length value output
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_compute(const PLAT_UI8 *p_payload, PLAT_UI16 payload_length,
                                                  const PLAT_UI32 key_idx,
                                                  PLAT_UI16 exp_tag_size,
                                                  PLAT_UI8 *p_tag, PLAT_UI16 *p_tag_length);

/*!
 * \brief      Perform an AES CMAC decryption
 * \param[in]  p_payload Pointer to Payload
 * \param[in]  payload_length Length of the payload in bytes
 * \param[in]  key_idx Index of the key to use for AES CMAC computation in secure storage
 * \param[in]  p_tag Pointer to Tag
 * \param[in]  tag_length Pointer to Tag length value output
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cmac_verify(const PLAT_UI8 *p_payload, PLAT_UI16 payload_length,
                                                 const PLAT_UI32 key_idx,
                                                 const PLAT_UI8 *p_tag, PLAT_UI16 tag_length);

/*!
 * \brief      Perform an AES CBC encryption
 * \param[in]  p_plaintext Pointer to the plaintext data
 * \param[in]  plaintext_length Length of the plaintext data
 * \param[in]  p_initial_value Pointer to encryption IV
 * \param[in]  key_idx Index of the key to use for AES encryption in secure storage
 * \param[out] p_encryptedtext Pointer to the encrypted payload
 * \param[out] p_encryptedtext_length Length of encrypted payload
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cbc_enc(const PLAT_UI8 *p_plaintext, PLAT_UI16 plaintext_length,
                                             PLAT_UI8 *p_initial_value, const PLAT_UI32 key_idx,
                                             PLAT_UI8 *p_encryptedtext,
                                             PLAT_UI16 *p_encryptedtext_length);

/*!
 * \brief      Perform an AES CBC decryption
 * \param[in]  p_encryptedtext Pointer to the encrypted payload
 * \param[in]  encryptedtext_length Length of encrypted payload
 * \param[in]  p_initial_value Pointer to decryption IV
 * \param[in]  key_idx Index of the key to use for AES decryption in secure storage
 * \param[out] p_plaintext Pointer to PlainText payload
 * \param[out] p_plaintext_length Length of the PlainText payload
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_cbc_dec(const PLAT_UI8 *p_encryptedtext, PLAT_UI16 encryptedtext_length,
                                             PLAT_UI8 *p_initial_value, const PLAT_UI32 key_idx, PLAT_UI8 *p_plaintext,
                                             PLAT_UI16 *p_plaintext_length);

/*!
 * \brief      Perform an AES ECB encryption
 * \param[in]  p_plaintext Pointer to the plaintext data
 * \param[in]  plaintext_length Length of the plaintext data
 * \param[in]  key_idx Index of the key to use for AES encryption in secure storage
 * \param[out] p_encryptedtext Pointer to the encrypted payload
 * \param[out] p_encryptedtext_length Length of encrypted payload
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_aes_ecb_enc(const PLAT_UI8 *p_plaintext, PLAT_UI16 plaintext_length,
                                             const PLAT_UI32 key_idx, PLAT_UI8 *p_encryptedtext,
                                             PLAT_UI16 *p_encryptedtext_length);

#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) ||           \
          defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) ||      \
          defined(STSE_CONF_USE_HOST_SESSION) */

/*!
 *  \brief Perform a NIST KW (keywrap) encrypt
 *  \param[in]  p_payload         Pointer to payload
 *  \param[in]  payload_length      Length of payload
 *  \param[in]  p_key           Pointer to the key
 *  \param[in]  key_length        Length of the key
 *  \param[out] p_output        Pointer to encrypted output
 *  \param[out] p_output_length       Length of the encrypted output
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_nist_kw_encrypt(PLAT_UI8 *p_payload, PLAT_UI32 payload_length,
                                                 PLAT_UI8 *p_key, PLAT_UI8 key_length,
                                                 PLAT_UI8 *p_output, PLAT_UI32 *p_output_length);

/*!
*  \brief Perform an HMAC Key derivation using HMAC extract then HMAC expand functions (cf. RFC 5869)
*  \param[in]     p_salt              HMAC salt
*  \param[in]     salt_length           HMAC salt length
*  \param[in]     p_input_keying_material     Input keying material (IKM), could be ECDH output
*  \param[in]     input_keying_material_length  Input keying material length
*  \param[in]     p_info              Application specific information
*  \param[in]     info_length           Information length
*  \param[out]  p_output_keying_material    Output keying material (OKM)
*  \param[in]     output_keying_material_length   Output keying material expected length
* \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
*/
stse_return_code_t stse_platform_hmac_sha256_compute(PLAT_UI8 *p_salt, PLAT_UI16 salt_length,
                                                     PLAT_UI8 *p_input_keying_material,
                                                     PLAT_UI16 input_keying_material_length,
                                                     PLAT_UI8 *p_info, PLAT_UI16 info_length,
                                                     PLAT_UI8 *p_output_keying_material,
                                                     PLAT_UI16 output_keying_material_length);

/*!
*  \brief Perform an HMAC extract using SHA256 (cf. RFC 5869)
*  \param[in]     p_salt                HMAC salt
*  \param[in]     salt_length             HMAC salt length
*  \param[in]     p_input_keying_material         Input keying material (IKM), could be ECDH output
*  \param[in]     input_keying_material_length    Input keying material length
*  \param[out]  p_pseudorandom_key          Pseudorandom key (PRK)
*  \param[in]     pseudorandom_key_expected_length  Pseudorandom key length
* \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
*/
stse_return_code_t stse_platform_hmac_sha256_extract(PLAT_UI8 *p_salt, PLAT_UI16 salt_length,
                                                     PLAT_UI8 *p_input_keying_material,
                                                     PLAT_UI16 input_keying_material_length,
                                                     PLAT_UI8 *p_pseudorandom_key,
                                                     PLAT_UI16 pseudorandom_key_expected_length);

/*!
*  \brief Perform an HMAC expand using SHA256 (cf. RFC 5869)
*  \param[in]     p_pseudorandom_key        Pseudorandom key (PRK)
*  \param[in]     pseudorandom_key_length     Pseudorandom key length
*  \param[in]     p_info              Application specific information
*  \param[in]     info_length           Information length
*  \param[out]  p_output_keying_material    Output keying material (OKM)
*  \param[in]     output_keying_material_length   Output keying material expected length
* \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
*/
stse_return_code_t stse_platform_hmac_sha256_expand(PLAT_UI8 *p_pseudorandom_key, PLAT_UI16 pseudorandom_key_length,
                                                    PLAT_UI8 *p_info, PLAT_UI16 info_length,
                                                    PLAT_UI8 *p_output_keying_material,
                                                    PLAT_UI16 output_keying_material_length);

/*!
*  \brief Platform Abstraction function for STSAFE power control initialization
*/
stse_return_code_t stse_platform_power_ctrl_init(void);

/*!
*  \brief Platform Abstraction function for target STSAFE power-on
*  \param[in]  bus_id Target STSAFE bus ID
*  \param[in]  dev_addr Target STSAFE device address
*/
stse_return_code_t stse_platform_power_on(PLAT_UI8 bus_id, PLAT_UI8 dev_addr);

/*!
 *  \brief Platform Abstraction function for target STSAFE power-off
 *  \param[in]  bus_id Target STSAFE bus ID
 *  \param[in]  dev_addr Target STSAFE device address
 */
stse_return_code_t stse_platform_power_off(PLAT_UI8 bus_id, PLAT_UI8 dev_addr);

#if defined(STSE_CONF_USE_I2C) || defined(STSE_CONF_STSAFE_A_SUPPORT)

/*!
 * \brief      Initialize I2C communication
 * \param[in]  bus_id I2C bus ID
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_init(PLAT_UI8 bus_id, void *p_arg);

/*!
 * \brief      Send data over I2C
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_frame Pointer to the data frame
 * \param[in]  frame_length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_send(PLAT_UI8 bus_id,
                                          PLAT_UI8 dev_addr,
                                          PLAT_UI16 speed,
                                          PLAT_UI8 *p_frame,
                                          PLAT_UI16 frame_length);

/*!
 * \brief      Receive data over I2C
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_frame_header Pointer to the frame header
 * \param[out] p_frame_payload Pointer to the frame payload
 * \param[out] p_frame_payload_length Pointer to the length of the frame payload
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_receive(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_frame_header,
  PLAT_UI8 *p_frame_payload,
  PLAT_UI16 *p_frame_payload_length);

/*!
 * \brief      Wake up I2C device
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_wake(PLAT_UI8 bus_id,
                                          PLAT_UI8 dev_addr,
                                          PLAT_UI16 speed);

/*!
 * \brief      Start I2C send operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  frame_length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_send_start(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI16 frame_length);

/*!
 * \brief      Continue I2C send operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_element Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_send_continue(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_element,
  PLAT_UI16 element_size);

/*!
 * \brief      Stop I2C send operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_element Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_send_stop(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_element,
  PLAT_UI16 element_size);

/*!
 * \brief      Start I2C receive operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  frame_length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_receive_start(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI16 frame_length);

/*!
 * \brief      Continue I2C receive operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_element Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_receive_continue(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_element,
  PLAT_UI16 element_size);

/*!
 * \brief      Stop I2C receive operation
 * \param[in]  bus_id I2C bus ID
 * \param[in]  dev_addr I2C device address
 * \param[in]  speed I2C speed
 * \param[in]  p_element Pointer to the data element
 * \param[in]  element_size Size of the data element
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_i2c_receive_stop(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_element,
  PLAT_UI16 element_size);

#endif /* defined(STSE_CONF_USE_I2C) || defined(STSE_CONF_STSAFE_A_SUPPORT) */

#if defined(STSE_CONF_USE_ST1WIRE)

/*!
 * \brief      Initialize 1-wire communication
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  p_arg Argument coming from \ref stse_init through its `p_arg` parameter
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_init(PLAT_UI8 bus_id, void *p_arg);

/*!
 * \brief      Wake up 1-wire device
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_wake(PLAT_UI8 bus_id,
                                              PLAT_UI8 dev_addr,
                                              PLAT_UI16 speed);

/*!
 * \brief      Start 1-wire send operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  frame_length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_send_start(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI16 frame_length);

/*!
 * \brief      Continue 1-wire send operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  p_data Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_send_continue(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_data,
  PLAT_UI16 data_size);

/*!
 * \brief      Stop 1-wire send operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  p_data Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_send_stop(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_data,
  PLAT_UI16 data_size);

/*!
 * \brief      Start 1-wire receive operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  frame_length Length of the data frame
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_receive_start(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI16 frame_length);

/*!
 * \brief      Continue 1-wire receive operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  p_data Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_receive_continue(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_data,
  PLAT_UI16 data_size);

/*!
 * \brief      Stop 1-wire receive operation
 * \param[in]  bus_id 1-wire bus ID
 * \param[in]  dev_addr 1-wire device address
 * \param[in]  speed 1-wire speed
 * \param[in]  p_data Pointer to the data
 * \param[in]  data_size Size of the data
 * \return     \ref STSE_OK on success; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_platform_st1wire_receive_stop(
  PLAT_UI8 bus_id,
  PLAT_UI8 dev_addr,
  PLAT_UI16 speed,
  PLAT_UI8 *p_data,
  PLAT_UI16 data_size);

#endif /* defined(STSE_CONF_USE_ST1WIRE) */

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*STSE_PLATFORM_H*/
