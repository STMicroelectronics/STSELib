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

/*! \defgroup stse_generic_typedef STSE Generic typedefs
*  \ingroup stse_core
 *  @{
 */

#ifndef STSE_GENERIC_TYPEDEF_H
#define STSE_GENERIC_TYPEDEF_H

#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "stse_conf.h"

#define STSE_8BIT_LEN 8U
#define STSE_7BIT_LEN 7U
#define STSE_6BIT_LEN 6U
#define STSE_5BIT_LEN 5U
#define STSE_4BIT_LEN 4U
#define STSE_3BIT_LEN 3U
#define STSE_2BIT_LEN 2U
#define STSE_1BIT_LEN 1U

#define STSE_CERTIFICATE_UID_SIZE 8U
#define STSE_MAC_SIZE 4U
#define STSE_KEY_CONFIRMATION_MAC_SIZE 8U
#define STSE_KEK_ENVELOPE_MAC_SIZE 8U
#define STSE_NONCE_SIZE 13U
#define STSE_SHARED_SECRET_SIZE 32U
#define STSE_SESSION_ID_SIZE 16U
#define STSE_PERIPHERAL_SESSION_ID_SIZE 4U
#define STSE_MIN_WRAP_ASSOCIATED_DATA_SIZE STSE_SESSION_ID_SIZE + 2
#define STSE_MIN_UNWRAP_ASSOCIATED_DATA_SIZE STSE_SESSION_ID_SIZE + 1
#define STSE_SEQUENCE_COUNTER_SIZE 3U
#define STSE_SEQUENCE_COUNTER_V2_SIZE 4U
#define STSE_AES_CHALLENGE_SIZE 8U
#define STSE_RMAC_ID 0b1 << 7

#define STSE_MAGIC_VALUE_SIZE 16U
#define STSE_EDDSA_SIGNATURE_SIZE 64U
#define STSE_TRACEABILITY_INFO_SIZE 23U
#define STSE_PRODUCT_TYPE_SIZE 8U
#define STSE_CPSN_SIZE 7U
#define STSE_PAIRING_CERTIFICATE_FORMAT 3U
#define STSE_EDDSA_CERTIFICATE_FORMAT 4U
#define STSE_EDDSA_CHALLENGE_SIZE 16U
#define STSE_COUNTER_VALUE_SIZE 4U

#define STSE_SECURE_CHANNEL_Msk 0x20U     /*!< STSE  Secure channel type bit */
#define STSE_SECURE_CHANNEL_HOST 0x20U    /*!< STSE  Host secure channel */
#define STSE_SECURE_CHANNEL_SESSION 0x00U /*!< STSE  Session secure channel */
#define STSE_AES_128_KEY_SIZE 16U
#define STSE_AES_256_KEY_SIZE 32U

/*!
 * \enum stse_ecc_key_type_t
 * \brief STSE ECC key type
 */
typedef enum stse_ecc_key_type_t {
#ifdef STSE_CONF_ECC_NIST_P_256
    STSE_ECC_KT_NIST_P_256,
#endif
#ifdef STSE_CONF_ECC_NIST_P_384
    STSE_ECC_KT_NIST_P_384,
#endif
#ifdef STSE_CONF_ECC_NIST_P_521
    STSE_ECC_KT_NIST_P_521,
#endif
#ifdef STSE_CONF_ECC_BRAINPOOL_P_256
    STSE_ECC_KT_BP_P_256,
#endif
#ifdef STSE_CONF_ECC_BRAINPOOL_P_384
    STSE_ECC_KT_BP_P_384,
#endif
#ifdef STSE_CONF_ECC_BRAINPOOL_P_512
    STSE_ECC_KT_BP_P_512,
#endif
#ifdef STSE_CONF_ECC_CURVE_25519
    STSE_ECC_KT_CURVE25519,
#endif
#ifdef STSE_CONF_ECC_EDWARD_25519
    STSE_ECC_KT_ED25519,
#endif
    STSE_ECC_KT_INVALID
} stse_ecc_key_type_t;

/*!
 * \enum stse_aes_key_type_t
 * \brief STSE AES key type
 */
typedef enum stse_aes_key_type_t {
    STSE_AES_128_KT = 0x00,
    STSE_AES_256_KT
} stse_aes_key_type_t;

