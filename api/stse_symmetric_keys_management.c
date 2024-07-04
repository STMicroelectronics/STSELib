/*!
 ******************************************************************************
 * \file	stse_symmetric_keys_management.c
 * \brief   STSE Symmetric keys management API (sources)
 * \author  STMicroelectronics - SMD application team
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
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <api/stse_symmetric_keys_management.h>

#define STSAFEA_HOST_KEY_WRAPPING_AUTHENTICATION_TAG_LENGTH STSE_KEK_ENVELOPE_MAC_SIZE
#define STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH 	3U
#define STSE_HOST_KEY_ENVELOPE_PADDING_LENGTH 			4U
#define STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH			1U

/* Static functions declaration ----------------------------------------------*/

static stse_ReturnCode_t stse_start_volatile_KEK_session(
		stse_Handler_t * pSTSE,
		stse_session_t * pSession,
		stse_ecc_key_type_t ecc_key_type)
{
	stse_ReturnCode_t ret;


	/* - Check function parameters */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}
	if(pSession == NULL)
	{
		return(STSE_API_SESSION_ERROR);
	}

	if(ecc_key_type == STSE_ECC_KT_ED25519)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/* - Set session type */
	pSession->type = STSE_VOLATILE_KEK_SESSION;

	/* - allocate ECDHE buffer sizes according to ECC key type */
	PLAT_UI16 pub_key_size = stsafea_ecc_info_table[ecc_key_type].public_key_size;
	PLAT_UI16 priv_key_size = stsafea_ecc_info_table[ecc_key_type].private_key_size;
	PLAT_UI16 shared_secret_size = stsafea_ecc_info_table[ecc_key_type].shared_secret_size;

	PLAT_UI8 host_ecdhe_public_key[pub_key_size];
	PLAT_UI8 host_ecdhe_private_key[priv_key_size];
	PLAT_UI8 stsafe_ecdhe_public_key[pub_key_size];
	PLAT_UI8 shared_secret[2*shared_secret_size];

	PLAT_UI16 hkdf_info_size = 2 * pub_key_size;
	PLAT_UI8 pHkdf_info[hkdf_info_size];
	PLAT_UI8 pHkdf_salt[STSAFEA_KEK_HKDF_SALT_SIZE] = STSAFEA_KEK_HKDF_SALT;

	/* - Generate local host ECDHE key pair */
	ret = stse_platform_ecc_generate_key_pair(
			ecc_key_type,
			host_ecdhe_private_key,
			host_ecdhe_public_key);
	if(ret != STSE_OK)
	{
		/* Clear generated keypair on ECC keygen failure */
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, priv_key_size);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Generate ECDHE key pair on target SE */
	ret = stsafea_generate_ECDHE_key_pair(
			pSTSE,
			ecc_key_type,
			stsafe_ecdhe_public_key);
	if(ret != STSE_OK)
	{
		/* Clear key pairs info on SE ECDHE key pair generation error  */
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, priv_key_size);
		return( ret );
	}

	/* - Start volatile KEK session with target SE*/
	ret = stsafea_start_volatile_KEK_session(
			pSTSE,
			ecc_key_type,
			host_ecdhe_public_key);
	if(ret != STSE_OK)
	{
		/* Clear key pairs info on KEK session open error  */
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, priv_key_size);
		stsafea_session_clear_context(pSession);
		return( ret );
	}

	/* - Process Diffie-Hellman on host side */
	ret = stse_platform_ecc_ecdh(
			ecc_key_type,
			stsafe_ecdhe_public_key,
			host_ecdhe_private_key,
			shared_secret);

	memset(host_ecdhe_private_key, 0, priv_key_size);
	if(ret != STSE_OK)
	{
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		stsafea_session_clear_context(pSession);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Derive base KEK from shared secret */
	memcpy(pHkdf_info,
		   stsafe_ecdhe_public_key,
		   pub_key_size);
	memcpy(pHkdf_info + pub_key_size,
		   host_ecdhe_public_key,
		   pub_key_size);

	ret = stse_platform_hmac_sha256_compute(pHkdf_salt,
											  STSAFEA_KEK_HKDF_SALT_SIZE,
											  shared_secret,
											  stsafea_ecc_info_table[ecc_key_type].shared_secret_size,
											  pHkdf_info,
											  2 * pub_key_size,
											  pSession->context.kek.base_key,
											  STSAFEA_KEK_KEY_SIZE);

	memset(stsafe_ecdhe_public_key, 0, pub_key_size);
	memset(host_ecdhe_public_key, 0, pub_key_size);
	memset(shared_secret, 0, stsafea_ecc_info_table[ecc_key_type].shared_secret_size);

	if(ret != STSE_OK)
	{
		stsafea_session_clear_context(pSession);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Reset volatile KEK counter */
	pSession->context.kek.working_kek_counter = 0;

	return ret;
}

static stse_ReturnCode_t stse_start_volatile_KEK_session_authenticated(
		stse_Handler_t * pSTSE,
		stse_session_t * pSession,
		stse_ecc_key_type_t ecc_key_type,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t hash_algo,
		stse_ecc_key_type_t private_ecc_key_type,
		PLAT_UI8* private_key)
{
	stse_ReturnCode_t ret;

	/* - Check STSE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pSession == NULL)
	{
		return(STSE_API_SESSION_ERROR);
	}

	if(ecc_key_type == STSE_ECC_KT_ED25519)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	pSession->type = STSE_VOLATILE_KEK_SESSION;

	PLAT_UI16 pub_key_size = stsafea_ecc_info_table[ecc_key_type].public_key_size;
	PLAT_UI16 ecdhe_priv_key_size = stsafea_ecc_info_table[ecc_key_type].private_key_size;
	PLAT_UI16 shared_secret_size = stsafea_ecc_info_table[ecc_key_type].shared_secret_size;
	PLAT_UI16 signature_size = stsafea_ecc_info_table[private_ecc_key_type].signature_size;

	PLAT_UI16 pub_key_size_stsafe_format = pub_key_size;
	if(ecc_key_type == STSE_ECC_KT_CURVE25519)
	{
		pub_key_size_stsafe_format += STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	}
	else
	{
		pub_key_size_stsafe_format += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE + 2 * STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	}
	PLAT_UI16 tbs_length = stsafea_ecc_info_table[ecc_key_type].curve_id_total_length + 2 * pub_key_size_stsafe_format;

	PLAT_UI8 pHkdf_salt[STSAFEA_KEK_HKDF_SALT_SIZE] = STSAFEA_KEK_HKDF_SALT;
	PLAT_UI8 host_ecdhe_public_key[pub_key_size];
	PLAT_UI8 host_ecdhe_private_key[ecdhe_priv_key_size];
	PLAT_UI8 stsafe_ecdhe_public_key[pub_key_size];
	PLAT_UI8 shared_secret[2 * shared_secret_size];
	PLAT_UI8 signature[signature_size];
	PLAT_UI8 pHkdf_info[2 * pub_key_size_stsafe_format];
	PLAT_UI8 pTBS[tbs_length];

	/* - Generate host ECDHE key pair */
	ret = stse_platform_ecc_generate_key_pair(
			ecc_key_type,
			host_ecdhe_private_key,
			host_ecdhe_public_key);
	if(ret != STSE_OK)
	{
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, ecdhe_priv_key_size);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Generate STSE ECDHE key pair */
	ret = stsafea_generate_ECDHE_key_pair(
			pSTSE,
			ecc_key_type,
			stsafe_ecdhe_public_key);
	if(ret != STSE_OK)
	{
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, ecdhe_priv_key_size);
		return( ret );
	}

	/* - Prepare TBS buffer (concatenation of the two parties ECDHE Ephemeral public key)*/
	memcpy(pTBS,
		   (PLAT_UI8*)&stsafea_ecc_info_table[ecc_key_type].curve_id,
		   stsafea_ecc_info_table[ecc_key_type].curve_id_total_length
	);
	PLAT_UI16 copy_index = stsafea_ecc_info_table[ecc_key_type].curve_id_total_length;
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
		copy_index += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE;
	}
	pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	memcpy(pTBS + copy_index,
		   host_ecdhe_public_key,
		   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

		memcpy(pTBS + copy_index,
			   host_ecdhe_public_key + stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size,
			   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	}
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
		copy_index += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE;
	}
	pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	memcpy(pTBS + copy_index,
		   stsafe_ecdhe_public_key,
		   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

		memcpy(pTBS + copy_index,
			   stsafe_ecdhe_public_key + stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size,
			   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	}

	ret = stsafea_sign_for_generic_public_key_slot(
			pSTSE,
			private_ecc_key_type,	/* Private key used to sign */
			private_key,			/* Private key length */
			hash_algo,				/* Hash algorithm to be used for the digest */
			tbs_length,				/* Length of the payload to sign*/
			pTBS,					/* Payload to sign */
			signature);				/* Signature output */
	if(ret != STSE_OK)
	{
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, ecdhe_priv_key_size);
		memset(pTBS, 0, tbs_length);
		stsafea_session_clear_context(pSession);
		return( ret );
	}

	/* - Start volatile KEK session */
	ret = stsafea_start_volatile_KEK_session_authenticated(
			pSTSE,
			ecc_key_type,
			host_ecdhe_public_key,
			hash_algo,
			signature_public_key_slot_number,
			private_ecc_key_type,
			signature);

	if(ret != STSE_OK)
	{
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_private_key, 0, ecdhe_priv_key_size);
		stsafea_session_clear_context(pSession);
		return( ret );
	}

	/* - Process Diffie-Hellman on host side */
	ret = stse_platform_ecc_ecdh(
			ecc_key_type,
			stsafe_ecdhe_public_key,
			host_ecdhe_private_key,
			shared_secret);
	memset(host_ecdhe_private_key, 0, ecdhe_priv_key_size);
	if(ret != STSE_OK)
	{
		memset(stsafe_ecdhe_public_key, 0, pub_key_size);
		memset(host_ecdhe_public_key, 0, pub_key_size);
		stsafea_session_clear_context(pSession);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Derive base KEK from shared secret */
	memcpy(pHkdf_info,
		   stsafe_ecdhe_public_key,
		   pub_key_size);
	memcpy(pHkdf_info + pub_key_size,
		   host_ecdhe_public_key,
		   pub_key_size);

	ret = stse_platform_hmac_sha256_compute(
			pHkdf_salt,
			STSAFEA_KEK_HKDF_SALT_SIZE,
			shared_secret,
			stsafea_ecc_info_table[ecc_key_type].shared_secret_size,
			pHkdf_info,
			( 2 * pub_key_size ),
			pSession->context.kek.base_key,
			STSAFEA_KEK_KEY_SIZE
	);

	memset(stsafe_ecdhe_public_key, 0, pub_key_size);
	memset(host_ecdhe_public_key, 0, pub_key_size);
	memset(shared_secret, 0, stsafea_ecc_info_table[ecc_key_type].shared_secret_size);

	if(ret != STSE_OK)
	{
		stsafea_session_clear_context(pSession);
		return( STSE_UNEXPECTED_ERROR );
	}

	/* - Reset volatile KEK counter */
	pSession->context.kek.working_kek_counter = 0;

	return ret;
}

