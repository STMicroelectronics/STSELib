/*!
 ******************************************************************************
 * \file	stsafea_password.c
 * \brief   password services for STSAFE-A
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


#include <services/stsafea/stsafea_password.h>

stse_ReturnCode_t stsafea_verify_password(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pPassword_buffer,
		PLAT_UI8 password_length,
		PLAT_UI8 * pVerification_status,
		PLAT_UI8 * pRemaining_tries)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_PASSWORD;
	PLAT_UI8 rsp_header;

	if((pSTSE == NULL) || (password_length != STSAFEA_PASSWORD_LENGTH))
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,ePassword,password_length,pPassword_buffer);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eVerStat,1,pVerification_status);
	stse_frame_element_allocate_push(&RspFrame,eRemTri,1,pRemaining_tries);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_VERIFY_PASSWORD]
	);

	return( ret );
}



stse_ReturnCode_t stsafea_delete_password(stse_Handler_t * pSTSE)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_DELETE;
	PLAT_UI8 tag = STSAFEA_DELETE_TAG_PASSWORD;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eTag,1,&tag);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_DELETE]
	);

	return( ret );
}
