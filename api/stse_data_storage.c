/*!
 ******************************************************************************
 * \file	stse_data_storage.c
 * \brief   STSE secure storage API (sources)
 * \author  STMicroelectronics - CS application team
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

#include <string.h>
#include <api/stse_data_storage.h>

stse_ReturnCode_t stse_data_storage_get_total_partition_count(
		stse_Handler_t* pSTSE,
		PLAT_UI8* total_partition_count)
{
	/* - Check device type */
	if (pSTSE->device_type == STSAFE_L010)
	{
		return( STSE_API_INCOMPATIBLE_DEVICE_TYPE );
	}

	return stsafea_get_total_partition_count(pSTSE, total_partition_count);
}

stse_ReturnCode_t stse_data_storage_get_partitioning_table(
		stse_Handler_t* pSTSE,
		PLAT_UI8 total_partition_count,
		stsafea_data_partition_record_t*  pPartitioning_table,
		PLAT_UI16 Partitioning_table_length)
{
	/* - Check device type */
	if (pSTSE->device_type == STSAFE_L010)
	{
		return( STSE_API_INCOMPATIBLE_DEVICE_TYPE );
	}

	return stsafea_get_data_partitions_configuration (
			pSTSE,
			total_partition_count,
			pPartitioning_table,
			Partitioning_table_length);
}

