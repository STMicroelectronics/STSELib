/*!
 ******************************************************************************
 * \file	stsafel_low_power.c
 * \brief   Low power services for STSAFE-L
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

#include "services/stsafel/stsafel_low_power.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_frame.h"
#include "services/stsafel/stsafel_echo.h"


stse_ReturnCode_t stsafel_hibernate(stse_Handler_t * pSTSE)
{
	PLAT_UI8 cmd_header = STSAFEL_CMD_HIBERNATE;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	return stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);
}

stse_ReturnCode_t stsafel_wakeup(stse_Handler_t * pSTSE)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	/* When device wakeup callback is available, call it */
	if(pSTSE->io.BusWake != NULL)
	{
		ret = pSTSE->io.BusWake(
				pSTSE->io.busID,
				pSTSE->io.Devaddr,
				pSTSE->io.BusSpeed);
	}
	/* When wakeup callback isn't available but bus type is I²C, send a small echo command */
	else if(pSTSE->io.BusType == STSE_BUS_TYPE_I2C) {
		PLAT_UI8 echo_message[1] = {0x00};
		ret = stsafel_echo(pSTSE, echo_message, echo_message, 1);
	}
	/* If wakeup callback isn't available and bus type isn't I²C, return an error */
	else {
		ret = STSE_SERVICE_INVALID_PARAMETER;
	}

	return( ret );
}
