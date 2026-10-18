/*!
 * ******************************************************************************
 * \file	stse_device.c
 * \brief   STSELib api for device (sources)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component. If no LICENSE file comes
 * with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include <string.h>

stse_return_code_t
stse_set_default_handler_value(stse_handle_t *p_stse_handle) {
  if (p_stse_handle == NULL) {
    return STSE_CORE_HANDLER_NOT_INITIALISED;
  }

  p_stse_handle->device_type = (stse_device_t)0;
  memset(&p_stse_handle->perso_info, 0, sizeof(p_stse_handle->perso_info));
  p_stse_handle->p_active_host_session = NULL;
  p_stse_handle->p_active_other_session = NULL;
#if defined(STSE_CONF_STSAFE_A_SUPPORT) ||                                     \
    (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
  p_stse_handle->io.bus_recv_start = stse_platform_i2c_receive_start;
  p_stse_handle->io.bus_recv_continue = stse_platform_i2c_receive_continue;
  p_stse_handle->io.bus_recv_stop = stse_platform_i2c_receive_stop;
  p_stse_handle->io.bus_send_start = stse_platform_i2c_send_start;
  p_stse_handle->io.bus_send_continue = stse_platform_i2c_send_continue;
  p_stse_handle->io.bus_send_stop = stse_platform_i2c_send_stop;
  p_stse_handle->io.bus_wake = stse_platform_i2c_wake;
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT &&         \
          defined(STSE_CONF_USE_I2C) */
  p_stse_handle->io.io_line_get = NULL;
  p_stse_handle->io.bus_recovery = NULL;
  p_stse_handle->io.power_line_off = stse_platform_power_off;
  p_stse_handle->io.power_line_on = stse_platform_power_on;
  p_stse_handle->io.bus_id = 0;
  p_stse_handle->io.devaddr = 0x20;
  p_stse_handle->io.bus_speed = 100;
#if defined(STSE_CONF_STSAFE_A_SUPPORT) ||                                     \
    (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
  p_stse_handle->io.bus_type = STSE_BUS_TYPE_I2C;
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT &&         \
          defined(STSE_CONF_USE_I2C) */
  return STSE_OK;
}
