/*!
 ******************************************************************************
 * \file	stsafea_host_key_slot.c
 * \brief   STSAFEA host key slot management services (source)
 * \author  STMicroelectronics - SMD application team
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

/* Includes ------------------------------------------------------------------*/

#include "services/stsafea/stsafea_host_key_slot.h"

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stsafea_query_host_key_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pCtrl_fields == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eCtrl_fields,sizeof(stsafea_host_key_provisioning_ctrl_fields_t),(PLAT_UI8*)pCtrl_fields);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_QUERY]
	);

	return ret;
}

stse_ReturnCode_t stsafea_put_host_key_provisioning_ctrl_fields(
		stse_Handler_t * pSTSE,
		stsafea_host_key_provisioning_ctrl_fields_t *pCtrl_fields)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pCtrl_fields == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);
	stse_frame_element_allocate_push(&CmdFrame,eCtrl_fields,sizeof(stsafea_host_key_provisioning_ctrl_fields_t),(PLAT_UI8*)pCtrl_fields);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_PUT_ATTRIBUTE]
	);
	return( ret );
}

stse_ReturnCode_t stsafea_query_host_key(
		stse_Handler_t * pSTSE ,
		stsafea_host_key_slot_t *pHost_key_slot
)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pHost_key_slot == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eHost_key_slot,sizeof(stsafea_host_key_slot_t),(PLAT_UI8*)pHost_key_slot);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_QUERY]
	);

	return ret;
}

stse_ReturnCode_t stsafea_query_host_key_v2(
		stse_Handler_t * pSTSE,
		stsafea_host_key_slot_v2_t *pHost_key_slot_v2)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pHost_key_slot_v2 == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V2;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eHost_key_slot,sizeof(stsafea_host_key_slot_v2_t),(PLAT_UI8*)pHost_key_slot_v2);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_QUERY]
	);

	return ret;
}

stse_ReturnCode_t stsafea_put_attribute_host_key(
		stse_Handler_t * pSTSE ,
		stsafea_aes_128_host_keys_t* host_keys)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(host_keys == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);
	stse_frame_element_allocate_push(&CmdFrame,eHost_keys,sizeof(stsafea_aes_128_host_keys_t),(PLAT_UI8*)host_keys);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_PUT_ATTRIBUTE]
	);
	return( ret );
}

stse_ReturnCode_t stsafea_host_key_provisioning (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t key_type,
		stsafea_host_keys_t* host_keys)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(host_keys == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 host_keys_length = (key_type == STSAFEA_AES_128_HOST_KEY ? STSAFEA_HOST_AES_128_KEYS_SIZE : STSAFEA_HOST_AES_256_KEYS_SIZE);

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 ext_cmd_header = STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT;
	PLAT_UI8 pPadding[3] = {0};
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eExt_cmd_header,STSAFEA_HEADER_SIZE,&ext_cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,ePadding,3,pPadding);
	stse_frame_element_allocate_push(&CmdFrame,eKey_type,1,(PLAT_UI8*)&key_type);
	stse_frame_element_allocate_push(&CmdFrame,eHost_keys,host_keys_length,(PLAT_UI8*)host_keys);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT]
	);
	return( ret );
}

stse_ReturnCode_t stsafea_host_key_provisioning_wrapped (
		stse_Handler_t * pSTSE ,
		stsafea_host_key_type_t key_type,
		PLAT_UI8* pHost_key_envelope)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pHost_key_envelope == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 host_keys_envelope_length = key_type == STSAFEA_AES_128_HOST_KEY ? STSAFEA_HOST_AES_128_KEYS_ENVELOPE_SIZE : STSAFEA_HOST_AES_256_KEYS_ENVELOPE_SIZE;

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 ext_cmd_header = STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eExt_cmd_header,STSAFEA_HEADER_SIZE,&ext_cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eHost_keys,host_keys_envelope_length,(PLAT_UI8*)pHost_key_envelope);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED]
	);

	return( ret );
}

stse_ReturnCode_t stsafea_establish_host_key (
		stse_Handler_t *pSTSE ,
		stse_ecc_key_type_t host_ecdh_public_key_type,
		PLAT_UI8 * pPublic_key,
		stsafea_host_key_type_t host_keys_type)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_header_extended = STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2;

	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
			UI16_B0(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, pPublic_key);
	stse_frame_element_allocate(ePublic_key_second_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, NULL);

	/* command frame */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header_extended, STSAFEA_HEADER_SIZE, &cmd_header_extended);

	stse_frame_element_allocate_push(&CmdFrame, eCurve_id, stse_ecc_info_table[host_ecdh_public_key_type].curve_id_total_length, (PLAT_UI8*)&stse_ecc_info_table[host_ecdh_public_key_type].curve_id);

	if(host_ecdh_public_key_type == STSE_ECC_KT_CURVE25519)
	{
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	}
	else
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	PLAT_UI8 algorithm_id = 0x03;
	stse_frame_element_allocate_push(&CmdFrame, eAlgorithm_id, 1, &algorithm_id);

	stse_frame_element_allocate_push(&CmdFrame, eHost_keys_type, 1, &host_keys_type);

	/* response frame */
	PLAT_UI8 rsp_header;
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2]
	);

	return( ret );
}

