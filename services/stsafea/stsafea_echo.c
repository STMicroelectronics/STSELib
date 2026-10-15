/*!
 ******************************************************************************
 * \file	stsafea_echo.c
 * \brief   STSAFE-A services for echo (source)
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

#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_echo(stse_handle_t *p_stse,
                               PLAT_UI8 *p_message,
                               PLAT_UI8 *p_echoed_message,
                               PLAT_UI16 message_length) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_ECHO;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if (p_message == NULL || p_echoed_message == NULL || message_length == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_message, message_length, p_message);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_echoed_message, message_length, p_echoed_message);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