/*!
 * \enum stse_hash_algorithm_t
 * \brief STSE Hash algorithm type
 */
typedef enum stse_hash_algorithm_t {
#ifdef STSE_CONF_HASH_SHA_1
    STSE_SHA_1, /**< SHA-1 algorithm (not supported as service on STSAFE-A120) */
#endif
#ifdef STSE_CONF_HASH_SHA_224
    STSE_SHA_224, /**< SHA-224 algorithm (not supported as service on STSAFE-A120) */
#endif
#ifdef STSE_CONF_HASH_SHA_256
    STSE_SHA_256,
#endif
#ifdef STSE_CONF_HASH_SHA_384
    STSE_SHA_384,
#endif
#ifdef STSE_CONF_HASH_SHA_512
    STSE_SHA_512,
#endif
#ifdef STSE_CONF_HASH_SHA_3_256
    STSE_SHA3_256,
#endif
#ifdef STSE_CONF_HASH_SHA_3_384
    STSE_SHA3_384,
#endif
#ifdef STSE_CONF_HASH_SHA_3_512
    STSE_SHA3_512,
#endif
    STSE_SHA_INVALID
} stse_hash_algorithm_t;

/*!
 * \enum stse_hibernate_wake_up_mode_t
 * \brief STSE hibernate wake up mode (STSAFE-A only)
 */
typedef enum {
    STSAFEA_HIBERNATE_WAKEUP_I2C_OR_RESET = 0x01, /*!< Wake up from hibernate after I2C start condition or after reset. */
    STSAFEA_HIBERNATE_WAKEUP_RESET_ONLY = 0x02    /*!< Wake up from hibernate after reset only. */
} stse_hibernate_wake_up_mode_t;

/*!
 * \enum stse_cmd_protection_t
 * \brief STSE Command Protection enumeration
 */
typedef enum stse_cmd_protection_t {
    STSE_NO_PROT = 0,       /*!< No command / response payload protection */
    STSE_HOST_C_MAC_R_MAC,  /*!< Authenticated command and response using HOST MAC Key */
    STSE_HOST_C_WRAP,       /*!< Encrypted and authenticated command using HOST MAC and cipher Keys*/
    STSE_HOST_R_WRAP,       /*!< Encrypted and authenticated response using HOST MAC and cipher Keys*/
    STSE_HOST_C_WRAP_R_WRAP /*!< Encrypted and authenticated command and response using HOST MAC and cipher Keys*/
} stse_cmd_protection_t;

/*!
 * \struct stse_cmd_authorization_CR_t
 * \brief STSE commands authorization change request type
 */
typedef struct stse_cmd_authorization_CR_t {
    PLAT_UI8 host_encryption_flag_CR : 1;
    PLAT_UI8 cmd_AC_CR : 1;
    PLAT_UI8 filler : 6;
} stse_cmd_authorization_CR_t;

/*!
 * \enum stse_cmd_access_conditions_t
 * \brief STSE commands access conditions enumeration
 */
typedef enum stse_cmd_access_conditions_t {
    STSE_CMD_AC_NEVER = 0,    /*!< NEVER */
    STSE_CMD_AC_FREE,         /*!< FREE */
    STSE_CMD_AC_ADMIN,        /*!< ADMIN */
    STSE_CMD_AC_HOST,         /*!< HOST */
    STSE_CMD_AC_ADMIN_OR_PWD, /*!< ADMIN or PASSWORD */
    STSE_CMD_AC_ADMIN_OR_HOST /*!< ADMIN or HOST */
} stse_cmd_access_conditions_t;

/*!
 * \struct stse_cmd_authorization_record_t
 * \brief STSE commands authorization records type
 */
typedef struct stse_cmd_authorization_record_t {
    PLAT_UI8 header;
    PLAT_UI8 extended_header;
    stse_cmd_access_conditions_t command_AC;
    struct {
        PLAT_UI8 filler : STSE_6BIT_LEN;
        PLAT_UI8 cmd : STSE_1BIT_LEN;
        PLAT_UI8 rsp : STSE_1BIT_LEN;
    } host_encryption_flags;
} stse_cmd_authorization_record_t;

