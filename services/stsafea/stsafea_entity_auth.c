/*!
 ******************************************************************************
 * \file	stsafea_entity_auth.c
 * \brief   Entity authentication services for STSAFE-A (source)
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

#include "services/stsafea/stsafea_entity_auth.h"


stse_ReturnCode_t stsafea_generate_challenge(
		stse_Handler_t * pSTSE ,
		PLAT_UI8 challenge_size,
		PLAT_UI8 *pChallenge)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 ext_cmd_header = STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE;
	PLAT_UI8 rsp_header;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}

	if((pChallenge == NULL)||(challenge_size < STSE_EDDSA_CHALLENGE_SIZE))
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}

	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eExt_cmd_header,STSAFEA_HEADER_SIZE,&ext_cmd_header);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eChallenge,STSE_EDDSA_CHALLENGE_SIZE,pChallenge);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE]);

	return( ret );
}

stse_ReturnCode_t stsafea_verify_entity_signature(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pSignature,
		PLAT_UI8 *pSignature_validity)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pSignature == NULL
	|| pSignature_validity == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_header_extended = STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE;

	PLAT_UI8 filler = 0x00;
	stse_frame_element_allocate(eFiller, 1, &filler);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header_extended,STSAFEA_HEADER_SIZE,&cmd_header_extended);
	stse_frame_push_element(&CmdFrame, &eFiller);
	stse_frame_element_allocate_push(&CmdFrame,eSlot_number,STSAFEA_SLOT_NUMBER_ID_SIZE,&slot_number);

	/* Signature elements */
	PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stse_ecc_info_table[key_type].signature_size>>1),
			UI16_B0(stse_ecc_info_table[key_type].signature_size>>1),
	};

	stse_frame_element_allocate_push(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R, (stse_ecc_info_table[key_type].signature_size>>1), pSignature);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size>>1), pSignature + (stse_ecc_info_table[key_type].signature_size>>1));

	PLAT_UI8 rsp_header;
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eSignature_validity,1,pSignature_validity);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE]
	);

	if(ret != STSE_OK)
	{
		*pSignature_validity = STSAFEA_FALSE;
	}

	return( ret );
}
