/*!
 ******************************************************************************
 * \file	stsafel_data_partition.c
 * \brief   Data partition services for STSAFE-L
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

#include "services/stsafel/stsafel_data_partition.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_frame_transfer.h"


#ifdef STSE_CONF_STSAFE_L_SUPPORT


stse_ReturnCode_t stsafel_read_data_zone(stse_Handler_t * pSTSE,
    PLAT_UI8 zone_index,
    stsafel_read_option_t option,
    PLAT_UI16 offset,
    PLAT_UI8 *pData,
    PLAT_UI16 data_length,
	stse_cmd_protection_t protection)
{
	PLAT_UI8 cmd_header = STSAFEL_CMD_READ;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if(pData == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEL_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eRead_option,sizeof(stsafel_read_option_t),(PLAT_UI8 *)&option);
	stse_frame_element_allocate_push(&CmdFrame,eZone_index,1,&zone_index);
	stse_frame_element_allocate_push(&CmdFrame,eOffset,2,(PLAT_UI8 *)&offset);
	stse_frame_element_allocate_push(&CmdFrame,eData_length,2,(PLAT_UI8 *)&data_length);

	/*- Create Rsp frame and populate elements */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEL_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eData,data_length,pData);

	stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eData_length);

	/*- Perform Transfer*/
	return stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);
}


stse_ReturnCode_t stsafel_update_data_zone(stse_Handler_t * pSTSE,
    PLAT_UI8 zone_index,
    stsafel_update_option_t option,
    PLAT_UI16 offset,
    PLAT_UI8 *pData ,
    PLAT_UI16 data_length,
	stse_cmd_protection_t protection)
{
	PLAT_UI8 cmd_header = STSAFEL_CMD_UPDATE;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if(pData == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEL_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eUpdate_option,sizeof(stsafel_update_option_t),(PLAT_UI8 *)&option);
	stse_frame_element_allocate_push(&CmdFrame,eZone_index,1,&zone_index);
	stse_frame_element_allocate_push(&CmdFrame,eOffset,2,(PLAT_UI8 *)&offset);
	stse_frame_element_allocate_push(&CmdFrame,eData,data_length,pData);

	/*- Create Rsp frame and populate elements */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEL_HEADER_SIZE,&rsp_header);

	stse_frame_element_swap_byte_order(&eOffset);

	/*- Perform Transfer*/
	/*- Perform Transfer*/
	return stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);
}


stse_ReturnCode_t stsafel_read_counter_zone(stse_Handler_t * pSTSE,
    PLAT_UI8 zone_index,
    stsafel_read_option_t option,
    PLAT_UI16 offset,
    PLAT_UI8 *pData,
    PLAT_UI16 data_length,
    PLAT_UI32 *pCounter_value,
	stse_cmd_protection_t protection)
{
    stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEL_CMD_READ;
	PLAT_UI8 rsp_header;
	PLAT_UI8 temp_counter[STSAFEL_COUNTER_VALUE_SIZE];

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if(pData == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEL_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eRead_option,sizeof(stsafel_read_option_t),(PLAT_UI8 *)&option);
	stse_frame_element_allocate_push(&CmdFrame,eZone_index,1,&zone_index);
	stse_frame_element_allocate_push(&CmdFrame,eOffset,2,(PLAT_UI8 *)&offset);
	stse_frame_element_allocate_push(&CmdFrame,eData_length,2,(PLAT_UI8 *)&data_length);

	/*- Create Rsp frame and populate elements */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEL_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eCounter,STSAFEL_COUNTER_VALUE_SIZE,temp_counter);
	stse_frame_element_allocate_push(&RspFrame,eData,data_length,pData);

	stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eData_length);

	/*- Perform Transfer*/
	ret = stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);

	if(ret == STSE_OK)
	{
		*pCounter_value = ((temp_counter[2])
						|  (temp_counter[1] << 8)
						|  (temp_counter[0] << 16));
	}

	return( ret );
}

stse_ReturnCode_t stsafel_decrement_counter_zone(stse_Handler_t * pSTSE,
    PLAT_UI8 zone_index,
    stsafel_decrement_option_t option,
    PLAT_UI32 amount,
    PLAT_UI16 offset,
    PLAT_UI8 *pData,
	PLAT_UI16  data_length,
    PLAT_UI32 *pNew_counter_value,
	stse_cmd_protection_t protection)
{
    stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEL_CMD_DECREMENT;
	PLAT_UI8 rsp_header;
    PLAT_UI8 decrement_amount[STSAFEL_COUNTER_VALUE_SIZE] = {
		((amount & 0xFF0000) >> 16),
		((amount & 0xFF00) >> 8),
		(amount & 0xFF)};
	PLAT_UI8 temp_counter[STSAFEL_COUNTER_VALUE_SIZE];

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if(pData == NULL)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEL_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eDecrement_option,sizeof(stsafel_decrement_option_t),(PLAT_UI8 *)&option);
	stse_frame_element_allocate_push(&CmdFrame,eZone_index,1,&zone_index);
	stse_frame_element_allocate_push(&CmdFrame,eOffset,2,(PLAT_UI8 *)&offset);
	stse_frame_element_allocate_push(&CmdFrame,eAmount,STSAFEL_COUNTER_VALUE_SIZE,decrement_amount);
	stse_frame_element_allocate_push(&CmdFrame,eData,data_length,pData);

	/*- Create Rsp frame and populate elements */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEL_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eCounter,STSAFEL_COUNTER_VALUE_SIZE,temp_counter);

	stse_frame_element_swap_byte_order(&eOffset);

	/*- Perform Transfer*/
	ret = stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);

	if(ret == STSE_OK)
	{
		*pNew_counter_value = ((temp_counter[2])
							|  (temp_counter[1] << 8)
							|  (temp_counter[0] << 16));
	}

	return( ret );
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
