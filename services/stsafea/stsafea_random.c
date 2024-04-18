/*!
 ******************************************************************************
 * \file	stsafea_random.c
 * \brief   Random services for STSAFE
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


#include <services/stsafea/stsafea_random.h>


stse_ReturnCode_t stsafea_generate_random(
		stse_Handler_t * pSTSE ,
		PLAT_UI8 * pRandom,
		PLAT_UI8 random_size
)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_RANDOM;
	PLAT_UI8 subject = 0x00;
	PLAT_UI8 rsp_header;

	if((pSTSE == NULL)||(pRandom == NULL)||(random_size == 0))
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject,1,&subject);
	stse_frame_element_allocate_push(&CmdFrame,eSize,1,&random_size);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eRandom,random_size,pRandom);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_GENERATE_RANDOM]
	);

	return( ret );
}
