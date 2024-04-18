/*!
 * ******************************************************************************
 * \file	stsafea_commands.h
 * \brief   STSAFE-A command typedefs and services
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


#ifndef STSAFE_COMMANDS_H
#define STSAFE_COMMANDS_H

/*! \defgroup stsafea_services STSAFE-Axxx
*  \ingroup stse_services
 *  @{
 */


/*! \defgroup stsafe_core_cmd_set Command Codes
*  \ingroup stsafea_services
 *  @{
 */

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_put_query.h"

#define STSAFEA_TRUE 						0b1
#define STSAFEA_FALSE						0b0

#define STSAFEA_EXT_HEADER_SIZE 			2U
#define STSAFEA_HEADER_SIZE 				1U
#define STSAFEA_CMD_RSP_LEN_SIZE 			2U
#define STSAFEA_SLOT_NUMBER_ID_SIZE			1U
#define STSAFEA_AES_KEY_SIZE 				16U
#define STSAFEA_COUNTER_VALUE_SIZE			4U
#define STSAFEA_GENERIC_LENGTH_SIZE			2U
#define STSAFEA_UID_SIZE 					8U
#define STSAFEA_MAXIMUM_CMD_RSP_LENGTH_A100	507U
#define STSAFEA_MAXIMUM_CMD_RSP_LENGTH_A110	507U
#define STSAFEA_MAXIMUM_CMD_RSP_LENGTH_A120	752U
#define STSAFEA_MAXIMUM_CMD_RSP_LENGTH_A200	507U

#define STSAFEA_PROT_Pos					5  	   /*!< STSAFE  Protected rsp mask */
#define STSAFEA_PROT_Msk					0xC0U  /*!< STSAFE  Protected mask */
#define STSAFEA_PROT_CMD_Msk				0x80U  /*!< STSAFE  Protected command mask */
#define STSAFEA_PROT_RSP_Msk				0x40U  /*!< STSAFE  Protected rsp mask */

/*!
 * \enum stsafea_cmd_code_t
 * \brief STSAFE-A command type
 */
typedef enum stsafea_cmd_code_t {
	STSAFEA_CMD_ECHO =0,                    /*!< STSAFEA Echo command code */
	STSAFEA_CMD_RESET,                      /*!< STSAFEA Reset command code*/
	STSAFEA_CMD_GENERATE_RANDOM,            /*!< STSAFEA Generate Random command code*/
	STSAFEA_CMD_START_SESSION,				/*!< STSAFEA Start Session command code*/
	STSAFEA_CMD_DECREMENT,					/*!< STSAFEA Decrement zone counter command code*/
	STSAFEA_CMD_READ,						/*!< STSAFEA Read zone command code*/
	STSAFEA_CMD_UPDATE,						/*!< STSAFEA Update zone command code*/
	STSAFEA_CMD_INCREMENT,					/*!< STSAFEA Increment zone counter command code*/
	STSAFEA_CMD_DERIVE_LORA_KEY,			/*!< STSAFEA Derive LORA key command code*/
	STSAFEA_CMD_GENERATE_MAC,				/*!< STSAFEA Generate MAC command code*/
	STSAFEA_CMD_VERIFY_MAC,					/*!< STSAFEA Verify MAC command code*/
	STSAFEA_CMD_UNWRAP_ISSUER_ENVELOPE,		/*!< STSAFEA Unwrap issuer envelope command code*/
	STSAFEA_CMD_DELETE,						/*!< STSAFEA Delete MAC command code*/
	STSAFEA_CMD_HIBERNATE,					/*!< STSAFEA Hibernate command code*/
	STSAFEA_CMD_WRAP_LOCAL_ENVELOPE,		/*!< STSAFEA Wrap Local Envelope command code*/
	STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE,		/*!< STSAFEA Unwrap Local Envelope command code*/
	STSAFEA_CMD_PUT_ATTRIBUTE,				/*!< STSAFEA Put attribute command code*/
	STSAFEA_CMD_GENERATE_KEY,				/*!< STSAFEA Generate key command code*/
	STSAFEA_CMD_PUT_KEY,					/*!< STSAFEA Put key command code*/
	STSAFEA_CMD_CONFIGURE,					/*!< STSAFEA Configure command code*/
	STSAFEA_CMD_QUERY,						/*!< STSAFEA Query command code*/
	STSAFEA_CMD_GET_SIGNATURE,				/*!< STSAFEA Get Signature command code*/
	STSAFEA_CMD_GENERATE_SIGNATURE,			/*!< STSAFEA Generate Signature command code*/
	STSAFEA_CMD_VERIFY_SIGNATURE,			/*!< STSAFEA Verify Signature command code*/
	STSAFEA_CMD_ESTABLISH_KEY,				/*!< STSAFEA Establish Key command code*/
	STSAFEA_CMD_STANDBY,					/*!< STSAFEA Standby command code*/
	STSAFEA_CMD_VERIFY_PASSWORD,			/*!< STSAFEA Verify Password command code*/
	STSAFEA_CMD_ENCRYPT,					/*!< STSAFEA Encrypt command code*/
	STSAFEA_CMD_DECRYPT,					/*!< STSAFEA Decrypt command code*/
	STSAFEA_EXTENDED_COMMAND_PREFIX	= 0x1FU
} stsafea_cmd_code_t;

