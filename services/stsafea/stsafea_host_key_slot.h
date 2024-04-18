 /******************************************************************************
 * \file	stsafe_symmetric_key_crypto_services.h
 * \brief   STSAFE Middleware services for symmetric key cryptography (header)
 * \author  STMicroelectronics - SMD application team
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

#ifndef STSAFEA_HOST_KEY_SLOT_H
#define STSAFEA_HOST_KEY_SLOT_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_put_query.h"


/** \defgroup 	stsafea_host_key_slot Host key slot management
 *  \ingroup 	stsafea_services
 *  @{
 */

#define STSAFEA_HOST_AES_BLOCK_SIZE						16U
#define STSAFEA_HOST_AES_128_MAC_KEY_SIZE 				16U
#define STSAFEA_HOST_AES_128_CIPHER_KEY_SIZE			16U
#define STSAFEA_HOST_AES_128_KEYS_ENVELOPE_SIZE			48U
#define STSAFEA_HOST_AES_128_KEYS_SIZE 					STSAFEA_HOST_AES_128_MAC_KEY_SIZE \
														+ STSAFEA_HOST_AES_128_CIPHER_KEY_SIZE

#define STSAFEA_HOST_AES_256_MAC_KEY_SIZE 				32U
#define STSAFEA_HOST_AES_256_CIPHER_KEY_SIZE			32U
#define STSAFEA_HOST_AES_256_KEYS_ENVELOPE_SIZE			80U
#define STSAFEA_HOST_AES_256_KEYS_SIZE 					STSAFEA_HOST_AES_256_MAC_KEY_SIZE \
														+ STSAFEA_HOST_AES_256_CIPHER_KEY_SIZE

#define STSAFEA_COMPANION_MAC_SIZE						4U
#define STSAFEA_KEY_USAGE_LIMIT_SIZE 					2U

#define STSAFEA_KEK_KEY_SIZE 							STSAFEA_AES_256_KEY_SIZE
#define STSAFEA_KEK_HKDF_SALT_SIZE 						STSAFEA_SHA_256_HASH_SIZE
#define STSAFEA_KEK_HKDF_SALT 	   						{0xE4,0x5D,0x4C,0xFD,0x20,0x15,0xDE,0x9E,\
														0x0A,0xA9,0xA9,0xF6,0xA4,0x9F,0x45,0x10,\
														0x0F,0x99,0xD9,0x63,0x3F,0xB1,0xB2,0xDD,\
														0xDB,0xEE,0xE6,0x75,0xD5,0x4D,0xE2,0x5B}
#define STSAFEA_WORKING_KEK_HKDF_INFO_SIZE				3U
typedef enum
{
	STSAFEA_AES_128_HOST_KEY = 0,
	STSAFEA_AES_256_HOST_KEY
}stsafea_host_key_type_t;

typedef struct{
	PLAT_UI8 key_presence_flag;
	PLAT_UI8 cmac_sequence_counter[3];
}stsafea_host_key_slot_t;

typedef struct{
	PLAT_UI8 key_presence_flag;
	PLAT_UI8 key_type;
	PLAT_UI8 cmac_sequence_counter[4];
}stsafea_host_key_slot_v2_t;

typedef struct
{
	PLAT_UI8 host_mac_key[STSAFEA_HOST_AES_128_MAC_KEY_SIZE];
	PLAT_UI8 host_cipher_key[STSAFEA_HOST_AES_128_CIPHER_KEY_SIZE];
}stsafea_aes_128_host_keys_t;

typedef struct
{
	PLAT_UI8 host_mac_key[STSAFEA_HOST_AES_256_MAC_KEY_SIZE];
	PLAT_UI8 host_cipher_key[STSAFEA_HOST_AES_256_CIPHER_KEY_SIZE];
}stsafea_aes_256_host_keys_t;

typedef union
{
	stsafea_aes_128_host_keys_t aes_128_key;
	stsafea_aes_256_host_keys_t aes_256_key;
}stsafea_host_keys_t;

typedef struct
{
	PLAT_UI8 wrapped_anonymous : 1;
	PLAT_UI8 plaintext 	  : 1;
	PLAT_UI8 reprovision  : 1;
	PLAT_UI8 change_right : 1;
	PLAT_UI8 filler : 4;
	PLAT_UI8 wrapped_authentication_key;
}stsafea_host_key_provisioning_ctrl_fields_t;

/**
 * \brief 		Query host key provisioning control fields
 * \details 	This service format and send query host key provisioning control fields command
 * \param[in] 	pSTSE 				Pointer to STSAFE Handler
 * \param[out] 	pCtrl_fields		Pointer to the provisioning control fields
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Put host key provisioning control fields
 * \details 	This service format and send put host key provisioning control fields command
 * \param[in] 	pSTSE 				Pointer to STSAFE Handler
 * \param[out] 	pCtrl_fields		Pointer to the provisioning control fields
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_put_host_key_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Query host key informations (host key V1)
 * \details 	This service format and send query host key informations command
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[out] 	pHostKeySlot		Pointer to the structure to output key informations
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key(
		stse_Handler_t * pSTSE,
		stsafea_host_key_slot_t *pHostKeySlot);

/**
 * \brief 		Query host key informations (host key V2)
 * \details 	This service format and send query host key informations command
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[out] 	pHostKeySlotV2		Pointer to the structure to output key informations
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key_v2(
		stse_Handler_t * pSTSE,
		stsafea_host_key_slot_v2_t *pHostKeySlotV2);

/**
 * \brief 		Provision host key V1
 * \details 	Provision host key using the put attributes command
 * \param[in] 	pSTSE 	Pointer to STSAFE Handler
 * \param[in] 	host_keys	Host key structure to be provisionned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_put_attribute_host_key(
		stse_Handler_t * pSTSE ,
		stsafea_aes_128_host_keys_t* host_keys);

/**
 * \brief 		Provision host key V2
 * \details 	Provision host key using the write host key command
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	key_type			Host key type
 * \param[in] 	host_keys			Host key structure to be provisionned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_host_key_provisioning (
		stse_Handler_t * pSTSE,
		stsafea_host_key_type_t key_type,
		stsafea_host_keys_t* host_keys);

/**
 * \brief 		Provision host key V2 wrapped
 * \details 	Provision host key using the write host key command
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	key_type			Host key type
 * \param[in] 	pHost_key_envelope	Envelope containing host key to be provisionned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_host_key_provisioning_wrapped (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t key_type,
		PLAT_UI8* pHost_key_envelope);

/** \}*/

#endif /*STSAFEA_HOST_KEY_SLOT_H*/