/**
 * \enum stse_zone_update_atomicity_t
 * \brief STSE Update command atomicity enumeration
 */
typedef enum stse_zone_update_atomicity_t {
    STSE_NON_ATOMIC_ACCESS = 0, /*!< Non Atomic Access*/
    STSE_ATOMIC_ACCESS          /*!< Atomic Access*/
} stse_zone_update_atomicity_t;

/**
 * \enum stse_zone_ac_t
 * \brief STSE data storage access condition enumeration
 */
typedef enum stse_zone_ac_t {
    STSE_AC_ALWAYS = 0, /*!< Zone/counter access always granted */
    STSE_AC_HOST,       /*!< Zone/counter access granted on Host C-MAC validation */
    STSE_AC_AUTH,       /*!< Zone/counter access granted on Auth C-MAC validation */
    STSE_AC_NEVER = 7   /*!< Zone/counter access never granted */
} stse_zone_ac_t;

/**
 * \enum stse_zone_ac_change_indicator_t
 * \brief STSE data storage access condition change indicator
 */
typedef enum stse_zone_ac_change_indicator_t {
    STSE_AC_IGNORE = 0, /*!< Ignore access condition change */
    STSE_AC_CHANGE      /*!< Request access condition change */
} stse_zone_ac_change_indicator_t;

/**
 * \enum stse_ac_change_right_t
 * \brief STSE data storage access condition change right enumeration
 */
typedef enum stse_ac_change_right_t {
    STSE_ACCR_DISABLED = 0, /*!< Change the AC is forbidden */
    STSE_ACCR_ENABLE        /*!< Change the AC is authorized */
} stse_ac_change_right_t;

#define STSE_ECC_GENERIC_LENGTH_SIZE 2U
#define STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID 0x04
#define STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE 1U
#define STSE_ECC_CURVE_ID_LENGTH_SIZE STSE_ECC_GENERIC_LENGTH_SIZE
#define STSE_ECC_CURVE_ID_VALUE_MAX_SIZE 9U

