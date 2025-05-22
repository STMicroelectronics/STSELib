/*!
 ******************************************************************************
 * \file	stsafea_public_key_slots.h
 * \brief   STSAFE Middleware services for genric public slots (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSAFEA_PUBLIC_KEY_SLOTS_H
#define STSAFEA_PUBLIC_KEY_SLOTS_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_public_key_slots STSAFE-A Generic public key slots
 * management \ingroup stsafea_services
 *  @{
 */

/*!
 * \struct stsafea_generic_public_key_configuration_flags_t
 * \brief stsafea generic public key configuration flags type
 */
typedef struct stsafea_generic_public_key_configuration_flags_t {
	PLAT_UI8 filler_byte;
	PLAT_UI8 change_right : STSE_1BIT_LEN;
	PLAT_UI8 establish_symmetric_key : STSE_1BIT_LEN;
	PLAT_UI8 start_volatile_kek_session : STSE_1BIT_LEN;
	PLAT_UI8 entity_authentication : STSE_1BIT_LEN;
	PLAT_UI8 establish_host_key : STSE_1BIT_LEN;
	PLAT_UI8 filler : STSE_3BIT_LEN;
} stsafea_generic_public_key_configuration_flags_t;

/*!
 * \brief 		Get public key slots count
 * \details 	This service format and send/receive the get public key slots
 * count command/response \param[in] 	pSTSE
 * Pointer to STSE Handler \param[out] 	pGeneric_public_key_slot_count Target
 * STSAFE-A total public key slot count \return 		\ref STSE_OK on
 * success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t
stsafea_query_generic_public_key_slots_count(stse_Handler_t *pSTSE,
					     PLAT_UI8 *pGeneric_public_key_slot_count);

/*!
 * \brief 		Get public key slot informations
 * \details 	This service format and send/receive the get public key slot's
 * informations command/response \param[in] 	pSTSE
 * Pointer to STSE Handler
 * \param[in] 	slot_number 			Public key slot value
 * \param[out] 	pPresence_flag 			Pointer indicating presence of
 * public into selected slot \param[out] 	pConfiguration_flags 	Pointer
 * indicating indicating configuration usage of public key slot \param[out]
 * pKey_type 				Pointer indicating key type of public
 * key
 * present into selected slot \return 		\ref STSE_OK on success ; \ref
 * stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_generic_public_key_slot_info(
	stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI8 *pPresence_flag,
	stsafea_generic_public_key_configuration_flags_t *pConfiguration_flags,
	stse_ecc_key_type_t *pKey_type);

/*!
 * \brief 		Get public key slot value
 * \details 	This service format and send/receive the get public key slot's
 * value command/response \param[in] 	pSTSE
 * Pointer to STSE Handler
 * \param[in] 	slot_number 			Public key slot value
 * \param[in] 	key_type 				Targeted STSAFE-A public
 * key
 * type \param[out] 	pPublic_key 			Targeted STSAFE-A public
 * key value \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t stsafea_query_generic_public_key_slot_value(stse_Handler_t *pSTSE,
							      PLAT_UI8 slot_number,
							      stse_ecc_key_type_t key_type,
							      PLAT_UI8 *pPublic_key);

/*!
 * \brief 		Write public into specified key slot
 * \details 	This service format and send/receive the write public into
 * specified key slot command/response
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	slot_number 			Public key slot value
 * \param[in] 	key_type 				Targeted STSAFE-A public
 * key
 * type \param[out] 	pPublic_key 			Targeted STSAFE-A public
 * key value \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t stsafea_write_generic_ecc_public_key(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
						       stse_ecc_key_type_t key_type,
						       PLAT_UI8 *pPublic_key);

/**
 * \brief 		STSAFEA put generic public slot's configuration flags
 * service \details 	This service set configuration flags of generic public
 * slot \param[in] 	pSTSE 				    Pointer to STSE
 * Handler \param[in] 	slot_number 			Public key slot value
 * \param[out] 	configuration_flags 	Generic public key slot's configuration
 * flags \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t
 * error code otherwise
 */
stse_ReturnCode_t stsafea_set_generic_public_slot_configuration_flag(
	stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
	stsafea_generic_public_key_configuration_flags_t configuration_flags);

/** \}*/

#endif /*STSAFEA_PUBLIC_KEY_SLOTS_H */
