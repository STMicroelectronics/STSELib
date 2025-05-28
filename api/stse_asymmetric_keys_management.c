/*!
 ******************************************************************************
 * \file	stse_asymmetric_keys_management.c
 * \brief   STSE Asymmetric keys management API (sources)
 * \author  STMicroelectronics - SMD application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <api/stse_asymmetric_keys_management.h>

/* Static functions declaration ----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_get_ecc_key_slots_count(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pPrivate_key_slot_count)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pPrivate_key_slot_count == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_query_private_key_slots_count(pSTSE, pPrivate_key_slot_count);

	return ret;
}

stse_ReturnCode_t stse_get_ecc_key_table_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_count,
		PLAT_UI8 * pChange_right,
		PLAT_UI16 * pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_table_info)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(private_key_table_info == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_query_private_key_table(pSTSE, private_key_slot_count, pChange_right, pGlobal_usage_limit, private_key_table_info);

	return ret;
}

stse_ReturnCode_t stse_get_ecc_key_slot_info(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_number,
		PLAT_UI8 * pChange_right,
		PLAT_UI16 * pGlobal_usage_limit,
		stsafea_private_key_slot_information_t * private_key_slot_info)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(private_key_slot_info == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	PLAT_UI8 private_key_slot_count;
	PLAT_UI8 slot_info_index;

	ret = stsafea_query_private_key_slots_count(pSTSE, &private_key_slot_count);

	stsafea_private_key_slot_information_t private_key_table_info[private_key_slot_count];

	ret = stsafea_query_private_key_table(pSTSE, private_key_slot_count, pChange_right, pGlobal_usage_limit, private_key_table_info);

	if(ret != STSE_OK)
	{
		return(ret);
	}

	for(slot_info_index=0; slot_info_index<private_key_slot_count; slot_info_index++)
	{
		if(private_key_table_info[slot_info_index].slot_number == private_key_slot_number)
		{
			memcpy(private_key_slot_info, &private_key_table_info[slot_info_index], sizeof(stsafea_private_key_slot_information_t));
			break;
		}
	}

	if(slot_info_index == private_key_slot_count)
	{
		return(STSE_API_KEY_NOT_FOUND);
	}

	return ret;
}

stse_ReturnCode_t stse_generate_ECDHE_key_pair(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 * pPublic_key)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_generate_ECDHE_key_pair(pSTSE, key_type, pPublic_key);

	return ret;
}

stse_ReturnCode_t stse_generate_ecc_key_pair(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI16 usage_limit,
		PLAT_UI8 * pPublic_key)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_generate_ecc_key_pair(pSTSE, slot_number, key_type, usage_limit, pPublic_key);

	return ret;
}

stse_ReturnCode_t stse_write_generic_ecc_public_key(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key)
{
	stse_ReturnCode_t ret;

	if(pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	ret = stsafea_write_generic_ecc_public_key(pSTSE, slot_number, key_type, pPublic_key);

	return ret;
}
