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
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "api/stse_device_management.h"

/* Exported variables --------------------------------------------------------*/

stse_perso_info_t dynamic_product_perso = {.cmd_encryption_status = 0,
					   .rsp_encryption_status = 0,
					   .ext_cmd_encryption_status = 0,
					   .ext_rsp_encryption_status = 0,
					   .cmd_AC_status = 0x5555555555555555,
					   .ext_cmd_AC_status = 0x5555555555555555};

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_init(stse_Handler_t *pSTSE)
{
	stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	switch (pSTSE->io.BusType) {
	case STSE_BUS_TYPE_I2C:
		ret = stse_platform_i2c_init(pSTSE->io.busID);
		if (ret != STSE_OK) {
			return ret;
		}
		break;
#ifdef STSE_CONF_USE_ST1WIRE
	case STSE_BUS_TYPE_ST1WIRE:
		ret = stse_platform_st1wire_init(pSTSE->io.busID);
		if (ret != STSE_OK) {
			return ret;
		}

		pSTSE->io.BusSendStart = stse_platform_st1wire_send_start;
		pSTSE->io.BusSendContinue = stse_platform_st1wire_send_continue;
		pSTSE->io.BusSendStop = stse_platform_st1wire_send_stop;
		pSTSE->io.BusRecvStart = stse_platform_st1wire_receive_start;
		pSTSE->io.BusRecvContinue = stse_platform_st1wire_receive_continue;
		pSTSE->io.BusRecvStop = stse_platform_st1wire_receive_stop;
		break;
#endif

	default:
		return (STSE_CORE_INVALID_PARAMETER);
	}

	if (ret != STSE_OK) {
		return ret;
	}

	/* - Initialize Host platform */
	ret = stse_platform_generate_random_init();
	if (ret != STSE_OK) {
		return ret;
	}
	ret = stse_platform_delay_init();
	if (ret != STSE_OK) {
		return ret;
	}
	ret = stse_platform_power_init();
	if (ret != STSE_OK) {
		return ret;
	}
	ret = stse_platform_crc16_init();
	if (ret != STSE_OK) {
		return ret;
	}
	ret = stse_platform_crypto_init();
	if (ret != STSE_OK) {
		return ret;
	}

#ifdef STSE_CONF_STSAFE_A_SUPPORT
	if (pSTSE->device_type != STSAFE_L010) {
		stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type]);

#ifndef STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS
		if (pSTSE->pPerso_info == NULL) {
			pSTSE->pPerso_info = &dynamic_product_perso;
			ret = stsafea_perso_info_update(pSTSE);
		}
#endif /* STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS */
	}
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

	return ret;
}

stse_ReturnCode_t stse_device_enter_hibernate(stse_Handler_t *pSTSE,
					      stse_hibernate_wake_up_mode_t wake_up_mode)
{
	stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	switch (pSTSE->device_type) {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
	case STSAFE_L010:
		ret = stsafel_hibernate(pSTSE);
		break;
#endif
#ifdef STSE_CONF_STSAFE_A_SUPPORT
	case STSAFE_A100:
	case STSAFE_A110:
	case STSAFE_A200:
		ret = stsafea_hibernate(pSTSE, wake_up_mode);
		break;
	case STSAFE_A120:
#endif
	default:
	}

	return ret;
}

stse_ReturnCode_t stse_device_power_on(stse_Handler_t *pSTSE)
{
	/* - Check STSE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	/* - Check STSE PowerLineOn callback initialization */
	if (pSTSE->io.PowerLineOn == NULL) {
		return (STSE_API_INVALID_PARAMETER);
	}

	/* - Power-on the device */
	pSTSE->io.PowerLineOn(pSTSE->io.busID, pSTSE->io.Devaddr);

	/* - Wait for device to boot (tboot) */
	stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type]);
	return (STSE_OK);
}

