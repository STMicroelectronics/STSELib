/*!
 ******************************************************************************
 * \file	stse_ecc.h
 * \brief   STSE ECC API set (header)
 * \author  STMicroelectronics - CS application team
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
 ******************************************************************************
 */

#ifndef STSE_ECC_H
#define STSE_ECC_H

/* Defines -------------------------------------------------------------------*/
#define STSE_PUB_KEY_COMPRESSED_EVEN 0x02
#define STSE_PUB_KEY_COMPRESSED_ODD 0x03
#define STSE_PUB_KEY_UNCOMPRESSED 0x04

#define IS_PUB_KEY_COMPRESSED(point_representation_id) ((point_representation_id == PUB_KEY_COMPRESSED_EVEN) || (point_representation_id == PUB_KEY_COMPRESSED_ODD))
#define IS_PUB_KEY_UNCOMPRESSED(point_representation_id) (point_representation_id == PUB_KEY_UNCOMPRESSED)

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafel/stsafel_ecc.h"

/** \defgroup 	stse_ecc 	STSE ECC
 *  \ingroup 	stse_api
 *  \brief		STSE Elliptic Curve Cryptography API set
 *  \details  	The ECC API set provides high level functions for ECC features.
 *  @{
 */

/**
 * \brief 		Verify the signature in arguments
 * \details 	This API use the STSE to verify a signature
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	key_type 				Signature key type
 * \param[in] 	pPublic_key 			Public key to verify the signature
 * \param[in] 	pSignature 				Signature buffer
 * \param[in] 	pMessage 				Message used in signature
 * \param[in] 	message_length 			Message length
 * \param[in] 	eddsa_variant 			Flag indicating a pre-hashed EdDSA (1) message or a pure EdDSA (0) plaintext message. Used only in case of Edwards25519 public key
 * \param[out] 	pSignature_validity		Signature validity flag (1 = valid signature, invalid otherwise)
 * \return     \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 *
 * \note The expected signature format is the concatenation of R and S values (R|S as defined in ASN.1 DER encoding).
         The public key format must match the expected ECC representation (typically concatenation of X and Y coordinates for uncompressed, or X with a prefix for compressed)
 *
 * \details 	\include{doc} stse_ecc_verify_signature.dox
 */
stse_ReturnCode_t stse_ecc_verify_signature(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pSignature,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 eddsa_variant,
    PLAT_UI8 *pSignature_validity);

/**
 * \brief 		Generate signature over message in arguments
 * \details 	This API use the STSE to sign a message using a key in the requested slot
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	slot_number 			Signature key slot
 * \param[in]  key_type        Signature key type
 * \param[in]  pMessage        Message to sign
 * \param[in]  message_length  Message length
 * \param[out] pSignature      Signature buffer (concatenation of R and S)
 * \return     \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * 
 * \note The signature output is the concatenation of R and S values (R|S as defined in ASN.1 DER encoding).
 * 
 * \details 	\include{doc} stse_ecc_generate_signature.dox
 */
stse_ReturnCode_t stse_ecc_generate_signature(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pMessage,
    PLAT_UI16 message_length,
    PLAT_UI8 *pSignature);

/**
 * \brief 		Establish a shared secret
 * \details 	This API use the STSE to establish a shared secret based on the private key in the target slot
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[in] 	private_key_slot_number 	Private key slot
 * \param[in] 	key_type 					Private key type
 * \param[in] 	pPublic_key 				Remote public key
 * \param[out] 	pShared_secret				Shared secret returned by the STSE
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * 
 * \details 	\include{doc} stse_ecc_establish_shared_secret.dox
 * 
 * \note The public key format must match the expected ECC representation (typically concatenation of X and Y coordinates for uncompressed, or X with a prefix for compressed).
 */
stse_ReturnCode_t stse_ecc_establish_shared_secret(
    stse_Handler_t *pSTSE,
    PLAT_UI8 private_key_slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key,
    PLAT_UI8 *pShared_secret);

/**
 * \brief 		Decompress a compressed public key (NIST or Brainpool)
 * \details 	This API use the STSE to decompress a compressed public key
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[in] 	key_type 					Public key type
 * \param[in] 	point_representation_id 	Parity of the Y (0x02=even & 0x03=odd Y coordinate)
 * \param[in] 	pPublic_key_X 				X coordinate given by the compressed public key
 * \param[out] 	pPublic_key_Y 				Computed Y coordinate output
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_ecc_decompress_public_key(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 point_representation_id,
    PLAT_UI8 *pPublic_key_X,
    PLAT_UI8 *pPublic_key_Y);

/** @}*/

#endif /*STSE_ECC_H*/
