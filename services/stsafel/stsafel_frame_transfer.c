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
#include <stdio.h>

#include "services/stsafel/stsafel_frame_transfer.h"
#include "services/stsafel/stsafel_timings.h"
#if defined(__linux__) && defined(STSE_CONF_USE_I2C)
#include <pthread.h>
#include <time.h>
#endif

#ifdef STSE_CONF_STSAFE_L_SUPPORT

const PLAT_UI16 stsafel_maximum_frame_length[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_MAX_FRAME_LENGTH_L010, /*!< STSAFE-L Maximum command length (bytes) */
};

#if defined(__linux__) && defined(STSE_CONF_USE_I2C)
static pthread_mutex_t stsafel_device_use_mutex = PTHREAD_MUTEX_INITIALIZER;

static stse_ReturnCode_t stsafel_guard_enter(void) {
#if defined(STSE_CONF_STSAFE_DEVICE_LOCK_TIMEOUT_MS) && (STSE_CONF_STSAFE_DEVICE_LOCK_TIMEOUT_MS > 0)
#define STSAFEL_DEVICE_LOCK_TIMEOUT_MS STSE_CONF_STSAFE_DEVICE_LOCK_TIMEOUT_MS
#elif defined(STSE_CONF_I2C_TRANSACTION_LOCK_TIMEOUT_MS) && (STSE_CONF_I2C_TRANSACTION_LOCK_TIMEOUT_MS > 0)
#define STSAFEL_DEVICE_LOCK_TIMEOUT_MS STSE_CONF_I2C_TRANSACTION_LOCK_TIMEOUT_MS
#endif

#if defined(STSAFEL_DEVICE_LOCK_TIMEOUT_MS)
    struct timespec abs_timeout;

    if (clock_gettime(CLOCK_REALTIME, &abs_timeout) != 0) {
        return STSE_PLATFORM_BUS_ACK_ERROR;
    }

    abs_timeout.tv_sec += (time_t)(STSAFEL_DEVICE_LOCK_TIMEOUT_MS / 1000U);
    abs_timeout.tv_nsec += (long)(STSAFEL_DEVICE_LOCK_TIMEOUT_MS % 1000U) * 1000000L;
    if (abs_timeout.tv_nsec >= 1000000000L) {
        abs_timeout.tv_sec += 1;
        abs_timeout.tv_nsec -= 1000000000L;
    }

    if (pthread_mutex_timedlock(&stsafel_device_use_mutex, &abs_timeout) != 0) {
        return STSE_PLATFORM_BUS_ACK_ERROR;
    }
#else
    if (pthread_mutex_lock(&stsafel_device_use_mutex) != 0) {
        return STSE_PLATFORM_BUS_ACK_ERROR;
    }
#endif

#if defined(STSAFEL_DEVICE_LOCK_TIMEOUT_MS)
#undef STSAFEL_DEVICE_LOCK_TIMEOUT_MS
#endif

    return STSE_OK;
}

static void stsafel_guard_exit(PLAT_UI8 guard_entered) {
    if (guard_entered != 0) {
        (void)pthread_mutex_unlock(&stsafel_device_use_mutex);
    }
}
#endif