static stse_ReturnCode_t stse_stop_volatile_KEK_session( stse_Handler_t *pSTSE , stse_session_t *pSession)
{
	stse_ReturnCode_t ret;

	/* - Check function parameters */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	/* - Clear KEK session context on local host */
	stsafea_session_clear_context(pSession);

	/* - Clear KEK session context in target SE */
	ret = stsafea_stop_volatile_KEK_session(pSTSE);

	return ret;
}

static stse_ReturnCode_t stse_derive_working_KEK(
		stse_Handler_t *pSTSE,
		stse_session_t *pSession,
		PLAT_UI8 * working_kek)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 pHkdf_salt[STSAFEA_KEK_HKDF_SALT_SIZE] = STSAFEA_KEK_HKDF_SALT;

	pSession->context.kek.working_kek_counter += 1;

	PLAT_UI8 pHkdf_info[STSAFEA_WORKING_KEK_HKDF_INFO_SIZE] = {STSAFEA_KT_VOLATILE_BASE_KEK,
															  STSAFEA_KT_VOLATILE_WORKING_KEK,
															  pSession->context.kek.working_kek_counter};

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	ret = stse_platform_hmac_sha256_compute(pHkdf_salt,
											  STSAFEA_KEK_HKDF_SALT_SIZE,
											  pSession->context.kek.base_key,
											  STSAFEA_KEK_KEY_SIZE,
											  pHkdf_info,
											  STSAFEA_WORKING_KEK_HKDF_INFO_SIZE,
											  working_kek,
											  STSAFEA_KEK_KEY_SIZE);

	if(ret != STSE_OK)
	{
		memset(working_kek, 0, STSAFEA_KEK_KEY_SIZE);
		return( STSE_UNEXPECTED_ERROR );
	}

	return ret;
}

