

#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_sessions.h"

#define STSAFEA_AES_SUBJECT_HOST_CMAC 0x00U
#define STSAFEA_AES_SUBJECT_HOST_RMAC 0x40U
#define STSAFEA_AES_SUBJECT_HOST_DECRYPT 0xC0U
#define STSAFEA_AES_SUBJECT_HOST_ENCRYPT 0x80U
#define STSAFEA_AES_PADDING_FIRST_BYTE 0x80U


/* Private variables ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

stse_ReturnCode_t stsafea_open_host_session( stse_Handler_t *pSTSE, stse_session_t *pSession , PLAT_UI8 *pHost_MAC_key , PLAT_UI8 *pHost_cypher_key)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return STSE_CORE_HANDLER_NOT_INITIALISED;
	}

	if(pSession == NULL)
	{
		return STSE_CORE_SESSION_ERROR;
	}

	if(pSTSE->device_type == STSAFE_A120)
	{
		stsafea_host_key_slot_v2_t host_key_slot;

		ret =  stsafea_query_host_key_v2(pSTSE, &host_key_slot);
		if(ret != STSE_OK)
		{
			return ret;
		}

		if(host_key_slot.key_presence_flag == 0)
		{
			return STSE_SERVICE_SESSION_ERROR;
		}
		pSession->context.host.key_type = (stse_aes_key_type_t)host_key_slot.key_type;
		pSession->context.host.MAC_counter = ARRAY_4B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
	} else {
		stsafea_host_key_slot_t host_key_slot;

		ret = stsafea_query_host_key(pSTSE, &host_key_slot);
		if(ret != STSE_OK)
		{
			return ret;
		}

		if(host_key_slot.key_presence_flag == 0)
		{
			return STSE_SERVICE_SESSION_ERROR;
		}
		pSession->context.host.key_type = STSE_AES_128_KT;
		pSession->context.host.MAC_counter = ARRAY_3B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
	}

	pSession->type = STSE_HOST_SESSION;
	pSession->context.host.pHost_MAC_key = pHost_MAC_key;
	pSession->context.host.pHost_cypher_key = pHost_cypher_key;
	pSession->context.host.pSTSE = pSTSE;
	pSTSE->pActive_host_session = pSession;

	return( STSE_OK );
}

void stsafea_close_host_session(stse_session_t *pSession)
{
	stsafea_session_clear_context(pSession);
}

void stsafea_session_clear_context(stse_session_t *pSession)
{
	/* - Check stsafe handler initialization */
	if (pSession==NULL)
	{
		return;
	}

	/*Todo validate correct erase */
	memset(pSession, 0x00, sizeof(stse_session_t));
}

stse_ReturnCode_t stsafea_set_active_host_session( stse_Handler_t *pSTSE, stse_session_t *pSession )
{
	if(pSTSE == NULL)
	{
		return STSE_CORE_HANDLER_NOT_INITIALISED;
	}

	if(pSession == NULL)
	{
		return STSE_CORE_SESSION_ERROR;
	}

	pSTSE->pActive_host_session = pSession;

	return( STSE_OK );
}

