/*!
 ******************************************************************************
 * \file	stsafea_low_power.c
 * \brief   low-power modes services for STSAFE-A
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


#include <services/stsafea/stsafea_low_power.h>

stse_ReturnCode_t stsafea_hibernate(stse_Handler_t *pSTSE,
									 stsafea_hibernate_wake_up_mode_t wake_up_mode)

{
	(void)wake_up_mode;
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_HIBERNATE;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_HIBERNATE]
	);

	return( ret );
}

