/*!
 * ******************************************************************************
 * \file	stse_frame.c
 * \brief   STSELib api for frame layer (source)
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

#include "core/stse_frame.h"

stse_return_code_t stse_frame_crc16_compute(stse_frame_t *p_frame, PLAT_UI16 *p_crc) {
    stse_frame_element_t *p_current_element;

    if (p_frame == NULL || p_crc == NULL) {
        return STSE_CORE_INCONSISTENT_FRAME;
    }

    p_current_element = p_frame->first_element;
    *p_crc = stse_platform_crc16_calculate(p_current_element->p_data, p_current_element->length);
    p_current_element = p_current_element->next;
    while (p_current_element != NULL) {
        if (p_current_element->length != 0) {
            if (p_current_element->p_data == NULL) {
                return STSE_CORE_INCONSISTENT_FRAME;
            }
            *p_crc = stse_platform_crc16_accumulate(p_current_element->p_data, p_current_element->length);
        }
        p_current_element = p_current_element->next;
    }

    return STSE_OK;
}

void stse_frame_element_swap_byte_order(stse_frame_element_t *p_element) {
    PLAT_UI8 tmp;

    for (PLAT_UI16 i = 0; i < p_element->length / 2; ++i) {
        tmp = *(p_element->p_data + i);
        *(p_element->p_data + i) = *(p_element->p_data + (p_element->length - 1 - i));
        *(p_element->p_data + (p_element->length - 1 - i)) = tmp;
    }
}

void stse_append_frame(stse_frame_t *p_frame1, stse_frame_t *p_frame2) {
    /* - Set Frame2 first element as last Frame1 element next  */
    if (p_frame1->first_element == NULL) {
        p_frame1->first_element = p_frame2->first_element;
    } else {
        p_frame1->last_element->next = p_frame2->first_element;
    }

    /* - Position element as last one in the frame*/
    p_frame1->last_element = p_frame2->last_element;

    /* - Increment Frame length and frame element count*/
    p_frame1->element_count += p_frame2->element_count;
    p_frame1->length += p_frame2->length;
}

void stse_frame_insert_strap(stse_frame_element_t *p_strap, stse_frame_element_t *p_element_1,
                             stse_frame_element_t *p_element_2) {
    /* store previous attachment to Strap p_data*/
    p_strap->p_data = (PLAT_UI8 *)p_element_1->next;
    /* attach Element one to strap */
    p_element_1->next = p_strap;
    /* Attach strap to Element_2 */
    p_strap->next = p_element_2;
}

void stse_frame_unstrap(stse_frame_t *p_frame) {
    stse_frame_element_t *p_element = p_frame->first_element;

    p_frame->length = 0;
    p_frame->element_count = 0;
    while (p_element != NULL) {
        p_frame->length += p_element->length;
        p_frame->element_count++;
        p_frame->last_element = p_element;
        if ((p_element->next != NULL) && (p_element->next->length == 0) && (p_element->next->p_data != NULL)) {
            p_element->next = (stse_frame_element_t *)p_element->next->p_data;
        }
        p_element = p_element->next;
    }
}

void stse_frame_update(stse_frame_t *p_frame) {
    stse_frame_element_t *p_current_element = p_frame->first_element;

    p_frame->length = 0;
    p_frame->element_count = 0;
    if (p_current_element == NULL) {
        p_frame->first_element = NULL;
        p_frame->last_element = NULL;
        return;
    } else {
        do {
            p_frame->length += p_current_element->length;
            p_frame->element_count++;
            p_frame->last_element = p_current_element;
            p_current_element = p_current_element->next;
        } while (p_current_element != NULL);
    }
}

void stse_frame_push_element(stse_frame_t *p_frame,
                             stse_frame_element_t *p_element)

{

    if (p_frame->first_element == NULL) {
        /* - Set Element as first one if Frame is empty */
        p_frame->first_element = p_element;
    } else {
        /* - Position element as last one in the frame*/
        p_frame->last_element->next = p_element;
    }
    p_frame->last_element = p_element;
    p_element->next = NULL;

    /* - Increment Frame length and frame element count*/
    p_frame->element_count += 1;
    p_frame->length += p_element->length;
}

void stse_frame_pop_element(stse_frame_t *p_frame) {
    stse_frame_element_t *p_current_element;

    if (p_frame->element_count > 1) {
        /* Select first Frame Element*/
        p_current_element = p_frame->first_element;
        /* Parse Frame until previous to last element */
        while (p_current_element->next != p_frame->last_element && p_current_element->next->next != NULL) {
            p_current_element = p_current_element->next;
        }
        /* Remove references/link to the last element */
        p_frame->length -= p_current_element->next->length;
        p_current_element->next = NULL;
        p_frame->last_element = p_current_element;
        p_frame->element_count--;
    } else {
        p_frame->first_element = NULL;
        p_frame->last_element = NULL;
        p_frame->element_count = 0;
        p_frame->length = 0;
    }
}

void stse_frame_debug_print(stse_frame_t *p_frame) {
    stse_frame_element_t *p_current_element;
    PLAT_UI16 data_index;

    if (p_frame->element_count == 0) {
        stse_platform_printf("\n\r (EMPTY)");
        return;
    }
    p_current_element = p_frame->first_element;
    stse_platform_printf(" (%d-byte) :", p_frame->length);
    do {
        stse_platform_printf(" { ");
        if (p_current_element->length == 0) {
            stse_platform_printf("S ");
        } else {
            for (data_index = 0; data_index < p_current_element->length; data_index++) {
                if (p_current_element->p_data != NULL) {
                    stse_platform_printf("0x%02X ", p_current_element->p_data[data_index]);
                } else {
                    stse_platform_printf("0x00 ");
                }
            }
        }
        stse_platform_printf("}");
        p_current_element = p_current_element->next;
    } while (p_current_element != NULL);
}