static stse_ReturnCode_t stse_KEK_wrap(
		stse_Handler_t *pSTSE,
		stse_session_t *pSession,
		PLAT_UI8 * pPayload,
		PLAT_UI32 payload_length,
		PLAT_UI8 * pEnvelope,
		PLAT_UI32 envelope_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 working_kek[STSAFEA_KEK_KEY_SIZE];

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(envelope_length < (payload_length + STSE_KEK_ENVELOPE_MAC_SIZE))
	{
		return STSE_API_INVALID_PARAMETER;
	}

	ret = stse_derive_working_KEK(
			pSTSE,
			pSession,
			working_kek);

	if(ret != STSE_OK)
	{
		memset(working_kek, 0, STSAFEA_KEK_KEY_SIZE);
		return( ret );
	}

	ret = stse_platform_nist_kw_encrypt(
			pPayload,payload_length,
			working_kek, STSAFEA_KEK_KEY_SIZE,
			pEnvelope,NULL);

	memset(working_kek, 0, STSAFEA_KEK_KEY_SIZE);

	if(ret != STSE_OK)
	{
		return( STSE_UNEXPECTED_ERROR );
	}

	return STSE_OK;
}


/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_host_key_provisioning (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_key_type,
		stsafea_host_keys_t* host_keys)
{
	stse_ReturnCode_t ret;

	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pSTSE->device_type == STSAFE_A120)
	{
		/* - Write host key plaintext to the STSAFE */
		ret = stsafea_host_key_provisioning(
				pSTSE,
				host_key_type,
				host_keys);
	}
	else
	{
		if(host_key_type != STSAFEA_AES_128_HOST_KEY)
		{
			return STSE_API_INVALID_PARAMETER;
		}

		ret = stsafea_put_attribute_host_key(
				pSTSE,
				(stsafea_aes_128_host_keys_t *)host_keys);
	}

	return ret;
}

