/*!
 ******************************************************************************
 * \file    stsafea_commands.c
 * \brief   STSAFE-A command services (source)
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

#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_timings.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_get_command_authorization_records_count(stse_handle_t *p_stse, PLAT_UI8 *p_command_count) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
    PLAT_UI8 rsp_header;
    PLAT_UI8 table_cr;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &tag);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_cr, 1, &table_cr);
    stse_frame_element_allocate_push(&rsp_frame, e_command_count, 1, (PLAT_UI8 *)p_command_count);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_get_command_ac_table(stse_handle_t *p_stse,
                                               PLAT_UI8 total_command_count,
                                               stse_cmd_authorization_cr_t *p_change_rights,
                                               stse_cmd_authorization_record_t *p_record_table) {
    stse_return_code_t ret;
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
    PLAT_UI8 rsp_header = 0;
    PLAT_UI8 raw_data[total_command_count * sizeof(stse_cmd_authorization_record_t)];
    PLAT_UI8 record_index = 0;
    PLAT_UI8 record_array_pos = 0;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &tag);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_cr, sizeof(stse_cmd_authorization_cr_t), (PLAT_UI8 *)p_change_rights);
    stse_frame_element_allocate_push(&rsp_frame, e_record_count, 1, &record_index);
    stse_frame_element_allocate_push(&rsp_frame, e_record_table, total_command_count * sizeof(stse_cmd_authorization_record_t), raw_data);

    /*- Perform Transfer*/
    ret = stsafea_frame_raw_transfer(p_stse,
                                     &cmd_frame,
                                     &rsp_frame,
                                     stsafea_cmd_timings[p_stse->device_type][cmd_header]);
    if (ret != STSE_OK) {
        return ret;
    }

    for (record_index = 0; record_index < total_command_count; record_index++) {
        p_record_table[record_index].header = raw_data[record_array_pos++];
        if (p_record_table[record_index].header == 0x1F) {
            p_record_table[record_index].extended_header = raw_data[record_array_pos++];
        } else {
            p_record_table[record_index].extended_header = 0;
        }
        p_record_table[record_index].command_ac = (stse_cmd_access_conditions_t)raw_data[record_array_pos++];
        p_record_table[record_index].host_encryption_flags.cmd = (raw_data[record_array_pos] & 0x02) >> 1;
        p_record_table[record_index].host_encryption_flags.rsp = (raw_data[record_array_pos++] & 0x01);
    }

    return ret;
}

stse_return_code_t stsafea_perso_info_update(stse_handle_t *p_stse) {
    stse_return_code_t ret;
    PLAT_UI8 total_command_count = 0;
    stse_cmd_authorization_cr_t change_rights;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    p_stse->perso_info.cmd_encryption_status = 0;
    p_stse->perso_info.rsp_encryption_status = 0;
    p_stse->perso_info.ext_cmd_encryption_status = 0;
    p_stse->perso_info.ext_rsp_encryption_status = 0;
    p_stse->perso_info.cmd_ac_status = 0x5555555555555555;
    p_stse->perso_info.ext_cmd_ac_status = 0x5555555555555555;

    ret = stsafea_get_command_authorization_records_count(p_stse, &total_command_count);
    if (ret != STSE_OK) {
        return ret;
    }

    stse_cmd_authorization_record_t record_table[total_command_count];

    ret = stsafea_get_command_ac_table(p_stse,
                                       total_command_count,
                                       &change_rights,
                                       record_table);
    if (ret != STSE_OK) {
        return ret;
    }

    for (PLAT_UI8 i = 0; i < total_command_count; i++) {
        if (record_table[i].extended_header == 0) {
            stsafea_perso_info_set_cmd_ac(&p_stse->perso_info, record_table[i].header, record_table[i].command_ac);
            stsafea_perso_info_set_cmd_encrypt_flag(&p_stse->perso_info, record_table[i].header, record_table[i].host_encryption_flags.cmd);
            stsafea_perso_info_set_rsp_encrypt_flag(&p_stse->perso_info, record_table[i].header, record_table[i].host_encryption_flags.rsp);
        } else {
            stsafea_perso_info_set_ext_cmd_ac(&p_stse->perso_info, record_table[i].extended_header, record_table[i].command_ac);
            stsafea_perso_info_set_ext_cmd_encrypt_flag(&p_stse->perso_info, record_table[i].extended_header, record_table[i].host_encryption_flags.cmd);
            stsafea_perso_info_set_ext_rsp_encrypt_flag(&p_stse->perso_info, record_table[i].extended_header, record_table[i].host_encryption_flags.rsp);
        }
    }

    return STSE_OK;
}

void stsafea_perso_info_get_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code, stse_cmd_access_conditions_t *p_protection) {
    *p_protection = (stse_cmd_access_conditions_t)((p_perso->cmd_ac_status >> (command_code + command_code)) & 0x03);
}

void stsafea_perso_info_get_ext_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code, stse_cmd_access_conditions_t *p_protection) {
    *p_protection = (stse_cmd_access_conditions_t)((p_perso->ext_cmd_ac_status >> (command_code + command_code)) & 0x03);
}

void stsafea_perso_info_get_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag) {
    *p_enc_flag = ((p_perso->cmd_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag) {
    *p_enc_flag = ((p_perso->rsp_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_ext_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag) {
    *p_enc_flag = ((p_perso->ext_cmd_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_ext_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag) {
    *p_enc_flag = ((p_perso->ext_rsp_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_set_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code, stse_cmd_access_conditions_t protection) {
    PLAT_UI8 offset = command_code + command_code;
    p_perso->cmd_ac_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
    p_perso->cmd_ac_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

void stsafea_perso_info_set_ext_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code, stse_cmd_access_conditions_t protection) {
    PLAT_UI8 offset = command_code + command_code;
    p_perso->ext_cmd_ac_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
    p_perso->ext_cmd_ac_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

void stsafea_perso_info_set_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag) {
    if (enc_flag) {
        p_perso->cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
    } else {
        p_perso->cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
    }
}

void stsafea_perso_info_set_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag) {
    if (enc_flag) {
        p_perso->rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
    } else {
        p_perso->rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
    }
}

void stsafea_perso_info_set_ext_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag) {
    if (enc_flag) {
        p_perso->ext_cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
    } else {
        p_perso->ext_cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
    }
}

void stsafea_perso_info_set_ext_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag) {
    if (enc_flag) {
        p_perso->ext_rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
    } else {
        p_perso->ext_rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
    }
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
