/*!
 ******************************************************************************
 * \file	stsafea_sessions.c
 * \brief   STSAFE-A services for sessions (source)
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
#include <stdio.h>
#include <string.h>

#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_sessions.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_AES_SUBJECT_HOST_CMAC 0x00U
#define STSAFEA_AES_SUBJECT_HOST_RMAC 0x40U
#define STSAFEA_AES_SUBJECT_HOST_DECRYPT 0xC0U
#define STSAFEA_AES_SUBJECT_HOST_ENCRYPT 0x80U
#define STSAFEA_AES_FIRST_PADDING_BYTE 0x80U

/* Private variables ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#ifdef STSE_CONF_USE_HOST_SESSION

stse_return_code_t stsafea_open_host_session_from_idx(stse_handle_t *p_stse, stse_session_t *p_session, PLAT_UI32 host_mac_key_idx, PLAT_UI32 host_cipher_key_idx) {
    stse_return_code_t ret;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if (p_session == NULL) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    if (p_stse->device_type == STSAFE_A120) {
        stsafea_host_key_slot_v2_t host_key_slot;

        ret = stsafea_query_host_key_v2(p_stse, &host_key_slot);
        if (ret != STSE_OK) {
            return ret;
        }

        if (host_key_slot.key_presence_flag == 0) {
            return STSE_SERVICE_SESSION_ERROR;
        }
        p_session->context.host.key_type = (stse_aes_key_type_t)host_key_slot.key_type;
        p_session->context.host.mac_counter = ARRAY_4B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
    } else {
        stsafea_host_key_slot_t host_key_slot;

        ret = stsafea_query_host_key(p_stse, &host_key_slot);
        if (ret != STSE_OK) {
            return ret;
        }

        if (host_key_slot.key_presence_flag == 0) {
            return STSE_SERVICE_SESSION_ERROR;
        }
        p_session->context.host.key_type = STSE_AES_128_KT;
        p_session->context.host.mac_counter = ARRAY_3B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
    }

    p_session->type = STSE_HOST_SESSION;
    p_session->context.host.host_mac_key_idx = host_mac_key_idx;
    p_session->context.host.host_cypher_key_idx = host_cipher_key_idx;
    p_session->context.host.p_stse = p_stse;
    p_stse->p_active_host_session = p_session;

    return (STSE_OK);
}

void stsafea_close_host_session(stse_session_t *p_session) {

    if (p_session == NULL) {
        return;
    }

    /* - Check if session is active in STSE handler*/
    if (p_session->context.host.p_stse->p_active_host_session == p_session) {
        /* Clear p_active_host_session */
        p_session->context.host.p_stse->p_active_host_session = NULL;
    }

    /* - Clear session context */
    stsafea_session_clear_context(p_session);
}

void stsafea_session_clear_context(stse_session_t *p_session) {

    /* - Check stsafe handler initialization */
    if (p_session == NULL) {
        return;
    }

    /* - Clear session context */
    memset(p_session, 0x00, sizeof(stse_session_t));
}

stse_return_code_t stsafea_set_active_host_session(stse_handle_t *p_stse, stse_session_t *p_session) {
    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if (p_session == NULL) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    p_stse->p_active_host_session = p_session;

    return (STSE_OK);
}

