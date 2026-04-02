/*!
 ******************************************************************************
 * \file	stsafea_data_partition.h
 * \brief   Data partition services for STSAFE-A
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

#ifndef STSAFEA_ECC_H
#define STSAFEA_ECC_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_timings.h"
#include "stse_platform_generic.h"

/*! \defgroup stsafea_ecc STSAFE-A Elliptic Curve Cryptography (ECC)
 *  \ingroup stsafea_services
 *  @{
 */

typedef enum {
    STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING = 0x01,
    STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY
} stsafea_host_kdf_algo_id_t;

/**
 * \brief 		Start volatile KEK session
 * \details 	This service initiates a volatile Key Encryption Key session
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	key_type				ECC key type for ECDHE
 * \param[in] 	host_ecdhe_public_key	Pointer to host ECDHE public key
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_start_volatile_KEK_session(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *host_ecdhe_public_key);

/**
 * \brief 		Start authenticated volatile KEK session
 * \details 	This service initiates an authenticated volatile Key Encryption Key session
 * \param[in] 	pSTSE 							Pointer to STSE Handler
 * \param[in] 	ecdhe_key_type					ECC key type for ECDHE
 * \param[in] 	host_ecdhe_public_key			Pointer to host ECDHE public key
 * \param[in] 	hash_algo						Hash algorithm for signature
 * \param[in] 	signature_public_key_slot_number	Slot number of signature public key
 * \param[in] 	signature_key_type				Signature key type
 * \param[in] 	pSignature						Pointer to signature buffer
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_start_volatile_KEK_session_authenticated(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t ecdhe_key_type,
    PLAT_UI8 *host_ecdhe_public_key,
    stse_hash_algorithm_t hash_algo,
    PLAT_UI8 signature_public_key_slot_number,
    stse_ecc_key_type_t signature_key_type,
    PLAT_UI8 *pSignature);

/**
 * \brief 		Stop volatile KEK session
 * \details 	This service terminates the active volatile Key Encryption Key session
 * \param[in] 	pSTSE 		Pointer to STSE Handler
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_stop_volatile_KEK_session(
    stse_Handler_t *pSTSE);

/**
 * \brief 		Verify the signature in arguments
 * \details 	This service formats and send/receive STSAFE-Axxx verify signature command/response
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	key_type 				Signature key type
 * \param[in] 	pPublic_key 			Public key to verify the signature
 * \param[in] 	pSignature 				Signature buffer
 * \param[in] 	pMessage 				Message used in signature
 * \param[in] 	message_length 			Message length
 * \param[in] 	eddsa_variant 			Flag indicating a pre-hashed EdDSA (1) message or a pure EdDSA (0) plaintext message. Used only in case of Edwards25519 public key
 * \param[out] 	pSignature_validity		Signature validity flag (1 = valid signature, invalid otherwise)
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_ecc_verify_signature.dox
 */
stse_ReturnCode_t stsafea_ecc_verify_signature(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pSignature,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 eddsa_variant,
    PLAT_UI8 *pSignature_validity);

/**
 * \brief 		Generate n ECDSA or an EdDSA signature depending on the curve in the private key slot
 * \details 	This service formats and send/receive STSAFE-Axxx generate signature command/response
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	slot_number				Slot to identify used private key
 * \param[in] 	key_type 				Private key type
 * \param[in] 	pMessage 				Message used in signature
 * \param[in] 	message_length 			Message length
 * \param[out] 	pSignature 				Signature buffer
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_ecc_verify_signature.dox
 */
stse_ReturnCode_t stsafea_ecc_generate_signature(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 *pSignature);

/**
 * \brief 		Establish shared secret using ECDH
 * \details 	This service performs ECDH key agreement to establish a shared secret
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	private_key_slot_number	Slot number containing the private key
 * \param[in] 	key_type				ECC key type
 * \param[in] 	pPublic_key				Pointer to the peer's public key
 * \param[out] 	pShared_secret			Pointer to buffer for the shared secret
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_ecc_establish_shared_secret(
    stse_Handler_t *pSTSE,
    PLAT_UI8 private_key_slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pShared_secret);

/**
 * \brief 		Decompress ECC public key
 * \details 	This service decompresses a compressed ECC public key point
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	key_type				ECC key type
 * \param[in] 	point_representation_id	Point representation identifier
 * \param[in] 	pPublic_key_X			Pointer to X coordinate of public key
 * \param[out] 	pPublic_key_Y			Pointer to buffer for Y coordinate of public key
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_ecc_decompress_public_key(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 point_representation_id,
    PLAT_UI8 *pPublic_key_X,
    PLAT_UI8 *pPublic_key_Y);

/** \}*/

#endif /* STSAFEA_ECC_H */
