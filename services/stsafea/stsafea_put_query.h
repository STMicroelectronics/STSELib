/*!
 ******************************************************************************
 * \file	stsafea_patch.c
 * \brief   Patch services for STSAFE-A
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

#ifndef STSAFEA_PUT_QUERY_H
#define STSAFEA_PUT_QUERY_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*  ---------------------------------------------------------------------------------------
 *
 *						       QUERY/PUT/GET Subject tags definitions
 *
 *  ---------------------------------------------------------------------------------------
*/
/*! \defgroup stsafea_put_query Put Query
 *  \ingroup stsafea_services
 *  @{
 */


#define STSAFEA_SUBJECT_TAG_ADMIN_BASE_KEY_SLOT 		 				0x01	/*!< Admin base key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_P1_BASE_KEY_SLOT_RETRO    					0x02	/*!< P1 base key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_MASTER_KEY_TABLE    		 				0x04	/*!< Master key table subject-tag */
#define STSAFEA_SUBJECT_TAG_LOCAL_ENVELOPE_KEY_TABLE	 				0x07	/*!< Local Envelope key table subject-tag */
#define STSAFEA_SUBJECT_TAG_LIFE_CYCLE_STATE							0x08	/*!< Life cycle state subject-tag */
#define STSAFEA_SUBJECT_TAG_PASSWORD_SLOT								0x09	/*!< Password slot subject-tag */
#define STSAFEA_SUBJECT_TAG_REMANING_CACHE_DERIVATION					0x0C	/*!< Remaining cache deriation subject-tag */
#define STSAFEA_SUBJECT_TAG_I2C_PARAMETERS								0x0A	/*!< I2C Parameters subject-tag */
#define STSAFEA_SUBJECT_TAG_PRODUCT_DATA								0x11	/*!< Product data subject-tag */
#define STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION 				0x12	/*!< data partition configuration subject-tag */
#define STSAFEA_SUBJECT_TAG_PRIVATE_KEY_SLOT 							0x13	/*!< Private key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_PRIVATE_KEY_TABLE 							0x14	/*!< Private key table subject-tag */
#define STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1 		  					0x17	/*!< Host key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_TABLE							0x23	/*!< Symmetric key table subject-tag */
#define STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG 				0x24	/*!< Command authorisation configuration subject-tag */
#define STSAFEA_SUBJECT_TAG_COMMAND_ACCESS_CONDITION 					0x29	/*!< Command access condition subject-tag */
#define STSAFEA_SUBJECT_TAG_COMMAND_HOST_ENCRYPTION_FLAGS 				0x2A	/*!< Command Host encryption subject-tag */
#define STSAFEA_SUBJECT_TAG_P2_BASE_KEY_SLOT    		 				0x35	/*!< P2 base key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_PHI_P_SLOT				 					0x36	/*!< PHI P slot subject-tag */
#define STSAFEA_SUBJECT_TAG_PHI_C_SLOT									0x37	/*!< PHI C slot subject-tag */
#define STSAFEA_SUBJECT_TAG_P1_BASE_KEY_SLOT    		 				0x3C    /*!< P1 base key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_PARENT_KEY    		 				0x3F	/*!< Patch parent key subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_START 								0x40	/*!< Patch start subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_UPDATE 								0x41	/*!< Patch update subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_FINALIZE 								0x42	/*!< Patch finalize subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_SIGNATURE_VERIFICATION_KEY 			0x43	/*!< Patch signature verification key subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_SIGNATURE_CONFIGURATION_FLAGS			0x44	/*!< Patch signature configuration flags subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_UPDATE_COMMAND_COUNTER				0x45	/*!< Patch update command counter subject-tag */
#define STSAFEA_SUBJECT_TAG_PATCH_PARENT_KEY_USAGE_LIMIT				0x46	/*!< Patch Parent key usage limit subject-tag */
#define STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V2 		  					0x47	/*!< Host key slot v2 subject-tag */
#define STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_TABLE 		  			0x48	/*!< Generic public key table subject-tag */
#define STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS	0x4A	/*!< Host key slot provitioning control fields subject-tag */
#define STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_SLOT_PROVISIONING_CONTROL 	0x4B	/*!< Symmetric key slot provitioning control subject-tag */
#define STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT						0x4C    /*!< Generic public key slot subject-tag */
#define STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_CONFIGURATION_FLAGS		0x4D    /*!< Generic public key configuration flags subject-tag */

/*!
 * \enum stsafea_life_cycle_state_t
 * STSAFEA Life cycle state type
 */
typedef enum stsafea_life_cycle_state_t {
    STSAFEA_LCS_BORN                   = 0x01, /*!< STSAFEA in born life-cycle state */
    STSAFEA_LCS_PATCHING               = 0x02, /*!< STSAFEA in patching life-cycle state */
    STSAFEA_LCS_OPERATIONAL            = 0x03, /*!< STSAFEA in operational life-cycle state */
    STSAFEA_LCS_TERMINATED             = 0x04, /*!< STSAFEA in terminated life-cycle state */
    STSAFEA_LCS_BORN_AND_LOCKED        = 0x81, /*!< STSAFEA in born+lock life-cycle state */
    STSAFEA_LCS_OPERATIONAL_AND_LOCKED = 0x83  /*!< STSAFEA in operational+lock life-cycle state */
} stsafea_life_cycle_state_t;


/**
 * \brief 		STSAFEA put life cycle service
 * \details 	This service format and send/receive the put life cycle command/response
 * \param[in] 	pSTSAFE 			Pointer to STSAFE Handler
 * \param[in] 	life_cycle_state 	Life cycle state
 * \return 		\ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_put_life_cyle_state(
		stse_Handler_t * pSTSAFE,
		stsafea_life_cycle_state_t life_cycle_state
);

/**
 * \brief 		STSAFEA put life cycle service
 * \details 	This service format and send/receive the put life cycle command/response
 * \param[in] 	pSTSAFE 			Pointer to STSAFE Handler
 * \param[out] 	pLife_cycle_state 	Life cycle state
 * \return 		\ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_life_cycle_state(
		stse_Handler_t * pSTSAFE,
		stsafea_life_cycle_state_t * pLife_cycle_state
);

/** \}*/

#endif /* STSAFEA_PUT_QUERY_H */
