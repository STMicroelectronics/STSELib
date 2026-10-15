/*!
 ******************************************************************************
 * \file	stsafea_symmetric_key_slots.c
 * \brief   STSAFE-A services for symmetric key slots management (source)
 * \author  STMicroelectronics - SMD application team
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
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/* Exported functions --------------------------------------------------------*/

stse_return_code_t stsafea_query_symmetric_key_slot_provisioning_ctrl_fields(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *p_ctrl_fields) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_SLOT_PROVISIONING_CONTROL;
    PLAT_UI8 rsp_header;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_ctrl_fields == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &slot_number);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_ctrl_fields, sizeof(stsafea_symmetric_key_slot_provisioning_ctrl_fields_t), (PLAT_UI8 *)p_ctrl_fields);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_put_symmetric_key_slot_provisioning_ctrl_fields(
    stse_handle_t *p_stse,
    PLAT_UI8 slot_number,
    stsafea_symmetric_key_slot_provisioning_ctrl_fields_t *p_ctrl_fields) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;
    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_SLOT_PROVISIONING_CONTROL;
    PLAT_UI8 rsp_header;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_ctrl_fields == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &slot_number);
    stse_frame_element_allocate_push(&cmd_frame, e_ctrl_fields, sizeof(stsafea_symmetric_key_slot_provisioning_ctrl_fields_t), (PLAT_UI8 *)p_ctrl_fields);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_symmetric_key_slots_count(stse_handle_t *p_stse, PLAT_UI8 *p_symmetric_key_slot_count) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_symmetric_key_slot_count == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_TABLE;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_symmetric_key_slot_count, 1, p_symmetric_key_slot_count);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_symmetric_key_table(
    stse_handle_t *p_stse,
    PLAT_UI8 symmetric_key_slot_count,
    stsafea_symmetric_key_slot_information_t *symmetric_key_table_info) {
    stse_return_code_t ret;
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (symmetric_key_table_info == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 slot_count, i;
    PLAT_UI8 *current_record;

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_SYMMETRIC_KEY_TABLE;
    PLAT_UI8 rsp_header;
    PLAT_UI16 rsp_raw_length = symmetric_key_slot_count * sizeof(stsafea_symmetric_key_slot_information_t);
    PLAT_UI8 p_rsp_raw[rsp_raw_length];

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_raw, rsp_raw_length, p_rsp_raw);

    /*- Perform Transfer*/
    ret = stsafea_frame_raw_transfer(p_stse,
                                     &cmd_frame,
                                     &rsp_frame,
                                     stsafea_cmd_timings[p_stse->device_type][cmd_header]);

    if (ret != STSE_OK) {
        return ret;
    }

    slot_count = p_rsp_raw[0];

    if (slot_count > symmetric_key_slot_count) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    /* Skip "Number of slot" */
    current_record = p_rsp_raw + 1;

    for (i = 0; i < slot_count; i++) {
        memset(&symmetric_key_table_info[i], 0, sizeof(stsafea_symmetric_key_slot_information_t));
        if ((current_record[0] & 0x01) == 1) {
            /* Parse common part of key information record */
            memcpy(&symmetric_key_table_info[i],
                   current_record,
                   STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH);
            /* Parse AES CMAC key information record parameters */
            if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_CMAC) {
                symmetric_key_table_info[i].parameters.cmac.minimum_mac_length = current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH];
                current_record += 1;
            } else
                /* Parse AES CCM* key information record parameters */
                if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_CCM) {
                    memcpy(&symmetric_key_table_info[i].parameters.ccm,
                           &current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH],
                           STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH);
                    if (symmetric_key_table_info[i].parameters.ccm.counter_presence == 1) {
                        memcpy(&symmetric_key_table_info[i].parameters.ccm.counter_value,
                               &current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH + STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH],
                               STSAFEA_COUNTER_VALUE_SIZE);
                        current_record += STSAFEA_COUNTER_VALUE_SIZE;
                    }
                    current_record += STSAFEA_SYMMETRIC_KEY_TABLE_CCM_PARAMETERS_LENGTH;
                } else
                    /* Parse AES GCM key information record parameters */
                    if (symmetric_key_table_info[i].mode_of_operation == STSAFEA_KEY_OPERATION_MODE_GCM) {
                        symmetric_key_table_info[i].parameters.gcm.auth_tag_length = current_record[STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH];
                        current_record += 1;
                    }
            current_record += STSAFEA_SYMMETRIC_KEY_TABLE_INFO_COMMON_VALUES_LENGTH;
        } else {
            current_record += 1;
        }
    }

    return ret;
}

