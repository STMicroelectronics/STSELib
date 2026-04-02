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

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_timings.h"

/*!
 *  \defgroup stsafea_asymmetric_key_crypto STSAFE-A Asymmetric key slots management
 *  \ingroup stsafea_services
 *  \brief STSAFEA Asymmetric key cryptography services
 *  \details The STSELib services layer provide set of functions that format all commands supported \n
 *  	by the targeted secure element and reports response to higher layers API/Application
 *  @{
 */

#define STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH 4U
#define STSAFEA_EPHEMERAL_KEY_USAGE_LIMIT 1U

/*!
 * \struct stsafea_private_key_operation_mode_t
 * \brief stsafea private key operation mode type
 */
typedef struct stsafea_private_key_operation_mode_t {
    /* 1st byte */
    PLAT_UI8 ephemeral_private_key_gen_method : STSE_1BIT_LEN; /*!< ephemeral_private_key_gen_method */
    PLAT_UI8 filler : STSE_6BIT_LEN;                           /*!< filler (always 00) */
    PLAT_UI8 change_right : STSE_1BIT_LEN;                     /*!< change_right */
    /* 2nd byte */
    PLAT_UI8 key_establishment : STSE_1BIT_LEN;                   /*!< key_establishment*/
    PLAT_UI8 reserved : STSE_1BIT_LEN;                            /*!< reserved = 0*/
    PLAT_UI8 sig_gen_over_external_data : STSE_1BIT_LEN;          /*!< sig_gen_over_external_data*/
    PLAT_UI8 sig_gen_over_internal_external_data : STSE_1BIT_LEN; /*!< sig_gen_over_internal_external_data*/
    PLAT_UI8 generate_key_AC : STSE_2BIT_LEN;                     /*!< generate_key_AC*/
    PLAT_UI8 EdDSA_variant : STSE_2BIT_LEN;                       /*!< EdDSA_variant*/
} stsafea_private_key_operation_mode_t;

/*!
 * \struct stsafea_private_key_slot_information_t
 * \brief stsafea private key slot information type
 */
typedef struct stsafea_private_key_slot_information_t {
    PLAT_UI8 slot_number;                                   /* !< slot_number*/
    PLAT_UI8 presence_flag;                                 /* !< presence_flag*/
    stsafea_private_key_operation_mode_t mode_of_operation; /* !< mode_of_operation*/
    stsafea_ecc_curve_id_t curve_id;                        /* !< curve_id*/
} stsafea_private_key_slot_information_t;

/*!
 * \brief 		Get private key slot count
 * \details 	This service format and send the get private key slot count command
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[out] 	pPrivate_key_slot_count 	Target STSAFE-A total Private key slot count
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_private_key_slots_count(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pPrivate_key_slot_count);

/**
 * \brief 		Query private key table information
 * \details 	This service formats and sends the query private key table command
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	private_key_slot_count	Number of private key slots
 * \param[out] 	pGlobal_usage_limit		Pointer to global usage limit value
 * \param[out] 	private_key_table_info	Pointer to private key table information structure
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_private_key_table(
    stse_Handler_t *pSTSE,
    PLAT_UI8 private_key_slot_count,
    PLAT_UI16 *pGlobal_usage_limit,
    stsafea_private_key_slot_information_t *private_key_table_info);

/**
 * \brief 		Generate ECC key pair in specified slot
 * \details 	This service formats and sends the generate ECC key pair command
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \param[in] 	slot_number		Slot number where to generate the key pair
 * \param[in] 	key_type		ECC key type to generate
 * \param[in] 	usage_limit		Usage limit for the generated key
 * \param[out] 	pPublic_key		Pointer to buffer for the public key
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_generate_ecc_key_pair(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stse_ecc_key_type_t key_type,
    PLAT_UI16 usage_limit,
    PLAT_UI8 *pPublic_key);

/*!
 * \brief 		Generate ECDHE key pair
 * \details 	This service format and send the generate ECDHE key pair command
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[in] 	key_type 					Curve type for the ECDHE key pair
 * \param[out] 	pPublic_key 				generated ECDHE public key
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_generate_ECDHE_key_pair(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *pPublic_key);

/** \}*/

#endif /*STSAFEA_ASYMMETRIC_KEY_SLOTS_H*/