stse_return_code_t stsafea_session_frame_encrypt(stse_session_t *p_session,
                                                stse_frame_t *p_frame,
                                                stse_frame_element_t *p_enc_payload_element) {
    stse_return_code_t ret;
    PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
    stse_frame_element_t *p_element;
    PLAT_UI16 i = 0;

    /* - Verify parameters */
    if ((p_session == NULL) ||
        (p_frame == NULL) ||
        (p_enc_payload_element == NULL) ||
        (p_enc_payload_element->length < (p_frame->length - p_frame->first_element->length + (16 - (p_frame->length - p_frame->first_element->length) % 16)))) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    /* - Prepare specific STSAFE AES IV */
    if (p_session->context.host.p_stse->device_type == STSAFE_A120) {
        initial_value[0] = UI32_B3(p_session->context.host.mac_counter + 1);
        initial_value[1] = UI32_B2(p_session->context.host.mac_counter + 1);
        initial_value[2] = UI32_B1(p_session->context.host.mac_counter + 1);
        initial_value[3] = UI32_B0(p_session->context.host.mac_counter + 1);
        initial_value[4] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
        initial_value[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
        (void)memset(&initial_value[6], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE)-6U);
    } else {
        initial_value[0] = UI32_B2(p_session->context.host.mac_counter + 1);
        initial_value[1] = UI32_B1(p_session->context.host.mac_counter + 1);
        initial_value[2] = UI32_B0(p_session->context.host.mac_counter + 1);
        initial_value[3] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
        initial_value[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
        (void)memset(&initial_value[5], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE)-5U);
    }

    PLAT_UI16 encrypted_iv_len = STSAFEA_HOST_AES_BLOCK_SIZE;

    /* - Perform first AES ECB round on IV */
    ret = stse_platform_aes_ecb_enc(initial_value,
                                    STSAFEA_HOST_AES_BLOCK_SIZE,
                                    p_session->context.host.host_cypher_key_idx,
                                    initial_value,
                                    &encrypted_iv_len);
    if (ret != STSE_OK) {
        return (ret);
    }
    /* - Copy Plain text Frame payload content in Ciphered   */
    p_element = p_frame->first_element->next;
    while (p_element != NULL) {
        memcpy(p_enc_payload_element->p_data + i,
               p_element->p_data,
               p_element->length);

        i += p_element->length;
        p_element = p_element->next;
    }
    /* - Add First padding byte */
    *(p_enc_payload_element->p_data + i++) = 0x80;

    /* - Add padding  */
    while (i < p_enc_payload_element->length) {
        *(p_enc_payload_element->p_data + i++) = 0x00;
    }

    PLAT_UI16 encrypted_payload_len = p_enc_payload_element->length;

    /* - Encrypt p_enc_frame content */
    ret = stse_platform_aes_cbc_enc(
        p_enc_payload_element->p_data,
        p_enc_payload_element->length,
        initial_value,
        p_session->context.host.host_cypher_key_idx,
        p_enc_payload_element->p_data,
        &encrypted_payload_len);
    if (ret != 0) {
        return (STSE_SESSION_ERROR);
    } else {
        return (STSE_OK);
    }
}

static stse_return_code_t stsafea_session_frame_decrypt(stse_session_t *p_session, stse_frame_t *p_frame) {
    stse_return_code_t ret;
    PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
    stse_frame_element_t *p_element;
    PLAT_UI16 i = 0;
    PLAT_UI16 encrypted_payload_len;

    p_element = p_frame->first_element->next;
    if (p_element == NULL) {
        return STSE_OK;
    }

    /* Total length of the encrypted part of the frame */
    encrypted_payload_len = p_frame->length - p_frame->first_element->length;

    /* Fill decrypt buffer with encrypted payload content */
    PLAT_UI8 decrypt_buffer[encrypted_payload_len];

    while (p_element != NULL) {
        if (p_element->length != 0) {
            memcpy(decrypt_buffer + i, p_element->p_data, p_element->length);
            i += p_element->length;
        }
        p_element = p_element->next;
    }

    /* - Prepare Plain text info for AES IV */
    if (p_session->context.host.p_stse->device_type == STSAFE_A120) {
        initial_value[0] = UI32_B3(p_session->context.host.mac_counter);
        initial_value[1] = UI32_B2(p_session->context.host.mac_counter);
        initial_value[2] = UI32_B1(p_session->context.host.mac_counter);
        initial_value[3] = UI32_B0(p_session->context.host.mac_counter);
        initial_value[4] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
        initial_value[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
        (void)memset(&initial_value[6], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE)-6U);
    } else {
        initial_value[0] = UI32_B2(p_session->context.host.mac_counter);
        initial_value[1] = UI32_B1(p_session->context.host.mac_counter);
        initial_value[2] = UI32_B0(p_session->context.host.mac_counter);
        initial_value[3] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
        initial_value[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
        (void)memset(&initial_value[5], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE)-5U);
    }

    PLAT_UI16 out_len = STSAFEA_HOST_AES_BLOCK_SIZE;

    /* - Transform IV using AES ECB */
    ret = stse_platform_aes_ecb_enc(initial_value,
                                    STSAFEA_HOST_AES_BLOCK_SIZE,
                                    p_session->context.host.host_cypher_key_idx,
                                    initial_value,
                                    &out_len);

    if (ret != 0) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    /* - Decrypt payload using CBC */
    PLAT_UI16 decrypted_payload_len = encrypted_payload_len;

    ret = stse_platform_aes_cbc_dec(decrypt_buffer,
                                    encrypted_payload_len,
                                    initial_value,
                                    p_session->context.host.host_cypher_key_idx,
                                    decrypt_buffer,
                                    &decrypted_payload_len);

    if (ret != STSE_OK) {
        return ret;
    }

    /* - Copy Decrypted payload content in un-strapped Frame  */
    stse_frame_unstrap(p_frame);
    p_element = p_frame->first_element->next;
    i = 0;
    while (p_element != NULL) {
        memcpy(p_element->p_data,
               decrypt_buffer + i,
               p_element->length);
        i += p_element->length;
        p_element = p_element->next;
    }

    return STSE_OK;
}

