/*!
 * ******************************************************************************
 * \file	stse_frame.c
 * \brief   STSAFE Frame layer (sources)
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

#include "core/stse_frame.h"


PLAT_UI16 stse_frame_crc16_compute (stse_frame_t* pFrame)
{
	stse_frame_element_t *pCurrent_element;
	PLAT_UI16 crc = 0;

	pCurrent_element = pFrame->first_element;
	crc = stse_platform_Crc16_Calculate(pCurrent_element->pData, pCurrent_element->length);
	pCurrent_element = pCurrent_element->next;
	while(pCurrent_element != NULL)
	{
		if((pCurrent_element->length != 0) && (pCurrent_element->pData != NULL) )
		{
			crc = stse_platform_Crc16_Accumulate(pCurrent_element->pData, pCurrent_element->length);
		}
		pCurrent_element = pCurrent_element->next;
	};

	return crc;
}

void stse_frame_element_swap_byte_order(stse_frame_element_t* pElement)
{
	PLAT_UI8 tmp;

	for (PLAT_UI16 i=0; i < pElement->length/2; ++i) {
		tmp = *(pElement->pData + i);
		*(pElement->pData + i) = *(pElement->pData + (pElement->length - 1 - i));
		*(pElement->pData + (pElement->length - 1 - i)) = tmp;
	}
}

void stse_append_frame(stse_frame_t* pFrame1,stse_frame_t* pFrame2)
{
	/* - Set Frame2 first element as last Frame1 element next  */
	if (pFrame1->first_element == NULL)
	{
		pFrame1->first_element = pFrame2->first_element;
	} else {
		pFrame1->last_element->next = pFrame2->first_element;
	}

	/* - Position element as last one in the frame*/
	pFrame1->last_element = pFrame2->last_element;

	/* - Increment Frame length and frame element count*/
	pFrame1->element_count += pFrame2->element_count;
	pFrame1->length += pFrame2->length;
}

void stse_frame_insert_strap(stse_frame_element_t *pStrap, stse_frame_element_t* pElement_1,
		stse_frame_element_t* pElement_2)
{
	/* store previous attachment to Strap pData*/
	pStrap->pData= (PLAT_UI8 *) pElement_1->next;
	/* attach Element one to strap */
	pElement_1->next = pStrap;
	/* Attach strap to Element_2 */
	pStrap->next = pElement_2;
}

void stse_frame_unstrap(stse_frame_t* pFrame)
{
	stse_frame_element_t* pElement = pFrame->first_element;

	pFrame->length = 0;
	pFrame->element_count = 0;
	while(pElement != NULL)
	{
		pFrame->length += pElement->length;
		pFrame->element_count ++;
		pFrame->last_element = pElement;
		if((pElement->next != NULL) && (pElement->next->length == 0) && (pElement->next->pData!=NULL))
		{
			pElement->next = (stse_frame_element_t *)pElement->next->pData;
		}
		pElement = pElement->next;
	}
}

void stse_frame_update(stse_frame_t* pFrame)
{
	stse_frame_element_t* pCurrent_element = pFrame->first_element;

	pFrame->length = 0;
	pFrame->element_count = 0;
	if(pCurrent_element == NULL )
	{
		pFrame->first_element = NULL;
		pFrame->last_element = NULL;
		return;
	} else {
		do
		{
			pFrame->length += pCurrent_element->length;
			pFrame->element_count ++;
			pFrame->last_element = pCurrent_element;
			pCurrent_element=pCurrent_element->next;
		}while(pCurrent_element != NULL);
	}
}

void stse_frame_push_element( stse_frame_t* pFrame,
		stse_frame_element_t* pElement)

{

	if (pFrame->first_element == NULL)
	{
		/* - Set Element as first one if Frame is empty */
		pFrame->first_element = pElement;
	} else {
		/* - Position element as last one in the frame*/
		pFrame->last_element->next = pElement;
	}
	pFrame->last_element = pElement;
	pElement->next = NULL;

	/* - Increment Frame length and frame element count*/
    pFrame->element_count += 1;
    pFrame->length += pElement->length;
}