stse_ReturnCode_t stse_host_key_provisioning_wrapped (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_key_type,
		stsafea_host_keys_t* host_keys,
		stse_ecc_key_type_t ecdhe_ecc_key_type)
{
	stse_ReturnCode_t ret;


	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pSTSE->device_type != STSAFE_A120)
	{
		return( STSE_COMMAND_CODE_NOT_SUPPORTED );
	}

	PLAT_UI8 host_keys_length = (host_key_type == STSAFEA_AES_128_HOST_KEY) ?
											   STSAFEA_HOST_AES_128_KEYS_SIZE
											   : STSAFEA_HOST_AES_256_KEYS_SIZE;

	PLAT_UI8 host_keys_envelope_length = STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH
									   + STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH
									   + host_keys_length
									   + STSE_HOST_KEY_ENVELOPE_PADDING_LENGTH
									   + STSAFEA_HOST_KEY_WRAPPING_AUTHENTICATION_TAG_LENGTH;

	PLAT_UI8 host_key_envelope[host_keys_envelope_length];


	stse_session_t volatile_KEK_session;
	stsafea_session_clear_context(&volatile_KEK_session);

	/* - Start volatile KEK  */
	ret = stse_start_volatile_KEK_session(
			pSTSE,
			&volatile_KEK_session,
			ecdhe_ecc_key_type);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format host key to be wrapped */
	memset(host_key_envelope, 0, host_keys_envelope_length);

	host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH] = host_key_type;

	memcpy(&host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH + STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH],
			host_keys,
			host_keys_length
	);

	host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH +
					  STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH +
					  host_keys_length] = 0x80;

	/* - Wrap */
	ret = stse_KEK_wrap(
			pSTSE,
			&volatile_KEK_session,
			host_key_envelope,
			( host_keys_envelope_length - STSAFEA_HOST_KEY_WRAPPING_AUTHENTICATION_TAG_LENGTH ),
			host_key_envelope,
			host_keys_envelope_length);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Write host key wrapped to the STSAFE */
	ret = stsafea_host_key_provisioning_wrapped(
			pSTSE,
			host_key_type,
			host_key_envelope);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Stop volatile KEK */
	ret = stsafea_stop_volatile_KEK_session(pSTSE);

	return ret;
}

