/*!
 ******************************************************************************
 * \file	stse_symmetric_keys_management.h
 * \brief   STSE Symmetric keys management API set (header)
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

#ifndef STSE_SYMMETRIC_KEY_MANAGMENT_H
#define STSE_SYMMETRIC_KEY_MANAGMENT_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_sessions.h"

/** \defgroup 	stse_api_symmetric_key_mngt 	STSE Symmetric key management
 *  \ingroup 	stse_api
 *  \brief		STSecureElement symmetric cryptography API
 *  \details  	The Application Programming Interface (API) layer is the entry point for the upper system application layer. \n
 *  			It provides high level functions to the application layer for symmetric symemtric key management on target STMicroelectronics Secure element device .
 *  @{
 */

/**
 * \brief 		Host key provisioning in plaintext
 * \details 	This API perform host key provisioning in plaintext, \n
 * 				it use the put attribute command if the device isn't a STSAFE-A120 \n
 * 				and it use the "write host key" command if it is a STSAFE-A120
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	host_ecc_key_type 		Key type
 * \param[in] 	host_keys 			Pointer to the key structure
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_host_key_provisioning.dox
 */
stse_ReturnCode_t stse_host_key_provisioning (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_ecc_key_type,
		stsafea_host_keys_t* host_keys);

/**
 * \brief 		Host key provisioning wrapped
 * \details 	This API perform host key provisioning with the key wrapped using a volatile KEK session
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	host_key_type 		Key type
 * \param[in] 	host_keys 			Pointer to the key structure
 * \param[in] 	ecdhe_key_type 		ECC key type to use in KEK session
 * \return 		\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_host_key_provisioning_wrapped.dox
 */
stse_ReturnCode_t stse_host_key_provisioning_wrapped (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_key_type,
		stsafea_host_keys_t* host_keys,
		stse_ecc_key_type_t ecdhe_key_type);

/**
 * \brief 		Host key provisioning wrapped and authenticated
 * \details 	This API perform host key provisioning with the key wrapped using an authenticated volatile KEK session
 * \param[in] 	pSTSE 								Pointer to STSAFE Handler
 * \param[in] 	host_key_type 						Key type
 * \param[in] 	host_keys 							Pointer to the key structure
 * \param[in] 	ecdhe_key_type 						ECC key type to use in KEK session
 * \param[in] 	signature_public_key_slot_number 	Slot number of the public key used in authentication
 * \param[in] 	signature_hash_algo 				Hash algo used for the signature digest
 * \param[in] 	signature_private_key_type 			Key type of the private key used in authentication
 * \param[in] 	signature_private_key 				Private key used in authentication
 * \return 		\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_host_key_provisioning_wrapped_authenticated.dox
 */
stse_ReturnCode_t stse_host_key_provisioning_wrapped_authenticated (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t host_key_type,
		stsafea_host_keys_t* host_keys,
		stse_ecc_key_type_t ecdhe_key_type,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t signature_hash_algo,
		stse_ecc_key_type_t signature_private_key_type,
		PLAT_UI8 * signature_private_key);

/**
 * \brief 		Get symmetric key slot count
 * \details 	Get the number of slot in symmetric key table
 * \param[in] 	pSTSE 			 		Pointer to STSAFE Handler
 * \param[out] 	pSymmetric_key_slot_count	Number of slot in symmetric key table
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_get_symmetric_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 *pSymmetric_key_slot_count);

/**
 * \brief 		Get one symmetric key slot info
 * \details 	Get the informations of one slot in symmetric key table
 * \param[in] 	pSTSE 					Pointer to STSAFE Handler
 * \param[in] 	slot_number					Slot number to get the information
 * \param[out]  pSymmetric_key_slot_info	Buffer to store the information structure
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_get_symmetric_key_slot_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stsafea_symmetric_key_slot_information_t * pSymmetric_key_slot_info);

/**
 * \brief 		Get symmetric key slot info
 * \details 	Get the informations of all slot of symmetric key table
 * \param[in] 	pSTSE 			 			Pointer to STSAFE Handler
 * \param[in]	total_slot_count			Target Device total symmetric key slots number
 * \param[out] 	symmetric_key_table_info	Buffer to store the information structure list for all slots
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_get_symmetric_key_table_info(
		stse_Handler_t * pSTSE,
		PLAT_UI16	total_slot_count,
		stsafea_symmetric_key_slot_information_t * symmetric_key_table_info);

/**
 * \brief 		Get symmetric key slot provisioning control fields
 * \details 	Get symmetric key slot provisioning control fields
 * \param[in] 	pSTSE 				Pointer to STSAFE Handler
 * \param[in] 	slot_number			Slot number of the slot to query
 * \param[out] 	pCtrl_fields		Output structure for the provisioning control fields of the target slot
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_get_symmetric_key_slot_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stsafea_symmetric_key_slot_provisioning_ctrl_fields_t * pCtrl_fields);

/**
 * \brief 		Write symmetric key plaintext in stsafe symmetric key table
 * \details 	This API Write a symmetric key in stsafe symmetric key table
 * \param[in] 	pSTSE 			 	 Pointer to STSAFE Handler
 * \param[in] 	pKey 				 Key material to write in STSAFE
 * \param[in] 	key_info 			 Key information used to populate the symmetric key table
 * \param[in]	kek_session_ecc_type Key type to use in volatile KEK session
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_write_symmetric_key_plaintext.dox
 */