/*  ---------------------------------------------------------------------------------------
 *
 *						    NIST-P 256 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_NIST_P_256_ID_LENGTH_SIZE 2U
#define STSE_NIST_P_256_ID_LENGTH \
    {0x00, 0x08}
#define STSE_NIST_P_256_ID_VALUE_SIZE 8U
#define STSE_NIST_P_256_ID_VALUE \
    {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07}
#define STSE_NIST_P_256_ID_SIZE    \
    STSE_NIST_P_256_ID_LENGTH_SIZE \
    +STSE_NIST_P_256_ID_VALUE_SIZE
#define STSE_NIST_P_256_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE 32U
#define STSE_NIST_P_256_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_256_Y_COORDINATE_VALUE_SIZE 32U
#define STSE_NIST_P_256_PUBLIC_KEY_SIZE              \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_NIST_P_256_X_COORDINATE_LENGTH_SIZE + STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_256_Y_COORDINATE_LENGTH_SIZE + STSE_NIST_P_256_Y_COORDINATE_VALUE_SIZE
#define STSE_NIST_P_256_PRIVATE_KEY_SIZE 32U
#define STSE_NIST_P_256_SHARED_SECRET_SIZE   \
    STSE_NIST_P_256_X_COORDINATE_LENGTH_SIZE \
    +STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE
#define STSE_NIST_P_256_EPHEMERAL_PUBLIC_KEY_SIZE STSE_NIST_P_256_ID_SIZE + STSE_NIST_P_256_PUBLIC_KEY_SIZE
#define STSE_NIST_P_256_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_NIST_P_256_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_NIST_P_256_SIGNATURE_R_VALUE_SIZE 32U
#define STSE_NIST_P_256_SIGNATURE_S_VALUE_SIZE 32U
#define STSE_NIST_P_256_SIGNATURE_SIZE      \
    STSE_NIST_P_256_SIGNATURE_R_LENGTH_SIZE \
    +STSE_NIST_P_256_SIGNATURE_S_LENGTH_SIZE + STSE_NIST_P_256_SIGNATURE_S_VALUE_SIZE + STSE_NIST_P_256_SIGNATURE_R_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    NIST-P 384 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/
#define STSE_NIST_P_384_ID_LENGTH_SIZE 2U
#define STSE_NIST_P_384_ID_LENGTH \
    {0x00, 0x05}
#define STSE_NIST_P_384_ID_VALUE_SIZE 5U
#define STSE_NIST_P_384_ID_VALUE \
    {0x2B, 0x81, 0x04, 0x00, 0x22}
#define STSE_NIST_P_384_ID_SIZE    \
    STSE_NIST_P_384_ID_LENGTH_SIZE \
    +STSE_NIST_P_384_ID_VALUE_SIZE
#define STSE_NIST_P_384_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_384_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE 48U
#define STSE_NIST_P_384_Y_COORDINATE_VALUE_SIZE 48U
#define STSE_NIST_P_384_PUBLIC_KEY_SIZE              \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_NIST_P_384_X_COORDINATE_LENGTH_SIZE + STSE_NIST_P_384_Y_COORDINATE_LENGTH_SIZE + STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_384_Y_COORDINATE_VALUE_SIZE
#define STSE_NIST_P_384_PRIVATE_KEY_SIZE 48U
#define STSE_NIST_P_384_EPHEMERAL_PUBLIC_KEY_SIZE \
    STSE_NIST_P_384_ID_SIZE                       \
    +STSE_NIST_P_384_PUBLIC_KEY_SIZE
#define STSE_NIST_P_384_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_NIST_P_384_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_NIST_P_384_SIGNATURE_R_VALUE_SIZE 48U
#define STSE_NIST_P_384_SIGNATURE_S_VALUE_SIZE 48U
#define STSE_NIST_P_384_SIGNATURE_SIZE      \
    STSE_NIST_P_384_SIGNATURE_R_LENGTH_SIZE \
    +STSE_NIST_P_384_SIGNATURE_S_LENGTH_SIZE + STSE_NIST_P_384_SIGNATURE_S_VALUE_SIZE + STSE_NIST_P_384_SIGNATURE_R_VALUE_SIZE
#define STSE_NIST_P_384_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_NIST_P_384_SHARED_SECRET_SIZE    \
    STSE_NIST_P_384_SHARED_SECRET_LENGTH_SIZE \
    +STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    NIST-P 521 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/
#define STSE_NIST_P_521_ID_LENGTH_SIZE 2U
#define STSE_NIST_P_521_ID_LENGTH \
    {0x00, 0x05}
#define STSE_NIST_P_521_ID_VALUE_SIZE 5U
#define STSE_NIST_P_521_ID_VALUE \
    {0x2B, 0x81, 0x04, 0x00, 0x23}
#define STSE_NIST_P_521_ID_SIZE    \
    STSE_NIST_P_521_ID_LENGTH_SIZE \
    +STSE_NIST_P_521_ID_VALUE_SIZE
#define STSE_NIST_P_521_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_521_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE 66U
#define STSE_NIST_P_521_Y_COORDINATE_VALUE_SIZE 66U
#define STSE_NIST_P_521_PUBLIC_KEY_SIZE              \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_NIST_P_521_X_COORDINATE_LENGTH_SIZE + STSE_NIST_P_521_Y_COORDINATE_LENGTH_SIZE + STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_521_Y_COORDINATE_VALUE_SIZE
#define STSE_NIST_P_521_PRIVATE_KEY_SIZE 66U
#define STSE_NIST_P_521_EPHEMERAL_PUBLIC_KEY_SIZE 2 + STSE_NIST_P_521_ID_SIZE + STSE_NIST_P_521_PUBLIC_KEY_SIZE
#define STSE_NIST_P_521_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_NIST_P_521_SHARED_SECRET_SIZE    \
    STSE_NIST_P_521_SHARED_SECRET_LENGTH_SIZE \
    +STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE
#define STSE_NIST_P_521_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_NIST_P_521_SIGNATURE_R_VALUE_SIZE 66U
#define STSE_NIST_P_521_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_NIST_P_521_SIGNATURE_S_VALUE_SIZE 66U
#define STSE_NIST_P_521_SIGNATURE_SIZE      \
    STSE_NIST_P_521_SIGNATURE_R_LENGTH_SIZE \
    +STSE_NIST_P_521_SIGNATURE_S_LENGTH_SIZE + STSE_NIST_P_521_SIGNATURE_S_VALUE_SIZE + STSE_NIST_P_521_SIGNATURE_R_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    BRAINPOOL-P 256 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_BRAINPOOL_P_256_ID_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_256_ID_LENGTH \
    {0x00, 0x09}
#define STSE_BRAINPOOL_P_256_ID_VALUE_SIZE 9U
#define STSE_BRAINPOOL_P_256_ID_VALUE \
    {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x07}
#define STSE_BRAINPOOL_P_256_ID_SIZE    \
    STSE_BRAINPOOL_P_256_ID_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_256_ID_VALUE_SIZE
#define STSE_BRAINPOOL_P_256_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE 32U
#define STSE_BRAINPOOL_P_256_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_256_Y_COORDINATE_VALUE_SIZE 32U
#define STSE_BRAINPOOL_P_256_PUBLIC_KEY_SIZE         \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_BRAINPOOL_P_256_X_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_256_Y_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_256_Y_COORDINATE_VALUE_SIZE
#define STSE_BRAINPOOL_P_256_PRIVATE_KEY_SIZE 32U
#define STSE_BRAINPOOL_P_256_SHARED_SECRET_SIZE   \
    STSE_BRAINPOOL_P_256_X_COORDINATE_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE
#define STSE_BRAINPOOL_P_256_EPHEMERAL_PUBLIC_KEY_SIZE \
    STSE_BRAINPOOL_P_256_ID_SIZE                       \
    +STSE_BRAINPOOL_P_256_PUBLIC_KEY_SIZE
#define STSE_BRAINPOOL_P_256_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_256_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_256_SIGNATURE_R_VALUE_SIZE 32U
#define STSE_BRAINPOOL_P_256_SIGNATURE_S_VALUE_SIZE 32U
#define STSE_BRAINPOOL_P_256_SIGNATURE_SIZE      \
    STSE_BRAINPOOL_P_256_SIGNATURE_R_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_256_SIGNATURE_S_LENGTH_SIZE + STSE_BRAINPOOL_P_256_SIGNATURE_S_VALUE_SIZE + STSE_BRAINPOOL_P_256_SIGNATURE_R_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    BRAINPOOL-P 384 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_BRAINPOOL_P_384_ID_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_ID_LENGTH \
    {0x00, 0x09}
#define STSE_BRAINPOOL_P_384_ID_VALUE_SIZE 9U
#define STSE_BRAINPOOL_P_384_ID_VALUE \
    {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0B}
#define STSE_BRAINPOOL_P_384_ID_SIZE    \
    STSE_BRAINPOOL_P_384_ID_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_384_ID_VALUE_SIZE
#define STSE_BRAINPOOL_P_384_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE 48U
#define STSE_BRAINPOOL_P_384_Y_COORDINATE_VALUE_SIZE 48U
#define STSE_BRAINPOOL_P_384_PUBLIC_KEY_SIZE         \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_BRAINPOOL_P_384_X_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_384_Y_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_384_Y_COORDINATE_VALUE_SIZE

#define STSE_BRAINPOOL_P_384_PRIVATE_KEY_SIZE 48U
#define STSE_BRAINPOOL_P_384_EPHEMERAL_PUBLIC_KEY_SIZE \
    STSE_BRAINPOOL_P_384_ID_SIZE                       \
    +STSE_BRAINPOOL_P_384_PUBLIC_KEY_SIZE
#define STSE_BRAINPOOL_P_384_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_SIGNATURE_R_VALUE_SIZE 48U
#define STSE_BRAINPOOL_P_384_SIGNATURE_S_VALUE_SIZE 48U
#define STSE_BRAINPOOL_P_384_SIGNATURE_SIZE      \
    STSE_BRAINPOOL_P_384_SIGNATURE_R_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_384_SIGNATURE_S_LENGTH_SIZE + STSE_BRAINPOOL_P_384_SIGNATURE_S_VALUE_SIZE + STSE_BRAINPOOL_P_384_SIGNATURE_R_VALUE_SIZE
#define STSE_BRAINPOOL_P_384_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_384_SHARED_SECRET_SIZE    \
    STSE_BRAINPOOL_P_384_SHARED_SECRET_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    BRAINPOOL-P 512 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_BRAINPOOL_P_512_ID_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_ID_LENGTH \
    {0x00, 0x09}
#define STSE_BRAINPOOL_P_512_ID_VALUE_SIZE 9U
#define STSE_BRAINPOOL_P_512_ID_VALUE \
    {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0D}
#define STSE_BRAINPOOL_P_512_ID_SIZE    \
    STSE_BRAINPOOL_P_512_ID_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_512_ID_VALUE_SIZE
#define STSE_BRAINPOOL_P_512_X_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_Y_COORDINATE_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE 64U
#define STSE_BRAINPOOL_P_512_Y_COORDINATE_VALUE_SIZE 64U
#define STSE_BRAINPOOL_P_512_PUBLIC_KEY_SIZE         \
    STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID_SIZE \
    +STSE_BRAINPOOL_P_512_X_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_512_Y_COORDINATE_LENGTH_SIZE + STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_512_Y_COORDINATE_VALUE_SIZE
#define STSE_BRAINPOOL_P_512_PRIVATE_KEY_SIZE 64U
#define STSE_BRAINPOOL_P_512_EPHEMERAL_PUBLIC_KEY_SIZE 2 + STSE_BRAINPOOL_P_512_ID_SIZE + STSE_BRAINPOOL_P_512_PUBLIC_KEY_SIZE
#define STSE_BRAINPOOL_P_512_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_SHARED_SECRET_SIZE    \
    STSE_BRAINPOOL_P_512_SHARED_SECRET_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE
#define STSE_BRAINPOOL_P_512_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_SIGNATURE_R_VALUE_SIZE 64U
#define STSE_BRAINPOOL_P_512_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_BRAINPOOL_P_512_SIGNATURE_S_VALUE_SIZE 64U
#define STSE_BRAINPOOL_P_512_SIGNATURE_SIZE      \
    STSE_BRAINPOOL_P_512_SIGNATURE_R_LENGTH_SIZE \
    +STSE_BRAINPOOL_P_512_SIGNATURE_S_LENGTH_SIZE + STSE_BRAINPOOL_P_512_SIGNATURE_S_VALUE_SIZE + STSE_BRAINPOOL_P_512_SIGNATURE_R_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    X25519 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_X25519_ID_LENGTH_SIZE 2U
#define STSE_X25519_ID_LENGTH \
    {0x00, 0x03}
#define STSE_X25519_ID_VALUE_SIZE 3U
#define STSE_X25519_ID_VALUE \
    {0x2B, 0x65, 0x6E}
#define STSE_X25519_ID_SIZE    \
    STSE_X25519_ID_LENGTH_SIZE \
    +STSE_X25519_ID_VALUE_SIZE
#define STSE_X25519_PUBLIC_KEY_LENGTH_SIZE 2U
#define STSE_X25519_PUBLIC_KEY_VALUE_SIZE 32U
#define STSE_X25519_PUBLIC_KEY_SIZE    \
    STSE_X25519_PUBLIC_KEY_LENGTH_SIZE \
    +STSE_X25519_PUBLIC_KEY_VALUE_SIZE
#define STSE_X25519_PRIVATE_KEY_SIZE 32U
#define STSE_X25519_EPHEMERAL_PUBLIC_KEY_SIZE \
    STSE_X25519_ID_SIZE                       \
    +STSE_X25519_PUBLIC_KEY_SIZE
#define STSE_X25519_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_X25519_SIGNATURE_R_VALUE_SIZE 32U
#define STSE_X25519_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_X25519_SIGNATURE_S_VALUE_SIZE 32U
#define STSE_X25519_SIGNATURE_SIZE      \
    STSE_X25519_SIGNATURE_R_LENGTH_SIZE \
    +STSE_X25519_SIGNATURE_R_VALUE_SIZE + STSE_X25519_SIGNATURE_S_LENGTH_SIZE + STSE_X25519_SIGNATURE_S_VALUE_SIZE
#define STSE_X25519_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_X25519_SHARED_SECRET_VALUE_SIZE 32U
#define STSE_X25519_SHARED_SECRET_SIZE    \
    STSE_X25519_SHARED_SECRET_LENGTH_SIZE \
    +STSE_X25519_SHARED_SECRET_VALUE_SIZE

/*  ---------------------------------------------------------------------------------------
 *
 *						    ED25519 cryptographic algorithm definitions
 *
 *  ---------------------------------------------------------------------------------------
*/

