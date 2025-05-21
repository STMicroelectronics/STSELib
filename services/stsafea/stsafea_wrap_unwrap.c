/*!
 ******************************************************************************
 * \file	stsafea_wrap_unwrap.c
 * \brief   Wrap & unwrap services for STSAFE-A
 * \author  STMicroelectronics - CS application team
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
 ******************************************************************************
 */

#include "services/stsafea/stsafea_wrap_unwrap.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_wrap_payload(stse_Handler_t *pSTSE, PLAT_UI8 wrap_key_slot,
				       PLAT_UI8 *pPayload, PLAT_UI16 payload_size,
				       PLAT_UI8 *pWrapped_Payload, PLAT_UI16 wrapped_payload_size)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_WRAP_LOCAL_ENVELOPE;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if ((pPayload == NULL) || (pWrapped_Payload == NULL) || (payload_size > 480) ||
	    (payload_size == 0) || (wrapped_payload_size != (payload_size + 8))) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, 1, &wrap_key_slot);
	stse_frame_element_allocate_push(&CmdFrame, ePayload, payload_size, pPayload);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eWrapped, wrapped_payload_size,
					 pWrapped_Payload);

	/*- Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_unwrap_payload(stse_Handler_t *pSTSE, PLAT_UI8 wrap_key_slot,
					 PLAT_UI8 *pWrapped_Payload, PLAT_UI16 wrapped_payload_size,
					 PLAT_UI8 *pPayload, PLAT_UI16 payload_size)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if ((pPayload == NULL) || (pWrapped_Payload == NULL) || (wrapped_payload_size > 488) ||
	    (wrapped_payload_size < 8) || (wrapped_payload_size != (payload_size + 8))) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, 1, &wrap_key_slot);
	stse_frame_element_allocate_push(&CmdFrame, ePayload, wrapped_payload_size,
					 pWrapped_Payload);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eWrapped, payload_size, pPayload);

	/*- Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