stse_ReturnCode_t stsafea_session_frame_encrypt( stse_session_t *pSession,
											  stse_frame_t* pFrame,
											  stse_frame_element_t* pEnc_payload_element)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
	stse_frame_element_t* pElement;
	PLAT_UI16 i=0;

	/* - Verify parameters */
	if ((pSession == NULL) ||
		(pFrame == NULL) ||
		(pEnc_payload_element == NULL) ||
		(pEnc_payload_element->length < (pFrame->length-(pFrame->first_element->length+1)) + (16-(pFrame->length%16)))
	)
	{
		return( STSE_CORE_INVALID_PARAMETER );
	}

	/* - Prepare specific STSAFE AES IV */
	if(pSession->context.host.pSTSE->device_type == STSAFE_A120)
	{
		initial_value[0] = UI32_B3(pSession->context.host.MAC_counter+1);
		initial_value[1] = UI32_B2(pSession->context.host.MAC_counter+1);
		initial_value[2] = UI32_B1(pSession->context.host.MAC_counter+1);
		initial_value[3] = UI32_B0(pSession->context.host.MAC_counter+1);
		initial_value[4] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
		initial_value[5] = STSAFEA_AES_PADDING_FIRST_BYTE;
		(void)memset( &initial_value[6], 0x00, ( STSAFEA_HOST_AES_BLOCK_SIZE ) - 6U );
	} else {
		initial_value[0] = UI32_B2(pSession->context.host.MAC_counter+1);
		initial_value[1] = UI32_B1(pSession->context.host.MAC_counter+1);
		initial_value[2] = UI32_B0(pSession->context.host.MAC_counter+1);
		initial_value[3] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
		initial_value[4] = STSAFEA_AES_PADDING_FIRST_BYTE;
		(void)memset( &initial_value[5], 0x00, ( STSAFEA_HOST_AES_BLOCK_SIZE ) - 5U );
	}

	/* - Perform first AES ECB round on IV */
	ret = stse_platform_aes_ecb_enc( initial_value,
		   STSAFEA_HOST_AES_BLOCK_SIZE,
		   pSession->context.host.pHost_cypher_key,
		   (pSession->context.host.key_type == STSE_AES_128_KT)?
				   STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
		   initial_value,
		   NULL );
	if ( ret != STSE_OK )
	{
		return( ret );
	}
	/* - Copy Plain text Frame payload content in Ciphered   */
	pElement = pFrame->first_element->next;
	while (pElement != NULL)
	{
		memcpy ( pEnc_payload_element->pData + i,
				 pElement->pData,
				 pElement->length);

		i += pElement->length;
		pElement = pElement->next;
	}
	/* - Add First padding byte */
	*(pEnc_payload_element->pData + i++) = 0x80;

	/* - Add padding  */
	while(i<pEnc_payload_element->length)
	{
		*(pEnc_payload_element->pData + i++) = 0x00;
	}

	/* - Encrypt pEncFrame content */
	ret = stse_platform_aes_cbc_enc(
			pEnc_payload_element->pData,
			pEnc_payload_element->length,
			initial_value,
			 pSession->context.host.pHost_cypher_key,
			(pSession->context.host.key_type == STSE_AES_128_KT)?
				   STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
			pEnc_payload_element->pData,
			NULL/*pEncFrame->first_element->length*/ );/* TODO : Check that NULL is OK */
	if ( ret != 0 )
	{
		return( STSE_SESSION_ERROR );
	} else {
		return( STSE_OK );
	}
}

static stse_ReturnCode_t stsafea_session_frame_decrypt( stse_session_t *pSession, stse_frame_t* pFrame)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
	stse_frame_element_t* pElement;
	PLAT_UI16 i = 0;

	/*Fill encrypt buffer with encrypted payload content*/
	PLAT_UI8 decrypt_buffer[pFrame->length - pFrame->first_element->length];
	pElement = pFrame->first_element->next;
	while(pElement != NULL)
	{
		memcpy(decrypt_buffer+i,pElement->pData,pElement->length+i);
		i += pElement->length;
		pElement = pElement->next;
	}


	/* - Prepare Plain text info for AES IV  */
	if(pSession->context.host.pSTSE->device_type == STSAFE_A120)
	{
		initial_value[0] = UI32_B3(pSession->context.host.MAC_counter);
		initial_value[1] = UI32_B2(pSession->context.host.MAC_counter);
		initial_value[2] = UI32_B1(pSession->context.host.MAC_counter);
		initial_value[3] = UI32_B0(pSession->context.host.MAC_counter);
		initial_value[4] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
		initial_value[5] = STSAFEA_AES_PADDING_FIRST_BYTE;
		(void)memset( &initial_value[6], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 6U );
	} else {
		initial_value[0] = UI32_B2(pSession->context.host.MAC_counter);
		initial_value[1] = UI32_B1(pSession->context.host.MAC_counter);
		initial_value[2] = UI32_B0(pSession->context.host.MAC_counter);
		initial_value[3] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
		initial_value[4] = STSAFEA_AES_PADDING_FIRST_BYTE;
		(void)memset( &initial_value[5], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 5U );
	}

	ret = stse_platform_aes_ecb_enc( initial_value,
											   STSAFEA_HOST_AES_BLOCK_SIZE,
											   pSession->context.host.pHost_cypher_key,
											   (pSession->context.host.key_type == STSE_AES_128_KT)?
													   STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
											   initial_value,
											   NULL );

	if ( ret != 0 )
	{
		return STSE_CORE_SESSION_ERROR;
	}

	/* - Decrypt pRsp_Frame */
	ret = stse_platform_aes_cbc_dec( decrypt_buffer,
											   pFrame->length - pFrame->first_element->length,
											   initial_value,
											   pSession->context.host.pHost_cypher_key,
											   (pSession->context.host.key_type == STSE_AES_128_KT)?
													   STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
											   decrypt_buffer,
											   NULL );

	/* - Copy Decrypted payload content in un-strapped Frame  */
	stse_frame_unstrap(pFrame);
	pElement = pFrame->first_element->next;
	i=0;
	while (pElement != NULL)
	{
		memcpy ( pElement->pData,
				 decrypt_buffer + i,
				 pElement->length);
		i += pElement->length;
		pElement = pElement->next;
	}

	if ( ret != 0 )
	{
		return ret;
	}

	return ret;
}

