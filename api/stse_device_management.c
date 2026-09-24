/**
  ******************************************************************************
  * @file    stse_device_management.c
  * @author  CS Application Team
  * @brief   STSE device management API set (sources)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "api/stse_device_management.h"

/* Exported variables --------------------------------------------------------*/
#define I2C_ADDR_MAX 0x7F
#define IDLE_BUS_DELAY_MAX 0x1F

/* Exported functions --------------------------------------------------------*/
stse_ReturnCode_t stse_init(stse_Handler_t *pSTSE)
{
  stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  PLAT_UI8 mask_id[STSAFEA_MASK_ID_SIZE];
  PLAT_UI16 mask_number;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  /* Initialization responsibilities at API layer:
   * 1) initialize bus backend based on configured transport,
   * 2) initialize host-side helper services (delay, crc, crypto, random),
   * 3) for STSAFE-A, optionally query mask id to refine detected product variant. */

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  switch (pSTSE->io.BusType)
  {
#if defined(STSE_CONF_STSAFE_A_SUPPORT) \
  || (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
    case STSE_BUS_TYPE_I2C:
      /* For I2C, callback table is preconfigured by platform integration layer. */
      ret = stse_platform_i2c_init(pSTSE->io.busID);
      if (ret != STSE_OK)
      {
        return ret;
      }
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT && defined(STSE_CONF_USE_I2C) */
#ifdef STSE_CONF_USE_ST1WIRE
    case STSE_BUS_TYPE_ST1WIRE:
      /* ST1WIRE requires explicit binding of transport primitives in handler IO table. */
      ret = stse_platform_st1wire_init(pSTSE->io.busID);
      if (ret != STSE_OK)
      {
        return ret;
      }

      pSTSE->io.BusSendStart = stse_platform_st1wire_send_start;
      pSTSE->io.BusSendContinue = stse_platform_st1wire_send_continue;
      pSTSE->io.BusSendStop = stse_platform_st1wire_send_stop;
      pSTSE->io.BusRecvStart = stse_platform_st1wire_receive_start;
      pSTSE->io.BusRecvContinue = stse_platform_st1wire_receive_continue;
      pSTSE->io.BusRecvStop = stse_platform_st1wire_receive_stop;
      break;
#endif /* STSE_CONF_USE_ST1WIRE */

    default:
      return (STSE_API_INVALID_PARAMETER);
  }

  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Initialize Host platform */
  ret = stse_platform_generate_random_init();
  if (ret != STSE_OK)
  {
    return ret;
  }
  ret = stse_platform_delay_init();
  if (ret != STSE_OK)
  {
    return ret;
  }
  ret = stse_platform_power_init();
  if (ret != STSE_OK)
  {
    return ret;
  }
  ret = stse_platform_crc16_init();
  if (ret != STSE_OK)
  {
    return ret;
  }
  ret = stse_platform_crypto_init();
  if (ret != STSE_OK)
  {
    return ret;
  }

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type != STSAFE_L010)
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type]);

#ifndef STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS
    /* Dynamic mask-id query is used to map legacy/unknown preset to exact STSAFE-A SKU. */
    ret = stsafea_query_mask_id(pSTSE, mask_id);
    if (ret != STSE_OK)
    {
      return ret;
    }

    mask_number = (mask_id[STSAFEA_MASK_ID_SIZE - 2] << 8) + mask_id[STSAFEA_MASK_ID_SIZE - 1];
    /* Threshold mapping follows ST internal mask-number allocation ranges. */
    if (mask_number >= 0x6000)
    {
      pSTSE->device_type = STSAFE_A120;
    }
    else if (mask_number >= 0x4600)
    {
      pSTSE->device_type = STSAFE_A110;
    }
    else if (mask_number >= 0x4000)
    {
      pSTSE->device_type = STSAFE_A100;
    }
    else
    {
      return (STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE);
    }

    ret = stsafea_perso_info_update(pSTSE);
#endif /* STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  return ret;
}

stse_ReturnCode_t stse_device_enter_hibernate(stse_Handler_t *pSTSE,
                                              stse_hibernate_wake_up_mode_t wake_up_mode)
{
  stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

  /* Hibernate capability differs by family; STSAFE-A120 path is intentionally unsupported. */

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  switch (pSTSE->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      ret = stsafel_hibernate(pSTSE);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
      ret = stsafea_hibernate(pSTSE, wake_up_mode);
      break;
    case STSAFE_A120:
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      break;
  }

  return ret;
}

stse_ReturnCode_t stse_device_power_on(stse_Handler_t *pSTSE)
{
  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* - Check STSE PowerLineOn callback initialization */
  if (pSTSE->io.PowerLineOn == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Power-on the device */
  pSTSE->io.PowerLineOn(pSTSE->io.busID, pSTSE->io.Devaddr);

  /* - Wait for device to boot (tboot) */
  /* Boot time lookup is device-family specific and expressed in milliseconds. */
  switch (pSTSE->device_type)
  {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type - STSAFE_A100]);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      stse_platform_Delay_ms(stsafel_boot_time[pSTSE->device_type - STSAFE_L010]);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    default:
      return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
  return (STSE_OK);
}

