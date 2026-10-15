/*!
 ******************************************************************************
 * \file	stsafea_put_query.c
 * \brief   STSAFE-A services for put and query (source)
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
#include "services/stsafea/stsafea_put_query.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_SUB_TAG_MASK_ID 0x01
#define STSAFEA_SUB_TAG_ST_NUMBER 0x02

stse_return_code_t stsafea_put_life_cyle_state(
    stse_handle_t *p_stse,
    stsafea_life_cycle_state_t life_cycle_state) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_LIFE_CYCLE_STATE;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &tag);
    stse_frame_element_allocate_push(&cmd_frame, e_life_cycle_state, 1, (PLAT_UI8 *)&life_cycle_state);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_life_cycle_state(
    stse_handle_t *p_stse,
    stsafea_life_cycle_state_t *p_life_cycle_state) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_LIFE_CYCLE_STATE;
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
    stse_frame_element_allocate_push(&rsp_frame, e_life_cycle_state, 1, (PLAT_UI8 *)p_life_cycle_state);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_put_i2c_parameters(
    stse_handle_t *p_stse,
    stsafea_i2c_parameters_t *p_i2c_parameters) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_I2C_PARAMETERS;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    if (p_stse->device_type >= STSAFE_L010) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
#endif

    if (p_stse->device_type == STSAFE_A100 ||
        p_stse->device_type == STSAFE_A110) {
        p_i2c_parameters->idle_bus_time_to_standby = 0;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &tag);
    stse_frame_element_allocate_push(&cmd_frame, e_i2c_parameters, sizeof(stsafea_i2c_parameters_t), (PLAT_UI8 *)p_i2c_parameters);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_i2c_parameters(
    stse_handle_t *p_stse,
    stsafea_i2c_parameters_t *p_i2c_parameters) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_I2C_PARAMETERS;
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
    stse_frame_element_allocate_push(&rsp_frame, e_life_cycle_state, sizeof(stsafea_i2c_parameters_t), (PLAT_UI8 *)p_i2c_parameters);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_mask_id_st_number(
    stse_handle_t *p_stse,
    PLAT_UI8 mask_id[STSAFEA_MASK_ID_SIZE],
    PLAT_UI8 st_number[STSAFEA_ST_NUMBER_SIZE]) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_PRODUCT_DATA;
    PLAT_UI8 rsp_header;
    PLAT_UI8 sub_tag_mask_id, sub_tag_mask_id_size;
    PLAT_UI8 sub_tag_st_number, sub_tag_st_number_size;
    stse_return_code_t ret;

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
    stse_frame_element_allocate_push(&rsp_frame, e_sub_tag_mask_id, 1, &sub_tag_mask_id);
    stse_frame_element_allocate_push(&rsp_frame, e_sub_tag_mask_id_size, 1, &sub_tag_mask_id_size);
    stse_frame_element_allocate_push(&rsp_frame, e_mask_id, STSAFEA_MASK_ID_SIZE, mask_id);
    stse_frame_element_allocate_push(&rsp_frame, e_sub_tag_st_number, 1, &sub_tag_st_number);
    stse_frame_element_allocate_push(&rsp_frame, e_sub_tag_st_number_size, 1, &sub_tag_st_number_size);
    stse_frame_element_allocate_push(&rsp_frame, e_st_number, STSAFEA_ST_NUMBER_SIZE, st_number);

    /*- Perform Transfer*/
    ret = stsafea_frame_raw_transfer(p_stse,
                                     &cmd_frame,
                                     &rsp_frame,
                                     stsafea_cmd_timings[p_stse->device_type][cmd_header]);

    if ((sub_tag_mask_id != STSAFEA_SUB_TAG_MASK_ID) || (sub_tag_mask_id_size != STSAFEA_MASK_ID_SIZE) ||
        (sub_tag_st_number != STSAFEA_SUB_TAG_ST_NUMBER) || (sub_tag_st_number_size != STSAFEA_ST_NUMBER_SIZE)) {
        return (STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE);
    }

    return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
