 /******************************************************************************
 * \file	stsafel_timings.c
 * \brief   STSAFE-L Timings definitions
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

#include "services/stsafel/stsafel_timings.h"


#ifdef STSE_CONF_STSAFE_L_SUPPORT


#define STSAFEL_BOOT_TIME_DEFAULT       10U
#define STSAFEL_WAKEUP_TIME_DEFAULT     10U


const PLAT_UI16 stsafel_cmd_timings[STSAFEL_PRODUCT_COUNT][STSAFEL_MAX_CMD_COUNT] = {
	/* STSAFE_L010 */
	{
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L general purpose "Echo" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L general purpose "Reset" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L general purpose "Hibernate" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L general purpose "Get data" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L general purpose "Get Public key certificate" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L data partition "Decrement" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L data partition "Read" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L data partition "Update" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L "Generate Signature" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L "Put Data" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L "Increment" command code */
		STSAFEL_EXEC_TIME_DEFAULT,	/*!< STSAFE-L "Repeat" command code */
    }
};

const PLAT_UI16 stsafel_boot_time[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_BOOT_TIME_DEFAULT, /* STSAFE_L010 */
};

const PLAT_UI16 stsafel_wakeup_time[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_WAKEUP_TIME_DEFAULT, /* STSAFE_L010 */
};

inline PLAT_UI16 stsafel_cmd_exec_duration(
    stse_Handler_t * pSTSE,
    stsafel_cmd_code_t command_code)
{
	PLAT_UI8 CommandCode = command_code;

	if (command_code >= STSAFEL_CMD_GENERATE_SIGNATURE)
	{
		CommandCode += 1 + STSAFEL_CMD_UPDATE - STSAFEL_CMD_GENERATE_SIGNATURE;
	}

	if (command_code >= STSAFEL_CMD_INCREMENT)
	{
		CommandCode += 1 + STSAFEL_CMD_PUT_DATA - STSAFEL_CMD_INCREMENT;
	}

	if (command_code >= STSAFEL_CMD_REPEAT)
	{
		CommandCode += 1 + STSAFEL_CMD_INCREMENT - STSAFEL_CMD_REPEAT;
	}

	return stsafel_cmd_timings[pSTSE->device_type - STSE_DEVICE_STSAFEL_FAMILY_INDEX][CommandCode];
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
