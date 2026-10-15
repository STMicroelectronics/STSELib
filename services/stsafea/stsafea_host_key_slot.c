/*!
 ******************************************************************************
 * \file	stsafea_host_key_slot.c
 * \brief   STSAFE-A services for host key slot management (source)
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_host_key_slot.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/* Exported functions --------------------------------------------------------*/

stse_return_code_t stsafea_query_host_key_provisioning_ctrl_fields(
    stse_handle_t *p_stse,
    stsafea_host_key_provisioning_ctrl_fields_t *p_ctrl_fields) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_ctrl_fields == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_ctrl_fields, sizeof(stsafea_host_key_provisioning_ctrl_fields_t), (PLAT_UI8 *)p_ctrl_fields);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_put_host_key_provisioning_ctrl_fields(
    stse_handle_t *p_stse,
    stsafea_host_key_provisioning_ctrl_fields_t *p_ctrl_fields) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_ctrl_fields == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_PROVISIONING_CONTROL_FIELDS;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);
    stse_frame_element_allocate_push(&cmd_frame, e_ctrl_fields, sizeof(stsafea_host_key_provisioning_ctrl_fields_t), (PLAT_UI8 *)p_ctrl_fields);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_host_key(
    stse_handle_t *p_stse,
    stsafea_host_key_slot_t *p_host_key_slot) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_host_key_slot == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_host_key_slot, sizeof(stsafea_host_key_slot_t), (PLAT_UI8 *)p_host_key_slot);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_query_host_key_v2(
    stse_handle_t *p_stse,
    stsafea_host_key_slot_v2_t *p_host_key_slot_v2) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_host_key_slot_v2 == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V2;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_host_key_slot, sizeof(stsafea_host_key_slot_v2_t), (PLAT_UI8 *)p_host_key_slot_v2);

    /*- Perform Transfer*/
    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_put_attribute_host_secure_channel_keys(
    stse_handle_t *p_stse,
    stse_aes_key_t *host_mac_key,
    stse_aes_key_t *host_cipher_key) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (host_mac_key == NULL || host_cipher_key == NULL || host_mac_key->type != host_cipher_key->type ||
        host_mac_key->type != STSE_AES_128_KT || host_cipher_key->type != STSE_AES_128_KT ||
        host_mac_key->usage != STSE_AES_KEY_USAGE_MAC || host_cipher_key->usage != STSE_AES_KEY_USAGE_CIPHER) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_HOST_KEY_SLOT_V1;
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_subject_tag, 1, &subject_tag);
    stse_frame_element_allocate_push(&cmd_frame, e_host_mac_key, STSE_AES_128_KEY_SIZE, host_mac_key->key);
    stse_frame_element_allocate_push(&cmd_frame, e_host_cipher_key, STSE_AES_128_KEY_SIZE, host_cipher_key->key);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(p_stse,
                                      &cmd_frame,
                                      &rsp_frame,
                                      stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

stse_return_code_t stsafea_host_secure_channel_keys_provisioning(
    stse_handle_t *p_stse,
    stse_aes_key_t *host_mac_key,
    stse_aes_key_t *host_cipher_key) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_PLAINTEXT};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (host_mac_key == NULL || host_cipher_key == NULL || host_mac_key->type != host_cipher_key->type ||
        host_mac_key->type >= STSE_AES_INVALID_KT || host_cipher_key->type >= STSE_AES_INVALID_KT ||
        host_mac_key->usage != STSE_AES_KEY_USAGE_MAC || host_cipher_key->usage != STSE_AES_KEY_USAGE_CIPHER) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 host_keys_length = (host_mac_key->type == STSE_AES_128_KT) ? STSE_AES_128_KEY_SIZE : STSE_AES_256_KEY_SIZE;

    PLAT_UI8 p_padding[3] = {0};
    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_padding, 3, p_padding);
    stse_frame_element_allocate_push(&cmd_frame, e_key_type, 1, &(host_mac_key->type));
    stse_frame_element_allocate_push(&cmd_frame, e_host_mac_key, host_keys_length, host_mac_key->key);
    stse_frame_element_allocate_push(&cmd_frame, e_host_cipher_key, host_keys_length, host_cipher_key->key);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_host_secure_channel_keys_provisioning_wrapped(
    stse_handle_t *p_stse,
    stse_aes_key_type_t key_type,
    PLAT_UI8 *p_host_key_envelope) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_WRITE_HOST_KEY_V2_WRAPPED};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (key_type > STSE_AES_INVALID_KT || p_host_key_envelope == NULL) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 host_keys_envelope_length = (key_type == STSE_AES_128_KT) ? STSE_AES_128_KEY_SIZE : STSE_AES_256_KEY_SIZE;

    PLAT_UI8 rsp_header;

    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_host_keys, host_keys_envelope_length, (PLAT_UI8 *)p_host_key_envelope);

    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_establish_host_secure_channel_keys(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t host_ecdh_public_key_type,
    PLAT_UI8 *p_public_key,
    stse_aes_key_type_t host_keys_type) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((host_ecdh_public_key_type >= STSE_ECC_KT_INVALID) || (p_public_key == NULL) || (host_keys_type >= STSE_AES_INVALID_KT)) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
    stse_frame_element_allocate(e_point_representation_id, 1, &point_representation_id);

    PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
        UI16_B0(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
    };
    stse_frame_element_allocate(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);
    stse_frame_element_allocate(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);

    stse_frame_element_allocate(e_public_key_first_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, p_public_key);
    stse_frame_element_allocate(e_public_key_second_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, NULL);

    /* command frame */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

    stse_frame_element_allocate_push(&cmd_frame, e_curve_id, stse_ecc_info_table[host_ecdh_public_key_type].curve_id_total_length, (PLAT_UI8 *)&stse_ecc_info_table[host_ecdh_public_key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
    if (host_ecdh_public_key_type == STSE_ECC_KT_CURVE25519) {
        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
    } else
#endif
    {
        stse_frame_push_element(&cmd_frame, &e_point_representation_id);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
        e_public_key_second_element.p_data = p_public_key + e_public_key_first_element.length;
        stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
    }

    PLAT_UI8 algorithm_id = 0x03;
    stse_frame_element_allocate_push(&cmd_frame, e_algorithm_id, 1, &algorithm_id);

    stse_frame_element_allocate_push(&cmd_frame, e_host_keys_type, 1, (PLAT_UI8 *)&host_keys_type);

    /* response frame */
    PLAT_UI8 rsp_header;
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_establish_host_secure_channel_keys_authenticated(
    stse_handle_t *p_stse,
    stse_ecc_key_type_t host_ecdh_public_key_type,
    PLAT_UI8 *p_public_key,
    stse_aes_key_type_t host_keys_type,
    PLAT_UI8 signature_public_key_slot,
    stse_ecc_key_type_t signature_public_key_type,
    stse_hash_algorithm_t signature_hash_algo,
    PLAT_UI8 *p_signature) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_ESTABLISH_HOST_KEY_V2};

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((host_ecdh_public_key_type >= STSE_ECC_KT_INVALID) || (p_public_key == NULL) ||
        (host_keys_type >= STSE_AES_INVALID_KT) || (signature_public_key_type >= STSE_ECC_KT_INVALID) ||
        (signature_hash_algo >= STSE_SHA_INVALID) || (p_signature == NULL)) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
    stse_frame_element_allocate(e_point_representation_id, 1, &point_representation_id);

    PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
        UI16_B0(stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size),
    };
    stse_frame_element_allocate(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);
    stse_frame_element_allocate(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE, p_public_key_length_element);

    stse_frame_element_allocate(e_public_key_first_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, p_public_key);
    stse_frame_element_allocate(e_public_key_second_element, stse_ecc_info_table[host_ecdh_public_key_type].coordinate_or_key_size, NULL);

    /* command frame */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

    /* Host ECDH curve ID of public key */
    stse_frame_element_allocate_push(&cmd_frame, e_curve_id, stse_ecc_info_table[host_ecdh_public_key_type].curve_id_total_length, (PLAT_UI8 *)&stse_ecc_info_table[host_ecdh_public_key_type].curve_id);

    /* Host ECDH public key */
