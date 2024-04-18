/*!
 ******************************************************************************
 * \file	stsafea_aes.c
 * \brief   STSAFE Middleware services for symmetric key cryptography (source)
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
 *****************************************************************************/

#include "services/stsafea/stsafea_aes.h"

stse_ReturnCode_t stsafea_aes_ecb_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */

	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	/* - Check stsafe handler initialization */

	if((pPlaintext_message == NULL)
	|| (pEncrypted_message == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);


	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MESSAGE]  */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,ePlaintext_message,message_length,pPlaintext_message);

	/* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE]  */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eEncrypted_message,message_length,pEncrypted_message);

	/* - Perform Transfer*/

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}

	if (ret != STSE_OK)
	{
		memset(pEncrypted_message, 0, message_length);
	}

	return ret;
}

stse_ReturnCode_t stsafea_aes_ecb_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pPlaintext_message)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((pPlaintext_message == NULL)
	|| (pEncrypted_message == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);


	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ENCRYPTED MESSAGE]  */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eEncrypted_message,message_length,pEncrypted_message);

	/* - Prepare RSP Frame : [HEADER] [PLAIN TEXT MESSAGE]  */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,ePlaintext_message,message_length,pPlaintext_message);

	/* - Perform Transfer*/

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}
	if (ret != STSE_OK)
	{
		memset(pPlaintext_message, 0, message_length);
	}

	return ret;
}

stse_ReturnCode_t stsafea_aes_ccm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pCounter_presence,
		PLAT_UI32 * pCounter)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;
	PLAT_UI8 alt_counter_presence;
	PLAT_UI8 alt_counter[STSAFEA_COUNTER_VALUE_SIZE];
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((pNonce == NULL)
	|| (pAssociated_data 	== NULL && associated_data_length != 0)
	|| (pAssociated_data 	!= NULL && associated_data_length == 0)
	|| (pPlaintext_message  == NULL)
	|| (pEncrypted_message  == NULL)
	|| (pAuthentication_tag == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ASSOCIATED DATA LENGHT] ...
	 *                       ... [ASSOCIATED DATA MESSAGE] [MESSAGE LENGHT] [MESSAGE] */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eNonce,STSAFEA_NONCE_SIZE,pNonce);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data_length,2,(PLAT_UI8*)&associated_data_length);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data,associated_data_length,pAssociated_data);
	stse_frame_element_allocate_push(&CmdFrame,eMessage_length,2,(PLAT_UI8*)&message_length);
	stse_frame_element_allocate_push(&CmdFrame,ePlaintext_message,message_length,pPlaintext_message);

	/* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE] [TAG LENGTH] [COUNTER PRES.] [COUNTER VAL] */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eEncrypted_message,message_length,pEncrypted_message);
	stse_frame_element_allocate_push(&RspFrame,eAuthentication_tag,authentication_tag_length,pAuthentication_tag);
	stse_frame_element_allocate_push(&RspFrame,eCounter_presence,1,pCounter_presence);
	stse_frame_element_allocate_push(&RspFrame,eCounter,STSAFEA_COUNTER_VALUE_SIZE,(PLAT_UI8*)pCounter);

	if(pCounter_presence == NULL)
	{
		eCounter_presence.pData = &alt_counter_presence;
	}

	if(pCounter == NULL)
	{
		eCounter.pData = alt_counter;
	}

	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	/* - Perform Transfer*/

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}

	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	return ret;
}

stse_ReturnCode_t stsafea_aes_ccm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI16 encrypted_message_length = message_length + authentication_tag_length;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */

	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((pNonce == NULL)
	|| (pAssociated_data 	== NULL && associated_data_length != 0)
	|| (pAssociated_data 	!= NULL && associated_data_length == 0)
	|| (pEncrypted_message 	 == NULL)
	|| (pAuthentication_tag  == NULL)
	|| (pVerification_result == NULL)
	|| (pPlaintext_message 	 == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [NONCE] [ASSOCIATED DATA LENGHT] ...
	 *                       ... [ASSOCIATED DATA] [MESSAGE LENGHT] [ENCRYPTED MESSAGE] [TAG] */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eNonce,STSAFEA_NONCE_SIZE,pNonce);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data_length,2,(PLAT_UI8*)&associated_data_length);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data,associated_data_length,pAssociated_data);
	stse_frame_element_allocate_push(&CmdFrame,eMessage_length,2,(PLAT_UI8*)&encrypted_message_length);
	stse_frame_element_allocate_push(&CmdFrame,eEncrypted_message,message_length,pEncrypted_message);
	stse_frame_element_allocate_push(&CmdFrame,eAuthentication_tag,authentication_tag_length,pAuthentication_tag);

	/* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] [PLAIN TEXT MESSAGE] */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eVerification_result,1,pVerification_result);
	stse_frame_element_allocate_push(&RspFrame,ePlaintext_message,message_length,pPlaintext_message);

	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}

	return ret;
}

