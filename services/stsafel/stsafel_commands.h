/*!
 * ******************************************************************************
 * \file	stsafel_commands.h
 * \brief   STSAFE-L command typedefs and services
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


#ifndef STSAFEL_COMMANDS_H
#define STSAFEL_COMMANDS_H

/*! \defgroup stsafea_services STSAFE-Lxxx
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

// #define STSAFEL_EXT_HEADER_SIZE 			    2U
// #define STSAFEL_HEADER_SIZE 				    1U
// #define STSAFEL_CMD_RSP_LEN_SIZE 			2U
// #define STSAFEL_COUNTER_VALUE_SIZE			4U
#define STSAFEL_GENERIC_LENGTH_SIZE			2U
#define STSAFEL_UID_SIZE 					8U
#define STSAFEL_MAXIMUM_CMD_RSP_LENGTH_L010	507U

#define STSAFEL_PROT_CMD_Msk				0x80U  /*!< STSAFE  Protected command mask */
#define STSAFEL_PROT_RSP_Msk				0x40U  /*!< STSAFE  Protected rsp mask */

/*!
 * \enum stsafel_cmd_code_t
 * \brief STSAFE-L command type
 */
typedef enum stsafel_cmd_code_t {
	STSAFEL_CMD_ECHO = 0x00U,                         /*!< STICK general purpose "Echo" command code */
	STSAFEL_CMD_RESET,                                /*!< STICK general purpose "Reset" command code */
	STSAFEL_CMD_HIBERNATE,                            /*!< STICK general purpose "Reset" command code */
	STSAFEL_CMD_GET_DATA,                             /*!< STICK general purpose "Get data" command code */
	STSAFEL_CMD_GET_PUBLIC_KEY_CERTIFICATE,           /*!< STICK general purpose "Get Public key certificate" command code */
	STSAFEL_CMD_DECREMENT,                            /*!< STICK data partition "Decrement" command code */
	STSAFEL_CMD_READ,                                 /*!< STICK data partition "Read" command code */
	STSAFEL_CMD_UPDATE,                               /*!< STICK data partition "Update" command code */
	STSAFEL_CMD_GENERATE_SIGNATURE = 0x11U,           /*!< STICK "Generate Signature" command code */
	STSAFEL_CMD_PUT_DATA,                             /*!< STICK "Put Data" command code */
	STSAFEL_CMD_INCREMENT = 0x16U,                    /*!< STICK "Regenerate" command code */
	STSAFEL_CMD_REPEAT = 0x3EU,                       /*!< STICK "Repeat" command code */
} stsafel_cmd_code_t;

extern const PLAT_UI16 stsafel_maximum_command_length[4];

/** \}*/
/** \}*/
#endif /*STSAFEL_COMMANDS_H*/