stse_ReturnCode_t stsafel_frame_transmit(stse_Handler_t *pSTSE, stse_frame_t *pFrame) {
    stse_ReturnCode_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;
    stse_frame_element_t *pCurrent_element;
    PLAT_UI16 crc_ret;
    PLAT_UI8 crc[STSE_FRAME_CRC_SIZE] = {0};

    /*- Verify Parameters */
    if ((pSTSE == NULL) || (pFrame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify Device type */
    if (pSTSE->device_type >= (STSAFE_L010 + STSAFEL_PRODUCT_COUNT)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify Frame length */
    if (pFrame->element_count == 0) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /*- Verify first element presence */
    if ((pFrame->first_element == NULL) || (pFrame->first_element->pData == NULL)) {
        return STSE_SERVICE_INVALID_FRAME;
    }
    /*- Verify Frame overflow */
    if (pFrame->length > stsafel_maximum_frame_length[pSTSE->device_type - STSAFE_L010]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }
    /*- Compute frame crc */
    ret = stse_frame_crc16_compute(pFrame, &crc_ret);
    if (ret != STSE_OK) {
        return ret;
    }
    crc[0] = (crc_ret >> 8) & 0xFF;
    crc[1] = crc_ret & 0xFF;

    /* - Append CRC element to the RSP Frame (valid only on Receive Scope) */
    stse_frame_element_allocate(crc_element, STSE_FRAME_CRC_SIZE, crc);
    stse_frame_push_element(pFrame, &crc_element);

#ifdef STSE_FRAME_DEBUG_LOG
    printf("\n\r STSAFE Frame > ");
    stse_frame_debug_print(pFrame);
    printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

    ret = STSE_PLATFORM_BUS_ACK_ERROR;
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        ret = pSTSE->io.BusSendStart(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            pFrame->length);

        if (ret == STSE_OK) {
            pCurrent_element = pFrame->first_element;
            while (pCurrent_element != pFrame->last_element) {
                ret = pSTSE->io.BusSendContinue(
                    pSTSE->io.busID,
                    pSTSE->io.Devaddr,
                    pSTSE->io.BusSpeed,
                    pCurrent_element->pData,
                    pCurrent_element->length);
                if (ret != STSE_OK) {
                    break;
                }
                pCurrent_element = pCurrent_element->next;
            }
            if (ret == STSE_OK) {
                ret = pSTSE->io.BusSendStop(
                    pSTSE->io.busID,
                    pSTSE->io.Devaddr,
                    pSTSE->io.BusSpeed,
                    pCurrent_element->pData,
                    pCurrent_element->length);
            }
        }

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Pop CRC element from Frame*/
    stse_frame_pop_element(pFrame);
    return ret;
}

#ifdef STSE_CONF_USE_I2C
stse_ReturnCode_t stsafel_i2c_frame_receive(stse_Handler_t *pSTSE, stse_frame_t *pFrame) {
    stse_ReturnCode_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    stse_frame_element_t *pCurrent_element;
    PLAT_UI8 received_header;
    PLAT_UI16 received_length;
    PLAT_UI8 received_crc[STSE_FRAME_CRC_SIZE];
    PLAT_UI16 computed_crc;
    PLAT_UI16 filler_size = 0;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;
    PLAT_UI8 length_value[STSE_FRAME_LENGTH_SIZE];

    /*- Verify Parameters */
    if ((pSTSE == NULL) || (pFrame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /* - Verify Frame length */
    if (pFrame->element_count == 0) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }
    /* - Verify First Frame Element */
    if ((pFrame->first_element == NULL) || (pFrame->first_element->pData == NULL) || (pFrame->first_element->length != STSE_RSP_FRAME_HEADER_SIZE)) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }
    /* - Verify Device type */
    if (pSTSE->device_type >= (STSAFE_L010 + STSAFEL_PRODUCT_COUNT)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /* ======================================================= */
    /* ============== Get the total frame length ============= */
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        /* - Receive frame length from target STSAFE */
        ret = pSTSE->io.BusRecvStart(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            STSE_FRAME_LENGTH_SIZE);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Verify correct reception*/
    if ((ret & STSE_STSAFEL_RSP_STATUS_MASK) != STSE_OK) {
        return ret;
    }

    /* - Get STSAFE Response Length */
    ret = pSTSE->io.BusRecvStop(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        length_value,
        STSE_FRAME_LENGTH_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Store response Length */
    received_length = ((length_value[0] << 8) + length_value[1]) - STSE_FRAME_CRC_SIZE;

    /*- Verify Frame overflow */
    if (pFrame->length > stsafel_maximum_frame_length[pSTSE->device_type - STSAFE_L010]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    /* ======================================================= */
    /* ====== Format the frame to handle CRC and filler ====== */

    /* - Compare STSAFE Received frame length with local RSP Frame length */
    if (received_length > pFrame->length) {
        /* Calculate needed filler to match both length */
        filler_size = received_length - pFrame->length;
    } else {
        /* Adjust frame size to fit received response size */
        pFrame->length = received_length;
    }

    /* Append filler frame element even if its length equal 0 */
    PLAT_UI8 filler[filler_size];
    stse_frame_element_allocate(eFiller,
                                filler_size,
                                filler);
    if (filler_size > 0) {
        stse_frame_push_element(pFrame,
                                &eFiller);
    }

    /* ======================================================= */
    /* ========= Receive the frame in frame elements ========= */

    ret = STSE_PLATFORM_BUS_ACK_ERROR;
    retry_count = STSE_MAX_POLLING_RETRY;
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        /* - Receive frame length from target STSAFE */
        ret = pSTSE->io.BusRecvStart(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            received_length + STSE_FRAME_CRC_SIZE);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Verify correct reception*/
    if (ret != STSE_OK) {
        /* - Pop Filler element from Frame*/
        if (filler_size > 0) {
            stse_frame_pop_element(pFrame);
        }
        return ret;
    }

    /* Receive response header */
    ret = pSTSE->io.BusRecvContinue(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        pFrame->first_element->pData,
        STSE_RSP_FRAME_HEADER_SIZE);

    if (ret != STSE_OK) {
        /* - Pop Filler element from Frame*/
        if (filler_size > 0) {
            stse_frame_pop_element(pFrame);
        }
        return ret;
    }

    received_header = (stse_ReturnCode_t)(pFrame->first_element->pData[0] & STSE_STSAFEL_RSP_STATUS_MASK);
    if (received_header != STSE_OK) {
        while (pFrame->element_count > 1) {
            stse_frame_pop_element(pFrame);
        }
    }

    /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
    stse_frame_element_allocate_push(pFrame, eCRC, STSE_FRAME_CRC_SIZE, received_crc);

    /* If first element is longer than just the header */
    if (pFrame->first_element->length > STSE_RSP_FRAME_HEADER_SIZE) {
        /* Receive missing bytes after discarding the 2 bytes length */
        ret = pSTSE->io.BusRecvContinue(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            pFrame->first_element->pData + STSE_RSP_FRAME_HEADER_SIZE,
            pFrame->first_element->length - STSE_RSP_FRAME_HEADER_SIZE);
        if (ret != STSE_OK) {
            /* - Pop CRC element from Frame*/
            stse_frame_pop_element(pFrame);
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(pFrame);
            }
            return ret;
        }
    }

    /* - Perform frame element reception and populate local RSP Frame */
    pCurrent_element = pFrame->first_element->next;
    while (pCurrent_element != pFrame->last_element) {
        if (received_length < pCurrent_element->length) {
            pCurrent_element->length = received_length;
        }
        ret = pSTSE->io.BusRecvContinue(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            pCurrent_element->pData,
            pCurrent_element->length);
        if (ret != STSE_OK) {
            /* - Pop CRC element from Frame*/
            stse_frame_pop_element(pFrame);
            /* - Pop Filler element from Frame*/
            if (filler_size > 0) {
                stse_frame_pop_element(pFrame);
            }
            return ret;
        }

        received_length -= pCurrent_element->length;
        pCurrent_element = pCurrent_element->next;
    }
    ret = pSTSE->io.BusRecvStop(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        pCurrent_element->pData,
        pCurrent_element->length);
    if (ret != STSE_OK) {
        /* - Pop CRC element from Frame*/
        stse_frame_pop_element(pFrame);
        /* - Pop Filler element from Frame*/
        if (filler_size > 0) {
            stse_frame_pop_element(pFrame);
        }
        return ret;
    }

#ifdef STSE_FRAME_DEBUG_LOG
    printf("\n\r STSAFE Frame < ");
    stse_frame_debug_print(pFrame);
    printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

    /* - Swap CRC */
    stse_frame_element_swap_byte_order(&eCRC);

    /* - Pop CRC element from Frame*/
    stse_frame_pop_element(pFrame);

    /* - Compute CRC */
    ret = stse_frame_crc16_compute(pFrame, &computed_crc);

    /* - Pop Filler element from Frame*/
    if (filler_size > 0) {
        stse_frame_pop_element(pFrame);
    }

    if (ret != STSE_OK) {
        return ret;
    }

    /* - Verify CRC */
    if (computed_crc != *(PLAT_UI16 *)received_crc) {
        return (STSE_SERVICE_FRAME_CRC_ERROR);
    }

    ret = (stse_ReturnCode_t)(pFrame->first_element->pData[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    return ret;
}
#endif /* STSE_CONF_USE_I2C */

#ifdef STSE_CONF_USE_ST1WIRE
stse_ReturnCode_t stsafel_st1wire_frame_receive(stse_Handler_t *pSTSE, stse_frame_t *pFrame) {
    stse_ReturnCode_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    stse_frame_element_t *pCurrent_element;
    PLAT_UI16 received_length;
    PLAT_UI8 received_crc[STSE_FRAME_CRC_SIZE];
    PLAT_UI16 computed_crc = 0;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;

    /*- Verify Parameters */
    if ((pSTSE == NULL) || (pFrame == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }
    /* - Verify Frame length */
    if (pFrame->element_count == 0) {
        return (STSE_SERVICE_INVALID_PARAMETER);
    }

    /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
    stse_frame_element_allocate_push(pFrame, eCRC, STSE_FRAME_CRC_SIZE, received_crc);

    /* - Store response Length */
    received_length = pFrame->length;

    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR)) {
        ret = pSTSE->io.BusRecvStart(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            pFrame->length);

        if (ret != STSE_OK) {
            retry_count--;
            stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
        }
    }

    /* - Receive response header */
    ret = pSTSE->io.BusRecvContinue(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        pFrame->first_element->pData,
        STSE_RSP_FRAME_HEADER_SIZE);

    ret = (stse_ReturnCode_t)(pFrame->first_element->pData[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    if (ret != STSE_OK) {
        return ret;
    }

    ret = pSTSE->io.BusRecvContinue(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        pFrame->first_element->pData + STSE_RSP_FRAME_HEADER_SIZE,
        pFrame->first_element->length - STSE_RSP_FRAME_HEADER_SIZE);
    if (ret != STSE_OK) {
        return ret;
    }

    received_length--;

    /* - Perform frame element reception and populate local RSP Frame */
    pCurrent_element = pFrame->first_element->next;
    while (pCurrent_element != pFrame->last_element) {
        if (received_length < pCurrent_element->length) {
            pCurrent_element->length = received_length;
        }
        ret = pSTSE->io.BusRecvContinue(
            pSTSE->io.busID,
            pSTSE->io.Devaddr,
            pSTSE->io.BusSpeed,
            pCurrent_element->pData,
            pCurrent_element->length);
        if (ret != STSE_OK) {
            return ret;
        }

        received_length -= pCurrent_element->length;
        pCurrent_element = pCurrent_element->next;
    }
    ret = pSTSE->io.BusRecvStop(
        pSTSE->io.busID,
        pSTSE->io.Devaddr,
        pSTSE->io.BusSpeed,
        pCurrent_element->pData,
        pCurrent_element->length);
    if (ret != STSE_OK) {
        return ret;
    }

#ifdef STSE_FRAME_DEBUG_LOG
    printf("\n\r STSE Frame < ");
    stse_frame_debug_print(pFrame);
    printf("\n\r");
#endif

    /* - Swap CRC */
    stse_frame_element_swap_byte_order(&eCRC);

    /* - Pop CRC element from Frame*/
    stse_frame_pop_element(pFrame);

    /* - Compute CRC */
    ret = stse_frame_crc16_compute(pFrame, &computed_crc);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Verify CRC */
    if (computed_crc != *(PLAT_UI16 *)received_crc) {
        return (STSE_SERVICE_FRAME_CRC_ERROR);
    }

    ret = (stse_ReturnCode_t)(pFrame->first_element->pData[0] & STSE_STSAFEL_RSP_STATUS_MASK);

    return ret;
}
#endif /* STSE_CONF_USE_ST1WIRE */

stse_ReturnCode_t stsafel_frame_raw_transfer(stse_Handler_t *pSTSE,
                                             stse_frame_t *pCmdFrame,
                                             stse_frame_t *pRspFrame,
                                             PLAT_UI16 inter_frame_delay) {
    stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
#if defined(__linux__) && defined(STSE_CONF_USE_I2C)
    PLAT_UI8 guard_entered = 0;
#endif

#if defined(__linux__) && defined(STSE_CONF_USE_I2C)
    if (pSTSE != NULL) {
        if (stsafel_guard_enter() != STSE_OK) {
            return STSE_PLATFORM_BUS_ACK_ERROR;
        }
        guard_entered = 1;
    }
#endif

#ifdef STSE_USE_RSP_POLLING
    (void)inter_frame_delay;
#endif /* STSE_USE_RSP_POLLING */

    /* - Send Non-protected Frame */
    ret = stsafel_frame_transmit(pSTSE, pCmdFrame);
    if (ret == STSE_OK) {
#ifdef STSE_USE_RSP_POLLING
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_Delay_ms(STSE_FIRST_POLLING_INTERVAL);
#else
        /* - Wait for command to be executed by target STSAFE  */
        stse_platform_Delay_ms(inter_frame_delay);
#endif /* STSE_USE_RSP_POLLING */

        /* - Receive non protected Frame */
        switch (pSTSE->io.BusType) {
#ifdef STSE_CONF_USE_I2C
        case STSE_BUS_TYPE_I2C:
            ret = stsafel_i2c_frame_receive(pSTSE, pRspFrame);
            break;
#endif /* STSE_CONF_USE_I2C */
#ifdef STSE_CONF_USE_ST1WIRE
        case STSE_BUS_TYPE_ST1WIRE:
            ret = stsafel_st1wire_frame_receive(pSTSE, pRspFrame);
            break;
#endif /* STSE_CONF_USE_ST1WIRE */
        default:
            break;
        }
    }

#if defined(__linux__) && defined(STSE_CONF_USE_I2C)
    stsafel_guard_exit(guard_entered);
#endif

    return ret;
}

stse_ReturnCode_t stsafel_frame_transfer(stse_Handler_t *pSTSE,
                                         stse_frame_t *pCmdFrame,
                                         stse_frame_t *pRspFrame) {
    stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header;

    PLAT_UI16 inter_frame_delay = STSAFEL_EXEC_TIME_DEFAULT;

    if (pCmdFrame->first_element != NULL && pCmdFrame->first_element->pData != NULL) {
        if (pCmdFrame->first_element->length == 1) // STSAFEA_HEADER_SIZE)
        {
            cmd_header = pCmdFrame->first_element->pData[0];
            inter_frame_delay = stsafel_cmd_exec_duration(pSTSE, (stsafel_cmd_code_t)cmd_header);
            ret = STSE_OK;
        }
    }

    if (ret != STSE_OK) {
        return ret;
    }

    /*- Perform Transfer*/
    ret = stsafel_frame_raw_transfer(pSTSE,
                                     pCmdFrame,
                                     pRspFrame,
                                     inter_frame_delay);

    return ret;
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
