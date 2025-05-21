/*!
 ******************************************************************************
 * \file	stsafea_mac.c
 * \brief   STSAFE Middleware services for Message Authentication Code - MAC
 *(source) \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 *****************************************************************************/

#include "services/stsafea/stsafea_mac.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_cmac_hmac_compute(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
					    PLAT_UI8 *pMessage, PLAT_UI8 message_length,
					    PLAT_UI8 *pMac, PLAT_UI8 mac_length)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_MAC;
	PLAT_UI8 sub_command_distinguisher = 0x03;
	PLAT_UI8 rsp_header;

	/* - Check stsafe handler initialization */

	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if ((pMac == NULL) || (pMessage == NULL)) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MAC LENGTH]
	 * [MESSAGE] */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSub_command_distinguisher, 1,
					 &sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, 1, &slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eMac_length, 1, &mac_length);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length, pMessage);

	/* - Prepare RSP Frame : [HEADER] [MAC] */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eMac, mac_length, pMac);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_cmac_hmac_verify(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
					   PLAT_UI8 *pMac, PLAT_UI8 mac_length, PLAT_UI8 *pMessage,
					   PLAT_UI8 message_length, PLAT_UI8 *pVerification_result)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_MAC;
	PLAT_UI8 sub_command_distinguisher = 0x02;
	PLAT_UI8 rsp_header;

	/* - Check stsafe handler initialization */

	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if ((pMac == NULL) || (pMessage == NULL) || (pVerification_result == NULL)) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	/* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MAC LENGTH]
	 * [MESSAGE] */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSub_command_distinguisher, 1,
					 &sub_command_distinguisher);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, 1, &slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eMac_length, 1, &mac_length);
	stse_frame_element_allocate_push(&CmdFrame, eMac, mac_length, pMac);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length, pMessage);

	/* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] */
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eVerification_result, 1, pVerification_result);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gmac_compute(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
					   PLAT_UI16 IV_length, PLAT_UI8 *pIV,
					   PLAT_UI16 associated_data_length,
					   PLAT_UI8 *pAssociated_data,
					   PLAT_UI8 authentication_tag_length,
					   PLAT_UI8 *pAuthentication_tag)
{
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	return (stsafea_aes_gcm_encrypt(pSTSE, slot_number, authentication_tag_length, IV_length,
					pIV, associated_data_length, pAssociated_data, 0, NULL,
					NULL, pAuthentication_tag));
}

stse_ReturnCode_t
stsafea_aes_gmac_verify(stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI16 IV_length,
			PLAT_UI8 *pIV, PLAT_UI16 associated_data_length, PLAT_UI8 *pAssociated_data,
			PLAT_UI8 authentication_tag_length, PLAT_UI8 *pAuthentication_tag,
			PLAT_UI8 *pVerification_result)
{
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	return (stsafea_aes_gcm_decrypt(pSTSE, slot_number, authentication_tag_length, IV_length,
					pIV, associated_data_length, pAssociated_data, 0, NULL,
					pAuthentication_tag, pVerification_result, NULL));
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