stse_ReturnCode_t stse_device_power_off(stse_Handler_t *pSTSE)
{
	/* - Check STSE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	/* - Check STSE PowerLineOff callback initialization */
	if (pSTSE->io.PowerLineOff == NULL) {
		return (STSE_API_INVALID_PARAMETER);
	}

	/* - Power-Off the device */
	pSTSE->io.PowerLineOff(pSTSE->io.busID, pSTSE->io.Devaddr);
	return (STSE_OK);
}

stse_ReturnCode_t stse_device_echo(stse_Handler_t *pSTSE, PLAT_UI8 *pIn, PLAT_UI8 *pOut,
				   PLAT_UI16 size)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	switch (pSTSE->device_type) {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
	case STSAFE_L010:
		return stsafel_echo(pSTSE, pIn, pOut, size);
#endif
#ifdef STSE_CONF_STSAFE_A_SUPPORT
	case STSAFE_A100:
	case STSAFE_A110:
	case STSAFE_A120:
	case STSAFE_A200:
		return stsafea_echo(pSTSE, pIn, pOut, size);
#endif
	default:
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}
}

stse_ReturnCode_t stse_device_lock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword,
				   PLAT_UI8 password_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 password_verification_status = 0;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	if (pSTSE->device_type == STSAFE_L010) {
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}

	if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH) {
		return STSE_API_INVALID_PARAMETER;
	}

	/*- Password submission */
	ret = stsafea_verify_password(pSTSE, pPassword, password_length,
				      &password_verification_status, NULL);

	if (ret != STSE_OK) {
		return ret;
	}

	if (password_verification_status == 0) {
		return STSE_API_INVALID_PARAMETER;
	}

	/* - Switch device Life-cycle to Lock */
	ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL_AND_LOCKED);

	return ret;
}

stse_ReturnCode_t stse_device_unlock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword,
				     PLAT_UI8 password_length)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 password_verification_status = 0;
	PLAT_UI8 remaining_tries;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	if (pSTSE->device_type == STSAFE_L010) {
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}

	if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH) {
		return STSE_API_INVALID_PARAMETER;
	}

	/*- Password submission */
	ret = stsafea_verify_password(pSTSE, pPassword, password_length,
				      &password_verification_status, &remaining_tries);

	if (ret != STSE_OK) {
		return ret;
	}

	if (password_verification_status == 0) {
		return STSE_API_INVALID_PARAMETER;
	}

	/* - Switch device Life-cycle to operational */
	ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL);

	return ret;
}

stse_ReturnCode_t stse_device_reset(stse_Handler_t *pSTSE)
{
	stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	switch (pSTSE->device_type) {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
	case STSAFE_L010:
		ret = stsafel_reset(pSTSE);
		break;
#endif
#ifdef STSE_CONF_STSAFE_A_SUPPORT
	case STSAFE_A100:
	case STSAFE_A110:
	case STSAFE_A120:
	case STSAFE_A200:
		ret = stsafea_reset(pSTSE);
		break;
#endif
	default:
	}

	return ret;
}

stse_ReturnCode_t stse_device_get_command_count(stse_Handler_t *pSTSE, PLAT_UI8 *pRecord_count)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	if (pSTSE->device_type == STSAFE_L010) {
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}

	return stsafea_get_command_count(pSTSE, pRecord_count);
}

stse_ReturnCode_t stse_device_get_command_AC_records(stse_Handler_t *pSTSE, PLAT_UI8 record_count,
						     stse_cmd_authorization_CR_t *pChange_rights,
						     stse_cmd_authorization_record_t *pRecord_table)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	if (pSTSE->device_type == STSAFE_L010) {
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}

	return stsafea_get_command_AC_table(pSTSE, record_count, pChange_rights, pRecord_table);
}

stse_ReturnCode_t stse_device_get_life_cycle_state(stse_Handler_t *pSTSE,
						   stsafea_life_cycle_state_t *pLife_cycle_state)
{
	/* - Check STSAFE handler initialization */
	if (pSTSE == NULL) {
		return (STSE_API_HANDLER_NOT_INITIALISED);
	}

	if (pSTSE->device_type == STSAFE_L010) {
		return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
	}

	return stsafea_query_life_cycle_state(pSTSE, pLife_cycle_state);
}
