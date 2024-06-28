/*!
 * ******************************************************************************
 * \file	stse_device.c
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

#include "core/stse_device.h"
#include "core/stse_platform.h"


stse_ReturnCode_t stse_set_default_handler_value(stse_Handler_t *pStseHandler)
{
  if (pStseHandler == NULL)
  {
    return STSE_CORE_HANDLER_NOT_INITIALISED;
  }
  
  pStseHandler->device_type = (stse_device_t)0;
  pStseHandler->pPerso_info = NULL;
  pStseHandler->pActive_host_session = NULL;
  pStseHandler->pActive_other_session = NULL;
  pStseHandler->io.BusRecvStart = stse_platform_i2c_receive_start;
  pStseHandler->io.BusRecvContinue = stse_platform_i2c_receive_continue;
  pStseHandler->io.BusRecvStop = stse_platform_i2c_receive_stop;
  pStseHandler->io.BusSendStart = stse_platform_i2c_send_start;
  pStseHandler->io.BusSendContinue = stse_platform_i2c_send_continue;
  pStseHandler->io.BusSendStop = stse_platform_i2c_send_stop;
  pStseHandler->io.IOLineGet = NULL;
  pStseHandler->io.BusWake = stse_platform_i2c_wake;
  pStseHandler->io.BusRecovery = NULL;
  pStseHandler->io.PowerLineOff = stse_platform_power_off;
  pStseHandler->io.PowerLineOn = stse_platform_power_on;
  pStseHandler->io.Busaddr = 0;
  pStseHandler->io.Devaddr = 0x20;
  pStseHandler->io.BusSpeed = 100;
  pStseHandler->io.BusType = STSE_BUS_TYPE_I2C;
  
  return STSE_OK;
}
