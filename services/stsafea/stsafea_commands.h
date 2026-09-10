/*!
 * ******************************************************************************
 * \file	stsafea_commands.h
 * \brief   STSAFE-A command typedefs and services (header)
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

/*! \defgroup stsafea_services STSAFE-A
*  \ingroup stse_services
 *  @{
 */

/*! \defgroup stsafea_cmd_set STSAFE-A commands codes
*  \ingroup stsafea_services
 *  @{
 */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"

#define STSAFEA_TRUE 0b1
#define STSAFEA_FALSE 0b0

#define STSAFEA_EXT_HEADER_SIZE 2U
#define STSAFEA_HEADER_SIZE 1U
#define STSAFEA_CMD_EXTENSION_SIZE 1U
#define STSAFEA_CMD_RSP_LEN_SIZE 2U
#define STSAFEA_SLOT_NUMBER_ID_SIZE 1U
#define STSAFEA_AES_KEY_SIZE 16U
#define STSAFEA_COUNTER_VALUE_SIZE 4U
#define STSAFEA_GENERIC_LENGTH_SIZE 2U
#define STSAFEA_UID_SIZE 8U
#define STSAFEA_MAX_FRAME_LENGTH_A100 507U
#define STSAFEA_MAX_FRAME_LENGTH_A110 507U
#define STSAFEA_MAX_FRAME_LENGTH_A120 752U

#define STSAFEA_PROT_Pos 5         /*!< STSAFE  Protected rsp mask */
#define STSAFEA_PROT_Msk 0xC0U     /*!< STSAFE  Protected mask */
#define STSAFEA_PROT_CMD_Msk 0x80U /*!< STSAFE  Protected command mask */
#define STSAFEA_PROT_RSP_Msk 0x40U /*!< STSAFE  Protected rsp mask */

#define STSAFEA_MAX_CMD_COUNT 29U
#define STSAFEA_MAX_EXT_CMD_COUNT 29U

/*!
 * \typedef stsafea_cmd_code_t
 * \brief STSAFE-A command type
 */