stse_ReturnCode_t stse_write_symmetric_key_plaintext(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pKey,
		stsafea_generic_key_information_t * key_info);

/**
 * \brief 		Write symmetric key wrapped in stsafe symmetric key table
 * \details 	This API Write a symmetric key in stsafe symmetric key table inside a volatile KEK session
 * \param[in] 	pSTSE 			 	 Pointer to STSAFE Handler
 * \param[in] 	pKey 				 Key material to write in STSAFE
 * \param[in] 	key_info 			 Key information used to populate the symmetric key table
 * \param[in]	kek_session_ecc_type Key type to use in volatile KEK session
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_write_symmetric_key_wrapped.dox
 */
stse_ReturnCode_t stse_write_symmetric_key_wrapped(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pKey,
		stsafea_generic_key_information_t * key_info,
		stse_ecc_key_type_t kek_session_ecc_type);

/**
 * \brief 		Authenticated write symmetric key in stsafe symmetric key table
 * \details 	This API Write a symmetric key in stsafe symmetric key table inside an authenticated volatile KEK session
 * \param[in] 	pSTSE 			 					Pointer to STSAFE Handler
 * \param[in] 	pKey 				 				Key material to write in STSAFE
 * \param[in] 	key_info 			 				Key information used to populate the symmetric key table
 * \param[in]	kek_session_ecc_type 				Key type to use in volatile KEK session
 * \param[in] 	signature_public_key_slot_number 	Slot number of the public key used in authentication
 * \param[in] 	signature_hash_algo 				Hash algo used for the signature digest
 * \param[in] 	signature_private_key_type 			Key type of the private key used in authentication
 * \param[in] 	signature_private_key 				Private key used in authentication
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_write_symmetric_key.dox
 */
stse_ReturnCode_t stse_write_symmetric_key_wrapped_authenticated(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pKey,
		stsafea_generic_key_information_t * key_info,
		stse_ecc_key_type_t kek_session_ecc_type,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t signature_hash_algo,
		stse_ecc_key_type_t signature_private_key_type,
		PLAT_UI8 * signature_private_key);

/**
 * \brief 		Establish symmetric key in stsafe symmetric key table
 * \details 	This API establish one or more symmetric keys in stsafe symmetric key table through an ECDHE process
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	ecc_key_type 		Key type to use for the ECDHE
 * \param[in] 	key_infos_list 		Key information list used to populate the symmetric key table
 * \param[out]	key_list			Key list resulting of the ECDHE ( OKM[1..N] )
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_establish_symmetric_key.dox
 */
stse_ReturnCode_t stse_establish_symmetric_key(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t ecc_key_type,
		PLAT_UI8 key_infos_count,
		stsafea_generic_key_information_t * key_infos_list,
		PLAT_UI8 * key_list);

/**
 * \brief 		Authenticated establish symmetric key in stsafe symmetric key table
 * \details 	This API establish one or more symmetric keys in stsafe symmetric key table through an authenticated ECDHE process
 * \param[in] 	pSTSE 						 	 Pointer to STSE Handler
 * \param[in] 	key_type 					 	 Key type to use for the ECDHE
 * \param[in] 	key_infos_list 					 Key information list used to populate the symmetric key table
 * \param[out]	key_list						 Key list resulting of the ECDHE ( OKM[1..N] )
 * \param[in] 	signature_public_key_slot_number Generic public key slot number used for the authentication
 * \param[in] 	hash_algo						 Hash algorithm ID used to hash the authentication digest
 * \param[in] 	private_key_type				 Key type of the authentication private key
 * \param[in] 	private_key						 Authentication private key buffer
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stsafe_establish_symmetric_key.dox
 */
stse_ReturnCode_t stse_establish_symmetric_key_authenticated(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 key_infos_count,
		stsafea_generic_key_information_t * key_infos_list,
		PLAT_UI8 * key_list,
		PLAT_UI8 signature_public_key_slot_number,
		stse_hash_algorithm_t hash_algo,
		stse_ecc_key_type_t private_key_type,
		PLAT_UI8* private_key);

/** \}*/

#endif /*STSAFE_SYMMETRIC_KEY_CRYPTO_H*/

