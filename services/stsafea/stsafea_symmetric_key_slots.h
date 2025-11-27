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

#ifndef STSAFEA_SYMMETRIC_KEY_SLOTS_H
#define STSAFEA_SYMMETRIC_KEY_SLOTS_H

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

/*! \defgroup stsafea_symmetric_key_slots STSAFE-A Symmetric key slots management
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_AES_128_KEY_SIZE 16U
#define STSAFEA_AES_256_KEY_SIZE 32U
#define STSAFEA_CA_PUB_KEY_SIZE 32U /*! < Certificate Authority public key size*/
#define STSAFEA_SYMMETRIC_KEY_USAGE_ENCRYPTION_Msk (0b1 << 3)
#define STSAFEA_SYMMETRIC_KEY_USAGE_DECRYPTION_Msk (0b1 << 2)
#define STSAFEA_SYMMETRIC_KEY_USAGE_MAC_GENERATION (0b1 << 1)
#define STSAFEA_SYMMETRIC_KEY_USAGE_MAC_VERIFICATION 0b1

#define STSAFEA_SYMMETRIC_KEY_USAGE_LENGTH 2U
#define STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH 5U
#define STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH 2U
#define STSAFEA_SYMMETRIC_KEY_INFORMATION_BASE_LENGTH 6U

#define STSAFEA_KT_LENGTH 1U
#define STSAFEA_KT_MAC_SESSION_KEY 0x04
#define STSAFEA_KT_EAP_BASE_KEY 0x08
#define STSAFEA_KT_KEY_CONFIRMATION_KEY 0x0B
#define STSAFEA_KT_DUMMY 0xFF
#define STSAFEA_KT_VOLATILE_BASE_KEK 0x10
#define STSAFEA_KT_VOLATILE_WORKING_KEK 0x11
#define STSAFEA_KT_SYMMETRIC_KEY 0x12
#define STSAFEA_KT_CONFIRMATION_KEY 0x13

/*! STSAFE-A Key slots enumeration */
typedef enum stsafea_symmetric_key_slot_t {
    STSAFEA_STATIC_PRIVATE_KEY_SLOT_0 = 0x00, /*!< static private key slot 0 */
    STSAFEA_STATIC_PRIVATE_KEY_SLOT_1,        /*!< static private key slot 1 */
    STSAFEA_STATIC_PRIVATE_KEY_SLOT_2,        /*!< static private key slot 2 */
    STSAFEA_STATIC_PRIVATE_KEY_SLOT_3,        /*!< static private key slot 3 */
    STSAFEA_STATIC_PRIVATE_KEY_SLOT_4,        /*!< static private key slot 4 */
    STSAFEA_EAP_PERIPHERAL_KEY_SLOTS = 0x80,  /*!< Enhanced Authentication Protocol peripheral key slots */
    STSAFEA_EAP_COMPANION_SLOT,               /*!< Enhanced Authentication Protocol companion key slots */
    STSAFEA_EAP_KEY_CONFIRMATION,             /*!< Enhanced Authentication Protocol key confirmation */
    STSAFEA_EPHEMERAL_KEY_SLOT = 0xFF         /*!< Ephemeral key slot 4 */
} stsafea_symmetric_key_slot_t;

/*! STSAFE-A symmetric key type */
typedef enum stsafea_symmetric_key_t {
    STSAFEA_SYMMETRIC_KEY_TYPE_AES_128 = 0x00, /*!< AES 128 symmetric Key type */
    STSAFEA_SYMMETRIC_KEY_TYPE_AES_256,        /*!< AES 256 symmetric Key type */
    STSAFEA_SYMMETRIC_KEY_TYPE_GENERIC_SECRET  /*!< Variable-length keys (from 16 to 32 bytes) */
} stsafea_symmetric_key_t;

/*! STSAFE-A symmetric key operation mode type */
typedef enum stsafea_symmetric_key_operation_mode_t {
    STSAFEA_KEY_OPERATION_MODE_CCM = 0x00, /*!< AES-CCM mode  */
    STSAFEA_KEY_OPERATION_MODE_CMAC,       /*!< AES-CMAC mode */
    STSAFEA_KEY_OPERATION_MODE_ECB,        /*!< AES-ECB mode  */
    STSAFEA_KEY_OPERATION_MODE_GCM,        /*!< AES-GCM mode  */
    STSAFEA_KEY_OPERATION_MODE_HKDF,       /*!< AES-HKDF mode */
    STSAFEA_KEY_OPERATION_MODE_HMAC        /*!< AES-HMAC mode */
} stsafea_symmetric_key_operation_mode_t;