void stse_frame_pop_element( stse_frame_t* pFrame)
{
	stse_frame_element_t *pCurrent_element;

	if (pFrame->element_count > 1) {
		/* Select first Frame Element*/
		pCurrent_element = pFrame->first_element;
		/* Parse Frame until previous to last element */
		while(pCurrent_element->next != pFrame->last_element) {
				pCurrent_element = pCurrent_element->next;
		}
		/* Remove references/link to the last element */
		pFrame->length -= pCurrent_element->next->length;
		pCurrent_element->next = NULL;
		pFrame->last_element = pCurrent_element;
		pFrame->element_count--;
	} else {
		pFrame->first_element = NULL;
		pFrame->last_element = NULL;
		pFrame->element_count = 0;
		pFrame->length = 0;
	}

}

void stse_frame_debug_print(stse_frame_t* pFrame)
{
    stse_frame_element_t *pCurrent_element;
    PLAT_UI16 data_index;

    if(pFrame->element_count == 0 )
    {
    	printf("\n\r (EMPTY)");
    	return;
    }
    pCurrent_element = pFrame->first_element;
    printf(" (%d-byte) :", pFrame->length);
    do{
    	printf(" { ");
    	if(pCurrent_element->length == 0)
    	{
    		printf("S ");
    	} else {
			for(data_index=0; data_index<pCurrent_element->length; data_index++)
			{
				if(pCurrent_element->pData != NULL) {
					printf("0x%02X ", pCurrent_element->pData[data_index]);
				} else {
					printf("- ");
				}
			}
    	}
        printf("}");
        pCurrent_element = pCurrent_element->next;
    }while(pCurrent_element != NULL);
}

stse_ReturnCode_t stse_frame_transmit(stse_Handler_t* pSTSE, stse_frame_t* pFrame)
{
    stse_ReturnCode_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;
    stse_frame_element_t *pCurrent_element;
    PLAT_UI16 crc_ret;
    PLAT_UI8 crc[STSE_FRAME_CRC_SIZE] = {0};

    /*- Verify Parameters */
    if((pSTSE == NULL) || (pFrame == NULL))
    {
    	return STSE_CORE_INVALID_PARAMETER;
    }
    /*- Verify Frame length */
    if(pFrame->element_count == 0 )
    {
    	return STSE_CORE_INVALID_PARAMETER;
    }
    /*- Compute frame crc */
    crc_ret = stse_frame_crc16_compute(pFrame);
    crc[0] = (crc_ret >> 8) & 0xFF;
    crc[1] = crc_ret & 0xFF;

    /* - Append CRC element to the RSP Frame (valid only on Receive Scope) */
	stse_frame_element_allocate(crc_element,STSE_FRAME_CRC_SIZE,crc);
	stse_frame_push_element(pFrame, &crc_element);

#ifdef STSAFE_FRAME_DEBUG_LOG
	printf("\n\r STSAFE Frame > ");
	stse_frame_debug_print(pFrame);
	printf("\n\r");
#endif
    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR))
    {
		/* - Receive frame length from target STSAFE */
    	ret = pSTSE->io.BusSendStart(
        		pSTSE->io.Busaddr,
    			pSTSE->io.Devaddr,
    			pSTSE->io.BusSpeed,
    			pFrame->length);

    	if(ret == STSE_OK)
    	{
			pCurrent_element = pFrame->first_element;
			while(pCurrent_element != pFrame->last_element) {
				ret = pSTSE->io.BusSendContinue(
						pSTSE->io.Busaddr,
						pSTSE->io.Devaddr,
						pSTSE->io.BusSpeed,
						pCurrent_element->pData,
						pCurrent_element->length);
				if(ret != STSE_OK)
				{
					break;
				}
				pCurrent_element = pCurrent_element->next;
			}
			if(ret == STSE_OK)
			{
				ret = pSTSE->io.BusSendStop(
						pSTSE->io.Busaddr,
						pSTSE->io.Devaddr,
						pSTSE->io.BusSpeed,
						pCurrent_element->pData,
						pCurrent_element->length);
			}
    	}

		retry_count--;
		stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
	}

	/* - Pop CRC element from Frame*/
	stse_frame_pop_element(pFrame);
	return( STSE_OK );
}