stse_ReturnCode_t stse_data_storage_read_data_zone(
		stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI16 chunk_size,
		stse_cmd_protection_t protection)
{
	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	PLAT_UI16 remaning_length = length;
	PLAT_UI16 chunck_length = 0;
	PLAT_UI16 chunck_offset = offset;
	union {
		stsafea_read_option_t stsafea;
		stsafel_read_option_t stsafel;
	} read_option;

	/* - Check STSE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Set read option with all zero for both members of the union as they act similarly */
	memset(&read_option, 0, sizeof(read_option));

	/* - Perform read by chunk */
	do {
		if ( (chunk_size != 0) && (remaning_length > chunk_size) )
		{
			chunck_length = chunk_size;
		} else {
			chunck_length = remaning_length;
		}

		/* - Transfer command/response */
		if (pSTSE->device_type == STSAFE_L010)
		{
			ret = stsafel_read_data_zone(
					pSTSE,
					zone,
					read_option.stsafel,
					chunck_offset,
					pBuffer + (chunck_offset)-offset,
					chunck_length,
					protection);
		} else {
			ret = stsafea_read_data_zone(
					pSTSE,
					zone,
					read_option.stsafea,
					chunck_offset,
					pBuffer + (chunck_offset)-offset,
					chunck_length,
					protection);
		}

		/* - Check if Transfer successful and format response data */
		if (ret != STSE_OK)
		{
			/* - (ERROR) Return communication error code */
			return(ret);
		}

		/* - Decrement Length value */
		remaning_length = (remaning_length - chunck_length);
		chunck_offset = (chunck_offset + chunck_length);

	} while (remaning_length > 0);

	/* - Return STSE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_update_data_zone(
		stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		stse_zone_update_atomicity_t atomicity,
		stse_cmd_protection_t protection
){

	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	union {
		stsafea_update_option_t stsafea;
		stsafel_update_option_t stsafel;
	} update_option;

	/* - Check STSE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Set update option with all zero for both members of the union as they act similarly */
	memset(&update_option, 0, sizeof(update_option));

	/*- Transfer command/response */
	if (pSTSE->device_type == STSAFE_L010)
	{
		ret = stsafel_update_data_zone(
				pSTSE,
				zone,
				update_option.stsafel,
				offset,
				pBuffer,
				length,
				protection);
	} else {
		ret = stsafea_update_data_zone(
				pSTSE,
				zone,
				update_option.stsafea,
				offset,
				pBuffer,
				length,
				protection);
	}

	/* - Return STSE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_decrement_counter_zone(
		stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		PLAT_UI32 amount,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI32 *new_counter_value,
		stse_cmd_protection_t protection)
{
	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	union {
		stsafea_decrement_option_t stsafea;
		stsafel_decrement_option_t stsafel;
	} decrement_option;

	/* - Check STSE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Set decrement option with all zero for both members of the union as they act similarly */
	memset(&decrement_option, 0, sizeof(decrement_option));

	/*- Transfer command/response */
	if (pSTSE->device_type == STSAFE_L010)
	{
		ret = stsafel_decrement_counter_zone(
				pSTSE,
				zone,
				decrement_option.stsafel,
				amount,
				offset,
				pBuffer,
				length,
				new_counter_value,
				protection);
	} else {
		ret = stsafea_decrement_counter_zone(
				pSTSE,
				zone,
				decrement_option.stsafea,
				amount,
				offset,
				pBuffer,
				length,
				new_counter_value,
				protection);
	}

		/* - Return STSAFE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_read_counter_zone(
		stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI16 chunk_size,
		PLAT_UI32 *pCounter_value,
		stse_cmd_protection_t protection)
{
	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	PLAT_UI16 remaning_length = length;
	PLAT_UI16 chunck_length = 0;
	PLAT_UI16 chunck_offset = offset;
	union {
		stsafea_read_option_t stsafea;
		stsafel_read_option_t stsafel;
	} read_option;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Set read option with all zero for both members of the union as they act similarly */
	memset(&read_option, 0, sizeof(read_option));

	/*- Perform read by chunk */
	do {
		if ( (chunk_size != 0) && (remaning_length > chunk_size) )
		{
			chunck_length = chunk_size;
		} else {
			chunck_length = remaning_length;
		}

		/* - Transfer command/response */
		if (pSTSE->device_type == STSAFE_L010)
		{
			ret = stsafel_read_counter_zone(
					pSTSE,
					zone,
					read_option.stsafel,
					chunck_offset,
					pBuffer + (chunck_offset)-offset,
					chunck_length,
					pCounter_value,
					protection);
		} else {
			ret = stsafea_read_counter_zone(
					pSTSE,
					zone,
					read_option.stsafea,
					chunck_offset,
					pBuffer + (chunck_offset)-offset,
					chunck_length,
					pCounter_value,
					protection);
		}

		/*- Check if Transfer successful and format response data */
		if (ret != STSE_OK)
		{
			return(ret);
		}

		/* - Decrement Length value */
		remaning_length = (remaning_length - chunck_length);
		chunck_offset = (chunck_offset + chunck_length);
	} while (remaning_length > 0);

	/* - Return STSAFE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_change_read_access_condition(
		stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		stse_zone_ac_t ac,
		stse_ac_change_right_t ac_change_right,
		stse_cmd_protection_t protection)
{
	volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

	stsafea_read_option_t options;

	options.change_ac_indicator = STSE_AC_IGNORE;
	options.filler = 0;
	options.new_read_ac = ac;
	options.new_read_ac_change_right = ac_change_right;

	/*- Read Zone (single access) */
	ret = stsafea_read_data_zone(
			pSTSE,
			zone,
			options,
			0x00,
			NULL,
			0x00,
			protection
	);

	/* - Return STSE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_change_update_access_condition(stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		stse_zone_ac_t ac,
		stse_ac_change_right_t ac_change_right,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		stse_zone_update_atomicity_t atomicity,
		stse_cmd_protection_t protection)
{
	volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	stsafea_update_option_t options;

	/*- Prepare update options */
	options.change_ac_indicator = STSE_AC_IGNORE;
	options.filler = 0;
	options.new_update_ac = ac;
	options.new_update_ac_change_right = ac_change_right;
	options.atomicity = atomicity;

	/*- Update zone */
	ret = stsafea_update_data_zone(
			pSTSE,
			zone,
			options,
			offset,
			pBuffer,
			length,
			protection
	);

	/* - Return STSE Status code */
	return(ret);
}

stse_ReturnCode_t stse_data_storage_change_decrement_access_condition(stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		stse_zone_ac_t ac,
		stse_ac_change_right_t ac_change_right,
		PLAT_UI32 amount,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI32 *new_counter_value,
		stse_cmd_protection_t protection)
{
	stsafea_decrement_option_t options;

	options.change_ac_indicator = STSE_AC_IGNORE;
	options.filler = 0;
	options.new_decrement_ac = ac;
	options.new_decrement_ac_change_right = ac_change_right;

	/* - Check STSE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	return stsafea_decrement_counter_zone(pSTSE,
			zone,
			options,
			amount,
			offset,
			pBuffer,
			length,
			new_counter_value,
			protection
	);
}