static stse_ReturnCode_t stsafea_session_frame_c_mac_compute( stse_session_t *pSession,
											  stse_frame_t *pCmd_frame,
											  PLAT_UI8 *pMAC)
{
	PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
	PLAT_UI8 mac_output_length;
	PLAT_UI8 mac_type = 0x00;
	stse_frame_element_t* pElement;
	PLAT_UI8 aes_block_idx = 0;
	PLAT_UI16 i ;
	PLAT_UI16 cmd_payload_length = pCmd_frame->length - pCmd_frame->first_element->length;
	stse_ReturnCode_t ret = STSE_CORE_INVALID_PARAMETER;

	if((pSession == NULL)
		|| (pCmd_frame == NULL)
		|| (pMAC == NULL))
	{
		return STSE_SERVICE_SESSION_ERROR;
	}

	/*- create C-MAC Frame : [0x00] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] */
	stse_frame_allocate(c_mac_frame);
	stse_frame_element_allocate_push(&c_mac_frame,eMACType,1,&mac_type);
	stse_frame_element_allocate_push(&c_mac_frame,
			eCMD_HEADER,
			pCmd_frame->first_element->length,
			pCmd_frame->first_element->pData);
	stse_frame_element_allocate_push(&c_mac_frame,
			eCmdPayloadLength,
			STSAFEA_CMD_RSP_LEN_SIZE,
			(PLAT_UI8*)&cmd_payload_length);
	stse_frame_element_swap_byte_order(&eCmdPayloadLength);
	eCmdPayloadLength.next = pCmd_frame->first_element->next;
	stse_frame_update(&c_mac_frame);

	/*- Initialize AES C-MAC computation */

	ret = stse_platform_aes_cmac_init( pSession->context.host.pHost_MAC_key,
			(pSession->context.host.key_type == STSE_AES_128_KT)? STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
			STSAFEA_MAC_SIZE);
	if(ret != STSE_OK)
	{
		return ret;
	}

	/*- Perform First AES-CMAC round with MAC subject info */
	if(pSession->context.host.pSTSE->device_type == STSAFE_A120)
	{
		aes_cmac_block[0] = UI32_B3(pSession->context.host.MAC_counter);
		aes_cmac_block[1] = UI32_B2(pSession->context.host.MAC_counter);
		aes_cmac_block[2] = UI32_B1(pSession->context.host.MAC_counter);
		aes_cmac_block[3] = UI32_B0(pSession->context.host.MAC_counter);
		aes_cmac_block[4] = 0x00U; 		/* Subject : Host C-MAC */
		aes_cmac_block[5] = 0x80U;		/* First byte of padding */
		for (i = 6 ; i < STSAFEA_HOST_AES_BLOCK_SIZE ; i++)
		{
			aes_cmac_block[i] = 0x00U; /* 0x00 padding */
		}
	} else {
		aes_cmac_block[0] = UI32_B2(pSession->context.host.MAC_counter);
		aes_cmac_block[1] = UI32_B1(pSession->context.host.MAC_counter);
		aes_cmac_block[2] = UI32_B0(pSession->context.host.MAC_counter);
		aes_cmac_block[3] = 0x00U; 		/* Subject : Host C-MAC */
		aes_cmac_block[4] = 0x80U;		/* First byte of padding */
		for (i = 5 ; i < STSAFEA_HOST_AES_BLOCK_SIZE ; i++)
		{
			aes_cmac_block[i] = 0x00U; /* 0x00 padding */
		}
	}


	ret = stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
	if(ret != STSE_OK)
	{
		return ret;
	}

	pElement = c_mac_frame.first_element;

	/*- Perform additional AES-CMAC round(s) for frame to Authenticate */
	while(pElement != NULL)
	{
		for(i=0;i<pElement->length;i++)
		{
			if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE)
			{
				stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
				aes_block_idx = 0;
			}
			aes_cmac_block[aes_block_idx]= *(pElement->pData+i);
			aes_block_idx++;
		}
		pElement = pElement->next;
	}
	if(aes_block_idx != 0)
	{
		ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
		if(ret != STSE_OK)
		{
			return ret;
		}
	}

	/*- Finish AES MAC computation */
	ret = stse_platform_aes_cmac_compute_finish(aes_cmac_block,&mac_output_length);
	if(ret != STSE_OK)
	{
		return ret;
	} else if ( mac_output_length != STSAFEA_MAC_SIZE )  {
		return STSE_CORE_SESSION_ERROR ;
	}
	memcpy(pMAC, aes_cmac_block, STSAFEA_MAC_SIZE);

	return ret;
}

