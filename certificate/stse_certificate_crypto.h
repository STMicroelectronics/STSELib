/**
  ******************************************************************************
  * @file    stse_certificate_crypto.h
  * @author  CS Application Team
  * @brief   Crypto functionalities for x509 verification
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

#ifndef STSE_CERTIFICATE_CRYPTO_H
#define STSE_CERTIFICATE_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "certificate/stse_certificate_types.h"

/** \defgroup   stse_certificate_crypto STSE Certificate cryptography
  *  \ingroup  stse_certificate
  *  \brief    STSE Certificate cryptography functions
  *  @{
  */

/**
  * \brief  Verify the signature of a digest with a public key extracted from the certificate
  * \param[in]  cert       pointer to the parsed x509 certificate containing the public key to be used for the
  *                        signature verification
  * \param[in]  digest       the digest of the message to be verified
  * \param[in]  digest_size    size in bytes of the digest of the message to be verified
  * \param[in]  signature_r    the R value of the signature to be verified
  * \param[in]  signature_rsize  the size in bytes of signature_r
  * \param[in]  signature_s    the S value of the signature to be verified
  * \param[in]  signature_ssize  the size in bytes of signature_s
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stse_certificate_verify_signature(
  const stse_certificate_t *cert,
  const PLAT_UI8 *digest,
  PLAT_I32 digest_size,
  const PLAT_UI8 *signature_r,
  PLAT_I32 signature_rsize,
  const PLAT_UI8 *signature_s,
  PLAT_I32 signature_ssize);

/**
  * \brief  Check whether "parent" signed "child"
  * \param[in]  parent       pointer to the parsed x509 certificate of the supposed issuer of child
  * \param[in]  child        pointer to the parsed x509 certificate of the certificated supposedly issued by parent
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stse_certificate_verify_cert_signature(
  const stse_certificate_t *parent,
  const stse_certificate_t *child);

/**
  * \brief  Get the certificate public key type
  * \param[in]  cert       pointer to the parsed x509 certificate
  * \return \ref stse_ecc_key_type_t : certificate public key type ; 0xFF if key type is invalid
  */
stse_ecc_key_type_t stse_certificate_get_key_type(
  const stse_certificate_t *cert);

/**
  * \brief  Get the certificate signature hash algorithm
  * \param[in]  cert       pointer to the parsed x509 certificate
  * \return \ref stse_hash_algorithm_t : certificate signature hash algorithm ; 0xFF if hash algo is invalid
  */
stse_hash_algorithm_t stse_certificate_get_sig_hash_algo(
  const stse_certificate_t *cert);

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  STSE_CERTIFICATE_CRYPTO_H */