stse_return_code_t stsafea_establish_symmetric_key(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *host_ecdhe_public_key) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (host_ecdhe_public_key == NULL || key_type >= STSE_ECC_KT_INVALID) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY;
    PLAT_UI8 rsp_header;

    PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
    stse_frame_element_allocate(e_point_representation_id, 1, &point_representation_id);

    PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
        UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
    };
    stse_frame_element_allocate(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);
    stse_frame_element_allocate(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);

    stse_frame_element_allocate(e_public_key_first_element, 0, NULL);
    stse_frame_element_allocate(e_public_key_second_element, 0, NULL);

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

    stse_frame_element_allocate_push(&cmd_frame, e_curve_id,
                                     stse_ecc_info_table[key_type].curve_id_total_length,
                                     (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
    if (key_type == STSE_ECC_KT_CURVE25519) {
        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_first_element.p_data = host_ecdhe_public_key;
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
    } else
#endif
    {
        stse_frame_push_element(&cmd_frame, &e_point_representation_id);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);

        e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_first_element.p_data = host_ecdhe_public_key;
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);

        e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_second_element.p_data = host_ecdhe_public_key + e_public_key_first_element.length;
        stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
    }

    stse_frame_element_allocate_push(&cmd_frame, e_algorithm_id, 1, &algorithm_id);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_establish_symmetric_key_authenticated(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t key_type,
    PLAT_UI8 *host_ecdhe_public_key,
    stse_hash_algorithm_t hash_algo,
    PLAT_UI8 signature_public_key_slot_number,
    stse_ecc_key_type_t signature_key_type,
    PLAT_UI8 *p_signature) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_ESTABLISH_SYMMETRIC_KEYS};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (host_ecdhe_public_key == NULL || p_signature == NULL || key_type >= STSE_ECC_KT_INVALID) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_ESTABLISH_SYM_KEY;
    PLAT_UI8 rsp_header;

    PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
    stse_frame_element_allocate(e_point_representation_id, 1, &point_representation_id);

    PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
        UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
    };
    stse_frame_element_allocate(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);
    stse_frame_element_allocate(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);

    stse_frame_element_allocate(e_public_key_first_element, 0, NULL);
    stse_frame_element_allocate(e_public_key_second_element, 0, NULL);

    PLAT_UI8 filler_1_byte = 0;

    PLAT_UI8 p_empty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
    stse_frame_element_allocate(e_hash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, p_empty_hash_algo_id);

    /* Divide Signature length By 2 to get R or S length */
    PLAT_UI16 signature_r_s_length = stse_ecc_info_table[signature_key_type].signature_size >> 1;
    PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(signature_r_s_length),
        UI16_B0(signature_r_s_length),
    };

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

    stse_frame_element_allocate_push(&cmd_frame, e_curve_id,
                                     stse_ecc_info_table[key_type].curve_id_total_length,
                                     (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
    if (key_type == STSE_ECC_KT_CURVE25519) {
        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_first_element.p_data = host_ecdhe_public_key;
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
    } else
#endif
    {
        stse_frame_push_element(&cmd_frame, &e_point_representation_id);
        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_first_element.p_data = host_ecdhe_public_key;
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
        stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
        e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
        e_public_key_second_element.p_data = host_ecdhe_public_key + e_public_key_first_element.length;
        stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
    }

    stse_frame_element_allocate_push(&cmd_frame, e_algorithm_id, 1, &algorithm_id);
    stse_frame_element_allocate_push(&cmd_frame, e_filler, 1, &filler_1_byte);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_public_key_slot_number, 1, &signature_public_key_slot_number);

#ifdef STSE_CONF_ECC_EDWARD_25519
    if (signature_key_type != STSE_ECC_KT_ED25519)
#endif
    {
        e_hash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
        e_hash_algo_id.p_data = (PLAT_UI8 *)&stsafea_hash_info_table[hash_algo].id;
    }
    stse_frame_push_element(&cmd_frame, &e_hash_algo_id);

    stse_frame_element_allocate_push(&cmd_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE, p_signature_length_element);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_r, signature_r_s_length, p_signature);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE, p_signature_length_element);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_s, signature_r_s_length, p_signature + signature_r_s_length);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#if defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED)

