/**
  ******************************************************************************
  * \file    stse_certificate_crypto.h
  * \author  STMicroelectronics - CS application team
  * \brief   Crypto functionalities for x509 verification
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

#ifndef STSE_CERTIFICATE_CRYPTO_H
#define STSE_CERTIFICATE_CRYPTO_H

#include "certificate/stse_certificate_types.h"

/** \defgroup 	stse_certificate_crypto STSE Certificate cryptography
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate cryptography functions
 *  @{
 */

/**
 * \brief  Verify the signature of a digest with a public key extracted from the certificate
 * \param[in]  cert 			pointer to the parsed x509 certificate containing the public key to be used for the signature verification
 * \param[in]  digest 			the digest of the message to be verified
 * \param[in]  digestSize 		size in bytes of the digest of the message to be verified
 * \param[in]  signatureR 		the R value of the signature to be verified
 * \param[in]  signatureRsize 	the size in bytes of signatureR
 * \param[in]  signatureS 		the S value of the signature to be verified
 * \param[in]  signatureSsize 	the size in bytes of signatureS
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_certificate_verify_signature(
		const stse_certificate_t *cert,
		const PLAT_UI8 *digest,
		PLAT_I32 digestSize,
		const PLAT_UI8 *signatureR,
		PLAT_I32 signatureRsize,
		const PLAT_UI8 *signatureS,
		PLAT_I32 signatureSsize);

/**
 * \brief  Check whether "parent" signed "child"
 * \param[in]  parent 			pointer to the parsed x509 certificate of the supposed issuer of child
 * \param[in]  child  			pointer to the parsed x509 certificate of the certificated supposedly issued by parent
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_certificate_verify_cert_signature(
		const stse_certificate_t *parent,
		const stse_certificate_t *child);

/**
 * \brief  Get the certificate public key type
 * \param[in]  cert 			pointer to the parsed x509 certificate
 * \return \ref stse_ecc_key_type_t : certificate public key type ; 0xFF if key type is invalid
 */
stse_ecc_key_type_t stse_certificate_get_key_type(
		const stse_certificate_t *cert);

/**
 * \brief  Get the certificate signature hash algorithm
 * \param[in]  cert 			pointer to the parsed x509 certificate
 * \return \ref stse_hash_algorithm_t : certificate signature hash algorithm ; 0xFF if hash algo is invalid
 */
stse_hash_algorithm_t stse_certificate_get_sig_hash_algo(
		const stse_certificate_t *cert);

/** @}*/

#endif /*  STSE_CERTIFICATE_CRYPTO_H */