static stse_return_code_t stsafea_session_frame_c_mac_compute(stse_session_t *p_session,
                                                             stse_frame_t *p_cmd_frame,
                                                             PLAT_UI8 *p_mac) {
    PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
    PLAT_UI8 mac_output_length;
    PLAT_UI8 mac_type = 0x00;
    stse_frame_element_t *p_element;
    PLAT_UI8 aes_block_idx = 0;
    PLAT_UI16 i;
    PLAT_UI16 cmd_payload_length = p_cmd_frame->length - p_cmd_frame->first_element->length;
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;

    if ((p_session == NULL) || (p_cmd_frame == NULL) || (p_mac == NULL)) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    /*- create C-MAC Frame : [0x00] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] */
    stse_frame_allocate(c_mac_frame);
    stse_frame_element_allocate_push(&c_mac_frame, e_mac_type, 1, &mac_type);
    stse_frame_element_allocate_push(&c_mac_frame,
                                     e_cmd_header,
                                     p_cmd_frame->first_element->length,
                                     p_cmd_frame->first_element->p_data);
    stse_frame_element_allocate_push(&c_mac_frame,
                                     e_cmd_payload_length,
                                     STSAFEA_CMD_RSP_LEN_SIZE,
                                     (PLAT_UI8 *)&cmd_payload_length);
    stse_frame_element_swap_byte_order(&e_cmd_payload_length);
    e_cmd_payload_length.next = p_cmd_frame->first_element->next;
    stse_frame_update(&c_mac_frame);

    /*- Initialize AES C-MAC computation */

    ret = stse_platform_aes_cmac_init(p_session->context.host.host_mac_key_idx,
                                      STSAFEA_MAC_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /*- Perform First AES-CMAC round with MAC subject info */
    if (p_session->context.host.p_stse->device_type == STSAFE_A120) {
        aes_cmac_block[0] = UI32_B3(p_session->context.host.mac_counter);
        aes_cmac_block[1] = UI32_B2(p_session->context.host.mac_counter);
        aes_cmac_block[2] = UI32_B1(p_session->context.host.mac_counter);
        aes_cmac_block[3] = UI32_B0(p_session->context.host.mac_counter);
        aes_cmac_block[4] = STSAFEA_AES_SUBJECT_HOST_CMAC;  /* Subject : Host C-MAC */
        aes_cmac_block[5] = STSAFEA_AES_FIRST_PADDING_BYTE; /* First byte of padding */
        for (i = 6; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++) {
            aes_cmac_block[i] = 0x00U; /* 0x00 padding */
        }
    } else {
        aes_cmac_block[0] = UI32_B2(p_session->context.host.mac_counter);
        aes_cmac_block[1] = UI32_B1(p_session->context.host.mac_counter);
        aes_cmac_block[2] = UI32_B0(p_session->context.host.mac_counter);
        aes_cmac_block[3] = STSAFEA_AES_SUBJECT_HOST_CMAC;  /* Subject : Host C-MAC */
        aes_cmac_block[4] = STSAFEA_AES_FIRST_PADDING_BYTE; /* First byte of padding */
        for (i = 5; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++) {
            aes_cmac_block[i] = 0x00U; /* 0x00 padding */
        }
    }

    ret = stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    p_element = c_mac_frame.first_element;

    /*- Perform additional AES-CMAC round(s) for frame to Authenticate */
    while (p_element != NULL) {
        for (i = 0; i < p_element->length; i++) {
            if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE) {
                stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
                aes_block_idx = 0;
            }
            aes_cmac_block[aes_block_idx] = *(p_element->p_data + i);
            aes_block_idx++;
        }
        p_element = p_element->next;
    }
    if (aes_block_idx != 0) {
        ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
        if (ret != STSE_OK) {
            return ret;
        }
    }

    /*- Finish AES MAC computation */
    ret = stse_platform_aes_cmac_compute_finish(aes_cmac_block, &mac_output_length);
    if (ret != STSE_OK) {
        return ret;
    } else if (mac_output_length != STSAFEA_MAC_SIZE) {
        return STSE_SERVICE_SESSION_ERROR;
    }
    memcpy(p_mac, aes_cmac_block, STSAFEA_MAC_SIZE);

    return ret;
}