/*! STSAFE-A Symmetric key operation type */
typedef enum stsafea_symmetric_key_information_length_t {
    STSAFEA_KEY_INFORMATION_LENGTH_ECB = STSAFEA_SYMMETRIC_KEY_INFORMATION_BASE_LENGTH, /*!< AES-ECB mode */
    STSAFEA_KEY_INFORMATION_LENGTH_CMAC,                                                /*!< AES-CMAC mode */
    STSAFEA_KEY_INFORMATION_LENGTH_CCM                                                  /*!< AES-CCM mode */
} stsafea_symmetric_key_information_length_t;

/*! STSAFE-A Symmetric key slot lock indicator */
typedef enum stsafea_symmetric_key_lock_indicator_t {
    STSAFEA_SYMMETRIC_KEY_LOCK_INDICATOR_UNLOCKED = 0b00,
    STSAFEA_SYMMETRIC_KEY_LOCK_INDICATOR_LOCKED,
    STSAFEA_SYMMETRIC_KEY_LOCK_INDICATOR_ERASABLE
} stsafea_symmetric_key_lock_indicator_t;

/*******************************************************************************
 * Key slots information structures
 *******************************************************************************/

/*! STSAFE-A Symmetric key slot provisioning control field */
typedef struct stsafea_symmetric_key_slot_provisioning_ctrl_fields_t {
    PLAT_UI8 ECDHE_anonymous : STSE_1BIT_LEN;
    PLAT_UI8 wrapped_anonymous : STSE_1BIT_LEN;
    PLAT_UI8 put_key : STSE_1BIT_LEN;
    PLAT_UI8 change_right : STSE_1BIT_LEN;
    PLAT_UI8 plaintext : STSE_1BIT_LEN;
    PLAT_UI8 derived : STSE_1BIT_LEN;
    PLAT_UI8 filler : STSE_2BIT_LEN;
    PLAT_UI8 wrapped_authentication_key;
    PLAT_UI8 ECDHE_authentication_key;
} stsafea_symmetric_key_slot_provisioning_ctrl_fields_t;

/*! STSAFE-A Symmetric key usage */
typedef struct stsafea_symmetric_key_usage_t {
    PLAT_UI8 empty_byte;
    PLAT_UI8 mac_verification : STSE_1BIT_LEN;
    PLAT_UI8 mac_generation : STSE_1BIT_LEN;
    PLAT_UI8 decryption : STSE_1BIT_LEN;
    PLAT_UI8 encryption : STSE_1BIT_LEN;
    PLAT_UI8 chunks_decryption : STSE_1BIT_LEN;
    PLAT_UI8 chunks_encryption : STSE_1BIT_LEN;
    PLAT_UI8 derive : STSE_1BIT_LEN;
    PLAT_UI8 filler : STSE_1BIT_LEN;
} stsafea_symmetric_key_usage_t;

typedef struct stsafea_symmetric_key_slot_information_record_t {
    PLAT_UI8 slot_number;
    stsafea_symmetric_key_t key_type;
    stsafea_symmetric_key_operation_mode_t mode_of_operation;
    stsafea_symmetric_key_usage_t key_usage;
    PLAT_UI8 parameters[2];
} stsafea_symmetric_key_slot_information_record_t;

typedef struct stsafea_symmetric_key_slot_information_t {
    PLAT_UI8 key_presence : STSE_1BIT_LEN;
    PLAT_UI8 lock_indicator : STSE_2BIT_LEN;
    PLAT_UI8 filler : STSE_5BIT_LEN;
    stsafea_symmetric_key_t key_type;
    stsafea_symmetric_key_operation_mode_t mode_of_operation;
    stsafea_symmetric_key_usage_t key_usage;
    union {
        struct {
            PLAT_UI8 auth_tag_length;
            PLAT_UI8 counter_offset_in_nonce : STSE_7BIT_LEN;
            PLAT_UI8 counter_presence : STSE_1BIT_LEN;
            PLAT_UI8 counter_value[4];
        } ccm;
        struct {
            PLAT_UI8 minimum_MAC_length;
        } cmac;
        struct {
            PLAT_UI8 minimum_MAC_length;
        } hmac;
        struct {
            PLAT_UI8 auth_tag_length;
        } gcm;
        struct {
            PLAT_UI8 derivation_output_in_response_allowed;
        } hkdf;
    } parameters;
} stsafea_symmetric_key_slot_information_t;

