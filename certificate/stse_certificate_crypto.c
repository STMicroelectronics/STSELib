/**
  ******************************************************************************
  * @file    stse_certificate_crypto.c
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "api/stse_hash.h"
#include "certificate/stse_certificate_crypto.h"

stse_return_code_t stse_certificate_verify_cert_signature(const stse_certificate_t *parent,
                                                          const stse_certificate_t *child)
{
  stse_return_code_t ret;
  stse_hash_algorithm_t hash_algo;

  if ((parent == NULL) || (child == NULL))
  {
    /* Parameter sanity check failed */
    /* Parameter sanity check failed */
    return (STSE_CERT_INVALID_PARAMETER);
  }

  /* Retrieve hash algorithm required by child certificate signature */
  /* Retrieve hash algorithm required by child certificate signature */
  hash_algo = stse_certificate_get_sig_hash_algo(child);
  if (hash_algo >= STSE_SHA_INVALID)
  {
    return (STSE_CERT_UNSUPPORTED_FEATURE);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  PLAT_UI16 digest_size = 32U;
#else
  PLAT_UI16 digest_size = stsafea_hash_info_table[hash_algo].length;
#endif /* STSE_CONF_STSAFE_A_SUPPORT*/
  PLAT_UI8 digest[digest_size];
  PLAT_UI8 *digest_ptr = digest;

  /* Ed25519 signatures operate directly on full TBS payload */
  /* Ed25519 signatures operate directly on full TBS payload */
  if (parent->signature_algorithm == SIG_EDDSA_ED25519)
  {
    digest_ptr = (PLAT_UI8 *)child->tbs;
    digest_size = child->tbs_size;
    ret = STSE_OK;
  }
#ifdef STSE_CONF_USE_COMPANION
  else if (stsafe_x509_parser_companion_handler != NULL &&
           stsafe_x509_parser_companion_handler->device_type == STSAFE_A120
#ifdef STSE_CONF_HASH_SHA_256
           && hash_algo >= STSE_SHA_256
#endif /* STSE_CONF_HASH_SHA_256 */
          )   /* Only STSAFE-A120 support Hash features */
  {
    ret = stse_compute_hash(stsafe_x509_parser_companion_handler, hash_algo,
                            (PLAT_UI8 *)child->tbs, child->tbs_size, digest_ptr,
                            (PLAT_UI16 *)&digest_size);
  }
#endif /* STSE_CONF_USE_COMPANION */
  else
  {
    /* Compute TBS hash using host platform crypto service */
    /* Compute TBS hash using host platform crypto service */
    ret = stse_platform_hash_compute(hash_algo, (PLAT_UI8 *)child->tbs, child->tbs_size,
                                     digest_ptr, &digest_size);
  }

  /* Validate operation return code */
  /* Validate operation return code */
  if (ret != STSE_OK)
  {
    return (ret);
  }

  return (stse_certificate_verify_signature(parent, digest_ptr, digest_size, child->sign.p_r,
                                            child->sign.r_size, child->sign.p_s,
                                            child->sign.s_size));
}