stse_ReturnCode_t stse_host_key_provisioning_wrapped_authenticated (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_key_type,
		stsafea_host_keys_t* host_keys,
		stse_ecc_key_type_t ecdhe_ecc_key_type,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t signature_hash_algo,
		stse_ecc_key_type_t signature_private_ecc_key_type,
		PLAT_UI8 * signature_private_key)
{
	stse_ReturnCode_t ret;

	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pSTSE->device_type != STSAFE_A120)
	{
		return( STSE_COMMAND_CODE_NOT_SUPPORTED );
	}

	PLAT_UI8 host_keys_length = (host_key_type == STSAFEA_AES_128_HOST_KEY) ?
			   	   	   	   	   	   	   STSAFEA_HOST_AES_128_KEYS_SIZE
									   : STSAFEA_HOST_AES_256_KEYS_SIZE;

	PLAT_UI8 host_keys_envelope_length = host_keys_length
								       + STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH
									   + STSE_HOST_KEY_ENVELOPE_PADDING_LENGTH
									   + STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH
									   + STSAFEA_HOST_KEY_WRAPPING_AUTHENTICATION_TAG_LENGTH;


	PLAT_UI8 host_key_envelope[host_keys_envelope_length];

	stse_session_t volatile_KEK_session;
	stsafea_session_clear_context(&volatile_KEK_session);

	/* - Start volatile KEK Authenticated */
	ret = stse_start_volatile_KEK_session_authenticated(
			pSTSE,
			&volatile_KEK_session,
			ecdhe_ecc_key_type,
			signature_public_key_slot_number,
			signature_hash_algo,
			signature_private_ecc_key_type,
			signature_private_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format host key to be wrapped */
	memset(&host_key_envelope, 0, host_keys_envelope_length );

	host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH] = host_key_type;

	memcpy(&host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH + STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH],
			host_keys,
			host_keys_length);

	host_key_envelope[STSE_HOST_KEY_ENVELOPE_FRONT_PADDING_LENGTH
					  + STSE_HOST_KEY_ENVELOPE_KEY_TYPE_LENGTH
					  + host_keys_length] = 0x80;

	/* - Wrap */
	ret = stse_KEK_wrap(
			pSTSE,
			&volatile_KEK_session,
			host_key_envelope,
			( host_keys_envelope_length - STSAFEA_HOST_KEY_WRAPPING_AUTHENTICATION_TAG_LENGTH ),
			host_key_envelope,
			host_keys_envelope_length);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Write host key wrapped to the STSAFE */
	ret = stsafea_host_key_provisioning_wrapped(
			pSTSE,
			host_key_type,
			host_key_envelope);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Stop volatile KEK */
	ret = stse_stop_volatile_KEK_session(pSTSE,&volatile_KEK_session);

	return ret;
}



/* ------------------------------------------------------------------------------------------

							STSAFE-Axxx Symmetric keys provisioning

   ------------------------------------------------------------------------------------------ */

stse_ReturnCode_t stse_get_symmetric_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 *pSymmetric_key_slot_count)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_query_symmetric_key_slots_count(pSTSE, pSymmetric_key_slot_count);
}

stse_ReturnCode_t stse_get_symmetric_key_slot_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stsafea_symmetric_key_slot_information_t * pSymmetric_key_slot_info)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 slot_count;

	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	ret = stsafea_query_symmetric_key_slots_count(pSTSE, &slot_count);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	if(slot_number >= slot_count)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	stsafea_symmetric_key_slot_information_t pSymmetric_key_table_info[slot_count];

	ret = stsafea_query_symmetric_key_table(pSTSE, slot_count, pSymmetric_key_table_info);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	memcpy(pSymmetric_key_slot_info,
		   &pSymmetric_key_table_info[slot_number],
		   sizeof(stsafea_symmetric_key_slot_information_t));

	return STSE_OK;
}

stse_ReturnCode_t stse_get_symmetric_key_table_info(
		stse_Handler_t * pSTSE,
		PLAT_UI16 total_slot_count,
		stsafea_symmetric_key_slot_information_t *pSymmetric_key_table_info)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_query_symmetric_key_table(pSTSE, total_slot_count ,pSymmetric_key_table_info);
}

stse_ReturnCode_t stse_get_symmetric_key_slot_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stsafea_symmetric_key_slot_provisioning_ctrl_fields_t * pCtrl_fields)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_query_symmetric_key_slot_provisioning_ctrl_fields(pSTSE, slot_number, pCtrl_fields);
}

stse_ReturnCode_t stse_write_symmetric_key_plaintext(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pKey,
		stsafea_generic_key_information_t * key_info)
{
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	/* - Write the plaintext key */
	return stsafea_write_symmetric_key_plaintext(pSTSE, key_info, pKey);
}