/*******************************************************************************
 * Key information structures
 *******************************************************************************/

#define STSAFEA_KEY_INFO_LENGTH_CCM 8U  /*!< AES-CCM key info length */
#define STSAFEA_KEY_INFO_LENGTH_CMAC 7U /*!< AES-CMAC key info length */
#define STSAFEA_KEY_INFO_LENGTH_ECB 6U  /*!< AES-ECB key info length  */
#define STSAFEA_KEY_INFO_LENGTH_GCM 7U  /*!< AES-GCM key info length  */
#define STSAFEA_KEY_INFO_LENGTH_HKDF 8U /*!< AES-HKDF key info length */
#define STSAFEA_KEY_INFO_LENGTH_HMAC 8U /*!< AES-HMAC key info length */

/*! STSAFE-A AES generic key information structure */
typedef struct stsafe_generic_key_information_t {
    PLAT_UI16 info_length;                                    /*!< stsafe_generic_key_information_t length excluding this field */
    stsafea_symmetric_key_lock_indicator_t lock_indicator;    /*!< key lock indicator */
    PLAT_UI8 slot_number;                                     /*!< key slot number */
    stsafea_symmetric_key_t type;                             /*!< key type */
    stsafea_symmetric_key_operation_mode_t mode_of_operation; /*!< mode of operation */
    stsafea_symmetric_key_usage_t usage;                      /*!< key usage */
    union {
        struct {
            PLAT_UI8 auth_tag_length;             /*!< Length of the authentication tag (0, 4, 8 or 16 bytes) */
            PLAT_UI8 counter_offset_in_nonce : 7; /*!< When counter present, it defines the offset in the nonce where the counter will be substituted (from 0 to 9)*/
            PLAT_UI8 counter_presence : 1;        /*!< Counter presence indicator */
        } CCM;
        struct {
            PLAT_UI8 min_MAC_length; /*!< Byte length of the MAC to verify (optional) */
        } CMAC;
        struct {
            PLAT_UI8 auth_tag_length; /*!< Length of the authentication tag (4, 8, 12, 13, 14, 15 or 16 bytes) */
        } GCM;
        struct {
            PLAT_UI8 allow_derived_key_to_host : 1; /*!< Allow to return some keys derived from the current key in the response */
            PLAT_UI8 filler : 7;                    /*!< Filler (Must be all 0b)*/
            PLAT_UI8 generic_secret_key_length;     /*!< Byte length of the key (only present if key type == STSAFEA_SYMMETRIC_KEY_TYPE_GENERIC_SECRET */
        } HKDF;
        struct {
            PLAT_UI8 min_MAC_length;            /*!< Byte length of the MAC to verify (optional) */
            PLAT_UI8 generic_secret_key_length; /*!< Byte length of the key (only present if key type == STSAFEA_SYMMETRIC_KEY_TYPE_GENERIC_SECRET */
        } HMAC;
    };
} stsafea_generic_key_information_t;

/*******************************************************************************
 * Functions
 *******************************************************************************/

