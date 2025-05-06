/*!
 ******************************************************************************
 * \file	stsafel_ecc.h
 * \brief   Elliptic Curves Cryptography (ECC) services for STSAFE-L
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

#include "services/stsafel/stsafel_ecc.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_frame.h"


stse_ReturnCode_t stsafel_ecc_generate_signature(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pChallenge,
		PLAT_UI16 challenge_length,
		PLAT_UI8 *pSignature)
{
	PLAT_UI8 cmd_header = STSAFEL_CMD_GENERATE_SIGNATURE;
	PLAT_UI8 rsp_header;
	PLAT_UI8 internal_data_subject = STSAFEL_ECC_SIGNATURE_SUBJECT_NONE;

	if(pSTSE == NULL)
	{
		return STSE_SERVICE_HANDLER_NOT_INITIALISED;
	}
#ifdef STSE_CONF_ECC_EDWARD_25519
	if((key_type != STSE_ECC_KT_ED25519) ||
	   (challenge_length != STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH) ||
	   (pChallenge == NULL) ||
	   (pSignature == NULL))
	{
		return STSE_SERVICE_INVALID_PARAMETER;
	}
#endif
	/*- Create CMD frame and populate elements */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eChallenge,challenge_length,pChallenge);
	stse_frame_element_allocate_push(&CmdFrame,eInternal_data_subject,1,&internal_data_subject);

	/*- Create Rsp frame and populate elements*/
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eSignature,stse_ecc_info_table[key_type].signature_size,pSignature);

	/*- Perform Transfer*/
	return stsafel_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame
			);
}
