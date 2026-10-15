/**
  ******************************************************************************
  * @file    stse_random.c
  * @author  CS Application Team
  * @brief   STSE Random API set (sources)
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

#include "api/stse_random.h"

stse_return_code_t stse_generate_random(
  stse_handle_t *p_stse,
  PLAT_UI8 *p_random,
  PLAT_UI16 random_size)
{
  stse_return_code_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  /* - Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  if (random_size == 0)
  {
    return (STSE_OK);
  }

  if (p_random == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (p_stse->device_type != STSAFE_L010)
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    while (0 < random_size)
    {
      PLAT_UI16 chunk = (random_size < STSAFEA_MAXIMUM_RNG_SIZE) ? random_size : STSAFEA_MAXIMUM_RNG_SIZE;

      ret = stsafea_generate_random(p_stse, p_random, chunk);

      if (ret != STSE_OK)
      {
        break;
      }

      random_size -= chunk;
      p_random += chunk;
    }
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  return ret;
}