/*!
 * \enum stsafea_extended_cmd_code_t
 * \brief STSAFEA-A extended command type
 */
typedef enum stsafea_extended_cmd_code_t {
	STSAFEA_EXTENDED_CMD_START_HASH = 0x00U,  			/*!< STSAFE-A Start hash extended command code */
	STSAFEA_EXTENDED_CMD_PROCESS_HASH,					/*!< STSAFE-A Process hash extended command code */
	STSAFEA_EXTENDED_CMD_FINISH_HASH,					/*!< STSAFE-A Finish hash extended command code */
	STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION,	/*!< STSAFE-A Start volatile KEK extended command code */
	STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS,		/*!< STSAFE-A Establish symmetric keys extended command code */
	STSAFEA_EXTENDED_CMD_CONFIRM_SYMMETRIC_KEYS,		/*!< STSAFE-A confirm symmetric keys extended command code */
	STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION,		/*!< STSAFE-A Stop volatile KEK extended command code */
	STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT,	/*!< STSAFE-A Plaintext write host key v2  extended command code */
	STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED,		/*!< STSAFE-A Wrapped write host key v2 extended command code */
	STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_WRAPPED,	/*!< STSAFE-A Write symmetric key extended command code */
	STSAFEA_EXTENDED_CMD_WRITE_PUBLIC_KEY,				/*!< STSAFE-A Write public key extended command code */
	STSAFEA_EXTENDED_CMD_GENERATE_ECDHE,				/*!< STSAFE-A Generate ECDHE extended command code */
	STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE = 14,		/*!< STSAFE-A Generate challenge command code */
	STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE,		/*!< STSAFE-A Verify entity signature command code */
	STSAFEA_EXTENDED_CMD_DERIVE_KEYS,					/*!< STSAFE-A Derive keys command code */
	STSAFEA_EXTENDED_CMD_START_ENCRYPT,					/*!< STSAFE-A Start encrypt command code */
	STSAFEA_EXTENDED_CMD_PROCESS_ENCRYPT,				/*!< STSAFE-A Process encrypt command code */
	STSAFEA_EXTENDED_CMD_FINISH_ENCRYPT,				/*!< STSAFE-A Finish encrypt command code */
	STSAFEA_EXTENDED_CMD_START_DECRYPT,					/*!< STSAFE-A Start decrypt command code */
	STSAFEA_EXTENDED_CMD_PROCESS_DECRYPT,				/*!< STSAFE-A Process decrypt command code */
	STSAFEA_EXTENDED_CMD_FINISH_DECRYPT,				/*!< STSAFE-A Finish decrypt command code */
	STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_PLAINTEXT, /*!< STSAFE-A Write symmetric key paintext command code */
	STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2,			/*!< STSAFE-A Establish host key V2 command code */
	STSAFEA_EXTENDED_CMD_ERASE_SYMMETRIC_KEY_SLOT,		/*!< STSAFE-A Erase symmetric key slot command code */
	STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY			/*!< STSAFE-A Decompress command code */
}stsafea_extended_cmd_code_t;