stse_return_code_t stse_certificate_verify_signature(const stse_certificate_t *cert,
                                                     const PLAT_UI8 *digest,
                                                     PLAT_I32 digest_size,
                                                     const PLAT_UI8 *signature_r,
                                                     PLAT_I32 signature_rsize,
                                                     const PLAT_UI8 *signature_s,
                                                     PLAT_I32 signature_ssize)
{
  (void)signature_rsize;
  (void)signature_ssize;
  stse_return_code_t ret;
  stse_ecc_key_type_t key_type;

  if ((cert == NULL) || (digest == NULL) || (signature_r == NULL) || (signature_s == NULL))
  {
    /* Parameter sanity check failed */
    /* Parameter sanity check failed */
    return (STSE_CERT_INVALID_PARAMETER);
  }

  key_type = stse_certificate_get_key_type(cert);
  if (key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  PLAT_UI8 pub_key_size = stse_ecc_info_table[key_type].public_key_size;
  PLAT_UI8 pub_key[pub_key_size];
  PLAT_UI8 signature_size = stse_ecc_info_table[key_type].signature_size;
  PLAT_UI8 signature[signature_size];
#ifdef STSE_CONF_USE_COMPANION
  PLAT_UI8 signature_validity;
#endif /* STSE_CONF_USE_COMPANION */

  /* Extract and format the public key from the certificate */
  if (cert->signature_algorithm == SIG_EDDSA_ED25519)
  {
    memcpy(pub_key, cert->pub_key.p_x, pub_key_size);
  }
  else
  {
    memcpy(pub_key, cert->pub_key.p_x, (pub_key_size >> 1));
    if (*cert->p_pub_key_point_representation_id == 0x04)
    {
      memcpy(pub_key + (pub_key_size >> 1), cert->pub_key.p_y, (pub_key_size >> 1));
    }
    else
    {
#ifdef STSE_CONF_USE_COMPANION
      if (stsafe_x509_parser_companion_handler != NULL)
      {
        stsafea_ecc_decompress_public_key(
          stsafe_x509_parser_companion_handler,
          key_type,
          *cert->p_pub_key_point_representation_id,
          pub_key,
          pub_key + (pub_key_size >> 1));
      }
      else
#endif /* STSE_CONF_USE_COMPANION */
      {
        return STSE_CERT_UNSUPPORTED_FEATURE;
      }
    }
  }

  /* Format the signature */
  memcpy(signature, signature_r, (signature_size >> 1));
  memcpy(signature + (signature_size >> 1), signature_s, (signature_size >> 1));

#ifdef STSE_CONF_USE_COMPANION
  if (stsafe_x509_parser_companion_handler != NULL)
  {
    /* Verify the signature using STSAFE */
    ret = stse_ecc_verify_signature(
            stsafe_x509_parser_companion_handler,
            key_type,
            pub_key,
            signature,
            (PLAT_UI8 *)digest,
            (PLAT_UI16)digest_size,
            1, /* Message is hashed */
            &signature_validity);

    if ((ret == STSE_OK) && (signature_validity == 1))
    {
      return (STSE_OK);
    }
  }
  else
#endif /* STSE_CONF_USE_COMPANION */
  {
    /* Verify certificate signature using parent public key */
    /* Verify certificate signature using parent public key */
    ret = stse_platform_ecc_verify(
            key_type,
            pub_key,
            (PLAT_UI8 *)digest,
            (PLAT_UI16)digest_size,
            signature);

    if (ret == STSE_OK)
    {
      return (STSE_OK);
    }
  }

  return (STSE_CERT_INVALID_SIGNATURE);
}

stse_ecc_key_type_t stse_certificate_get_key_type(const stse_certificate_t *cert)
{
  switch (cert->elliptic_curve)
  {
#ifdef STSE_CONF_ECC_NIST_P_256
    case EC_P256:
      return STSE_ECC_KT_NIST_P_256;
#endif /* STSE_CONF_ECC_NIST_P_256 */
#ifdef STSE_CONF_ECC_NIST_P_384
    case EC_P384:
      return STSE_ECC_KT_NIST_P_384;
#endif /* STSE_CONF_ECC_NIST_P_384 */
#ifdef STSE_CONF_ECC_NIST_P_521
    case EC_P521:
      return STSE_ECC_KT_NIST_P_521;
#endif /* STSE_CONF_ECC_NIST_P_521 */
#ifdef STSE_CONF_ECC_BRAINPOOL_P_256
    case EC_BP_256_R1:
      return STSE_ECC_KT_BP_P_256;
#endif /* STSE_CONF_ECC_BRAINPOOL_P_256 */
#ifdef STSE_CONF_ECC_BRAINPOOL_P_384
    case EC_BP_384_R1:
      return STSE_ECC_KT_BP_P_384;
#endif /* STSE_CONF_ECC_BRAINPOOL_P_384 */
#ifdef STSE_CONF_ECC_BRAINPOOL_P_512
    case EC_BP_512_R1:
      return STSE_ECC_KT_BP_P_512;
#endif /* STSE_CONF_ECC_BRAINPOOL_P_512 */
#ifdef STSE_CONF_ECC_EDWARD_25519
    case EC_ED_25519:
      return STSE_ECC_KT_ED25519;
#endif /* STSE_CONF_ECC_EDWARD_25519 */
    default:
      return STSE_ECC_KT_INVALID;
  }
}

stse_hash_algorithm_t stse_certificate_get_sig_hash_algo(const stse_certificate_t *cert)
{
  switch (cert->signature_algorithm)
  {
#ifdef STSE_CONF_HASH_SHA_1
    case SIG_ECDSA_SHA1:
      return STSE_SHA_1;
#endif /* STSE_CONF_HASH_SHA_1 */
#ifdef STSE_CONF_HASH_SHA_224
    case SIG_ECDSA_SHA224:
      return STSE_SHA_224;
#endif /* STSE_CONF_HASH_SHA_224 */
#ifdef STSE_CONF_HASH_SHA_256
    case SIG_ECDSA_SHA256:
      return STSE_SHA_256;
    case SIG_EDDSA_ED25519:
      return STSE_SHA_256;
#endif /* STSE_CONF_HASH_SHA_256 */
#ifdef STSE_CONF_HASH_SHA_384
    case SIG_ECDSA_SHA384:
      return STSE_SHA_384;
#endif /* STSE_CONF_HASH_SHA_384 */
#ifdef STSE_CONF_HASH_SHA_512
    case SIG_ECDSA_SHA512:
      return STSE_SHA_512;
#endif /* STSE_CONF_HASH_SHA_512 */
    default:
      return STSE_SHA_INVALID;
  }
}
