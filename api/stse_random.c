/*!
 ******************************************************************************
 * \file	stse_random.c
 * \brief   STSE Random API set (sources)
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
 *****************************************************************************/

#include "api/stse_random.h"

stse_ReturnCode_t stse_generate_random(
		stse_Handler_t *pSTSE,
		PLAT_UI8 *pRandom,
		PLAT_UI16 random_size)
{
	stse_ReturnCode_t ret = STSE_API_HANDLER_NOT_INITIALISED;
	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pRandom == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	for(PLAT_UI16 index = 0; index < random_size; )
	{
		ret = stsafea_generate_random(
				pSTSE,
				&pRandom[index],
				((random_size - index) < STSAFEA_MAXIMUM_RNG_SIZE) ? (random_size - index) : STSAFEA_MAXIMUM_RNG_SIZE);

		if(ret != STSE_OK)
		{
			break;
		}

		index += STSAFEA_MAXIMUM_RNG_SIZE;
	}

	return ret;
}