static stse_return_code_t stsafea_session_frame_r_mac_verify(stse_session_t *p_session,
                                                            stse_frame_t *p_cmd_frame,
                                                            stse_frame_t *p_rsp_frame,
                                                            PLAT_UI8 *p_mac) {
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
    PLAT_UI16 cmd_payload_length = p_cmd_frame->length - p_cmd_frame->first_element->length;
    PLAT_UI8 aes_block_idx = 0;
    PLAT_UI16 i;
    PLAT_UI8 mac_type = 0x80;
    stse_frame_element_t *p_element;

    if ((p_session == NULL) || (p_cmd_frame == NULL) || (p_rsp_frame == NULL)) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    if (*(p_cmd_frame->first_element->p_data) & STSAFEA_PROT_RSP_Msk) {

        /*- Pop R-MAC from frame*/
        stse_frame_pop_element(p_rsp_frame);

        PLAT_UI16 rsp_payload_length = (p_rsp_frame->length - (p_rsp_frame->first_element->length));

        /*- Initialize AES CMAC computation */
        stse_platform_aes_cmac_init(
            p_session->context.host.host_mac_key_idx,
            STSAFEA_MAC_SIZE);

        /*- Perform First AES-CMAC round */
        if (p_session->context.host.p_stse->device_type == STSAFE_A120) {
            aes_cmac_block[0] = UI32_B3(p_session->context.host.mac_counter);
            aes_cmac_block[1] = UI32_B2(p_session->context.host.mac_counter);
            aes_cmac_block[2] = UI32_B1(p_session->context.host.mac_counter);
            aes_cmac_block[3] = UI32_B0(p_session->context.host.mac_counter);
            aes_cmac_block[4] = STSAFEA_AES_SUBJECT_HOST_RMAC;
            aes_cmac_block[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
            for (i = 6; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++) {
                aes_cmac_block[i] = 0x00U; /* 0x00 padding */
            }
        } else {
            aes_cmac_block[0] = UI32_B2(p_session->context.host.mac_counter);
            aes_cmac_block[1] = UI32_B1(p_session->context.host.mac_counter);
            aes_cmac_block[2] = UI32_B0(p_session->context.host.mac_counter);
            aes_cmac_block[3] = STSAFEA_AES_SUBJECT_HOST_RMAC;
            aes_cmac_block[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
            for (i = 5; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++) {
                aes_cmac_block[i] = 0x00U; /* 0x00 padding */
            }
        }

        stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);

        /*- Prepare AES CMAC input for response MAC verification  */
        stse_frame_allocate(r_mac_frame);

        /*- Create r_mac_frame head :[MAC TYPE] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] ... */
        stse_frame_element_allocate_push(
            &r_mac_frame,
            e_mac_type,
            1,
            &mac_type);

        stse_frame_element_allocate_push(
            &r_mac_frame,
            e_cmd_header,
            p_cmd_frame->first_element->length,
            p_cmd_frame->first_element->p_data);

        stse_frame_element_allocate_push(
            &r_mac_frame,
            e_cmd_length,
            STSAFEA_CMD_RSP_LEN_SIZE,
            (PLAT_UI8 *)&cmd_payload_length);
        stse_frame_element_swap_byte_order(&e_cmd_length);

        if (p_cmd_frame->first_element->next->length == 0) {
            e_cmd_length.next = p_cmd_frame->first_element->next->next;
        } else {
            e_cmd_length.next = p_cmd_frame->first_element->next;
        }

        stse_frame_update(&r_mac_frame);

        /*- Create r_mac_frame head : ... [RSP HEADER] [RSP PAYLOAD LENGTH] [RSP PAYLOAD] */
        stse_frame_element_allocate_push(
            &r_mac_frame,
            e_rsp_header,
            p_rsp_frame->first_element->length,
            p_rsp_frame->first_element->p_data);

        stse_frame_element_allocate_push(
            &r_mac_frame,
            e_rsp_length,
            STSAFEA_CMD_RSP_LEN_SIZE,
            (PLAT_UI8 *)&rsp_payload_length);
        stse_frame_element_swap_byte_order(&e_rsp_length);

        e_rsp_length.next = p_rsp_frame->first_element->next;
        stse_frame_update(&r_mac_frame);
        p_element = r_mac_frame.first_element;

        /*- Perform additional AES-CMAC round(s) on R-MAC verification frame*/
        while (p_element != NULL) {
            for (i = 0; i < p_element->length; i++) {
                if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE) {
                    stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
                    aes_block_idx = 0;
                }
                aes_cmac_block[aes_block_idx] = *(p_element->p_data + i);
                aes_block_idx++;
            }
            p_element = p_element->next;
        }
        if (aes_block_idx != 0) {
            ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
            if (ret != STSE_OK) {
                return ret;
            }
        }

        memcpy(aes_cmac_block, p_mac, STSAFEA_MAC_SIZE);
        ret = stse_platform_aes_cmac_verify_finish(aes_cmac_block);
    }
    return ret;
}

