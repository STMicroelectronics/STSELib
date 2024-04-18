/**
  ******************************************************************************
  * @file    stse_certificate_crypto.c
  * @author  STMicroelectronics - CS application team
  * @brief   Crypto functionalities for x509 verification
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "certificate/stse_certificate_crypto.h"
#include "api/stse_ecc.h"
#include "api/stse_hash.h"

stse_ReturnCode_t stse_certificate_verify_cert_signature(const stse_certificate_t *parent, const stse_certificate_t *child)
{
	stse_ReturnCode_t ret;
	stse_hash_algorithm_t hash_algo = stse_certificate_get_sig_hash_algo(child);

	PLAT_UI32 digestSize = stsafea_hash_info_table[hash_algo].length;
	PLAT_UI8 digest[digestSize];

	/*
	 * Only STSAFE-A120 support Hash features
	 */
	if(stsafe_x509_parser_companion_handler != NULL
	&& stsafe_x509_parser_companion_handler->device_type == STSAFE_A120
	&& hash_algo >= STSE_SHA_256)
	{
		ret = stse_compute_hash(
				stsafe_x509_parser_companion_handler,
				hash_algo,
				(PLAT_UI8*)child->tbs,
				child->tbsSize,
				digest,
				(PLAT_UI16*)&digestSize);
	}
	else
	{
		ret = stse_platform_hash_compute(
				hash_algo,
				(PLAT_UI8*)child->tbs,
				child->tbsSize,
				digest,
				&digestSize);
	}

	if(ret != STSE_OK)
	{
	  return (ret);
	}

	return(stse_certificate_verify_signature(parent, digest, digestSize, child->Sign.pR, child->Sign.rSize, child->Sign.pS, child->Sign.sSize));
}

stse_ReturnCode_t stse_certificate_verify_signature(const stse_certificate_t *cert,
  const PLAT_UI8 *digest,
  PLAT_I32 digestSize,
  const PLAT_UI8 *signatureR,
  PLAT_I32 signatureRsize,
  const PLAT_UI8 *signatureS,
  PLAT_I32 signatureSsize)
{
	(void)signatureRsize;
	(void)signatureSsize;
	stse_ReturnCode_t ret;
	stse_ecc_key_type_t key_type = stse_certificate_get_key_type(cert);
	PLAT_UI8 pub_key_size = stsafea_ecc_info_table[key_type].public_key_size;
	PLAT_UI8 pub_key[pub_key_size];
	PLAT_UI8 signature_size = stsafea_ecc_info_table[key_type].signature_size;
	PLAT_UI8 signature[signature_size];
	PLAT_UI8 signature_validity;

	/* Extract and format the public key from the certificate */
	memcpy(pub_key, cert->PubKey.pX, (pub_key_size>>1));
	if(*cert->pPubKey_point_representation_id == 0x04)
	{
		memcpy(pub_key + (pub_key_size>>1), 	cert->PubKey.pY, (pub_key_size>>1));
	}
	else
	{
		if(stsafe_x509_parser_companion_handler != NULL)
		{
			stsafea_ecc_decompress_public_key(
				stsafe_x509_parser_companion_handler,
				key_type,
				*cert->pPubKey_point_representation_id,
				pub_key,
				pub_key + (pub_key_size>>1));
		}
		else
		{
			return STSE_CERT_UNSUPPORTED_FEATURE;
		}
	}

	/* Format the signature */
	memcpy(signature, 						signatureR, 	 (signature_size>>1));
	memcpy(signature + (signature_size>>1), signatureS, 	 (signature_size>>1));

	if(stsafe_x509_parser_companion_handler != NULL)
	{
		/* Verify the signature using STSAFE */
		ret = stse_ecc_verify_signature(
				stsafe_x509_parser_companion_handler,
				key_type,
				pub_key,
				signature,
				(PLAT_UI8*)digest,
				(PLAT_UI16)digestSize,
				1, 					/* Message is hashed */
				&signature_validity);

		if((ret == STSE_OK) && (signature_validity == 1))
		{
			return (STSE_OK);
		}
	}
	else
	{
		ret = stse_platform_ecc_verify(
			  key_type,
			  pub_key,
			  (PLAT_UI8*)digest,
			  (PLAT_UI16)digestSize,
			  signature);

		if(ret == STSE_OK)
		{
		  return (STSE_OK);
		}
	}

	return (STSE_CERT_INVALID_SIGNATURE);
}

stse_ecc_key_type_t stse_certificate_get_key_type(const stse_certificate_t *cert)
{
	switch (cert->EllipticCurve)
	{
		  case EC_P256:
			return STSE_ECC_KT_NIST_P_256;
		  case EC_P384:
			return STSE_ECC_KT_NIST_P_384;
		  case EC_P521:
			return STSE_ECC_KT_NIST_P_521;
		  case EC_bp256r1:
			return STSE_ECC_KT_BP_P_256;
		  case EC_bp384r1:
			return STSE_ECC_KT_BP_P_384;
		  case EC_bp512r1:
			return STSE_ECC_KT_BP_P_512;
		  default:
			  return STSE_ECC_KT_INVALID;
	}
}


stse_hash_algorithm_t stse_certificate_get_sig_hash_algo(const stse_certificate_t *cert)
{
	switch (cert->SignatureAlgorithm)
	{
		case SIG_ECDSA_SHA1:
			return STSE_SHA_1;
		case SIG_ECDSA_SHA224:
			return STSE_SHA_224;
		case SIG_ECDSA_SHA256:
			return STSE_SHA_256;
		case SIG_ECDSA_SHA384:
			return STSE_SHA_384;
		case SIG_ECDSA_SHA512:
			return STSE_SHA_512;
		default:
			return STSE_SHA_INVALID;
	}
}
