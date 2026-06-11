/******************************************************************************
 * \file	stsafe_symmetric_key_crypto_services.h
 * \brief   STSAFE-A services for host key slot management (header)
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

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_timings.h"

/** \defgroup stsafea_host_key_slot STSAFE-A Host key slot management
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_HOST_AES_BLOCK_SIZE 16U

#define STSAFEA_HOST_AES_128_KEYS_ENVELOPE_SIZE 48U
#define STSAFEA_HOST_AES_256_KEYS_ENVELOPE_SIZE 80U

#define STSAFEA_KEK_KEY_SIZE STSE_AES_256_KEY_SIZE
#define STSAFEA_KEK_HKDF_SALT_SIZE STSAFEA_SHA_256_HASH_SIZE
#define STSAFEA_KEK_HKDF_SALT                        \
    {0xE4, 0x5D, 0x4C, 0xFD, 0x20, 0x15, 0xDE, 0x9E, \
     0x0A, 0xA9, 0xA9, 0xF6, 0xA4, 0x9F, 0x45, 0x10, \
     0x0F, 0x99, 0xD9, 0x63, 0x3F, 0xB1, 0xB2, 0xDD, \
     0xDB, 0xEE, 0xE6, 0x75, 0xD5, 0x4D, 0xE2, 0x5B}
#define STSAFEA_WORKING_KEK_HKDF_INFO_SIZE 3U

typedef struct
{
    PLAT_UI8 key_presence_flag;
    PLAT_UI8 cmac_sequence_counter[3];
} stsafea_host_key_slot_t;

typedef struct
{
    PLAT_UI8 key_presence_flag;
    PLAT_UI8 key_type;
    PLAT_UI8 cmac_sequence_counter[4];
} stsafea_host_key_slot_v2_t;

typedef struct
{
    PLAT_UI8 wrapped_anonymous : STSE_1BIT_LEN;
    PLAT_UI8 plaintext : STSE_1BIT_LEN;
    PLAT_UI8 reprovision : STSE_1BIT_LEN;
    PLAT_UI8 change_right : STSE_1BIT_LEN;
    PLAT_UI8 filler : STSE_4BIT_LEN;
    PLAT_UI8 wrapped_or_DH_derived_authentication_key;
} stsafea_host_key_provisioning_ctrl_fields_t;

/**
 * \brief 		Query host key provisioning control fields
 * \details 	This service format and send query host key provisioning control fields command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[out] 	pCtrl_fields		Pointer to the provisioning control fields
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key_provisioning_ctrl_fields(
    stse_Handle_t *pSTSE,
    stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Put host key provisioning control fields
 * \details 	This service format and send put host key provisioning control fields command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[out] 	pCtrl_fields		Pointer to the provisioning control fields
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_put_host_key_provisioning_ctrl_fields(
    stse_Handle_t *pSTSE,
    stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Query host key informations (host key V1)
 * \details 	This service format and send query host key informations command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[out] 	pHostKeySlot		Pointer to the structure to output key informations
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key(
    stse_Handle_t *pSTSE,
    stsafea_host_key_slot_t *pHostKeySlot);

/**
 * \brief 		Query host key informations (host key V2)
 * \details 	This service format and send query host key informations command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[out] 	pHostKeySlotV2		Pointer to the structure to output key informations
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_query_host_key_v2(
    stse_Handle_t *pSTSE,
    stsafea_host_key_slot_v2_t *pHostKeySlotV2);

/**
 * \brief 		Provision host secure channel keys V1
 * \details 	Provision host secure channel keys using the put attributes command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	host_mac_key		Pointer to 128 bits host MAC key (AES key structure) to be provisioned
 * \param[in] 	host_cipher_key		Pointer to 128 bits Host cipher key (AES key structure) to be provisioned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_put_attribute_host_secure_channel_keys(
    stse_Handle_t *pSTSE,
    stse_aes_key_t *host_mac_key,
    stse_aes_key_t *host_cipher_key);
/**
 * \brief 		Provision host secure channel keys V2
 * \details 	Provision host secure channel keys using the write host key command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	host_mac_key		Pointer to host MAC key (AES key structure) to be provisioned
 * \param[in] 	host_cipher_key		Pointer to Host cipher key (AES key structure) to be provisioned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_host_secure_channel_keys_provisioning(
    stse_Handle_t *pSTSE,
    stse_aes_key_t *host_mac_key,
    stse_aes_key_t *host_cipher_key);

/**
 * \brief 		Provision host secure channel keys V2 wrapped
 * \details 	Provision host secure channel keys using the write host key command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	key_type			Host key type
 * \param[in] 	pHost_key_envelope	Envelope containing host key to be provisioned
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_host_secure_channel_keys_provisioning_wrapped(
    stse_Handle_t *pSTSE,
    stse_aes_key_type_t key_type,
    PLAT_UI8 *pHost_key_envelope);

/**
 * \brief 		Establish host secure channel keys using ECDH & HKDF processes
 * \details 	Provision host secure channel keys using the establish host key command
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[in]   host_ecdh_public_key_type	ECDHE host public key type
 * \param[in]   pPublic_key 				ECDHE host public key
 * \param[in] 	host_keys_type				Host key type
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_establish_host_secure_channel_keys(
    stse_Handle_t *pSTSE,
    stse_ecc_key_type_t host_ecdh_public_key_type,
    PLAT_UI8 *pPublic_key,
    stse_aes_key_type_t host_keys_type);

/**
 * \brief 		Authenticated establish host secure channel keys using ECDH & HKDF processes
 * \details 	Provision host secure channel keys using the authenticated establish host secure channel keys command
 * \param[in] 	pSTSE 						Pointer to STSE Handler
 * \param[in]   host_ecdh_public_key_type	ECDHE host public key type
 * \param[in]   pPublic_key 				ECDHE host public key
 * \param[in] 	host_keys_type				Host key type
 * \param[in] 	signature_public_key_slot	Generic public key slot holding key used to verify signature
 * \param[in] 	signature_public_key_type	Public key's type stored through generic public key slot
 * \param[in] 	signature_hash_algo			Hashing algorithm used for the signature
 * \param[in] 	pSignature					Pointer to buffer containing signature
 * \return \ref stse_ReturnCode_t : STSAFEA_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_establish_host_secure_channel_keys_authenticated(
    stse_Handle_t *pSTSE,
    stse_ecc_key_type_t host_ecdh_public_key_type,
    PLAT_UI8 *pPublic_key,
    stse_aes_key_type_t host_keys_type,
    PLAT_UI8 signature_public_key_slot,
    stse_ecc_key_type_t signature_public_key_type,
    stse_hash_algorithm_t signature_hash_algo,
    PLAT_UI8 *pSignature);

/** \}*/

#endif /*STSAFEA_HOST_KEY_SLOT_H*/