stse_return_code_t stsafea_confirm_symmetric_key(
    stse_handle_t *p_stse,
    PLAT_UI32 confirmation_key_index,
    PLAT_UI8 key_count,
    stsafea_generic_key_information_t *p_key_information_list) {
    stse_return_code_t ret;
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_CONFIRM_SYMMETRIC_KEYS};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((p_key_information_list == NULL) || (key_count == 0)) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 p_confirmation_mac[STSE_KEY_CONFIRMATION_MAC_SIZE];
    stse_frame_element_t e_key_information_list[key_count];
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_confirmation_mac, STSE_KEY_CONFIRMATION_MAC_SIZE, p_confirmation_mac);

    ret = stse_platform_aes_cmac_init(
        confirmation_key_index,
        STSE_KEY_CONFIRMATION_MAC_SIZE);
    if (ret != STSE_OK) {
        return (ret);
    }

    for (PLAT_UI8 i = 0; i < key_count; i++) {
        PLAT_UI8 temp_buffer;

        e_key_information_list[i].length = p_key_information_list[i].info_length + STSAFEA_GENERIC_LENGTH_SIZE;
        e_key_information_list[i].p_data = (PLAT_UI8 *)&p_key_information_list[i];

        /* Swap 2 bytes of length */
        temp_buffer = e_key_information_list[i].p_data[0];
        e_key_information_list[i].p_data[0] = e_key_information_list[i].p_data[1];
        e_key_information_list[i].p_data[1] = temp_buffer;

        ret = stse_platform_aes_cmac_append(
            e_key_information_list[i].p_data,
            e_key_information_list[i].length);
        if (ret != STSE_OK) {
            return (ret);
        }

        stse_frame_push_element(&cmd_frame, &e_key_information_list[i]);
    }

    ret = stse_platform_aes_cmac_compute_finish(p_confirmation_mac, NULL);
    if (ret != STSE_OK) {
        return (ret);
    }

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(p_stse,
                                 &cmd_frame,
                                 &rsp_frame);

    for (PLAT_UI8 i = 0; i < key_count; i++) {
        PLAT_UI8 temp_buffer = e_key_information_list[i].p_data[0];
        e_key_information_list[i].p_data[0] = e_key_information_list[i].p_data[1];
        e_key_information_list[i].p_data[1] = temp_buffer;
    }

    return ret;
}

#endif

#if defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

stse_return_code_t stsafea_write_symmetric_key_wrapped(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_symmetric_key_envelope,
    PLAT_UI8 symmetric_key_envelope_length) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_WRAPPED};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_symmetric_key_envelope == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_symmetric_key_envelope, symmetric_key_envelope_length, p_symmetric_key_envelope);
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif

stse_return_code_t stsafea_write_symmetric_key_plaintext(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_symmetric_key_value,
    stsafea_generic_key_information_t *p_symmetric_key_info) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_WRITE_SYMMETRIC_KEY_PLAINTEXT};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_symmetric_key_info == NULL || p_symmetric_key_value == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 rsp_header;

    PLAT_UI16 key_value_length = 0;

    switch (p_symmetric_key_info->type) {
    case STSAFEA_SYMMETRIC_KEY_TYPE_AES_128:
        key_value_length = STSE_AES_128_KEY_SIZE;
        break;
    case STSAFEA_SYMMETRIC_KEY_TYPE_AES_256:
        key_value_length = STSE_AES_256_KEY_SIZE;
        break;
    case STSAFEA_SYMMETRIC_KEY_TYPE_GENERIC_SECRET:
        key_value_length = p_symmetric_key_info->hmac.generic_secret_key_length; /* HMAC or HKDF */
        break;
    default:
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_symmetric_key_info, p_symmetric_key_info->info_length, (PLAT_UI8 *)&p_symmetric_key_info->lock_indicator);
    stse_frame_element_allocate_push(&cmd_frame, e_symmetric_key_value, key_value_length, p_symmetric_key_value);
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_generate_wrap_unwrap_key(stse_handle_t *p_stse,
                                                   PLAT_UI8 wrap_key_slot,
                                                   stse_aes_key_type_t key_type) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_KEY;

    PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_LOCAL_ENVELOPE_KEY_TABLE;
    PLAT_UI8 key_length = (key_type == STSE_AES_128_KT) ? 0x00 : 0x01;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_tag, 1, &attribute_tag);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &wrap_key_slot);
    stse_frame_element_allocate_push(&cmd_frame, e_key_l_enght, 1, &key_length);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_erase_symmetric_key_slot(
    stse_handle_t *p_stse,
    PLAT_UI8 symmetric_key_slot_number) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_ERASE_SYMMETRIC_KEY_SLOT};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_slot_number, 1, &symmetric_key_slot_number);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
