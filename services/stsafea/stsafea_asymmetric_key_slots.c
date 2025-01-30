/*!
 ******************************************************************************
 * \file	stsafea_asymmetric_key_slots.c
 * \brief   STSAFEA services for symmetric key slots management (source)
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

#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "core/stse_frame.h"

stse_ReturnCode_t stsafea_query_private_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pPrivate_key_slot_count)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pPrivate_key_slot_count == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_TABLE;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,ePrivate_key_slot_count,1,pPrivate_key_slot_count);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_QUERY]
	);

	return ret;
}

stse_ReturnCode_t stsafea_query_private_key_table(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_count,
		PLAT_UI8 * pChange_right,
		PLAT_UI16 * pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_table_info)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(private_key_table_info == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 slot_count, i;
	PLAT_UI8 * current_record;

	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_TABLE;
	PLAT_UI8 rsp_header;
	PLAT_UI16 raw_table_length = private_key_slot_count * sizeof(stsafea_private_key_slot_information_t);
	PLAT_UI8 pTable_raw[raw_table_length];

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eSlot_count,1,&slot_count);
	stse_frame_element_allocate_push(&RspFrame,eChange_right,1,pChange_right);
	stse_frame_element_allocate_push(&RspFrame,eGlobal_usage_limit,2,(PLAT_UI8*)pGlobal_usage_limit);
	stse_frame_element_allocate_push(&RspFrame,eTable_raw,raw_table_length,pTable_raw);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_QUERY]
	);

	if (ret != STSE_OK)
	{
		return ret;
	}

	stse_frame_element_swap_byte_order(&eGlobal_usage_limit);

	if(private_key_slot_count > slot_count)
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	current_record = pTable_raw;

	for(i=0; i < slot_count; i++)
	{
		memset(&private_key_table_info[i], 0, sizeof(stsafea_private_key_slot_information_t));
		memcpy(&private_key_table_info[i],
			   current_record,
			   STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH);
		current_record += STSAFEA_PRIVATE_KEY_TABLE_INFO_COMMON_VALUES_LENGTH;
		if(private_key_table_info[i].presence_flag == 1)
		{
			if(ARRAY_2B_SWAP_TO_UI16(current_record) > STSE_ECC_CURVE_ID_VALUE_MAX_SIZE)
			{
				return STSE_UNEXPECTED_ERROR;
			}
			memcpy(&private_key_table_info[i].curve_id,
				   current_record,
				   ARRAY_2B_SWAP_TO_UI16(current_record)+STSAFEA_GENERIC_LENGTH_SIZE);
			current_record += ARRAY_2B_SWAP_TO_UI16(current_record)+STSAFEA_GENERIC_LENGTH_SIZE;
		}
	}

	return ret;
}

stse_ReturnCode_t stsafea_generate_ecc_key_pair(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI16 usage_limit,
		PLAT_UI8 * pPublic_key)
{
	stse_ReturnCode_t ret;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if ((pPublic_key == NULL) || (key_type >= STSE_ECC_KT_INVALID))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_KEY;
	PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_PRIVATE_KEY_SLOT;
	PLAT_UI8 pFiller[2] = {0};

	PLAT_UI8 rsp_header;
	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
			UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eAttribute_tag,1,&attribute_tag);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,STSAFEA_SLOT_NUMBER_ID_SIZE,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame,eUsage_limit,2,(PLAT_UI8*)&usage_limit);
	stse_frame_element_swap_byte_order(&eUsage_limit);
	stse_frame_element_allocate_push(&CmdFrame,eFiller,2,pFiller);
	stse_frame_element_allocate_push(&CmdFrame,eCurve_id,
			stse_ecc_info_table[key_type].curve_id_total_length,
			(PLAT_UI8*)&stse_ecc_info_table[key_type].curve_id);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

#ifdef STSE_CONF_ECC_EDWARD_25519
	if(key_type == STSE_ECC_KT_ED25519)
	{
		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
	}
	else
#endif
	{
		stse_frame_push_element(&RspFrame, &ePoint_representation_id);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
	}

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_GENERATE_KEY]
	);
	return ret;
}

stse_ReturnCode_t stsafea_generate_ECDHE_key_pair(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 * pPublic_key)
{
	stse_ReturnCode_t ret;
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if (key_type >= STSE_ECC_KT_INVALID)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_header_extended = STSAFEA_EXTENDED_CMD_GENERATE_ECDHE;

	PLAT_UI8 rsp_header;
	PLAT_UI8 pCurve_id_rsp[stse_ecc_info_table[key_type].curve_id_total_length];
	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
			UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header_extended,STSAFEA_HEADER_SIZE,&cmd_header_extended);
	stse_frame_element_allocate_push(&CmdFrame,eCurve_id_cmd,
			stse_ecc_info_table[key_type].curve_id_total_length,
			(PLAT_UI8*)&stse_ecc_info_table[key_type].curve_id);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eCurve_id_rsp,
			stse_ecc_info_table[key_type].curve_id_total_length,
			pCurve_id_rsp);

#ifdef STSE_CONF_ECC_CURVE_25519
	if(key_type == STSE_ECC_KT_CURVE25519)
	{
		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
	}
	else
#endif
	{
		stse_frame_push_element(&RspFrame, &ePoint_representation_id);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
	}


	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_GENERATE_ECDHE]
	);


	return ret;
}

stse_ReturnCode_t stsafea_sign_for_generic_public_key_slot(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t private_key_type,
		PLAT_UI8 *pPrivate_key,
		stse_hash_algorithm_t hash_algo,
		PLAT_UI16 payload_length,
		PLAT_UI8 *pPayload,
		PLAT_UI8 *pSignature)
{
	(void)pSTSE;
	stse_ReturnCode_t ret;
	PLAT_UI8 tbs_data[payload_length];
	PLAT_UI32 hash_length = stsafea_hash_info_table[hash_algo].length;

#ifdef STSE_CONF_ECC_EDWARD_25519
	if(private_key_type == STSE_ECC_KT_ED25519)
	{
		memcpy(tbs_data, pPayload, payload_length);
	}
	else
#endif
	{
		/* - Hash the payload */
		ret = stse_platform_hash_compute(
				hash_algo,
				pPayload, payload_length,
				tbs_data, &hash_length);

		if(ret != STSE_OK)
		{
			return( ret );
		}
	}

	/* - Sign the hash of concatenation of pub keys */
	ret = stse_platform_ecc_sign(
			private_key_type,
			pPrivate_key,
			tbs_data,
#ifdef STSE_CONF_ECC_EDWARD_25519
			(private_key_type == STSE_ECC_KT_ED25519) ? payload_length : hash_length,
#else
			hash_length,
#endif
			pSignature);

	return( ret );
}