/**
 * \brief 		Query symmetric key slot provisioning control fields
 * \details 	This service format and send query symmetric key slot provisioning control fields
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \param[in] 	slot_number			Slot number of the slot to query
 * \param[out] 	pCtrl_fields		Output structure for the provisioning control fields of the target slot
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_symmetric_key_slot_provisioning_ctrl_fields(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Put symmetric key slot provisioning control fields
 * \details 	This service format and send put symmetric key slot provisioning control fields
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	slot_number			Slot number of the slot to query
 * \param[in] 	pCtrl_fields		Structure for the provisioning control fields of the target slot
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_put_symmetric_key_slot_provisioning_ctrl_fields(
    stse_Handler_t *pSTSE,
    PLAT_UI8 slot_number,
    stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *pCtrl_fields);

/**
 * \brief 		Query symmetric key slot count
 * \details 	This service format and send query symmetric key slot count
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[out] 	pSymmetric_key_slot_count	Slot count output
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_symmetric_key_slots_count(stse_Handler_t *pSTSE, PLAT_UI8 *pSymmetric_key_slot_count);

/**
 * \brief 		Query symmetric key table informations
 * \details 	This service format and send query symmetric key table informations
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	symmetric_key_slot_count	Expected table slot count
 * \param[out] 	symmetric_key_table_info	Output key slot info list of length equal to symmetric_key_slot_count
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_query_symmetric_key_table(
    stse_Handler_t *pSTSE,
    PLAT_UI8 symmetric_key_slot_count,
    stsafea_symmetric_key_slot_information_t *symmetric_key_table_info);

/**
 * \brief 		Establish symmetric key in stsafe symmetric key table
 * \details 	This service format and send establish symmetric key command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	key_type 				Type of the key used in ECDHE
 * \param[in] 	host_ecdhe_public_key	Key used in ECDHE
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_establish_symmetric_key(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *host_ecdhe_public_key);

/**
 * \brief 		Authenticate establish symmetric key in stsafe symmetric key table
 * \details 	This service format and send establish symmetric key command with an authentication signature
 * \param[in] 	pSTSE 							Pointer to STSE Handler
 * \param[in] 	key_type 							Type of the key used in ECDHE
 * \param[in] 	host_ecdhe_public_key				Key used in ECDHE
 * \param[in] 	hash_algo							Signature hash algorithm ID
 * \param[in] 	signature_public_key_slot_number	Generic public key slot number to be used for the authentication
 * \param[in] 	signature_key_type					Authentication signature key type
 * \param[in] 	pSignature							Authentication signature
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_establish_symmetric_key_authenticated(
    stse_Handler_t *pSTSE,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *host_ecdhe_public_key,
    stse_hash_algorithm_t hash_algo,
    PLAT_UI8 signature_public_key_slot_number,
    stse_ecc_key_type_t signature_key_type,
    PLAT_UI8 *pSignature);

/**
 * \brief 		Confirm symmetric key establishment in stsafe symmetric key table
 * \details 	This service format and send confirm symmetric key command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	pMac_confirmation_key 	Key used to MAC the confirmation command payload
 * \param[in] 	key_count				Number of key to populate
 * \param[in]	pKey_information_list	Key information list used to populate the symmetric key table
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_confirm_symmetric_key(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pMac_confirmation_key,
    PLAT_UI8 key_count,
    stsafea_generic_key_information_t *pKey_information_list);

/**
 * \brief 		Write symmetric key envelope in stsafe symmetric key table
 * \details 	This service format and send write symmetric key command
 * \param[in] 	pSTSE 			 			Pointer to STSE Handler
 * \param[in] 	pSymmetric_key_envelope 		Envelope containing key materials to write in STSAFE
 * \param[in] 	symmetric_key_envelope_length 	Envelope length in bytes
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_write_symmetric_key_wrapped(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pSymmetric_key_envelope,
    PLAT_UI8 symmetric_key_envelope_length);

/**
 * \brief 		Write symmetric key plaintext in stsafe symmetric key table
 * \details 	This service format and send write symmetric key command
 * \param[in] 	pSTSE 			 		Pointer to STSE Handler
 * \param[in] 	pSymmetric_key_value 	Key value buffer
 * \param[in] 	pSymmetric_key_info 	Key information used to populate the symmetric key slot
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_write_symmetric_key_plaintext(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pSymmetric_key_value,
    stsafea_generic_key_information_t *pSymmetric_key_info);

/**
 * \brief 		Generate wrap/unwrap key
 * \details 	This service format and send symmetric key command
 * \param[in] 	pSTSE 			 	Pointer to STSE Handler
 * \param[in] 	wrap_key_slot 		wrap/unwrap key slot number
 * \param[in] 	key_type 			\ref stse_aes_key_type_t key wrap/unwrap key type
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_generate_wrap_unwrap_key(
    stse_Handler_t *pSTSE,
    PLAT_UI8 wrap_key_slot,
    stse_aes_key_type_t key_type);

/**
 * \brief 		Erase symmetric key
 * \details 	This service erase content of specified key slot in symmetric key table
 * \param[in] 	pSTSE 							Pointer to STSE Handler
 * \param[in] 	symmetric_key_slot_number		Key's slot number in symmetric key table
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_erase_symmetric_key_slot(
    stse_Handler_t *pSTSE,
    PLAT_UI8 symmetric_key_slot_number);

/** \}*/

#endif /*STSAFEA_SYMMETRIC_KEY_SLOTS_H*/