static stse_ReturnCode_t stsafea_session_frame_r_mac_verify( stse_session_t *pSession,
													stse_frame_t *pCmd_frame,
		   	   	   	   	   	   	   	   	   	   	   	stse_frame_t *pRsp_frame,
													PLAT_UI8* pMAC)
{
	stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
	PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
	PLAT_UI16 cmd_payload_length = pCmd_frame->length - pCmd_frame->first_element->length;
	PLAT_UI8 aes_block_idx = 0;
	PLAT_UI16 i;
	PLAT_UI8 mac_type = 0x80;
	stse_frame_element_t* pElement;

	if((pSession == NULL)
		|| (pCmd_frame == NULL)
		|| (pRsp_frame == NULL))
	{
		return STSE_CORE_SESSION_ERROR;
	}

	if (*(pCmd_frame->first_element->pData) & STSAFEA_PROT_RSP_Msk)
	{


		/*- Pop R-MAC from frame*/
		stse_frame_pop_element(pRsp_frame);
		stse_frame_pop_element(pRsp_frame);
		PLAT_UI16 rsp_payload_length = (pRsp_frame->length - (pRsp_frame->first_element->length));

		/*- Initialize AES C-MAC computation */

		stse_platform_aes_cmac_init(
				pSession->context.host.pHost_MAC_key,
				(pSession->context.host.key_type == STSE_AES_128_KT)?
						STSE_AES_128_KEY_SIZE:STSE_AES_256_KEY_SIZE,
				STSAFEA_MAC_SIZE);

		/*- Perform First AES-CMAC round */
		if(pSession->context.host.pSTSE->device_type == STSAFE_A120)
		{
			aes_cmac_block[0] = UI32_B3(pSession->context.host.MAC_counter);
			aes_cmac_block[1] = UI32_B2(pSession->context.host.MAC_counter);
			aes_cmac_block[2] = UI32_B1(pSession->context.host.MAC_counter);
			aes_cmac_block[3] = UI32_B0(pSession->context.host.MAC_counter);
			aes_cmac_block[4] = STSAFEA_AES_SUBJECT_HOST_RMAC;
			aes_cmac_block[5] = 0x80U;
			for (i = 6 ; i < STSAFEA_HOST_AES_BLOCK_SIZE ; i++)
			{
				aes_cmac_block[i] = 0x00U; /* 0x00 padding */
			}
		} else {
			aes_cmac_block[0] = UI32_B2(pSession->context.host.MAC_counter);
			aes_cmac_block[1] = UI32_B1(pSession->context.host.MAC_counter);
			aes_cmac_block[2] = UI32_B0(pSession->context.host.MAC_counter);
			aes_cmac_block[3] = STSAFEA_AES_SUBJECT_HOST_RMAC;
			aes_cmac_block[4] = 0x80U;
			for (i = 5 ; i < STSAFEA_HOST_AES_BLOCK_SIZE ; i++)
			{
				aes_cmac_block[i] = 0x00U; /* 0x00 padding */
			}
		}

		stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);

		/*- Prepare AES CMAC input for response MAC verification  */

		stse_frame_allocate(r_mac_frame);

		/*- Create r_mac_frame head :[MAC TYPE] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] ... */

		stse_frame_element_allocate_push(
			&r_mac_frame,
			eMACType,
			1,
			&mac_type);

		stse_frame_element_allocate_push(
						&r_mac_frame,
						eCMD_header,
						pCmd_frame->first_element->length,
						pCmd_frame->first_element->pData);

		stse_frame_element_allocate_push(
				&r_mac_frame,
				eCMD_Length,
				STSAFEA_CMD_RSP_LEN_SIZE,
				(PLAT_UI8*)&cmd_payload_length);
		stse_frame_element_swap_byte_order(&eCMD_Length);

		eCMD_Length.next = pCmd_frame->first_element->next;

		stse_frame_update(&r_mac_frame);

		/*- Create r_mac_frame head : ... [RSP HEADER] [RSP PAYLOAD LENGTH] [RSP PAYLOAD] */

		stse_frame_element_allocate_push(
				&r_mac_frame,
				eRSP_header,
				pRsp_frame->first_element->length,
				pRsp_frame->first_element->pData);

		stse_frame_element_allocate_push(
				&r_mac_frame,
				eRsp_Length,
				STSAFEA_CMD_RSP_LEN_SIZE,
				(PLAT_UI8*)&rsp_payload_length);
		stse_frame_element_swap_byte_order(&eRsp_Length);

		eRsp_Length.next = pRsp_frame->first_element->next;

		/*- Perform additional AES-CMAC round(s) on R-MAC frame*/

		pElement = r_mac_frame.first_element;
		while(pElement != NULL)
		{
			for(i=0;i<pElement->length;i++)
			{
				if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE)
				{
					stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
					aes_block_idx = 0;
				}
				aes_cmac_block[aes_block_idx]= *(pElement->pData+i);
				aes_block_idx++;
			}
			pElement = pElement->next;
		}
		if(aes_block_idx != 0)
		{
			ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
			if(ret != STSE_OK)
			{
				return ret;
			}
		}

		memcpy(aes_cmac_block, pMAC, STSAFEA_MAC_SIZE);
		ret = stse_platform_aes_cmac_verify_finish(aes_cmac_block);

	}
	return ret;
}

