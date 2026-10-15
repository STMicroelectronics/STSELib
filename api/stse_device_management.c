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
stse_return_code_t stse_init_platform(void *p_arg)
{
  stse_return_code_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

  /* Initialize platform random number generator */
  ret = stse_platform_generate_random_init(p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Initialize platform timing and delay services */
  ret = stse_platform_delay_init(p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Initialize platform power management pins */
  ret = stse_platform_power_init(p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Initialize platform CRC calculation module */
  ret = stse_platform_crc16_init(p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Initialize platform cryptographic hardware or library */
  ret = stse_platform_crypto_init(p_arg);

  return ret;
}

stse_return_code_t stse_init_bus(stse_io_t *io, void *p_arg)
{
  stse_return_code_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

  switch (io->bus_type)
  {
#if defined(STSE_CONF_STSAFE_A_SUPPORT) || \
    (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
    /* Initialize I2C communication peripheral */
    case STSE_BUS_TYPE_I2C:
      ret = stse_platform_i2c_init(io->bus_id, p_arg);
      /* Check execution result status */
      if (ret != STSE_OK)
      {
        return ret;
      }
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT && defined(STSE_CONF_USE_I2C) */
#ifdef STSE_CONF_USE_ST1WIRE
    /* Initialize ST1Wire communication peripheral */
    case STSE_BUS_TYPE_ST1WIRE:
      ret = stse_platform_st1wire_init(io->bus_id, p_arg);
      /* Check execution result status */
      if (ret != STSE_OK)
      {
        return ret;
      }

      io->bus_send_start = stse_platform_st1wire_send_start;
      io->bus_send_continue = stse_platform_st1wire_send_continue;
      io->bus_send_stop = stse_platform_st1wire_send_stop;
      io->bus_recv_start = stse_platform_st1wire_receive_start;
      io->bus_recv_continue = stse_platform_st1wire_receive_continue;
      io->bus_recv_stop = stse_platform_st1wire_receive_stop;
      break;
#endif /* STSE_CONF_USE_ST1WIRE */

    default:
      return (STSE_PLATFORM_INVALID_PARAMETER);
  }

  return ret;
}

stse_return_code_t stse_init_device(stse_handle_t *p_stse, void *p_arg)
{
  stse_return_code_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  PLAT_UI8 mask_id[STSAFEA_MASK_ID_SIZE];
  PLAT_UI16 mask_number;
  PLAT_UI8 st_number[STSAFEA_ST_NUMBER_SIZE];
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  ret = stse_device_power_on(p_stse);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type != STSAFE_L010)
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifndef STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS
    ret = stsafea_query_mask_id_st_number(p_stse, mask_id, st_number);
    /* Check execution result status */
    if (ret != STSE_OK)
    {
      return ret;
    }

    mask_number = (mask_id[STSAFEA_MASK_ID_SIZE - 2] << 8) + mask_id[STSAFEA_MASK_ID_SIZE - 1];
    if (mask_number >= 0x6000)
    {
      p_stse->device_type = STSAFE_A120;
    }
    else if (mask_number >= 0x4600)
    {
      p_stse->device_type = STSAFE_A110;
    }
    else if (mask_number >= 0x4000)
    {
      p_stse->device_type = STSAFE_A100;
    }
    else
    {
      return (STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE);
    }

    ret = stsafea_perso_info_update(p_stse);
#endif /* STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  return ret;
}

stse_return_code_t stse_init(stse_handle_t *p_stse, void *p_arg)
{
  stse_return_code_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

  /* - Check STSAFE handler initialization */
  /* Verify device handle pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* - Initialize communication bus */
  ret = stse_init_bus(&p_stse->io, p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Initialize Host platform */
  ret = stse_init_platform(p_arg);
  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* - Initialize STSE device */
  ret = stse_init_device(p_stse, p_arg);

  return ret;
}

stse_return_code_t stse_device_power_on(stse_handle_t *p_stse)
{
  /* - Check STSE handler initialization */
  /* Verify device handle pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* - Check STSE power_line_on callback initialization */
  if (p_stse->io.power_line_on == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Power-on the device */
  p_stse->io.power_line_on(p_stse->io.bus_id, p_stse->io.dev_addr);

  /* - Wait for device to boot (tboot) */
  switch (p_stse->device_type)
  {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      stse_platform_delay_ms(stsafea_boot_time[p_stse->device_type - STSAFE_A100]);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      stse_platform_delay_ms(stsafel_boot_time[p_stse->device_type - STSAFE_L010]);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    default:
      return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
  return (STSE_OK);
}

stse_return_code_t stse_device_power_off(stse_handle_t *p_stse)
{
  /* - Check STSE handler initialization */
  /* Verify device handle pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* - Check STSE power_line_off callback initialization */
  if (p_stse->io.power_line_off == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Power-Off the device */
  p_stse->io.power_line_off(p_stse->io.bus_id, p_stse->io.dev_addr);
  return (STSE_OK);
}

stse_return_code_t stse_device_lock(stse_handle_t *p_stse, PLAT_UI8 *p_password, PLAT_UI8 password_length)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_return_code_t ret;
  PLAT_UI8 password_verification_status = 0;

  /* - Check STSAFE handler initialization */
  /* Verify device handle pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  if (p_password == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /*- Password submission */
  ret = stsafea_verify_password(p_stse,
                                p_password,
                                password_length,
                                &password_verification_status,
                                NULL);

  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  if (password_verification_status == 0)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /* - Switch device Life-cycle to Lock */
  ret = stsafea_put_life_cyle_state(p_stse, STSAFEA_LCS_OPERATIONAL_AND_LOCKED);

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_device_unlock(stse_handle_t *p_stse, PLAT_UI8 *p_password, PLAT_UI8 password_length)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_return_code_t ret;
  PLAT_UI8 password_verification_status = 0;
  PLAT_UI8 remaining_tries;

  /* - Check STSAFE handler initialization */
  /* Verify device handle pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type == STSAFE_L010)
  {
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

  if (p_password == NULL || password_length != STSAFEA_PASSWORD_LENGTH)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /*- Password submission */
  ret = stsafea_verify_password(p_stse,
                                p_password,
                                password_length,
                                &password_verification_status,
                                &remaining_tries);

  /* Check execution result status */
  if (ret != STSE_OK)
  {
    return ret;
  }

  if (password_verification_status == 0)
  {
    return STSE_API_INVALID_PARAMETER;
  }

  /* - Switch device Life-cycle to operational */
  ret = stsafea_put_life_cyle_state(p_stse, STSAFEA_LCS_OPERATIONAL);

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
