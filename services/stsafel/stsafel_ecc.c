/*!
 ******************************************************************************
 * \file	stsafel_ecc.h
 * \brief   STSAFE-L services for ECC (source)
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_ecc.h"
#include "services/stsafel/stsafel_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_return_code_t stsafel_ecc_generate_signature(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t key_type,
    const PLAT_UI8 *p_challenge,
    PLAT_UI16 challenge_length,
    PLAT_UI8 *p_signature) {
    PLAT_UI8 cmd_header = STSAFEL_CMD_GENERATE_SIGNATURE;
    PLAT_UI8 rsp_header;
    PLAT_UI8 internal_data_subject = STSAFEL_ECC_SIGNATURE_SUBJECT_NONE;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }
    if ((challenge_length != STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH) ||
        (p_challenge == NULL) ||
        (p_signature == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_challenge, challenge_length, (PLAT_UI8 *)p_challenge);
    stse_frame_element_allocate_push(&cmd_frame, e_internal_data_subject, 1, &internal_data_subject);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_signature, stse_ecc_info_table[key_type].signature_size, p_signature);

    /*- Perform Transfer*/
    return stsafel_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