stse_ReturnCode_t stsafea_session_encrypted_transfer ( stse_session_t *pSession,
		stse_frame_t *pCmdFrame,
		stse_frame_t *pRspFrame,
		PLAT_UI8 cmd_encryption_flag,
		PLAT_UI8 rsp_encryption_flag,
		stse_cmd_access_conditions_t cmd_ac_info,
		PLAT_UI16 processing_time
)
{
	stse_ReturnCode_t ret;
	PLAT_UI16 encrypted_cmd_payload_size = 0;
	PLAT_UI16 encrypted_rsp_payload_size = 0;
	PLAT_UI8 padding = 0;

	if(cmd_encryption_flag == 1)
	{
		PLAT_UI16 encrypted_payload_length = (pCmdFrame->length - pCmdFrame->first_element->length)+1;

		if ((encrypted_payload_length % 16)!=0)
		{
			padding = 16 - (encrypted_payload_length % 16);
		}
		encrypted_cmd_payload_size = pCmdFrame->length + padding;
	}

	PLAT_UI8 encrypted_cmd_payload[encrypted_cmd_payload_size];
	stse_frame_element_allocate(eEncrypted_cmd_payload,encrypted_cmd_payload_size,encrypted_cmd_payload);
	stse_frame_strap_allocate(S1);

	if(cmd_encryption_flag == 1)
	{
		ret = stsafea_session_frame_encrypt(pSession, pCmdFrame, &eEncrypted_cmd_payload);
		if(ret != STSE_OK)
		{
			return ret;
		}
		stse_frame_insert_strap(&S1, pCmdFrame->first_element, &eEncrypted_cmd_payload);
		stse_frame_update(pCmdFrame);
	}


	if(rsp_encryption_flag == 1)
	{

		if (( pRspFrame->length % 16)!=0)
		{
			padding = 16 - ( pRspFrame->length % 16);
		}
		encrypted_rsp_payload_size = pRspFrame->length + padding;
	}

	PLAT_UI8 encrypted_rsp_payload[encrypted_rsp_payload_size];
	stse_frame_element_allocate(eEncrypted_rsp_payload,encrypted_rsp_payload_size,encrypted_rsp_payload);
	stse_frame_strap_allocate(S2);

	if(rsp_encryption_flag == 1)
	{
		stse_frame_insert_strap(&S2, pRspFrame->first_element, &eEncrypted_rsp_payload);
		stse_frame_update(pRspFrame);
	}


	ret = stsafea_session_authenticated_transfer(pSession,
			pCmdFrame,
			pRspFrame,
			cmd_ac_info,
			processing_time);



	if((ret == STSE_OK) && (rsp_encryption_flag == 1))
	{
		ret = stsafea_session_frame_decrypt(pSession, pRspFrame);
	}

	return ret;
}

