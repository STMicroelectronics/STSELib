/*!
 ******************************************************************************
 * \file	stsafea_put_query.c
 * \brief   Put and query services for STSAFE-A
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

#include <services/stsafea/stsafea_put_query.h>

stse_ReturnCode_t stsafea_put_life_cyle_state(
		stse_Handler_t * pSTSAFE,
		stsafea_life_cycle_state_t life_cycle_state)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
	PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_LIFE_CYCLE_STATE;
	PLAT_UI8 rsp_header;

	if(pSTSAFE == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eTag,1,&tag);
	stse_frame_element_allocate_push(&CmdFrame,eLifeCycleState,1,(PLAT_UI8*)&life_cycle_state);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSAFE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSAFE->device_type][STSAFEA_CMD_QUERY]
	);
	return( ret );
}

stse_ReturnCode_t stsafea_query_life_cycle_state(
		stse_Handler_t * pSTSAFE,
		stsafea_life_cycle_state_t *pLife_cycle_state)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_LIFE_CYCLE_STATE;
	PLAT_UI8 rsp_header;

	if(pSTSAFE == NULL)
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
	stse_frame_element_allocate_push(&RspFrame,eLife_cycle_state,1,(PLAT_UI8*)pLife_cycle_state);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSAFE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSAFE->device_type][STSAFEA_CMD_QUERY]
	);
	return( ret );
}