typedef PLAT_UI8 stsafea_cmd_code_t;
#define STSAFEA_CMD_ECHO ((stsafea_cmd_code_t)0x00U)                   /*!< STSAFEA Echo command code */
#define STSAFEA_CMD_RESET ((stsafea_cmd_code_t)0x01U)                  /*!< STSAFEA Reset command code*/
#define STSAFEA_CMD_GENERATE_RANDOM ((stsafea_cmd_code_t)0x02U)        /*!< STSAFEA Generate Random command code*/
#define STSAFEA_CMD_START_SESSION ((stsafea_cmd_code_t)0x03U)          /*!< STSAFEA Start Session command code*/
#define STSAFEA_CMD_DECREMENT ((stsafea_cmd_code_t)0x04U)              /*!< STSAFEA Decrement zone counter command code*/
#define STSAFEA_CMD_READ ((stsafea_cmd_code_t)0x05U)                   /*!< STSAFEA Read zone command code*/
#define STSAFEA_CMD_UPDATE ((stsafea_cmd_code_t)0x06U)                 /*!< STSAFEA Update zone command code*/
#define STSAFEA_CMD_INCREMENT ((stsafea_cmd_code_t)0x07U)              /*!< STSAFEA Increment zone counter command code*/
#define STSAFEA_CMD_DERIVE_LORA_KEY ((stsafea_cmd_code_t)0x08U)        /*!< STSAFEA Derive LORA key command code*/
#define STSAFEA_CMD_GENERATE_MAC ((stsafea_cmd_code_t)0x09U)           /*!< STSAFEA Generate MAC command code*/
#define STSAFEA_CMD_VERIFY_MAC ((stsafea_cmd_code_t)0x0AU)             /*!< STSAFEA Verify MAC command code*/
#define STSAFEA_CMD_UNWRAP_ISSUER_ENVELOPE ((stsafea_cmd_code_t)0x0BU) /*!< STSAFEA Unwrap issuer envelope command code*/
#define STSAFEA_CMD_DELETE ((stsafea_cmd_code_t)0x0CU)                 /*!< STSAFEA Delete MAC command code*/
#define STSAFEA_CMD_HIBERNATE ((stsafea_cmd_code_t)0x0DU)              /*!< STSAFEA Hibernate command code*/
#define STSAFEA_CMD_WRAP_LOCAL_ENVELOPE ((stsafea_cmd_code_t)0x0EU)    /*!< STSAFEA Wrap Local Envelope command code*/
#define STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE ((stsafea_cmd_code_t)0x0FU)  /*!< STSAFEA Unwrap Local Envelope command code*/
#define STSAFEA_CMD_PUT_ATTRIBUTE ((stsafea_cmd_code_t)0x10U)          /*!< STSAFEA Put attribute command code*/
#define STSAFEA_CMD_GENERATE_KEY ((stsafea_cmd_code_t)0x11U)           /*!< STSAFEA Generate key command code*/
#define STSAFEA_CMD_PUT_KEY ((stsafea_cmd_code_t)0x12U)                /*!< STSAFEA Put key command code*/
#define STSAFEA_CMD_CONFIGURE ((stsafea_cmd_code_t)0x13U)              /*!< STSAFEA Configure command code*/
#define STSAFEA_CMD_QUERY ((stsafea_cmd_code_t)0x14U)                  /*!< STSAFEA Query command code*/
#define STSAFEA_CMD_GET_SIGNATURE ((stsafea_cmd_code_t)0x15U)          /*!< STSAFEA Get Signature command code*/
#define STSAFEA_CMD_GENERATE_SIGNATURE ((stsafea_cmd_code_t)0x16U)     /*!< STSAFEA Generate Signature command code*/
#define STSAFEA_CMD_VERIFY_SIGNATURE ((stsafea_cmd_code_t)0x17U)       /*!< STSAFEA Verify Signature command code*/
#define STSAFEA_CMD_ESTABLISH_KEY ((stsafea_cmd_code_t)0x18U)          /*!< STSAFEA Establish Key command code*/
#define STSAFEA_CMD_STANDBY ((stsafea_cmd_code_t)0x19U)                /*!< STSAFEA Standby command code*/
#define STSAFEA_CMD_VERIFY_PASSWORD ((stsafea_cmd_code_t)0x1AU)        /*!< STSAFEA Verify Password command code*/
#define STSAFEA_CMD_ENCRYPT ((stsafea_cmd_code_t)0x1BU)                /*!< STSAFEA Encrypt command code*/
#define STSAFEA_CMD_DECRYPT ((stsafea_cmd_code_t)0x1CU)                /*!< STSAFEA Decrypt command code*/
#define STSAFEA_CMD_INVALID ((stsafea_cmd_code_t)0x1DU)
#define STSAFEA_CHUNK_PREFIX ((stsafea_cmd_code_t)0x1EU)
#define STSAFEA_EXTENDED_COMMAND_PREFIX ((stsafea_cmd_code_t)0x1FU)

/*!
 * \typedef stsafea_extended_cmd_code_t
 * \brief STSAFEA-A extended command type
 */