stse_ReturnCode_t stse_write_symmetric_key_wrapped(
		stse_Handler_t *pSTSE,
		PLAT_UI8 *pKey,
		stsafea_generic_key_information_t * key_info,
		stse_ecc_key_type_t kek_session_ecc_type)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	stse_session_t volatile_KEK_session;
	stsafea_session_clear_context(&volatile_KEK_session);

	/* - Start Volatile KEK session */
	ret = stse_start_volatile_KEK_session(pSTSE, &volatile_KEK_session,kek_session_ecc_type);
	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format envelope to wrap */
	PLAT_UI8 envelope_key_length = key_info->type == STSAFEA_SYMMETRIC_KEY_TYPE_AES_256 ? STSAFEA_AES_256_KEY_SIZE : STSAFEA_AES_128_KEY_SIZE;
	PLAT_UI8 envelope_length = envelope_key_length + key_info->info_length;
	PLAT_UI8 envelope_padding_length = 8 - (envelope_length % 8);

	envelope_length += envelope_padding_length;

	PLAT_UI8 envelope[envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE];
	memset(envelope, 0, envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	memcpy(envelope,
		   (PLAT_UI8*) &key_info->lock_indicator,
		   key_info->info_length);
	memcpy(envelope+key_info->info_length,
		   pKey,
		   envelope_key_length);
	envelope[key_info->info_length+envelope_key_length] = 0x80;

	/* - Wrap the envelope */
	ret = stse_KEK_wrap(
			pSTSE,
			&volatile_KEK_session,
			envelope,
			envelope_length,
			envelope,
			envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Write the envelope */
	ret = stsafea_write_symmetric_key_wrapped(
			pSTSE,
			envelope,
			envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Close Volatile KEK session */
	ret = stse_stop_volatile_KEK_session(pSTSE,&volatile_KEK_session);

	return ret;
}

stse_ReturnCode_t stse_write_symmetric_key_wrapped_authenticated (
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pKey,
		stsafea_generic_key_information_t * key_info,
		stse_ecc_key_type_t kek_session_ecc_type,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t signature_hash_algo,
		stse_ecc_key_type_t signature_private_ecc_key_type,
		PLAT_UI8 * signature_private_key)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pSTSE->device_type != STSAFE_A120)
	{
		return( STSE_COMMAND_CODE_NOT_SUPPORTED );
	}

	stse_session_t volatile_KEK_session;
	stsafea_session_clear_context(&volatile_KEK_session);

	/* - Start volatile KEK Authenticated */
	ret = stse_start_volatile_KEK_session_authenticated(
			pSTSE,
			&volatile_KEK_session,
			kek_session_ecc_type,
			signature_public_key_slot_number,
			signature_hash_algo,
			signature_private_ecc_key_type,
			signature_private_key);
	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format envelope to wrap */
	PLAT_UI8 envelope_key_length =  key_info->type == STSAFEA_SYMMETRIC_KEY_TYPE_AES_256 ? STSAFEA_AES_256_KEY_SIZE : STSAFEA_AES_128_KEY_SIZE;
	PLAT_UI8 envelope_length = envelope_key_length + key_info->info_length;
	PLAT_UI8 envelope_padding_length = 8 - (envelope_length % 8);

	envelope_length += envelope_padding_length;

	PLAT_UI8 envelope[envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE];
	memset(envelope, 0, envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	memcpy(envelope,
		   (PLAT_UI8*) &key_info->lock_indicator,
		   key_info->info_length);
	memcpy(envelope+key_info->info_length,
		   pKey,
		   envelope_key_length);
	envelope[key_info->info_length+envelope_key_length] = 0x80;

	/* - Wrap the envelope */
	ret = stse_KEK_wrap(
			pSTSE,
			&volatile_KEK_session,
			envelope,
			envelope_length,
			envelope,
			envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Write the envelope */
	ret = stsafea_write_symmetric_key_wrapped(
			pSTSE,
			envelope,
			envelope_length + STSE_KEK_ENVELOPE_MAC_SIZE);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Close Volatile KEK session */
	ret = stse_stop_volatile_KEK_session(pSTSE,&volatile_KEK_session);

	return ret;
}

stse_ReturnCode_t stse_establish_symmetric_key(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t ecc_key_type,
		PLAT_UI8 key_infos_count,
		stsafea_generic_key_information_t * key_infos_list,
		PLAT_UI8 * key_list)
{
	stse_ReturnCode_t ret;

	/* - HKDF variables */
	PLAT_UI8 pHkdf_salt[STSAFEA_KEK_HKDF_SALT_SIZE] = STSAFEA_KEK_HKDF_SALT;
	PLAT_UI8 pHkdf_info[STSAFEA_KT_LENGTH + STSAFEA_KT_LENGTH] = {STSAFEA_KT_SYMMETRIC_KEY, STSAFEA_KT_CONFIRMATION_KEY};

	/* - Prepare asymmetric keys and shared secret buffers */
	PLAT_UI16 host_public_key_size;
	PLAT_UI16 host_private_key_size;
	PLAT_UI16 stsafe_ecdhe_public_key_size;
	PLAT_UI16 shared_secret_size;

	if(ecc_key_type == STSE_ECC_KT_ED25519)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	host_public_key_size = stsafea_ecc_info_table[ecc_key_type].public_key_size;
	host_private_key_size = stsafea_ecc_info_table[ecc_key_type].private_key_size;
	stsafe_ecdhe_public_key_size = stsafea_ecc_info_table[ecc_key_type].public_key_size;
	shared_secret_size = stsafea_ecc_info_table[ecc_key_type].shared_secret_size;

	PLAT_UI8 host_public_key[host_public_key_size];
	PLAT_UI8 host_private_key[host_private_key_size];
	PLAT_UI8 stsafe_public_key[stsafe_ecdhe_public_key_size];
	PLAT_UI8 shared_secret[2*shared_secret_size];

	/* - Initialize the OKM length with the confirmation key length */
	PLAT_UI16 okm_length = STSAFEA_AES_256_KEY_SIZE;

	for(PLAT_UI8 i=0; i<key_infos_count; i++)
	{
		/* - Add the current key length (16 or 32 bytes) depending on key type */
		okm_length += key_infos_list[i].type == STSAFEA_SYMMETRIC_KEY_TYPE_AES_256 ? STSAFEA_AES_256_KEY_SIZE : STSAFEA_AES_128_KEY_SIZE;
	}

	PLAT_UI8 okm_buffer[okm_length];

	/* - Generate host ECDHE key pair */
	ret = stse_platform_ecc_generate_key_pair(
			ecc_key_type,
			host_private_key,
			host_public_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Generate stsafea ECDHE key pair */
	ret = stsafea_generate_ECDHE_key_pair(
			pSTSE,
			ecc_key_type,
			stsafe_public_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Establish the symmetric key shared secret in STSAFE RAM */
	ret = stsafea_establish_symmetric_key(
			pSTSE,
			ecc_key_type,
			host_public_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Compute the shared secret on host side */
	ret = stse_platform_ecc_ecdh(
			ecc_key_type,
			stsafe_public_key,
			host_private_key,
			shared_secret);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Derive base KEK from shared secret */
	ret = stse_platform_hmac_sha256_compute(
			pHkdf_salt,
			STSAFEA_KEK_HKDF_SALT_SIZE,
			shared_secret,
			shared_secret_size,
			pHkdf_info,
			STSAFEA_KT_LENGTH + STSAFEA_KT_LENGTH,
			okm_buffer,
			okm_length);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Send confirmation MAC + Key information list */
	ret = stsafea_confirm_symmetric_key(
			pSTSE,
			okm_buffer,
			key_infos_count,
			key_infos_list);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format OKM for return value */
	memcpy(key_list,
		   okm_buffer + STSAFEA_AES_256_KEY_SIZE,
		   okm_length - STSAFEA_AES_256_KEY_SIZE);

	return ret;
}

stse_ReturnCode_t stse_establish_symmetric_key_authenticated(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t ecc_key_type,
		PLAT_UI8 key_infos_count,
		stsafea_generic_key_information_t * key_infos_list,
		PLAT_UI8 * key_list,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t hash_algo,
		stse_ecc_key_type_t private_ecc_key_type,
		PLAT_UI8* private_key)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if((key_infos_list   == NULL)
	|| (key_list 	   	 == NULL)
	|| (private_key    	 == NULL)
	|| (ecc_key_type 	   	 == STSE_ECC_KT_ED25519)
	|| (private_ecc_key_type == STSE_ECC_KT_CURVE25519))
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/* - HKDF variables */
	PLAT_UI8 pHkdf_salt[STSAFEA_KEK_HKDF_SALT_SIZE] = STSAFEA_KEK_HKDF_SALT;
	PLAT_UI8 pHkdf_info[STSAFEA_KT_LENGTH + STSAFEA_KT_LENGTH] = {STSAFEA_KT_SYMMETRIC_KEY, STSAFEA_KT_CONFIRMATION_KEY};

	/* - Prepare asymmetric keys and shared secret buffers */
	PLAT_UI8 host_ecdhe_public_key[stsafea_ecc_info_table[ecc_key_type].public_key_size];
	PLAT_UI8 host_ecdhe_private_key[stsafea_ecc_info_table[ecc_key_type].private_key_size];
	PLAT_UI8 stsafe_ecdhe_public_key[stsafea_ecc_info_table[ecc_key_type].public_key_size];
	PLAT_UI8 shared_secret[2*stsafea_ecc_info_table[ecc_key_type].shared_secret_size];

	PLAT_UI16 pub_key_size_stsafe_format = stsafea_ecc_info_table[ecc_key_type].public_key_size;
	if(ecc_key_type == STSE_ECC_KT_CURVE25519)
	{
		pub_key_size_stsafe_format += STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	}
	else
	{
		pub_key_size_stsafe_format += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE + 2 * STSAFEA_ECC_GENERIC_LENGTH_SIZE;
	}
	PLAT_UI16 tbs_length = stsafea_ecc_info_table[ecc_key_type].curve_id_total_length + 2 * pub_key_size_stsafe_format;
	PLAT_UI8 pTBS[tbs_length];

	PLAT_UI16 signature_size = stsafea_ecc_info_table[private_ecc_key_type].signature_size;
	PLAT_UI8 signature[signature_size];

	/* - Initialize the OKM length with the confirmation key length */
	PLAT_UI16 okm_length = STSAFEA_AES_256_KEY_SIZE;

	for(PLAT_UI8 i=0; i<key_infos_count; i++)
	{
		/* - Add the current key length (16 or 32 bytes) depending on key type */
		okm_length += key_infos_list[i].type == STSAFEA_SYMMETRIC_KEY_TYPE_AES_256 ? STSAFEA_AES_256_KEY_SIZE : STSAFEA_AES_128_KEY_SIZE;
	}

	PLAT_UI8 okm_buffer[okm_length];

	/* - Generate host ECDHE key pair */
	ret = stse_platform_ecc_generate_key_pair(
			ecc_key_type,
			host_ecdhe_private_key,
			host_ecdhe_public_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Generate stsafe ECDHE key pair */
	ret = stsafea_generate_ECDHE_key_pair(
			pSTSE,
			ecc_key_type,
			stsafe_ecdhe_public_key);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Sign concatenation of both public key to authenticate to the STSAFE */

	/* Copy curve ID for host key */
	memcpy(pTBS,
		   (PLAT_UI8*)&stsafea_ecc_info_table[ecc_key_type].curve_id,
		   stsafea_ecc_info_table[ecc_key_type].curve_id_total_length);

	PLAT_UI16 copy_index = stsafea_ecc_info_table[ecc_key_type].curve_id_total_length;

	/* Copy Host ECDHE public key */
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
		copy_index += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE;
	}

	pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

	memcpy(pTBS + copy_index,
		   host_ecdhe_public_key,
		   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;

	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

		memcpy(pTBS + copy_index,
			   host_ecdhe_public_key + stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size,
			   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	}

	/* Copy STSAFE ECDHE public key */
	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
		copy_index += STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE;
	}

	pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

	memcpy(pTBS + copy_index,
		   stsafe_ecdhe_public_key,
		   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
	copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;

	if(ecc_key_type != STSE_ECC_KT_CURVE25519)
	{
		pTBS[copy_index] = UI16_B1(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		pTBS[copy_index+1] = UI16_B0(stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += STSAFEA_ECC_GENERIC_LENGTH_SIZE;

		memcpy(pTBS + copy_index,
			   stsafe_ecdhe_public_key + stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size,
			   stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size);
		copy_index += stsafea_ecc_info_table[ecc_key_type].coordinate_or_key_size;
	}

	ret = stsafea_sign_for_generic_public_key_slot(
			pSTSE,
			private_ecc_key_type,	/* Private key used to sign */
			private_key,		/* Private key length */
			hash_algo,			/* Hash algorithm to be used for the digest */
			tbs_length,			/* Length of the payload to sign*/
			pTBS,				/* Payload to sign */
			signature);			/* Signature output */

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Establish the symmetric key shared secret in STSAFE RAM */
	ret = stsafea_establish_symmetric_key_authenticated(
			pSTSE,
			ecc_key_type,
			host_ecdhe_public_key,
			hash_algo,
			signature_public_key_slot_number,
			private_ecc_key_type,
			signature);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Compute the shared secret on host side */
	ret = stse_platform_ecc_ecdh(
			ecc_key_type,
			stsafe_ecdhe_public_key,
			host_ecdhe_private_key,
			shared_secret);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Derive base KEK from shared secret */
	ret = stse_platform_hmac_sha256_compute(
			pHkdf_salt,
			STSAFEA_KEK_HKDF_SALT_SIZE,
			shared_secret,
			stsafea_ecc_info_table[ecc_key_type].shared_secret_size,
			pHkdf_info,
			STSAFEA_KT_LENGTH + STSAFEA_KT_LENGTH,
			okm_buffer,
			okm_length);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Send confirmation MAC + Key information list */
	ret = stsafea_confirm_symmetric_key(
			pSTSE,
			okm_buffer,
			key_infos_count,
			key_infos_list);

	if(ret != STSE_OK)
	{
		return( ret );
	}

	/* - Format OKM for return value */
	memcpy(key_list,
		   okm_buffer + STSAFEA_AES_256_KEY_SIZE,
		   okm_length - STSAFEA_AES_256_KEY_SIZE);

	return ret;
}
