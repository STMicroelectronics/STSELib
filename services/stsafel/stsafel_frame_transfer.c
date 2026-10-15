/*!
 * ******************************************************************************
 * \file	stsafel_frame_transfer.c
 * \brief   STSAFE-L services for frame transfer (source)
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

#include "services/stsafel/stsafel_frame_transfer.h"
#include "services/stsafel/stsafel_timings.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

const PLAT_UI16 stsafel_maximum_frame_length[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_MAX_FRAME_LENGTH_L010, /*!< STSAFE-L Maximum command length (bytes) */
};

stse_return_code_t stsafel_frame_transmit(stse_handle_t *p_stse, stse_frame_t *p_frame) {
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
    if (p_stse->device_type >= (STSAFE_L010 + STSAFEL_PRODUCT_COUNT)) {
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
    if (p_frame->length > stsafel_maximum_frame_length[p_stse->device_type - STSAFE_L010]) {
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

#ifdef STSE_CONF_USE_I2C
stse_return_code_t stsafel_i2c_frame_receive(stse_handle_t *p_stse, stse_frame_t *p_frame) {
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
    if (p_stse->device_type >= (STSAFE_L010 + STSAFEL_PRODUCT_COUNT)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* ======================================================= */
    /* ============== Get the total frame length ============= */
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        /* - Receive frame length from target STSAFE */
        ret = p_stse->io.bus_recv_start(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            STSE_FRAME_LENGTH_SIZE);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Verify correct reception*/
    if ((ret & STSE_STSAFEL_RSP_STATUS_MASK) != STSE_OK) {
        return ret;
    }

    /* - Get STSAFE Response Length */
    ret = p_stse->io.bus_recv_stop(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        length_value,
        STSE_FRAME_LENGTH_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Store response Length */
    received_length = ((length_value[0] << 8) + length_value[1]) - STSE_FRAME_CRC_SIZE;

    /*- Verify Frame overflow */
    if (p_frame->length > stsafel_maximum_frame_length[p_stse->device_type - STSAFE_L010]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

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
            received_length + STSE_FRAME_CRC_SIZE);

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

    received_header = (stse_return_code_t)(p_frame->first_element->p_data[0] & STSE_STSAFEL_RSP_STATUS_MASK);
    if (received_header != STSE_OK) {
        while (p_frame->element_count > 1) {
            stse_frame_pop_element(p_frame);
        }
    }

    /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
    stse_frame_element_allocate_push(p_frame, e_crc, STSE_FRAME_CRC_SIZE, received_crc);

    /* If first element is longer than just the header */
    if (p_frame->first_element->length > STSE_RSP_FRAME_HEADER_SIZE) {
        /* Receive missing bytes after discarding the 2 bytes length */
        ret = p_stse->io.bus_recv_continue(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            p_frame->first_element->p_data + STSE_RSP_FRAME_HEADER_SIZE,
            p_frame->first_element->length - STSE_RSP_FRAME_HEADER_SIZE);
        if (ret != STSE_OK) {
            /* - Pop CRC element from Frame*/
            stse_frame_pop_element(p_frame);
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(p_frame);
            }
            return ret;
        }
    }

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

    ret = (stse_return_code_t)(p_frame->first_element->p_data[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    return ret;
}
#endif /* STSE_CONF_USE_I2C */

#ifdef STSE_CONF_USE_ST1WIRE
stse_return_code_t stsafel_st1wire_frame_receive(stse_handle_t *p_stse, stse_frame_t *p_frame) {
    stse_return_code_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    stse_frame_element_t *p_current_element;
    PLAT_UI16 received_length;
    PLAT_UI8 received_crc[STSE_FRAME_CRC_SIZE];
    PLAT_UI16 computed_crc = 0;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;

    /*- Verify Parameters */
    if ((p_stse == NULL) || (p_frame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /* - Verify Frame length */
    if (p_frame->element_count == 0) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
    stse_frame_element_allocate_push(p_frame, e_crc, STSE_FRAME_CRC_SIZE, received_crc);

    /* - Store response Length */
    received_length = p_frame->length;

    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        ret = p_stse->io.bus_recv_start(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed,
            p_frame->length);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Receive response header */
    ret = p_stse->io.bus_recv_continue(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        p_frame->first_element->p_data,
        STSE_RSP_FRAME_HEADER_SIZE);

    ret = (stse_return_code_t)(p_frame->first_element->p_data[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    if (ret != STSE_OK) {
        return ret;
    }

    ret = p_stse->io.bus_recv_continue(
        p_stse->io.bus_id,
        p_stse->io.devaddr,
        p_stse->io.bus_speed,
        p_frame->first_element->p_data + STSE_RSP_FRAME_HEADER_SIZE,
        p_frame->first_element->length - STSE_RSP_FRAME_HEADER_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    received_length--;

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
        return ret;
    }

#ifdef STSE_FRAME_DEBUG_LOG
    stse_platform_printf("\n\r STSE Frame < ");
    stse_frame_debug_print(p_frame);
    stse_platform_printf("\n\r");
#endif

    /* - Swap CRC */
    stse_frame_element_swap_byte_order(&e_crc);

    /* - Pop CRC element from Frame*/
    stse_frame_pop_element(p_frame);

    /* - Compute CRC */
    ret = stse_frame_crc16_compute(p_frame, &computed_crc);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Verify CRC */
    if (computed_crc != *(PLAT_UI16 *)received_crc) {
        return (STSE_SERVICE_FRAME_CRC_ERROR);
    }

    ret = (stse_return_code_t)(p_frame->first_element->p_data[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    return ret;
}
#endif /* STSE_CONF_USE_ST1WIRE */

stse_return_code_t stsafel_frame_raw_transfer(stse_handle_t *p_stse,
                                             stse_frame_t *p_cmd_frame,
                                             stse_frame_t *p_rsp_frame,
                                             PLAT_UI16 inter_frame_delay) {
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;

#ifdef STSE_USE_RSP_POLLING
    (void)inter_frame_delay;
#endif /* STSE_USE_RSP_POLLING */

    /* - Send Non-protected Frame */
    ret = stsafel_frame_transmit(p_stse, p_cmd_frame);
    if (ret == STSE_OK) {
#ifdef STSE_USE_RSP_POLLING
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_delay_ms(STSE_FIRST_POLLING_INTERVAL);
#else
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_delay_ms(inter_frame_delay);
#endif /* STSE_USE_RSP_POLLING */

        /* - Receive non protected Frame */
        switch (p_stse->io.bus_type) {
#ifdef STSE_CONF_USE_I2C
        case STSE_BUS_TYPE_I2C:
            ret = stsafel_i2c_frame_receive(p_stse, p_rsp_frame);
            break;
#endif /* STSE_CONF_USE_I2C */
#ifdef STSE_CONF_USE_ST1WIRE
        case STSE_BUS_TYPE_ST1WIRE:
            ret = stsafel_st1wire_frame_receive(p_stse, p_rsp_frame);
            break;
#endif /* STSE_CONF_USE_ST1WIRE */
        default:
            break;
        }
    }

    return ret;
}

stse_return_code_t stsafel_frame_transfer(stse_handle_t *p_stse,
                                         stse_frame_t *p_cmd_frame,
                                         stse_frame_t *p_rsp_frame) {
    stse_return_code_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header;

    PLAT_UI16 inter_frame_delay = STSAFEL_EXEC_TIME_DEFAULT;

    if (p_cmd_frame->first_element != NULL && p_cmd_frame->first_element->p_data != NULL) {
        if (p_cmd_frame->first_element->length == 1) // STSAFEA_HEADER_SIZE)
        {
            cmd_header = p_cmd_frame->first_element->p_data[0];
            inter_frame_delay = stsafel_cmd_exec_duration(p_stse, (stsafel_cmd_code_t)cmd_header);
            ret = STSE_OK;
        }
    }

    if (ret != STSE_OK) {
        return ret;
    }

    /*- Perform Transfer*/
    ret = stsafel_frame_raw_transfer(p_stse,
                                     p_cmd_frame,
                                     p_rsp_frame,
                                     inter_frame_delay);

    return ret;
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