typedef PLAT_UI8 stsafea_extended_cmd_code_t;
#define STSAFEA_EXTENDED_CMD_START_HASH ((stsafea_extended_cmd_code_t)0x00U)                    /*!< STSAFE-A Start hash extended command code */
#define STSAFEA_EXTENDED_CMD_PROCESS_HASH ((stsafea_extended_cmd_code_t)0x01U)                  /*!< STSAFE-A Process hash extended command code */
#define STSAFEA_EXTENDED_CMD_FINISH_HASH ((stsafea_extended_cmd_code_t)0x02U)                   /*!< STSAFE-A Finish hash extended command code */
#define STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION ((stsafea_extended_cmd_code_t)0x03U)    /*!< STSAFE-A Start volatile KEK extended command code */
#define STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS ((stsafea_extended_cmd_code_t)0x04U)      /*!< STSAFE-A Establish symmetric keys extended command code */
#define STSAFEA_EXTENDED_CMD_CONFIRM_SYMMETRIC_KEYS ((stsafea_extended_cmd_code_t)0x05U)        /*!< STSAFE-A confirm symmetric keys extended command code */
#define STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION ((stsafea_extended_cmd_code_t)0x06U)     /*!< STSAFE-A Stop volatile KEK extended command code */
#define STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT ((stsafea_extended_cmd_code_t)0x07U)   /*!< STSAFE-A Plaintext write host key v2  extended command code */
#define STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED ((stsafea_extended_cmd_code_t)0x08U)     /*!< STSAFE-A Wrapped write host key v2 extended command code */
#define STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_WRAPPED ((stsafea_extended_cmd_code_t)0x09U)   /*!< STSAFE-A Write symmetric key extended command code */
#define STSAFEA_EXTENDED_CMD_WRITE_PUBLIC_KEY ((stsafea_extended_cmd_code_t)0x0AU)              /*!< STSAFE-A Write public key extended command code */
#define STSAFEA_EXTENDED_CMD_GENERATE_ECDHE ((stsafea_extended_cmd_code_t)0x0BU)                /*!< STSAFE-A Generate ECDHE extended command code */
#define STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE ((stsafea_extended_cmd_code_t)0x0EU)            /*!< STSAFE-A Generate challenge command code */
#define STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE ((stsafea_extended_cmd_code_t)0x0FU)       /*!< STSAFE-A Verify entity signature command code */
#define STSAFEA_EXTENDED_CMD_DERIVE_KEYS ((stsafea_extended_cmd_code_t)0x10U)                   /*!< STSAFE-A Derive keys command code */
#define STSAFEA_EXTENDED_CMD_START_ENCRYPT ((stsafea_extended_cmd_code_t)0x11U)                 /*!< STSAFE-A Start encrypt command code */
#define STSAFEA_EXTENDED_CMD_PROCESS_ENCRYPT ((stsafea_extended_cmd_code_t)0x12U)               /*!< STSAFE-A Process encrypt command code */
#define STSAFEA_EXTENDED_CMD_FINISH_ENCRYPT ((stsafea_extended_cmd_code_t)0x13U)                /*!< STSAFE-A Finish encrypt command code */
#define STSAFEA_EXTENDED_CMD_START_DECRYPT ((stsafea_extended_cmd_code_t)0x14U)                 /*!< STSAFE-A Start decrypt command code */
#define STSAFEA_EXTENDED_CMD_PROCESS_DECRYPT ((stsafea_extended_cmd_code_t)0x15U)               /*!< STSAFE-A Process decrypt command code */
#define STSAFEA_EXTENDED_CMD_FINISH_DECRYPT ((stsafea_extended_cmd_code_t)0x16U)                /*!< STSAFE-A Finish decrypt command code */
#define STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_PLAINTEXT ((stsafea_extended_cmd_code_t)0x17U) /*!< STSAFE-A Write symmetric key paintext command code */
#define STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2 ((stsafea_extended_cmd_code_t)0x18U)         /*!< STSAFE-A Establish host key V2 command code */
#define STSAFEA_EXTENDED_CMD_ERASE_SYMMETRIC_KEY_SLOT ((stsafea_extended_cmd_code_t)0x19U)      /*!< STSAFE-A Erase symmetric key slot command code */
#define STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY ((stsafea_extended_cmd_code_t)0x1AU)         /*!< STSAFE-A Decompress command code */
#define STSAFEA_EXTENDED_CMD_INVALID ((stsafea_extended_cmd_code_t)0x1FU)

stse_ReturnCode_t stsafea_get_command_count(stse_Handle_t *pSTSE, PLAT_UI8 *pCommand_count);

/**
 * \brief 		Get command access control table
 * \details 	This service retrieves the command access control table from the device
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	total_command_count		Total number of commands
 * \param[out] 	pChange_rights			Pointer to change rights structure
 * \param[out] 	pRecord_table			Pointer to record table array
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_get_command_AC_table(stse_Handle_t *pSTSE,
                                               PLAT_UI8 total_command_count,
                                               stse_cmd_authorization_CR_t *pChange_rights,
                                               stse_cmd_authorization_record_t *pRecord_table);

stse_ReturnCode_t stsafea_perso_info_update(stse_Handle_t *pSTSE);

/**
 * \brief 		Get command access conditions from personalization info
 * \details 	This function retrieves the access conditions for a specific command
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Command code to query
 * \param[out] 	pProtection		Pointer to store access conditions
 */
void stsafea_perso_info_get_cmd_AC(stse_perso_info_t *pPerso,
                                   PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t *pProtection);

/**
 * \brief 		Get extended command access conditions from personalization info
 * \details 	This function retrieves the access conditions for a specific extended command
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Extended command code to query
 * \param[out] 	pProtection		Pointer to store access conditions
 */
void stsafea_perso_info_get_ext_cmd_AC(stse_perso_info_t *pPerso,
                                       PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t *pProtection);

