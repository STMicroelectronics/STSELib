/*!
 ******************************************************************************
 * \file	stse_hash.c
 * \brief   STSE Hash API set (sources)
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

#include "api/stse_hash.h"
#include "services/stsafea/stsafea_hash.h"

stse_ReturnCode_t stse_start_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t 	sha_algorithm,
		PLAT_UI8				*pMessage,
		PLAT_UI16 				message_size)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pMessage == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_start_hash(pSTSE, sha_algorithm, pMessage, message_size);

	return ret;
}

stse_ReturnCode_t stse_process_hash(
		stse_Handler_t * pSTSE,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pMessage == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_process_hash(pSTSE, pMessage, message_size);

	return ret;
}

stse_ReturnCode_t stse_finish_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t sha_algorithm,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size,
		PLAT_UI8* pDigest,
		PLAT_UI16* pDigest_size)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pMessage 	== NULL
	|| pDigest 		== NULL
	|| pDigest_size == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_finish_hash(pSTSE, sha_algorithm, pMessage, message_size, pDigest, pDigest_size);

	return ret;
}

stse_ReturnCode_t stse_compute_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t sha_algorithm,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size,
		PLAT_UI8* pDigest,
		PLAT_UI16* pDigest_size)
{
	stse_ReturnCode_t ret;
	PLAT_UI16 remaining_length = message_size;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pMessage 	== NULL
	|| pDigest 		== NULL
	|| pDigest_size == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	PLAT_UI16 maximum_chunck_size = stsafea_maximum_command_length[pSTSE->device_type];

	message_size = ((remaining_length + STSAFEA_HASH_ALGO_ID_SIZE) > maximum_chunck_size) ?
					maximum_chunck_size - STSAFEA_HASH_ALGO_ID_SIZE:
					remaining_length;
	ret = stsafea_start_hash(pSTSE, sha_algorithm, pMessage, message_size);
	if(ret != STSE_OK)
	{
		return(ret);
	}
	remaining_length -= message_size;
	pMessage += message_size;

	while(remaining_length > 0)
	{
		message_size = (remaining_length > maximum_chunck_size) ?
						maximum_chunck_size :
						remaining_length;
		ret = stsafea_process_hash(pSTSE, pMessage, message_size);
		if(ret != STSE_OK)
		{
			return(ret);
		}
		remaining_length -= message_size;
		pMessage += message_size;
	}

	ret = stsafea_finish_hash(pSTSE, sha_algorithm, NULL, 0, pDigest, pDigest_size);

	return ret;
}
