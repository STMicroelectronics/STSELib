/*!
 ******************************************************************************
 * \file	stse_generic_typedef.h
 * \brief   STSE Generic typedefs (header)
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

#ifndef STSAFE_GENERIC_TYPEDEF_H
#define STSAFE_GENERIC_TYPEDEF_H


/*! \defgroup stse_generic_typedef STSE Generic typedefs
*  \ingroup stse_core
 *  @{
 */

#define STSE_8BIT_LEN 8U
#define STSE_7BIT_LEN 7U
#define STSE_6BIT_LEN 6U
#define STSE_5BIT_LEN 5U
#define STSE_4BIT_LEN 4U
#define STSE_3BIT_LEN 3U
#define STSE_2BIT_LEN 2U
#define STSE_1BIT_LEN 1U

#define STSE_CERTIFICATE_UID_SIZE 							8U
#define STSE_MAC_SIZE 										4U
#define STSE_KEY_CONFIRMATION_MAC_SIZE						8U
#define STSE_KEK_ENVELOPE_MAC_SIZE							8U
#define STSE_NONCE_SIZE 									13U
#define STSE_SHARED_SECRET_SIZE								32U
#define STSE_SESSION_ID_SIZE 								16U
#define STSE_PERIPHERAL_SESSION_ID_SIZE 					4U
#define STSE_MIN_WRAP_ASSOCIATED_DATA_SIZE 					STSE_SESSION_ID_SIZE + 2
#define STSE_MIN_UNWRAP_ASSOCIATED_DATA_SIZE 				STSE_SESSION_ID_SIZE + 1
#define STSE_SEQUENCE_COUNTER_SIZE							3U
#define STSE_SEQUENCE_COUNTER_V2_SIZE						4U
#define STSE_AES_CHALLENGE_SIZE 							8U
#define STSE_RMAC_ID 										0b1<<7


#define STSE_MAGIC_VALUE_SIZE								16U
#define STSE_EDDSA_SIGNATURE_SIZE							64U
#define STSE_TRACEABILITY_INFO_SIZE							23U
#define STSE_PRODUCT_TYPE_SIZE 								8U
#define STSE_CPSN_SIZE 										7U
#define STSE_PAIRING_CERTIFICATE_FORMAT						3U
#define STSE_EDDSA_CERTIFICATE_FORMAT						4U
#define STSE_EDDSA_CHALLENGE_SIZE							16U
#define STSE_COUNTER_VALUE_SIZE								4U

#define STSE_SECURE_CHANNEL_Msk								0x20U  /*!< STSE  Secure channel type bit */
#define STSE_SECURE_CHANNEL_HOST							0x20U  /*!< STSE  Host secure channel */
#define STSE_SECURE_CHANNEL_SESSION							0x00U  /*!< STSE  Session secure channel */
#define STSE_AES_128_KEY_SIZE 								16U
#define STSE_AES_256_KEY_SIZE 								32U

/*!
 * \enum stse_ecc_key_type_t
 * \brief STSE ECC key type
 */
typedef enum stse_ecc_key_type_t {
	STSE_ECC_KT_NIST_P_256 = 0x00,
	STSE_ECC_KT_NIST_P_384,
	STSE_ECC_KT_NIST_P_521,
	STSE_ECC_KT_BP_P_256,
	STSE_ECC_KT_BP_P_384,
	STSE_ECC_KT_BP_P_512,
	STSE_ECC_KT_CURVE25519,
	STSE_ECC_KT_ED25519,
	STSE_ECC_KT_INVALID = 0xFF
}stse_ecc_key_type_t;

typedef enum stse_aes_key_type_t{
	STSE_AES_128_KT = 0x00,
	STSE_AES_256_KT
}stse_aes_key_type_t;


/*!
 * \enum stse_hash_algorithm_t
 * \brief STSE Hash algorithm type
 */
typedef enum stse_hash_algorithm_t{
	STSE_SHA_1 = 0x00,
	STSE_SHA_224,
	STSE_SHA_256,
	STSE_SHA_384,
	STSE_SHA_512,
	STSE_SHA3_256,
	STSE_SHA3_384,
	STSE_SHA3_512,
	STSE_SHA_INVALID = 0xFF,
}stse_hash_algorithm_t;

/*!
 * \struct stse_cmd_authorization_CR_t
 * \brief STSE commands authorization change request type
 */
typedef struct stse_cmd_authorization_CR_t{
	PLAT_UI8 host_encryption_flag_CR : 1;
	PLAT_UI8 cmd_AC_CR : 1;
	PLAT_UI8 filler : 6;
} stse_cmd_authorization_CR_t;

/*!
 * \enum stse_cmd_access_conditions_t
 * \brief STSE commands access conditions enumeration
 */
typedef enum stse_cmd_access_conditions_t {
	STSE_CMD_AC_NEVER = 0,		/*!< NEVER */
	STSE_CMD_AC_FREE,			/*!< FREE */
	STSE_CMD_AC_ADMIN,			/*!< ADMIN */
	STSE_CMD_AC_HOST,			/*!< HOST */
	STSE_CMD_AC_ADMIN_OR_PWD,	/*!< ADMIN or PASSWORD */
	STSE_CMD_AC_ADMIN_OR_HOST	/*!< ADMIN or HOST */
}stse_cmd_access_conditions_t;


/*!
 * \struct stse_cmd_authorization_record_t
 * \brief STSE commands authorization records type
 */
typedef struct stse_cmd_authorization_record_t{
	PLAT_UI8 header;
	PLAT_UI8 extended_header;
	stse_cmd_access_conditions_t command_AC;
	struct {
	PLAT_UI8 filler : STSE_6BIT_LEN;
	PLAT_UI8 cmd : STSE_1BIT_LEN;
	PLAT_UI8 rsp : STSE_1BIT_LEN;
	} host_encryption_flags;
} stse_cmd_authorization_record_t;

/** \}*/

#endif /* STSAFE_GENERIC_TYPEDEF_H */
