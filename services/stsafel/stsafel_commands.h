/*!
 * ******************************************************************************
 * \file	stsafel_commands.h
 * \brief   STSAFE-A command services (header)
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

/*! \defgroup stsafel_services STSAFE-L
*  \ingroup stse_services
 *  @{
 */

/*! \defgroup stsafel_cmd_set  STSAFE-L Command Codes
*  \ingroup stsafel_services
 *  @{
 */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"

#define STSAFEL_MAX_CMD_COUNT 12U

#define STSAFEL_HEADER_SIZE 1U
#define STSAFEL_COUNTER_VALUE_SIZE 3U
#define STSAFEL_MAX_FRAME_LENGTH_L010 750U

/*!
 * \typedef stsafel_cmd_code_t
 * \brief STSAFE-L command type
 */
typedef PLAT_UI8 stsafel_cmd_code_t;
#define STSAFEL_CMD_ECHO ((stsafel_cmd_code_t)0x00U)                       /*!< STSAFE-L010 general purpose "Echo" command code */
#define STSAFEL_CMD_RESET ((stsafel_cmd_code_t)0x01U)                      /*!< STSAFE-L010 general purpose "Reset" command code */
#define STSAFEL_CMD_HIBERNATE ((stsafel_cmd_code_t)0x02U)                  /*!< STSAFE-L010 general purpose "Reset" command code */
#define STSAFEL_CMD_GET_DATA ((stsafel_cmd_code_t)0x03U)                   /*!< STSAFE-L010 general purpose "Get data" command code */
#define STSAFEL_CMD_GET_PUBLIC_KEY_CERTIFICATE ((stsafel_cmd_code_t)0x04U) /*!< STSAFE-L010 general purpose "Get Public key certificate" command code */
#define STSAFEL_CMD_DECREMENT ((stsafel_cmd_code_t)0x05U)                  /*!< STSAFE-L010 data partition "Decrement" command code */
#define STSAFEL_CMD_READ ((stsafel_cmd_code_t)0x06U)                       /*!< STSAFE-L010 data partition "Read" command code */
#define STSAFEL_CMD_UPDATE ((stsafel_cmd_code_t)0x07U)                     /*!< STSAFE-L010 data partition "Update" command code */
#define STSAFEL_CMD_GENERATE_SIGNATURE ((stsafel_cmd_code_t)0x11U)         /*!< STSAFE-L010 "Generate Signature" command code */
#define STSAFEL_CMD_PUT_DATA ((stsafel_cmd_code_t)0x12U)                   /*!< STSAFE-L010 "Put Data" command code */
#define STSAFEL_CMD_INCREMENT ((stsafel_cmd_code_t)0x16U)                  /*!< STSAFE-L010 "Regenerate" command code */
#define STSAFEL_CMD_REPEAT ((stsafel_cmd_code_t)0x3EU)                     /*!< STSAFE-L010 "Repeat" command code */

/** \}*/
/** \}*/
#endif /*STSAFEL_COMMANDS_H*/