/**
 * \brief 		Get command encryption flag from personalization info
 * \details 	This function retrieves the encryption flag for a specific command
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Command code to query
 * \param[out] 	pEnc_flag		Pointer to store encryption flag
 */
void stsafea_perso_info_get_cmd_encrypt_flag(stse_perso_info_t *pPerso,
                                             PLAT_UI8 command_code,
                                             PLAT_UI8 *pEnc_flag);

/**
 * \brief 		Get response encryption flag from personalization info
 * \details 	This function retrieves the encryption flag for a specific command response
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Command code to query
 * \param[out] 	pEnc_flag		Pointer to store encryption flag
 */
void stsafea_perso_info_get_rsp_encrypt_flag(stse_perso_info_t *pPerso,
                                             PLAT_UI8 command_code,
                                             PLAT_UI8 *pEnc_flag);

/**
 * \brief 		Get extended command encryption flag from personalization info
 * \details 	This function retrieves the encryption flag for a specific extended command
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Extended command code to query
 * \param[out] 	pEnc_flag		Pointer to store encryption flag
 */
void stsafea_perso_info_get_ext_cmd_encrypt_flag(stse_perso_info_t *pPerso,
                                                 PLAT_UI8 command_code,
                                                 PLAT_UI8 *pEnc_flag);

/**
 * \brief 		Get extended response encryption flag from personalization info
 * \details 	This function retrieves the encryption flag for a specific extended command response
 * \param[in] 	pPerso			Pointer to personalization info structure
 * \param[in] 	command_code	Extended command code to query
 * \param[out] 	pEnc_flag		Pointer to store encryption flag
 */
void stsafea_perso_info_get_ext_rsp_encrypt_flag(stse_perso_info_t *pPerso,
                                                 PLAT_UI8 command_code,
                                                 PLAT_UI8 *pEnc_flag);

/**
 * \brief 		Set command access conditions in personalization info
 * \details 	This function sets the access conditions for a specific command
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Command code to configure
 * \param[in] 		protection		Access conditions to set
 */
void stsafea_perso_info_set_cmd_AC(stse_perso_info_t *pPerso,
                                   PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t protection);

/**
 * \brief 		Set extended command access conditions in personalization info
 * \details 	This function sets the access conditions for a specific extended command
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Extended command code to configure
 * \param[in] 		protection		Access conditions to set
 */
void stsafea_perso_info_set_ext_cmd_AC(stse_perso_info_t *pPerso,
                                       PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t protection);

/**
 * \brief 		Set command encryption flag in personalization info
 * \details 	This function sets the encryption flag for a specific command
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Command code to configure
 * \param[in] 		enc_flag		Encryption flag to set
 */
void stsafea_perso_info_set_cmd_encrypt_flag(stse_perso_info_t *pPerso,
                                             PLAT_UI8 command_code,
                                             PLAT_UI8 enc_flag);

/**
 * \brief 		Set response encryption flag in personalization info
 * \details 	This function sets the encryption flag for a specific command response
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Command code to configure
 * \param[in] 		enc_flag		Encryption flag to set
 */
void stsafea_perso_info_set_rsp_encrypt_flag(stse_perso_info_t *pPerso,
                                             PLAT_UI8 command_code,
                                             PLAT_UI8 enc_flag);

/**
 * \brief 		Set extended command encryption flag in personalization info
 * \details 	This function sets the encryption flag for a specific extended command
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Extended command code to configure
 * \param[in] 		enc_flag		Encryption flag to set
 */
void stsafea_perso_info_set_ext_cmd_encrypt_flag(stse_perso_info_t *pPerso,
                                                 PLAT_UI8 command_code,
                                                 PLAT_UI8 enc_flag);

/**
 * \brief 		Set extended response encryption flag in personalization info
 * \details 	This function sets the encryption flag for a specific extended command response
 * \param[in,out] 	pPerso			Pointer to personalization info structure
 * \param[in] 		command_code	Extended command code to configure
 * \param[in] 		enc_flag		Encryption flag to set
 */
void stsafea_perso_info_set_ext_rsp_encrypt_flag(stse_perso_info_t *pPerso,
                                                 PLAT_UI8 command_code,
                                                 PLAT_UI8 enc_flag);

/** \}*/
/** \}*/
#endif /*STSAFE_COMMANDS_H*/