stse_ReturnCode_t stse_device_power_off(stse_Handler_t *pSTSE)
{
  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* - Check STSE PowerLineOff callback initialization */
  if (pSTSE->io.PowerLineOff == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Power-Off the device */
  /* API does not delay on power-off; caller controls power-down sequencing policy. */
  pSTSE->io.PowerLineOff(pSTSE->io.busID, pSTSE->io.Devaddr);
  return (STSE_OK);
}

stse_ReturnCode_t stse_device_echo(stse_Handler_t *pSTSE, PLAT_UI8 *pIn, PLAT_UI8 *pOut, PLAT_UI16 size)
{
  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  switch (pSTSE->device_type)
  {
      /* Echo command is delegated to family-specific service implementation. */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      return stsafel_echo(pSTSE, pIn, pOut, size);
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      return stsafea_echo(pSTSE, pIn, pOut, size);
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
}

stse_ReturnCode_t stse_device_lock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_ReturnCode_t ret;
  PLAT_UI8 password_verification_status = 0;

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /* API performs password verification first; lifecycle update is attempted only on success. */

  /*- Password submission */
  ret = stsafea_verify_password(pSTSE,
                                pPassword,
                                password_length,
                                &password_verification_status,
                                NULL);

  if (ret != STSE_OK)
  {
    return ret;
  }

  if (password_verification_status == 0)
  {
    /* Wrong password is normalized to API invalid-parameter status for caller simplicity. */
    return STSE_API_INVALID_PARAMETER;
  }

  /* - Switch device Life-cycle to Lock */
  ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL_AND_LOCKED);

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_device_unlock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_ReturnCode_t ret;
  PLAT_UI8 password_verification_status = 0;
  PLAT_UI8 remaining_tries;

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /*- Password submission */
  ret = stsafea_verify_password(pSTSE,
                                pPassword,
                                password_length,
                                &password_verification_status,
                                &remaining_tries);

  if (ret != STSE_OK)
  {
    return ret;
  }

  if (password_verification_status == 0)
  {
    /* remaining_tries is available to caller through low-level API when needed. */
    return STSE_API_INVALID_PARAMETER;
  }

  /* - Switch device Life-cycle to operational */
  ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL);

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_device_reset(stse_Handler_t *pSTSE)
{
  stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

  /* Reset implementation is delegated to family-specific service backends. */

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  switch (pSTSE->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      ret = stsafel_reset(pSTSE);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      ret = stsafea_reset(pSTSE);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      break;
  }

  return ret;
}

stse_ReturnCode_t stse_device_get_command_count(stse_Handler_t *pSTSE, PLAT_UI8 *pRecord_count)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Command count retrieval is available only on STSAFE-A personalization model. */
  /* Count value is later used by callers to size authorization table buffers. */
  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  return stsafea_get_command_count(pSTSE, pRecord_count);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_device_get_command_AC_records(stse_Handler_t *pSTSE,
                                                     PLAT_UI8 record_count,
                                                     stse_cmd_authorization_CR_t *pChange_rights,
                                                     stse_cmd_authorization_record_t *pRecord_table)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* record_count is caller-provided sizing guard for response parsing buffer. */
  /* API keeps record transport/parsing details in service layer implementation. */
  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  return stsafea_get_command_AC_table(pSTSE, record_count, pChange_rights, pRecord_table);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_device_get_life_cycle_state(stse_Handler_t *pSTSE,
                                                   stsafea_life_cycle_state_t *pLife_cycle_state)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Lifecycle query gives current device operational state without side effects. */
  /* Returned enum is direct secure-element state, not an API-side cached value. */
  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  return stsafea_query_life_cycle_state(pSTSE, pLife_cycle_state);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_put_i2c_parameters(
  stse_Handler_t *pSTSE,
  PLAT_UI8 i2c_address,
  stse_low_power_mode_t low_power_mode,
  PLAT_UI8 idle_bus_time_to_standby,
  PLAT_UI8 i2c_lock_parameters)
{

#ifdef STSE_CONF_STSAFE_A_SUPPORT

  if (pSTSE == NULL)
  {
    return STSE_API_HANDLER_NOT_INITIALISED;
  }

  if (i2c_address > I2C_ADDR_MAX || idle_bus_time_to_standby > IDLE_BUS_DELAY_MAX)
  {
    /* API enforces protocol limits before issuing irreversible parameter updates. */
    return STSE_API_INVALID_PARAMETER;
  }

  /*Create new I2C parameters structure */
  /* Structure is assembled locally so only validated fields reach secure element command. */
  stsafea_i2c_parameters_t i2c_param = {0};
  i2c_param.i2c_address = i2c_address;
  i2c_param.idle_bus_time_to_standby = idle_bus_time_to_standby;
  i2c_param.low_power_mode = low_power_mode;
  i2c_param.i2c_paramers_lock = i2c_lock_parameters;

  /*- Update I2C parameters*/
  return stsafea_put_i2c_parameters(pSTSE, &i2c_param);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
