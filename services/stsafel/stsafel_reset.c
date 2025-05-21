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
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "services/stsafel/stsafel_reset.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_ReturnCode_t stsafel_reset(stse_Handler_t *pSTSE)
{
	PLAT_UI8 cmd_header = STSAFEL_CMD_RESET;
	PLAT_UI8 rsp_header;

	if (pSTSE == NULL) {
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEL_HEADER_SIZE, &cmd_header);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEL_HEADER_SIZE, &rsp_header);

	/*- Perform Transfer*/
	return stsafel_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
