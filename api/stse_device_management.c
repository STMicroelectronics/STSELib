/*!
 ******************************************************************************
 * \file	stse_device_management.c
 * \brief   STSE device management API set (sources)
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

/* Includes ------------------------------------------------------------------*/

#include "api/stse_device_management.h"

/* Exported variables --------------------------------------------------------*/

stse_perso_info_t dynamic_product_perso = {0} ;

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_init(stse_Handler_t *pSTSE)
{
	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

	if(pSTSE != NULL)
	{
		switch(pSTSE->io.BusType)
		{
			case STSE_BUS_TYPE_I2C :
				ret = stse_platform_i2c_init(pSTSE->io.Busaddr);
				if(ret != STSE_OK)
				{
					return ret;
				}
				break;

			default :
				return (STSE_CORE_INVALID_PARAMETER);
		}

		if(ret != STSE_OK)
		{
			return ret;
		}

		/* - Initialize Host platform */
		ret = stse_services_platform_init();
		if(ret != STSE_OK)
		{
			return ret;
		}
		ret = stse_crypto_platform_init();
		if(ret != STSE_OK)
		{
			return ret;
		}

		/*- STSE device power-on delay */
		stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type]);

		if (pSTSE->pPerso_info == NULL)
		{
			pSTSE->pPerso_info = &dynamic_product_perso;
			ret = stsafea_perso_info_update(pSTSE);
		}

	}

	return ret;
}

stse_ReturnCode_t stse_device_enter_hibernate(stse_Handler_t *pSTSE,
										stsafea_hibernate_wake_up_mode_t wake_up_mode)
{
	return stsafea_hibernate(pSTSE, wake_up_mode);
}

stse_ReturnCode_t stse_device_power_on(stse_Handler_t * pSTSE)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Check STSAFE PowerLineOn callback initialization */
	if (pSTSE->io.PowerLineOn == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Power-on the device */
	pSTSE->io.PowerLineOn(pSTSE->io.Busaddr,pSTSE->io.Devaddr);

	/* - Wait for device to boot (tboot) */
	stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type]);
	return( STSE_OK );
}

stse_ReturnCode_t stse_device_power_off(stse_Handler_t * pSTSE)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Check STSAFE PowerLineOff callback initialization */
	if (pSTSE->io.PowerLineOff == NULL)
	{
		return( STSE_API_INVALID_PARAMETER );
	}

	/* - Power-Off the device */
	pSTSE->io.PowerLineOff(pSTSE->io.Busaddr,pSTSE->io.Devaddr);
	return( STSE_OK );
}

stse_ReturnCode_t stse_device_echo(stse_Handler_t *pSTSE, PLAT_UI8 *pIn, PLAT_UI8 *pOut, PLAT_UI16 size)
{
	return stsafea_echo(pSTSE, pIn, pOut, size);
}

stse_ReturnCode_t stse_device_lock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 password_verification_status = 0;

	if(pSTSE == NULL)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	if(pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/*- Password submission */
	ret = stsafea_verify_password(pSTSE,
							     pPassword,
								 password_length,
								 &password_verification_status,
								 NULL);

	if(ret != STSE_OK)
	{
		return ret;
	}

	if(password_verification_status == 0)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/* - Switch device Life-cycle to Lock */
	ret = stsafea_put_life_cyle_state(pSTSE,STSAFEA_LCS_OPERATIONAL_AND_LOCKED);

    return ret;
}

stse_ReturnCode_t stse_device_unlock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 password_verification_status = 0;
	PLAT_UI8 remaining_tries;

	if(pSTSE == NULL)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	if(pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/*- Password submission */
	ret = stsafea_verify_password(pSTSE,
								 pPassword,
								 password_length,
								 &password_verification_status,
								 &remaining_tries);

	if(ret != STSE_OK)
	{
		return ret;
	}

	if(password_verification_status == 0)
	{
		return STSE_API_INVALID_PARAMETER;
	}

	/* - Switch device Life-cycle to operational */
	ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL);

	return ret;
}

stse_ReturnCode_t stse_device_reset(stse_Handler_t *pSTSE)
{
	return stsafea_reset(pSTSE);
}


stse_ReturnCode_t stse_device_get_command_count(stse_Handler_t *pSTSE, PLAT_UI8 *pRecord_count)
{

	return stsafea_get_command_count(pSTSE,pRecord_count);

}

stse_ReturnCode_t stse_device_get_command_AC_records(stse_Handler_t *pSTSE,
										 PLAT_UI8 record_count,
										 stse_cmd_authorization_CR_t *pChange_rights,
										 stse_cmd_authorization_record_t *pRecord_table)
{
    return stsafea_get_command_AC_table(pSTSE, record_count,pChange_rights,pRecord_table);
}

stse_ReturnCode_t stse_device_get_life_cycle_state(stse_Handler_t *pSTSE,
		stsafea_life_cycle_state_t *pLife_cycle_state)
{
    return stsafea_query_life_cycle_state(pSTSE, pLife_cycle_state);
}

