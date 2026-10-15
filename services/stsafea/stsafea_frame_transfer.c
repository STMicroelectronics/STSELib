/*!
 * ******************************************************************************
 * \file    stsafea_frame_transfer.c
 * \brief   STSAFE-A services for frame transfer (source)
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
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_timings.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

const PLAT_UI16 stsafea_maximum_frame_length[STSAFEA_PRODUCT_COUNT] = {
    STSAFEA_MAX_FRAME_LENGTH_A100,
    STSAFEA_MAX_FRAME_LENGTH_A110,
    STSAFEA_MAX_FRAME_LENGTH_A120};

stse_return_code_t stsafea_frame_transmit(stse_handle_t *p_stse, stse_frame_t *p_frame) {
    stse_return_code_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;
    stse_frame_element_t *p_current_element;
    PLAT_UI16 crc_ret;
    PLAT_UI8 crc[STSE_FRAME_CRC_SIZE] = {0};

    /*- Verify Parameters */
    if ((p_stse == NULL) || (p_frame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify Device type */
    if (p_stse->device_type >= (STSAFE_A100 + STSAFEA_PRODUCT_COUNT)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify Frame length */
    if (p_frame->element_count == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify first element presence */
    if ((p_frame->first_element == NULL) || (p_frame->first_element->p_data == NULL)) {
        return STSE_SERVICE_INVALID_FRAME;
    }
    /*- Verify Frame overflow */
    if (p_frame->length > stsafea_maximum_frame_length[p_stse->device_type - STSAFE_A100]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }
    /*- Compute frame crc */
    ret = stse_frame_crc16_compute(p_frame, &crc_ret);
    if (ret != STSE_OK) {
        return ret;
    }
    crc[0] = (crc_ret >> 8) & 0xFF;
    crc[1] = crc_ret & 0xFF;

    /* - Append CRC element to the RSP Frame (valid only on Receive Scope) */
    stse_frame_element_allocate(crc_element, STSE_FRAME_CRC_SIZE, crc);
    stse_frame_push_element(p_frame, &crc_element);

#ifdef STSE_FRAME_DEBUG_LOG
    stse_platform_printf("\n\r STSAFE Frame > ");
    stse_frame_debug_print(p_frame);
    stse_platform_printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

    ret = STSE_PLATFORM_BUS_ACK_ERROR;
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        ret = p_stse->io.bus_send_start(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            p_frame->length);

        if (ret == STSE_OK) {
            p_current_element = p_frame->first_element;
            while (p_current_element != p_frame->last_element && p_current_element != NULL) {
                ret = p_stse->io.bus_send_continue(
                    p_stse->io.bus_id,
                    p_stse->io.devaddr,
                    p_stse->io.bus_speed,
                    p_current_element->p_data,
                    p_current_element->length);
                if (ret != STSE_OK) {
                    break;
                }
                p_current_element = p_current_element->next;
            }
            if (ret == STSE_OK) {
                ret = p_stse->io.bus_send_stop(
                    p_stse->io.bus_id,
                    p_stse->io.devaddr,
                    p_stse->io.bus_speed,
                    p_current_element->p_data,
                    p_current_element->length);
            }
        }

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Pop CRC element from Frame*/
    stse_frame_pop_element(p_frame);
    return ret;
}

stse_return_code_t stsafea_frame_receive(stse_handle_t *p_stse, stse_frame_t *p_frame) {
    stse_return_code_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    stse_frame_element_t *p_current_element;
    PLAT_UI8 received_header;
    PLAT_UI16 received_length;
    PLAT_UI8 received_crc[STSE_FRAME_CRC_SIZE];
    PLAT_UI16 computed_crc;
    PLAT_UI16 filler_size = 0;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;
    PLAT_UI8 length_value[STSE_FRAME_LENGTH_SIZE];

    /*- Verify Parameters */
    if ((p_stse == NULL) || (p_frame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /* - Verify Frame length */
    if (p_frame->element_count == 0) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }
    /* - Verify First Frame Element */
    if ((p_frame->first_element == NULL) || (p_frame->first_element->p_data == NULL) || (p_frame->first_element->length != STSE_RSP_FRAME_HEADER_SIZE)) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }
    /* - Verify Device type */
    if (p_stse->device_type >= (STSAFE_A100 + STSAFEA_PRODUCT_COUNT)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* ================================================================================= */
    /* ============== Get the total frame length + 2 bytes (potential CRC) ============= */
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        /* - Receive frame length from target STSAFE */
        ret = p_stse->io.bus_recv_start(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            STSE_FRAME_LENGTH_SIZE + STSE_RSP_FRAME_HEADER_SIZE + STSE_FRAME_CRC_SIZE);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Verify correct reception*/
    if (ret != STSE_OK) {
        return ret;
    }

    /* Discard response header */
    ret = p_stse->io.bus_recv_continue(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        &received_header,
        STSE_RSP_FRAME_HEADER_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Get STSAFE Response Length */
    ret = p_stse->io.bus_recv_continue(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        length_value,
        STSE_FRAME_LENGTH_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Get STSAFE Response Potential CRC */
    ret = p_stse->io.bus_recv_stop(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        received_crc,
        STSE_FRAME_CRC_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Store response Length */
    received_length = ((length_value[0] << 8) + length_value[1]) - STSE_FRAME_CRC_SIZE + STSE_RSP_FRAME_HEADER_SIZE;

    /*- Verify Frame overflow */
    if (received_length > stsafea_maximum_frame_length[p_stse->device_type - STSAFE_A100]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    if ((received_header & STSE_STSAFEA_RSP_STATUS_MASK) != STSE_OK) {
        while (p_frame->element_count > 1) {
            stse_frame_pop_element(p_frame);
        }
    }

    if (received_length == 1) {
        /* ====================================================== */
        /* ====== compute CRC for response without payload ====== */

        computed_crc = stse_platform_crc16_calculate(&received_header, STSE_RSP_FRAME_HEADER_SIZE);

#ifdef STSE_FRAME_DEBUG_LOG
        stse_platform_printf("\n\r STSAFE Frame <  (%d-byte) : { 0x%02X } { 0x%02X 0x%02X }\n\r",
                             received_length + STSE_FRAME_CRC_SIZE,
                             received_header,
                             received_crc[0],
                             received_crc[1]);
#endif /* STSE_FRAME_DEBUG_LOG */

        /* - Verify CRC */
        if (computed_crc != ((received_crc[0] << 8) + received_crc[1])) {
            return (STSE_SERVICE_FRAME_CRC_ERROR);
        }

        ret = (stse_return_code_t)(received_header & STSE_STSAFEA_RSP_STATUS_MASK);
    } else {
        /* ======================================================= */
        /* ====== Format the frame to handle CRC and filler ====== */

        /* - Compare STSAFE Received frame length with local RSP Frame length */
        if (received_length > p_frame->length) {
            /* Calculate needed filler to match both length */
            filler_size = received_length - p_frame->length;
        } else {
            /* Adjust frame size to fit received response size */
            p_frame->length = received_length;
        }

        /* Append filler frame element even if its length equal 0 */
        PLAT_UI8 filler[filler_size];
        stse_frame_element_allocate(e_filler,
                                    filler_size,
                                    filler);
        if (filler_size > 0) {
            stse_frame_push_element(p_frame,
                                    &e_filler);
        }

        /* ======================================================= */
        /* ========= Receive the frame in frame elements ========= */

        ret = STSE_PLATFORM_BUS_ACK_ERROR;
        retry_count = STSE_MAX_POLLING_RETRY;
        while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
            /* - Receive frame length from target STSAFE */
            ret = p_stse->io.bus_recv_start(
                p_stse->io.bus_id,
                p_stse->io.devaddr,
                p_stse->io.bus_speed,
                STSE_FRAME_LENGTH_SIZE + received_length + STSE_FRAME_CRC_SIZE);

            if (ret != STSE_OK) {
                retry_count--;
                stse_platform_delay_ms(STSE_POLLING_RETRY_INTERVAL);
            }
        }

        /* - Verify correct reception*/
        if (ret != STSE_OK) {
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(p_frame);
            }
            return ret;
        }

        /* Receive response header */
        ret = p_stse->io.bus_recv_continue(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            p_frame->first_element->p_data,
            STSE_RSP_FRAME_HEADER_SIZE);

        if (ret != STSE_OK) {
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(p_frame);
            }
            return ret;
        }

        /* Substract response header already read in STSAFE-A */
        received_length -= STSE_RSP_FRAME_HEADER_SIZE;

        /* Receive and discard length (already stored) */
        ret = p_stse->io.bus_recv_continue(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            NULL,
            STSE_FRAME_LENGTH_SIZE);
        if (ret != STSE_OK) {
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(p_frame);
            }
            return ret;
        }

        /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
        stse_frame_element_allocate_push(p_frame, e_crc, STSE_FRAME_CRC_SIZE, received_crc);

        /* - Perform frame element reception and populate local RSP Frame */
        p_current_element = p_frame->first_element->next;
        while (p_current_element != p_frame->last_element && p_current_element != NULL) {
            if (received_length < p_current_element->length) {
                p_current_element->length = received_length;
            }
            ret = p_stse->io.bus_recv_continue(
                p_stse->io.bus_id,
                p_stse->io.devaddr,
                p_stse->io.bus_speed,
                p_current_element->p_data,
                p_current_element->length);
            if (ret != STSE_OK) {
                /* - Pop CRC element from Frame*/
                stse_frame_pop_element(p_frame);
                /* - Pop Filler element from Frame*/
                if (filler_size > 0) {
                    stse_frame_pop_element(p_frame);
                }
                return ret;
            }

            received_length -= p_current_element->length;
            p_current_element = p_current_element->next;
        }

        ret = p_stse->io.bus_recv_stop(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            p_current_element->p_data,
            p_current_element->length);
        if (ret != STSE_OK) {
            /* - Pop CRC element from Frame*/
            stse_frame_pop_element(p_frame);
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(p_frame);
            }
            return ret;
        }

#ifdef STSE_FRAME_DEBUG_LOG
        stse_platform_printf("\n\r STSAFE Frame < ");
        stse_frame_debug_print(p_frame);
        stse_platform_printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

        /* - Swap CRC */
        stse_frame_element_swap_byte_order(&e_crc);

        /* - Pop CRC element from Frame*/
        stse_frame_pop_element(p_frame);

        /* - Compute CRC */
        ret = stse_frame_crc16_compute(p_frame, &computed_crc);

        /* - Pop Filler element from Frame*/
        if (filler_size > 0) {
            stse_frame_pop_element(p_frame);
        }

        if (ret != STSE_OK) {
            return ret;
        }

        /* - Verify CRC */
        if (computed_crc != *(PLAT_UI16 *)received_crc) {
            return (STSE_SERVICE_FRAME_CRC_ERROR);
        }

        ret = (stse_return_code_t)(p_frame->first_element->p_data[0] & STSE_STSAFEA_RSP_STATUS_MASK);
    }

    return ret;
}

stse_return_code_t stsafea_frame_raw_transfer(stse_handle_t *p_stse,
                                             stse_frame_t *p_cmd_frame,
                                             stse_frame_t *p_rsp_frame,
                                             PLAT_UI16 inter_frame_delay) {
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;

#ifdef STSE_USE_RSP_POLLING
    (void)inter_frame_delay;
#endif /* STSE_USE_RSP_POLLING */

    /* - Send Non-protected Frame */
    ret = stsafea_frame_transmit(p_stse, p_cmd_frame);
    if (ret == STSE_OK) {
#ifdef STSE_USE_RSP_POLLING
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_delay_ms(STSE_FIRST_POLLING_INTERVAL);
#else
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_delay_ms(inter_frame_delay);
#endif /* STSE_USE_RSP_POLLING */
        /* - Receive non protected Frame */
        ret = stsafea_frame_receive(p_stse, p_rsp_frame);
    }

    return ret;
}

stse_return_code_t stsafea_frame_transfer(stse_handle_t *p_stse, stse_frame_t *p_cmd_frame,
                                         stse_frame_t *p_rsp_frame) {
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI16 inter_frame_delay = STSAFEA_EXEC_TIME_DEFAULT;

#ifdef STSE_CONF_USE_HOST_SESSION
    stse_cmd_access_conditions_t cmd_ac_info;
    PLAT_UI8 cmd_encryption_flag = 0;
    PLAT_UI8 rsp_encryption_flag = 0;
#endif /* STSE_CONF_USE_HOST_SESSION */

    if (p_cmd_frame->first_element != NULL && p_cmd_frame->first_element->p_data != NULL) {
        if (p_cmd_frame->first_element->length == STSAFEA_EXT_HEADER_SIZE && p_cmd_frame->first_element->p_data[0] == STSAFEA_EXTENDED_COMMAND_PREFIX) {
            inter_frame_delay = stsafea_extended_cmd_timings[p_stse->device_type][p_cmd_frame->first_element->p_data[1]];
#ifdef STSE_CONF_USE_HOST_SESSION
            stsafea_perso_info_get_ext_cmd_ac(&p_stse->perso_info, p_cmd_frame->first_element->p_data[1], &cmd_ac_info);
            stsafea_perso_info_get_ext_cmd_encrypt_flag(&p_stse->perso_info, p_cmd_frame->first_element->p_data[1], &cmd_encryption_flag);
            stsafea_perso_info_get_ext_rsp_encrypt_flag(&p_stse->perso_info, p_cmd_frame->first_element->p_data[1], &rsp_encryption_flag);
#endif /* STSE_CONF_USE_HOST_SESSION */
            ret = STSE_OK;
        } else if (p_cmd_frame->first_element->length == STSAFEA_HEADER_SIZE && p_cmd_frame->first_element->p_data[0] != STSAFEA_EXTENDED_COMMAND_PREFIX) {
            inter_frame_delay = stsafea_cmd_timings[p_stse->device_type][p_cmd_frame->first_element->p_data[0]];
#ifdef STSE_CONF_USE_HOST_SESSION
            stsafea_perso_info_get_cmd_ac(&p_stse->perso_info, p_cmd_frame->first_element->p_data[0], &cmd_ac_info);
            stsafea_perso_info_get_cmd_encrypt_flag(&p_stse->perso_info, p_cmd_frame->first_element->p_data[0], &cmd_encryption_flag);
            stsafea_perso_info_get_rsp_encrypt_flag(&p_stse->perso_info, p_cmd_frame->first_element->p_data[0], &rsp_encryption_flag);
#endif /* STSE_CONF_USE_HOST_SESSION */
            ret = STSE_OK;
        }
    }

    if (ret != STSE_OK) {
        return ret;
    }

    /*- Perform Transfer*/
#ifdef STSE_CONF_USE_HOST_SESSION
    if (cmd_encryption_flag || rsp_encryption_flag) {
        ret = stsafea_session_encrypted_transfer(p_stse->p_active_host_session,
                                                 p_cmd_frame,
                                                 p_rsp_frame,
                                                 cmd_encryption_flag,
                                                 rsp_encryption_flag,
                                                 cmd_ac_info,
                                                 inter_frame_delay);
    } else if (cmd_ac_info != STSE_CMD_AC_FREE) {
        ret = stsafea_session_authenticated_transfer(p_stse->p_active_host_session,
                                                     p_cmd_frame,
                                                     p_rsp_frame,
                                                     cmd_ac_info,
                                                     inter_frame_delay);
    } else
#endif /* STSE_CONF_USE_HOST_SESSION */
    {
        ret = stsafea_frame_raw_transfer(p_stse,
                                         p_cmd_frame,
                                         p_rsp_frame,
                                         inter_frame_delay);
    }

    return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT **/