#ifdef STSE_CONF_ECC_CURVE_25519
    if (host_ecdh_public_key_type == STSE_ECC_KT_CURVE25519) {
        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
    } else
#endif
    {
        stse_frame_push_element(&cmd_frame, &e_point_representation_id);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
        stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

        stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
        e_public_key_second_element.p_data = p_public_key + e_public_key_first_element.length;
        stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
    }

    /* Algorithm ID */
    PLAT_UI8 algorithm_id = 0x03;
    stse_frame_element_allocate_push(&cmd_frame, e_algorithm_id, 1, &algorithm_id);

    /* Host key type */
    stse_frame_element_allocate_push(&cmd_frame, e_host_keys_type, 1, (PLAT_UI8 *)&host_keys_type);

    /* Filler */
    PLAT_UI8 filler = 0x00;
    stse_frame_element_allocate_push(&cmd_frame, e_filler, 1, &filler);

    /* Public key slot number */
    stse_frame_element_allocate_push(&cmd_frame, e_signature_public_key_slot, 1, &signature_public_key_slot);

    /* Hash algo ID */
    stse_frame_element_allocate(e_hash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, NULL);
#ifdef STSE_CONF_ECC_EDWARD_25519
    PLAT_UI8 padding_hash[STSAFEA_HASH_ALGO_ID_LENGTH_SIZE] = {0};
    if (signature_public_key_type == STSE_ECC_KT_ED25519) {
        e_hash_algo_id.length = STSAFEA_HASH_ALGO_ID_LENGTH_SIZE;
        e_hash_algo_id.p_data = padding_hash;
    } else
#endif
    {
        e_hash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
        e_hash_algo_id.p_data = (PLAT_UI8 *)&stsafea_hash_info_table[signature_hash_algo].id;
    }
    stse_frame_push_element(&cmd_frame, &e_hash_algo_id);

    /* Signature elements */
    PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
        UI16_B1(stse_ecc_info_table[signature_public_key_type].signature_size >> 1),
        UI16_B0(stse_ecc_info_table[signature_public_key_type].signature_size >> 1),
    };

    stse_frame_element_allocate_push(&cmd_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE, p_signature_length_element);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_r, (stse_ecc_info_table[signature_public_key_type].signature_size >> 1), p_signature);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE, p_signature_length_element);
    stse_frame_element_allocate_push(&cmd_frame, e_signature_s, (stse_ecc_info_table[signature_public_key_type].signature_size >> 1), p_signature + (stse_ecc_info_table[signature_public_key_type].signature_size >> 1));

    /* response frame */
    PLAT_UI8 rsp_header;
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