stse_ReturnCode_t stsafea_aes_gcm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((pIV == NULL || IV_length == 0)
	|| (pAssociated_data 	== NULL && pPlaintext_message 	== NULL)
	|| (pAssociated_data 	== NULL && associated_data_length != 0)
	|| (pAssociated_data 	!= NULL && associated_data_length == 0)
	|| (pPlaintext_message 	== NULL && message_length != 0)
	|| (pPlaintext_message 	!= NULL && message_length == 0)
	|| (pEncrypted_message  == NULL && pPlaintext_message != NULL)
	|| (pAuthentication_tag == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ASSOCIATED DATA LENGHT] ...
	 *                       ... [ASSOCIATED DATA MESSAGE] [MESSAGE LENGHT] [MESSAGE] */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eIV_length,STSAFEA_GENERIC_LENGTH_SIZE,(PLAT_UI8*)&IV_length);
	stse_frame_element_allocate_push(&CmdFrame,eIV,IV_length,pIV);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data_length,2,(PLAT_UI8*)&associated_data_length);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data,associated_data_length,pAssociated_data);
	stse_frame_element_allocate_push(&CmdFrame,eMessage_length,2,(PLAT_UI8*)&message_length);
	stse_frame_element_allocate_push(&CmdFrame,ePlaintext_message,message_length,pPlaintext_message);

	/* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE] [TAG LENGTH] */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eEncrypted_message,message_length,pEncrypted_message);
	stse_frame_element_allocate_push(&RspFrame,eAuthentication_tag,authentication_tag_length,pAuthentication_tag);

	stse_frame_element_swap_byte_order(&eIV_length);
	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	/* - Perform Transfer*/

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}

	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	return ret;
}

stse_ReturnCode_t stsafea_aes_gcm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */

	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((pIV == NULL || IV_length == 0)
	|| (pAssociated_data 	== NULL && pEncrypted_message 	== NULL)
	|| (pAssociated_data 	== NULL && associated_data_length != 0)
	|| (pAssociated_data 	!= NULL && associated_data_length == 0)
	|| (pEncrypted_message 	== NULL && message_length != 0)
	|| (pEncrypted_message 	!= NULL && message_length == 0)
	|| (pPlaintext_message  == NULL && pEncrypted_message != NULL)
	|| (pAuthentication_tag == NULL))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [IV] [ASSOCIATED DATA LENGHT] ...
	 *                       ... [ASSOCIATED DATA] [MESSAGE LENGHT] [ENCRYPTED MESSAGE] [TAG] */

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSub_command_distinguisher,1,&sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,1,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eIV_length,STSAFEA_GENERIC_LENGTH_SIZE,(PLAT_UI8*)&IV_length);
	stse_frame_element_allocate_push(&CmdFrame,eIV,IV_length,pIV);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data_length,2,(PLAT_UI8*)&associated_data_length);
	stse_frame_element_allocate_push(&CmdFrame,eAssociated_data,associated_data_length,pAssociated_data);
	stse_frame_element_allocate_push(&CmdFrame,eMessage_length,2,(PLAT_UI8*)&message_length);
	stse_frame_element_allocate_push(&CmdFrame,eEncrypted_message,message_length,pEncrypted_message);
	stse_frame_element_allocate_push(&CmdFrame,eAuthentication_tag,authentication_tag_length,pAuthentication_tag);

	/* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] [PLAIN TEXT MESSAGE] */

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eVerification_result,1,pVerification_result);
	stse_frame_element_allocate_push(&RspFrame,ePlaintext_message,message_length,pPlaintext_message);

	stse_frame_element_swap_byte_order(&eIV_length);
	stse_frame_element_swap_byte_order(&eAssociated_data_length);
	stse_frame_element_swap_byte_order(&eMessage_length);

	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {

		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);

	}

	return ret;
}
