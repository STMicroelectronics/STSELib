/*!
 ******************************************************************************
 * \file	stsafea_echo.c
 * \brief   Echo services for STSAFE-A
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

#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_sessions.h"


stse_ReturnCode_t stsafea_echo( stse_Handler_t * pSTSE ,
		PLAT_UI8 * pMessage ,
		PLAT_UI8 * pEchoed_message,
		PLAT_UI16  message_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_ECHO;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if (pMessage == NULL || pEchoed_message == NULL || message_length == 0)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eMessage,message_length,pMessage);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eEchoed_message,message_length,pEchoed_message);

#ifdef STSE_CONF_USE_HOST_SESSION
	/*- Perform Transfer*/
	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_ECHO]
		);
	}
	else if (cmd_ac_info != STSE_CMD_AC_FREE)
	{
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_ECHO]
		);
	}
	else
#endif
	{
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_ECHO]
		);
	}

	return( ret );
}

