/**
  ******************************************************************************
  * @file    stse_frame.c
  * @author  CS Application Team
  * @brief   STSELib api for frame layer (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdio.h>

#include "core/stse_frame.h"

stse_ReturnCode_t stse_frame_crc16_compute(stse_frame_t *pFrame, PLAT_UI16 *pCrc)
{
  stse_frame_element_t *pCurrent_element;

  if (pFrame == NULL || pCrc == NULL)
  {
    return STSE_CORE_INCONSISTENT_FRAME;
  }

  /* A valid frame always starts with one concrete element carrying payload/header bytes. */

  pCurrent_element = pFrame->first_element;
  /* CRC is initialized on the first element then accumulated on subsequent ones. */
  *pCrc = stse_platform_Crc16_Calculate(pCurrent_element->pData, pCurrent_element->length);
  pCurrent_element = pCurrent_element->next;
  while (pCurrent_element != NULL)
  {
    /* Zero-length elements are frame markers and do not contribute to CRC. */
    if (pCurrent_element->length != 0)
    {
      if (pCurrent_element->pData == NULL)
      {
        return STSE_CORE_INCONSISTENT_FRAME;
      }
      *pCrc = stse_platform_Crc16_Accumulate(pCurrent_element->pData, pCurrent_element->length);
    }
    pCurrent_element = pCurrent_element->next;
  }

  return STSE_OK;
}

void stse_frame_element_swap_byte_order(stse_frame_element_t *pElement)
{
  PLAT_UI8 tmp;

  /* In-place byte reversal used for endian adaptation of numeric payload fields. */
  for (PLAT_UI16 i = 0; i < pElement->length / 2; ++i)
  {
    tmp = *(pElement->pData + i);
    *(pElement->pData + i) = *(pElement->pData + (pElement->length - 1 - i));
    *(pElement->pData + (pElement->length - 1 - i)) = tmp;
  }
}

void stse_append_frame(stse_frame_t *pFrame1, stse_frame_t *pFrame2)
{
  /* This helper concatenates two prebuilt frame chains without element copies. */
  /* - Set Frame2 first element as last Frame1 element next  */
  if (pFrame1->first_element == NULL)
  {
    pFrame1->first_element = pFrame2->first_element;
  }
  else
  {
    pFrame1->last_element->next = pFrame2->first_element;
  }

  /* - Position element as last one in the frame*/
  pFrame1->last_element = pFrame2->last_element;

  /* - Increment Frame length and frame element count*/
  /* Metadata is additive because both source frames remain in the same linked list. */
  pFrame1->element_count += pFrame2->element_count;
  pFrame1->length += pFrame2->length;
}

void stse_frame_insert_strap(stse_frame_element_t *pStrap, stse_frame_element_t *pElement_1,
                             stse_frame_element_t *pElement_2)
{
  /* Strap acts like a temporary junction node that preserves the previous next pointer
   * in pData so the original chain can be restored later by unstrap. */
  pStrap->pData = (PLAT_UI8 *)pElement_1->next;
  /* attach Element one to strap */
  pElement_1->next = pStrap;
  /* Attach strap to Element_2 */
  pStrap->next = pElement_2;
}

void stse_frame_unstrap(stse_frame_t *pFrame)
{
  stse_frame_element_t *pElement = pFrame->first_element;

  /* Recompute frame metrics while removing transient strap nodes on the fly. */
  pFrame->length = 0;
  pFrame->element_count = 0;
  while (pElement != NULL)
  {
    pFrame->length += pElement->length;
    pFrame->element_count++;
    pFrame->last_element = pElement;
    if ((pElement->next != NULL) && (pElement->next->length == 0) && (pElement->next->pData != NULL))
    {
      /* length==0 and pData!=NULL identifies an injected strap node. */
      pElement->next = (stse_frame_element_t *)pElement->next->pData;
    }
    pElement = pElement->next;
  }
}

void stse_frame_update(stse_frame_t *pFrame)
{
  stse_frame_element_t *pCurrent_element = pFrame->first_element;

  /* Canonical recomputation of cached metadata after list edits. */
  pFrame->length = 0;
  pFrame->element_count = 0;
  if (pCurrent_element == NULL)
  {
    pFrame->first_element = NULL;
    pFrame->last_element = NULL;
    return;
  }
  else
  {
    do
    {
      pFrame->length += pCurrent_element->length;
      pFrame->element_count++;
      pFrame->last_element = pCurrent_element;
      pCurrent_element = pCurrent_element->next;
    } while (pCurrent_element != NULL);
  }
}

void stse_frame_push_element(stse_frame_t *pFrame,
                             stse_frame_element_t *pElement)

{

  if (pFrame->first_element == NULL)
  {
    /* - Set Element as first one if Frame is empty */
    pFrame->first_element = pElement;
  }
  else
  {
    /* - Position element as last one in the frame*/
    pFrame->last_element->next = pElement;
  }
  pFrame->last_element = pElement;
  pElement->next = NULL;

  /* Keep length and count synchronized with structural append operation. */
  pFrame->element_count += 1;
  pFrame->length += pElement->length;
}

void stse_frame_pop_element(stse_frame_t *pFrame)
{
  stse_frame_element_t *pCurrent_element;

  /* Pop removes only tail element; middle removals are intentionally unsupported. */
  if (pFrame->element_count > 1)
  {
    /* Select first Frame Element*/
    pCurrent_element = pFrame->first_element;
    /* Parse Frame until previous to last element */
    while (pCurrent_element->next != pFrame->last_element)
    {
      pCurrent_element = pCurrent_element->next;
    }
    /* Remove references/link to the last element and update cached metadata. */
    pFrame->length -= pCurrent_element->next->length;
    pCurrent_element->next = NULL;
    pFrame->last_element = pCurrent_element;
    pFrame->element_count--;
  }
  else
  {
    /* Popping the last element resets the frame to an empty canonical state. */
    pFrame->first_element = NULL;
    pFrame->last_element = NULL;
    pFrame->element_count = 0;
    pFrame->length = 0;
  }
}

void stse_frame_debug_print(stse_frame_t *pFrame)
{
  stse_frame_element_t *pCurrent_element;
  PLAT_UI16 data_index;

  /* Debug helper prints current linked-frame content in transport order. */
  if (pFrame->element_count == 0)
  {
    printf("\n\r (EMPTY)");
    return;
  }
  pCurrent_element = pFrame->first_element;
  printf(" (%d-byte) :", pFrame->length);
  /* 'S' marker denotes strap/sentinel elements with zero payload length. */
  do
  {
    printf(" { ");
    if (pCurrent_element->length == 0)
    {
      printf("S ");
    }
    else
    {
      for (data_index = 0; data_index < pCurrent_element->length; data_index++)
      {
        if (pCurrent_element->pData != NULL)
        {
          printf("0x%02X ", pCurrent_element->pData[data_index]);
        }
        else
        {
          printf("0x00 ");
        }
      }
    }
    printf("}");
    pCurrent_element = pCurrent_element->next;
  } while (pCurrent_element != NULL);
}