stse_ReturnCode_t stse_frame_receive(stse_Handler_t* pSTSE, stse_frame_t* pFrame)
{
    stse_ReturnCode_t ret = STSE_PLATFORM_BUS_ACK_ERROR;
    stse_frame_element_t *pCurrent_element;
    PLAT_UI16 received_length;
    PLAT_UI8 received_crc[STSE_FRAME_CRC_SIZE];
    PLAT_UI16 computed_crc;
    PLAT_UI16 filler_size=0;
    PLAT_UI16 retry_count = STSE_MAX_POLLING_RETRY;

    /*- Verify Parameters */
    if((pSTSE == NULL) || (pFrame == NULL))
    {
    	return STSE_CORE_INVALID_PARAMETER;
    }
    /* - Verify Frame length */
    if(pFrame->element_count == 0 )
    {
    	return( STSE_CORE_INVALID_PARAMETER );
    }

    while ((retry_count != 0) && (ret == STSE_PLATFORM_BUS_ACK_ERROR))
    {
		/* - Receive frame length from target STSAFE */
		ret = pSTSE->io.BusRecvStart(
				pSTSE->io.Busaddr,
				pSTSE->io.Devaddr,
				pSTSE->io.BusSpeed,
				&received_length
		);

		retry_count--;
		stse_platform_Delay_ms(STSE_POLLING_RETRY_INTERVAL);
    }

    /* - Verify correct reception*/
    if((ret & 0x3F)!= STSE_OK)
    {
    	return( ret );
    }

    /* - Remove Length and CRC from received length value */
    received_length -= STSE_FRAME_CRC_SIZE + STSE_FRAME_LENGTH_SIZE ;

    /* - Compare STSAFE Received frame length with local RSP Frame length */
    if(received_length > pFrame->length)
    {
    	filler_size = received_length - pFrame->length ;
    }
    PLAT_UI8 filler[filler_size];
    stse_frame_element_allocate_push(pFrame,
        			eFiller,
    				filler_size,
    				filler);

    /* - Append CRC element to the RSP Frame (valid only in Receive Scope) */
    stse_frame_element_allocate_push(pFrame,eCRC,STSE_FRAME_CRC_SIZE,received_crc);

    /* - Perform frame element reception and populate local RSP Frame */
    pCurrent_element = pFrame->first_element;
    while (pCurrent_element != pFrame->last_element){
    	if(received_length < pCurrent_element->length)
    	{
    		pCurrent_element->length = received_length;
    	}
        pSTSE->io.BusRecvContinue(
    			pSTSE->io.Busaddr,
    			pSTSE->io.Devaddr,
    			pSTSE->io.BusSpeed,
    			pCurrent_element->pData,
    			pCurrent_element->length
		);
        received_length -= pCurrent_element->length;
        pCurrent_element = pCurrent_element->next;
    }
    pSTSE->io.BusRecvStop(
			pSTSE->io.Busaddr,
			pSTSE->io.Devaddr,
			pSTSE->io.BusSpeed,
			pCurrent_element->pData,
			pCurrent_element->length
	);

#ifdef STSAFE_FRAME_DEBUG_LOG
    printf("\n\r STSAFE Frame < ");
    stse_frame_debug_print(pFrame);
    printf("\n\r");
#endif

    /* - swap CRC */

    stse_frame_element_swap_byte_order(&eCRC);

    /* - Pop CRC element from Frame*/

    stse_frame_pop_element(pFrame);


    /* - Verify CRC */

    computed_crc = stse_frame_crc16_compute(pFrame);
    if(computed_crc != *(PLAT_UI16 *)received_crc)
    {
    	return( STSE_CORE_FRAME_CRC_ERROR );
    }
    return( STSE_OK );
}

stse_ReturnCode_t stse_frame_transfer (stse_Handler_t* pSTSE,
		stse_frame_t* pCmdFrame,
		stse_frame_t* pRspFrame,
		PLAT_UI16 inter_frame_delay)
{
	(void)inter_frame_delay;
	stse_ReturnCode_t ret = STSE_CORE_INVALID_PARAMETER ;

	/* - Send Non-protected Frame */
	ret = stse_frame_transmit(pSTSE,pCmdFrame);
	if(ret == STSE_OK)
	{
#ifdef STSE_USE_RSP_POLLING
		/* - Wait for command to be executed by target STSAFE  */
		stse_platform_Delay_ms(STSE_FIRST_POLLING_INTERVAL);
#else
		/* - Wait for command to be executed by target STSAFE  */
		stse_platform_Delay_ms(inter_frame_delay);
#endif
		/* - Receive non protected Frame */
		ret = stse_frame_receive(pSTSE,pRspFrame);
	}

	return( ret );
}