/*!
 * \enum stsafea_cmd_protection_t
 * \brief STSAFE-Axxx Command Protection enumeration
 */
typedef enum stsafea_cmd_protection_t {
	STSAFEA_NO_PROT = 0,						/*!< No command / response payload protection */
	STSAFEA_OTHER_SESSION_RMAC_PROTECTED = 2,	/*!< Response protection over other session */
	STSAFEA_HOST_SESSION_RMAC_PROTECTED,		/*!< Response protection over Host session */
	STSAFEA_OTHER_SESSION_CMAC_PROTECTED,		/*!< Command protection over other session */
	STSAFEA_HOST_SESSION_CMAC_PROTECTED,		/*!< Command protection over Host session */
	STSAFEA_OTHER_SESSION_CMAC_RMAC_PROTECTED,	/*!< Command and response protection over other session */
	STSAFEA_HOST_SESSION_CMAC_RMAC_PROTECTED	/*!< Command and response protection over Host session */
}stsafea_cmd_protection_t;

extern const PLAT_UI16 stsafea_maximum_command_length[4];

stse_ReturnCode_t stsafea_get_command_count(stse_Handler_t *pSTSE , PLAT_UI8 *pCommand_count);

stse_ReturnCode_t stsafea_get_command_AC_table(stse_Handler_t *pSTSE,
											 PLAT_UI8 total_command_count,
											 stse_cmd_authorization_CR_t * pChange_rights,
											 stse_cmd_authorization_record_t * pRecord_table);

stse_ReturnCode_t stsafea_perso_info_update(stse_Handler_t *pSTSE);

void stsafea_perso_info_get_cmd_AC (stse_perso_info_t *pPerso ,
		PLAT_UI8 command_code,
		stse_cmd_access_conditions_t *pProtection);

void stsafea_perso_info_get_ext_cmd_AC (stse_perso_info_t *pPerso,
		PLAT_UI8 command_code,
		stse_cmd_access_conditions_t *pProtection);


void stsafea_perso_info_get_cmd_encrypt_flag (stse_perso_info_t *pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 *pEnc_flag);

void stsafea_perso_info_get_rsp_encrypt_flag (stse_perso_info_t *pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 *pEnc_flag);

void stsafea_perso_info_get_ext_cmd_encrypt_flag (stse_perso_info_t *pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 *pEnc_flag);

void stsafea_perso_info_get_ext_rsp_encrypt_flag (stse_perso_info_t *pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 *pEnc_flag);


void stsafea_perso_info_set_cmd_AC (stse_perso_info_t* pPerso ,
		PLAT_UI8 command_code ,
		stse_cmd_access_conditions_t protection);

void stsafea_perso_info_set_ext_cmd_AC (stse_perso_info_t* pPerso,
		PLAT_UI8 command_code,
		stse_cmd_access_conditions_t protection);

void stsafea_perso_info_set_cmd_encrypt_flag (stse_perso_info_t* pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 enc_flag);

void stsafea_perso_info_set_rsp_encrypt_flag (stse_perso_info_t* pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 enc_flag);

void stsafea_perso_info_set_ext_cmd_encrypt_flag (stse_perso_info_t* pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 enc_flag);

void stsafea_perso_info_set_ext_rsp_encrypt_flag (stse_perso_info_t* pPerso,
		PLAT_UI8 command_code,
		PLAT_UI8 enc_flag);

/** \}*/
/** \}*/
#endif /*STSAFE_COMMANDS_H*/