#define STSE_ED25519_ID_SIZE 5U
#define STSE_ED25519_ID_LENGTH_SIZE 2U
#define STSE_ED25519_ID_LENGTH \
    {0x00, 0x03}
#define STSE_ED25519_ID_VALUE_SIZE 3U
#define STSE_ED25519_ID_VALUE \
    {0x2B, 0x65, 0x70}
#define STSE_ED25519_PUBLIC_KEY_LENGTH_SIZE 2U
#define STSE_ED25519_PUBLIC_KEY_VALUE_SIZE 32U
#define STSE_ED25519_PUBLIC_KEY_SIZE    \
    STSE_ED25519_PUBLIC_KEY_LENGTH_SIZE \
    +STSE_ED25519_PUBLIC_KEY_VALUE_SIZE
#define STSE_ED25519_PRIVATE_KEY_SIZE 32U
#define STSE_ED25519_EPHEMERAL_PUBLIC_KEY_SIZE \
    STSE_ED25519_ID_SIZE                       \
    +STSE_ED25519_PUBLIC_KEY_SIZE
#define STSE_ED25519_SIGNATURE_R_LENGTH_SIZE 2U
#define STSE_ED25519_SIGNATURE_R_VALUE_SIZE 32U
#define STSE_ED25519_SIGNATURE_S_LENGTH_SIZE 2U
#define STSE_ED25519_SIGNATURE_S_VALUE_SIZE 32U
#define STSE_ED25519_SIGNATURE_SIZE      \
    STSE_ED25519_SIGNATURE_R_LENGTH_SIZE \
    +STSE_ED25519_SIGNATURE_R_VALUE_SIZE + STSE_ED25519_SIGNATURE_S_LENGTH_SIZE + STSE_ED25519_SIGNATURE_S_VALUE_SIZE
