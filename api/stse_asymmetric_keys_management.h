/*!
 ******************************************************************************
 * \file	stse_asymmetric_keys_management.h
 * \brief   STSE Asymmetric keys management API set (header)
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
 ******************************************************************************
 */

#ifndef STSE_ASYMMETRIC_KEY_MANAGMENT_H
#define STSE_ASYMMETRIC_KEY_MANAGMENT_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_ecc.h"
#include "core/stse_session.h"

/** \defgroup 	stse_api_asymmetric_key_mngt 	STSE Asymmetric key management
 *  \ingroup 	stse_api
 *  \brief		STSecureElement asymmetric cryptography API
 *  \details  	The Application Programming Interface (API) layer is the entry point for the upper system application layer. \n
 *  			It provides high level functions to the application layer for symmetric asymmetric key management on target STMicroelectronics Secure element device .
 *  @{
 */

/**
 * \brief 		Get ECC key slots count
 * \details 	This API query STSAFE-A and return the number of slots in the private key table
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[out] 	pPrivate_key_slot_count 	ECC key slots count
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_get_ecc_key_slots_count.dox
 */
stse_ReturnCode_t stse_get_ecc_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pPrivate_key_slot_count);

/**
 * \brief 		Get ECC key table informations
 * \details 	This API query STSAFE-A and return ecc key informations list for all key slots in the table
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[in] 	private_key_slot_count 		Expected slot count in the table (length of private_key_table_info)
 * \param[out] 	pChange_right 				Private key table information change right
 * \param[out] 	pGlobal_usage_limit 		Private key table information global usage limit
 * \param[out] 	private_key_table_info 		Key slot informations table
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_get_ecc_key_table_info.dox
 */
stse_ReturnCode_t stse_get_ecc_key_table_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_count,
		PLAT_UI8 *pChange_right,
		PLAT_UI16 *pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_table_info);

/**
 * \brief 		Get ECC key slot informations
 * \details 	This API query STSAFE-A and return ecc key informations for a given key slot
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[in] 	private_key_slot_number 	Slot number to query
 * \param[out] 	pChange_right 				Private key table information change right
 * \param[out] 	pGlobal_usage_limit 		Private key table information global usage limit
 * \param[out] 	private_key_slot_info 		Key slot informations
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_get_ecc_key_slot_info.dox
 */
stse_ReturnCode_t stse_get_ecc_key_slot_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_number,
		PLAT_UI8 * pChange_right,
		PLAT_UI16 * pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_slot_info);

/**
 * \brief 		Generate an ECDHE key pair
 * \details 	This API request STSE to generate an ECDHE key pair
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	key_type 		Key type to generate
 * \param[out] 	pPublic_key 	Public key of the generated key pair
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_generate_ECDHE_key_pair.dox
 */
stse_ReturnCode_t stse_generate_ECDHE_key_pair(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 * pPublic_key);

/**
 * \brief 		Generate an ECC key pair
 * \details 	This API request STSE to generate an ECC key pair in the private key table
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 	Private key table slot used to generate the key pair
 * \param[in] 	key_type 		Key pair type
 * \param[in] 	usage_limit 	Usage limit of the key pair
 * \param[out] 	pPublic_key 	Public key of the generated key pair
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_generate_ecc_key_pair.dox
 */
stse_ReturnCode_t stse_generate_ecc_key_pair(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI16 usage_limit,
		PLAT_UI8 * pPublic_key);

/**
 * \brief 		Write a public key in a generic public key slot
 * \details 	This API write a public key in the STSE generic public key slot
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	slot_number 	Generic public key slot to write
 * \param[in] 	key_type 		Public key type
 * \param[in] 	pPublic_key 	Public key to write
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_write_generic_ecc_public_key.dox
 */
stse_ReturnCode_t stse_write_generic_ecc_public_key(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key);


/** \}*/

#endif /*STSAFE_ASYMMETRIC_KEY_CRYPTO_H*/

