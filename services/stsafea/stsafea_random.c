/*!
 ******************************************************************************
 * \file	stsafea_random.c
 * \brief   STSAFE-A services for random (source)
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
//#include <string.h>

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_random.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_generate_random(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_random,
    PLAT_UI8 random_size) {
    stse_return_code_t ret;
    PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_RANDOM;
    PLAT_UI8 subject = 0x00;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if ((p_random == NULL) || (random_size == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject, 1, &subject);
    stse_frame_element_allocate_push(&cmd_frame, e_size, 1, &random_size);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_random, random_size, p_random);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(p_stse,
                                 &cmd_frame,
                                 &rsp_frame);

    return (ret);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
