/*!
 ******************************************************************************
 * \file	stsafel_echo.c
 * \brief   Echo services for STSAFE-L
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

#include <services/stsafel/stsafel_echo.h>
#include <services/stsafel/stsafel_timings.h>


stse_ReturnCode_t stsafel_echo( stse_Handler_t * pSTSE ,
		PLAT_UI8 * pMessage ,
		PLAT_UI8 * pEchoed_message,
		PLAT_UI16  message_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEL_CMD_ECHO;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if((pMessage == NULL) ||
	   (pEchoed_message == NULL))
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	if (message_length == 0)
	{
		return STSE_OK;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eMessage,message_length,pMessage);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eEchoed_message,message_length,pEchoed_message);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafel_cmd_exec_duration(pSTSE, STSAFEL_CMD_ECHO)
	);

	return( ret );
}
