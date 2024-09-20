/*!
 ******************************************************************************
 * \file	stsafel_reset.c
 * \brief   Reset services for STSAFE-L
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

#include <services/stsafel/stsafel_reset.h>
#include <services/stsafel/stsafel_timings.h>

stse_ReturnCode_t stsafel_reset(stse_Handler_t * pSTSE)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEL_CMD_RESET;
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
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafel_cmd_exec_duration(pSTSE, STSAFEL_CMD_RESET)
	);

	return( ret );
}