stse_return_code_t stsafea_session_encrypted_transfer(stse_session_t *p_session,
                                                     stse_frame_t *p_cmd_frame,
                                                     stse_frame_t *p_rsp_frame,
                                                     PLAT_UI8 cmd_encryption_flag,
                                                     PLAT_UI8 rsp_encryption_flag,
                                                     stse_cmd_access_conditions_t cmd_ac_info,
                                                     PLAT_UI16 processing_time) {
    stse_return_code_t ret;
    PLAT_UI16 encrypted_cmd_payload_size = 0;
    PLAT_UI16 encrypted_rsp_payload_size = 0;
    PLAT_UI8 padding = 16;

    if (p_session == NULL || p_cmd_frame == NULL || p_rsp_frame == NULL ||
        p_cmd_frame->first_element == NULL || p_cmd_frame->first_element->p_data == NULL ||
        p_rsp_frame->first_element == NULL || p_rsp_frame->first_element->p_data == NULL) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    if (cmd_encryption_flag == 1) {
#ifdef STSE_FRAME_DEBUG_LOG
        printf("\n\r STSAFE Plaintext Frame > ");
        stse_frame_debug_print(p_cmd_frame);
        printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

        PLAT_UI16 plaintext_payload_size = p_cmd_frame->length - p_cmd_frame->first_element->length;
        if ((plaintext_payload_size % 16) != 0) {
            padding = 16 - (plaintext_payload_size % 16);
        }
        encrypted_cmd_payload_size = plaintext_payload_size + padding;
    }

    PLAT_UI8 encrypted_cmd_payload[encrypted_cmd_payload_size];
    stse_frame_element_allocate(e_encrypted_cmd_payload, encrypted_cmd_payload_size, encrypted_cmd_payload);
    stse_frame_strap_allocate(S1);

    if (cmd_encryption_flag == 1) {
        ret = stsafea_session_frame_encrypt(p_session, p_cmd_frame, &e_encrypted_cmd_payload);
        if (ret != STSE_OK) {
            return ret;
        }
        stse_frame_insert_strap(&S1, p_cmd_frame->first_element, &e_encrypted_cmd_payload);
        stse_frame_update(p_cmd_frame);
    }

    if (rsp_encryption_flag == 1) {
        padding = 16;
        PLAT_UI16 plaintext_payload_size = p_rsp_frame->length - p_rsp_frame->first_element->length;
        if (plaintext_payload_size != 0) {
	    if ((plaintext_payload_size % 16) != 0) {
                padding = 16 - (plaintext_payload_size % 16);
            }
        } else {
            padding = 0;
        }
        encrypted_rsp_payload_size = plaintext_payload_size + padding;
    }

    PLAT_UI8 encrypted_rsp_payload[encrypted_rsp_payload_size];
    stse_frame_element_allocate(e_encrypted_rsp_payload, encrypted_rsp_payload_size, encrypted_rsp_payload);
    stse_frame_strap_allocate(S2);

    if (rsp_encryption_flag == 1 && p_rsp_frame->first_element->next != NULL) {
        stse_frame_insert_strap(&S2, p_rsp_frame->first_element, &e_encrypted_rsp_payload);
        stse_frame_update(p_rsp_frame);
    }

    ret = stsafea_session_authenticated_transfer(p_session,
                                                 p_cmd_frame,
                                                 p_rsp_frame,
                                                 cmd_ac_info,
                                                 processing_time);

    if ((ret == STSE_OK) && (rsp_encryption_flag == 1)) {
        ret = stsafea_session_frame_decrypt(p_session, p_rsp_frame);

#ifdef STSE_FRAME_DEBUG_LOG
        printf("\n\r STSAFE Plaintext Frame < ");
        stse_frame_debug_print(p_rsp_frame);
        printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */
    }

    return ret;
}

