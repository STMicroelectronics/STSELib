 /******************************************************************************
 * \file	stsafe_asymmetric_key_crypto_services.h
 * \brief   STSAFE-Axxx Middleware Asymmetric key cryptography services (header)
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

#ifndef STSAFEA_ASYMMETRIC_KEY_SLOTS_H
#define STSAFEA_ASYMMETRIC_KEY_SLOTS_H

/*
 *  \defgroup 	stsafe_asymmetric_key_crypto Asymmetric key slots management
 *  \ingroup 	stsafea_services
 *  \brief		STSAFE Middleware Asymmetric key cryptography  services
 *  \details  	The Services layer provide set of functions that format all commands supported \n
 *  			by the targeted secure element and reports response to higher layers API/Application
 *  @{
 */


#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_put_query.h"

/*! \defgroup stsafea_asymmetric_key_slots  Asymmetric key slots
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH 	4U
#define STSAFEA_EPHEMERAL_KEY_USAGE_LIMIT						1U

/*!
 * \struct stsafea_private_key_operation_mode_t
 * \brief stsafea private key operation mode type
 */
typedef struct stsafea_private_key_operation_mode_t{
	/* 1st byte */
	PLAT_UI8 ephemeral_private_key_gen_method	 : STSE_1BIT_LEN; /*!< ephemeral_private_key_gen_method */
	PLAT_UI8 filler 							 : STSE_6BIT_LEN; /*!< filler (always 00) */
	PLAT_UI8 change_right 						 : STSE_1BIT_LEN; /*!< change_right */
	/* 2nd byte */
	PLAT_UI8 key_establishment 					 : STSE_1BIT_LEN; /*!< key_establishment*/
	PLAT_UI8 reserved 			 				 : STSE_1BIT_LEN; /*!< reserved = 0*/
	PLAT_UI8 sig_gen_over_external_data 		 : STSE_1BIT_LEN; /*!< sig_gen_over_external_data*/
	PLAT_UI8 sig_gen_over_internal_external_data : STSE_1BIT_LEN; /*!< sig_gen_over_internal_external_data*/
	PLAT_UI8 generate_key_AC 					 : STSE_2BIT_LEN; /*!< generate_key_AC*/
	PLAT_UI8 EdDSA_variant 						 : STSE_2BIT_LEN; /*!< EdDSA_variant*/
}stsafea_private_key_operation_mode_t;

/*!
 * \struct stsafea_private_key_slot_information_t
 * \brief stsafea private key slot information type
 */
typedef struct stsafea_private_key_slot_information_t{
	PLAT_UI8 slot_number;									/* !< slot_number*/
	PLAT_UI8 presence_flag;									/* !< presence_flag*/
	stsafea_private_key_operation_mode_t mode_of_operation; /* !< mode_of_operation*/
	stsafea_ecc_curve_id_t curve_id;						/* !< curve_id*/
}stsafea_private_key_slot_information_t;

/*!
 * \struct stsafea_generic_public_key_configuration_flags_t
 * \brief stsafea generic public key configuration flags type
 */
typedef struct stsafea_generic_public_key_configuration_flags_t{
	PLAT_UI8 filler_byte;
	PLAT_UI8 change_right 				: STSE_1BIT_LEN;
	PLAT_UI8 establish_symmetric_key 	: STSE_1BIT_LEN;
	PLAT_UI8 start_volatile_kek_session : STSE_1BIT_LEN;
	PLAT_UI8 filler 					: STSE_6BIT_LEN;
}stsafea_generic_public_key_configuration_flags_t;

/*!
 * \brief 		Encrypt a message
 * \details 	This service format and send the reset command
 * \param[in] 	pSTSE 						Pointer to STSAFE Handler
 * \param[out] 	pPrivate_key_slot_count 	Target STSAFE-A total Private key slot count
 * \return 		\ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_private_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pPrivate_key_slot_count);

stse_ReturnCode_t stsafea_query_private_key_table(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_count,
		PLAT_UI8 * pChange_right,
		PLAT_UI16 * pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_table_info);

stse_ReturnCode_t stsafea_query_generic_public_key_slot_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pGeneric_public_key_slot_count);

stse_ReturnCode_t stsafea_query_generic_public_key_slot_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 * pPresence_flag,
		stsafea_generic_public_key_configuration_flags_t * pConfiguration_flags,
		stse_ecc_key_type_t * pKey_type);

stse_ReturnCode_t stsafea_query_generic_public_key_slot_value(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 * pPublic_key);

stse_ReturnCode_t stsafea_generate_ecc_key_pair(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI16 usage_limit,
		PLAT_UI8 * pPublic_key);

stse_ReturnCode_t stsafea_write_generic_ecc_public_key(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key);

stse_ReturnCode_t stsafea_generate_ECDHE_key_pair(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 * pPublic_key);

stse_ReturnCode_t stsafea_sign_for_generic_public_key_slot(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t private_key_type,
		PLAT_UI8 *pPrivate_key,
		stse_hash_algorithm_t hash_algo,
		PLAT_UI16 payload_length,
		PLAT_UI8 *pPayload,
		PLAT_UI8 *pSignature);

/** \}*/

#endif /*STSAFEA_ASYMMETRIC_KEY_SLOTS_H*/
