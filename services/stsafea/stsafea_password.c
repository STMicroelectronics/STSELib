/*!
 ******************************************************************************
 * \file	stsafea_password.c
 * \brief   STSAFE-A services for password (source)
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
#include "services/stsafea/stsafea_password.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_verify_password(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_password_buffer,
    PLAT_UI8 password_length,
    PLAT_UI8 *p_verification_status,
    PLAT_UI8 *p_remaining_tries) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_PASSWORD;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((password_length != STSAFEA_PASSWORD_LENGTH)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_password, password_length, p_password_buffer);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_ver_stat, 1, p_verification_status);
    stse_frame_element_allocate_push(&rsp_frame, e_rem_tri, 1, p_remaining_tries);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_delete_password(stse_handle_t *p_stse) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_DELETE;
    PLAT_UI8 tag = STSAFEA_DELETE_TAG_PASSWORD;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &tag);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