stse_return_code_t stsafea_session_authenticated_transfer(stse_session_t *p_session,
                                                         stse_frame_t *p_cmd_frame,
                                                         stse_frame_t *p_rsp_frame,
                                                         stse_cmd_access_conditions_t cmd_ac_info,
                                                         PLAT_UI16 processing_time) {
    (void)cmd_ac_info;
    stse_return_code_t ret;
    PLAT_UI8 cmd_mac[STSAFEA_MAC_SIZE];
    PLAT_UI8 rsp_mac[STSAFEA_MAC_SIZE];

    if (p_session == NULL || p_cmd_frame == NULL || p_rsp_frame == NULL ||
        p_cmd_frame->first_element == NULL || p_cmd_frame->first_element->p_data == NULL ||
        p_rsp_frame->first_element == NULL || p_rsp_frame->first_element->p_data == NULL) {
        return STSE_SERVICE_SESSION_ERROR;
    }

    if (p_session->type == STSE_HOST_SESSION) {
        *(p_cmd_frame->first_element->p_data) |= (1 << 5);
    }

    *(p_cmd_frame->first_element->p_data) |= ((1 << 7) | (1 << 6));

    stse_frame_element_allocate_push(p_rsp_frame, e_rsp_mac, STSAFEA_MAC_SIZE, rsp_mac);

    ret = stsafea_session_frame_c_mac_compute(p_session, p_cmd_frame, cmd_mac);
    if (ret != STSE_OK) {
        return ret;
    }

    stse_frame_element_allocate_push(p_cmd_frame, e_cmd_mac, STSAFEA_MAC_SIZE, cmd_mac);

    switch (p_session->type) {

    case STSE_HOST_SESSION:
        ret = stsafea_frame_raw_transfer(p_session->context.host.p_stse, p_cmd_frame, p_rsp_frame, processing_time);
        if (ret <= 0xFF && ret != STSE_INVALID_C_MAC && ret != STSE_COMMUNICATION_ERROR) {
            p_session->context.host.mac_counter++;
        }
        break;

    default:
        ret = STSE_SERVICE_SESSION_ERROR;
        break;
    }

    /*- Pop C-MAC from frame*/
    stse_frame_pop_element(p_cmd_frame);

    if (ret == STSE_OK) {
        ret = stsafea_session_frame_r_mac_verify(p_session, p_cmd_frame, p_rsp_frame, rsp_mac);
    }

    return ret;
}

#endif /* STSE_CONF_USE_HOST_SESSION */

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
