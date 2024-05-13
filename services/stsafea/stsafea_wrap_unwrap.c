
#include "services/stsafea/stsafea_wrap_unwrap.h"

stse_ReturnCode_t stsafea_wrap_payload( stse_Handler_t 	*pSTSE,
		PLAT_UI8 		wrap_key_slot,
		PLAT_UI8 		*pPayload,
		PLAT_UI16 		payload_size,
		PLAT_UI8 		*pWrapped_Payload,
		PLAT_UI16 		wrapped_payload_size
)
{
	volatile stse_ReturnCode_t ret;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, STSAFEA_CMD_WRAP_LOCAL_ENVELOPE, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, STSAFEA_CMD_WRAP_LOCAL_ENVELOPE, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, STSAFEA_CMD_WRAP_LOCAL_ENVELOPE, &cmd_ac_info);

	if((pPayload == NULL)
	|| (pWrapped_Payload == NULL)
	|| (payload_size > 480)
	|| (payload_size == 0)
	|| (wrapped_payload_size != (payload_size + 8)))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_WRAP_LOCAL_ENVELOPE;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&wrap_key_slot);
	stse_frame_element_allocate_push(&CmdFrame,ePayload,payload_size,pPayload);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eWrapped,wrapped_payload_size,pWrapped_Payload);

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_WRAP_LOCAL_ENVELOPE]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_WRAP_LOCAL_ENVELOPE]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_WRAP_LOCAL_ENVELOPE]
		);

	}

	return ret;
}


stse_ReturnCode_t stsafea_unwrap_payload( stse_Handler_t *pSTSE,
		PLAT_UI8 		wrap_key_slot,
		PLAT_UI8 		*pWrapped_Payload,
		PLAT_UI16 		wrapped_payload_size,
		PLAT_UI8 		*pPayload,
		PLAT_UI16 		payload_size
)
{
	volatile stse_ReturnCode_t ret;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE, &cmd_ac_info);

	if((pPayload == NULL)
	|| (pWrapped_Payload == NULL)
	|| (wrapped_payload_size > 488)
	|| (wrapped_payload_size < 8)
	|| (wrapped_payload_size != (payload_size + 8)) )
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&wrap_key_slot);
	stse_frame_element_allocate_push(&CmdFrame,ePayload,wrapped_payload_size,pWrapped_Payload);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eWrapped,payload_size,pPayload);

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_WRAP_LOCAL_ENVELOPE]
		);

	}

	return ret;
}

