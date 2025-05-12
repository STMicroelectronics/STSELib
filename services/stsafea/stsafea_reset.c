/*!
 ******************************************************************************
 * \file	stsafea_reset.c
 * \brief   Reset services for STSAFE
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

#include "services/stsafea/stsafea_reset.h"
#include "services/stsafea/stsafea_frame_transfer.h"


#ifdef STSE_CONF_STSAFE_A_SUPPORT


stse_ReturnCode_t stsafea_reset( stse_Handler_t * pSTSE )
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_RESET;
	PLAT_UI8 rsp_header;

	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	return stsafea_frame_raw_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][cmd_header]
			);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
