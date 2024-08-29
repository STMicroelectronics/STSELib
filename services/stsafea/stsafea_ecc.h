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

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"


/*! \defgroup stsafea_ecc ECC
 *  \ingroup stsafea_services
 *  @{
 */

typedef enum
{
	STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING 		= 0x01,
	STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY
}stsafea_host_kdf_algo_id_t;

stse_ReturnCode_t stsafea_start_volatile_KEK_session(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8* host_ecdhe_public_key);

stse_ReturnCode_t stsafea_start_volatile_KEK_session_authenticated(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t ecdhe_key_type,
		PLAT_UI8* host_ecdhe_public_key,
		stse_hash_algorithm_t hash_algo,
		PLAT_UI8 signature_public_key_slot_number,
		stse_ecc_key_type_t signature_key_type,
		PLAT_UI8* pSignature);

stse_ReturnCode_t stsafea_stop_volatile_KEK_session(
		stse_Handler_t * pSTSE);

stse_ReturnCode_t stsafea_ecc_verify_signature(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key,
		PLAT_UI8 *pSignature,
		PLAT_UI8 *pMessage,
		PLAT_UI16 message_length,
		PLAT_UI8 message_is_hashed,
		PLAT_UI8 *pSignature_validity);

stse_ReturnCode_t stsafea_ecc_generate_signature(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pMessage,
		PLAT_UI16 message_length,
		PLAT_UI8 *pSignature);

stse_ReturnCode_t stsafea_ecc_establish_shared_secret(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key,
		PLAT_UI8 *pShared_secret);

stse_ReturnCode_t stsafea_ecc_decompress_public_key(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 point_representation_id,
		PLAT_UI8 *pPublic_key_X,
		PLAT_UI8 *pPublic_key_Y);


stse_ReturnCode_t stsafea_verify_entity_signature(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pSignature,
		PLAT_UI8 *pSignature_validity);

/** \}*/

#endif /* STSAFEA_ECC_H */