stse_ReturnCode_t stsafea_session_authenticated_transfer ( stse_session_t *pSession,
		stse_frame_t *pCmdFrame,
		stse_frame_t *pRspFrame,
		stse_cmd_access_conditions_t cmd_ac_info,
		PLAT_UI16 processing_time
)
{
	(void)cmd_ac_info;
	stse_ReturnCode_t ret;
	PLAT_UI8 Cmd_MAC[STSAFEA_MAC_SIZE];
	PLAT_UI8 Rsp_MAC[STSAFEA_MAC_SIZE];

	if(pSession->type == STSE_HOST_SESSION)
	{
		*(pCmdFrame->first_element->pData) |= (1<<5);
	}

	*(pCmdFrame->first_element->pData) |= ((1<<7)|(1<<6));

	stse_frame_element_allocate_push(pRspFrame,eRspMAC,STSAFEA_MAC_SIZE,Rsp_MAC);

	ret = stsafea_session_frame_c_mac_compute(pSession, pCmdFrame, Cmd_MAC);
	if (ret != STSE_OK)
	{
		return ret;
	}

	stse_frame_element_allocate_push(pCmdFrame,eCmdMAC,STSAFEA_MAC_SIZE,Cmd_MAC);

	switch(pSession->type)
	{

	case STSE_HOST_SESSION :
		ret = stse_frame_transfer(pSession->context.host.pSTSE, pCmdFrame, pRspFrame, processing_time);
		if(ret == STSE_OK)
		{
				pSession->context.host.MAC_counter ++;
		}
		break;

	default :
		ret = STSE_CORE_SESSION_ERROR;
		break;

	}

	/*- Pop C-MAC from frame*/
	stse_frame_pop_element(pCmdFrame);


	if(ret == STSE_OK)
	{
		ret = stsafea_session_frame_r_mac_verify(pSession, pCmdFrame, pRspFrame, Rsp_MAC);
	}

	return ret;
}