#define STSE_ED25519_SHARED_SECRET_LENGTH_SIZE 2U
#define STSE_ED25519_SHARED_SECRET_VALUE_SIZE 32U
#define STSE_ED25519_SHARED_SECRET_SIZE    \
    STSE_ED25519_SHARED_SECRET_LENGTH_SIZE \
    +STSE_ED25519_SHARED_SECRET_VALUE_SIZE

typedef struct
{
    PLAT_UI8 length[STSE_ECC_CURVE_ID_LENGTH_SIZE];
    PLAT_UI8 value[STSE_ECC_CURVE_ID_VALUE_MAX_SIZE];
} PLAT_PACKED_STRUCT stsafea_ecc_curve_id_t;

typedef struct {
    PLAT_UI16 curve_id_total_length;
    stsafea_ecc_curve_id_t curve_id;
    PLAT_UI16 coordinate_or_key_size;
    PLAT_UI16 public_key_size;
    union {
        PLAT_UI16 private_key_size;
        PLAT_UI16 min_signature_message_size;
    };
    PLAT_UI16 shared_secret_size;
    PLAT_UI16 signature_size;
} stse_ecc_info_t;

extern const stse_ecc_info_t stse_ecc_info_table[];

#if defined(STSE_CONF_ECC_NIST_P_256) || defined(STSE_CONF_ECC_NIST_P_384) || defined(STSE_CONF_ECC_NIST_P_521) ||                \
    defined(STSE_CONF_ECC_BRAINPOOL_P_256) || defined(STSE_CONF_ECC_BRAINPOOL_P_384) || defined(STSE_CONF_ECC_BRAINPOOL_P_512) || \
    defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519)

/**
 * \brief 		Get ECC key type from curve identifier
 * \details 	This function resolves the ECC key type from a given curve identifier value
 * \param[in]	curve_id_length		Length of the curve identifier
 * \param[in]	pCurve_id_value		Pointer to the curve identifier value
 * \param[out]	pKey_type			Pointer to store the resolved ECC key type
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_get_ecc_key_type_from_curve_id(
    PLAT_UI8 curve_id_length,
    const PLAT_UI8 *pCurve_id_value,
    stse_ecc_key_type_t *pKey_type);

#endif

/** \}*/

#endif /* STSE_GENERIC_TYPEDEF_H */
