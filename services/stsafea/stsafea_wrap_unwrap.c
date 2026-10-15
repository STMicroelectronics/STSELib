/*!
 ******************************************************************************
 * \file	stsafea_wrap_unwrap.c
 * \brief   STSAFE-A services for wrap & unwrap (source)
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_wrap_unwrap.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_wrap_payload(stse_handle_t *p_stse,
                                       PLAT_UI8 wrap_key_slot,
                                       PLAT_UI8 *p_payload,
                                       PLAT_UI16 payload_size,
                                       PLAT_UI8 *p_wrapped_payload,
                                       PLAT_UI16 wrapped_payload_size) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_WRAP_LOCAL_ENVELOPE;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((p_payload == NULL) || (p_wrapped_payload == NULL) || (payload_size > 480) || (payload_size == 0) || (wrapped_payload_size != (payload_size + 8))) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &wrap_key_slot);
    stse_frame_element_allocate_push(&cmd_frame, e_payload, payload_size, p_payload);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_wrapped, wrapped_payload_size, p_wrapped_payload);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_unwrap_payload(stse_handle_t *p_stse,
                                         PLAT_UI8 wrap_key_slot,
                                         PLAT_UI8 *p_wrapped_payload,
                                         PLAT_UI16 wrapped_payload_size,
                                         PLAT_UI8 *p_payload,
                                         PLAT_UI16 payload_size) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((p_payload == NULL) || (p_wrapped_payload == NULL) || (wrapped_payload_size > 488) || (wrapped_payload_size < 8) || (wrapped_payload_size != (payload_size + 8))) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &wrap_key_slot);
    stse_frame_element_allocate_push(&cmd_frame, e_payload, wrapped_payload_size, p_wrapped_payload);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_wrapped, payload_size, p_payload);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
