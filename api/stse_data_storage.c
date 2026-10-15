/**
  ******************************************************************************
  * @file    stse_data_storage.c
  * @author  CS Application Team
  * @brief   STSE secure storage API (sources)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "api/stse_data_storage.h"
#include <string.h>

stse_return_code_t stse_data_storage_read_data_zone(
  stse_handle_t *p_stse,
  PLAT_UI32 zone,
  PLAT_UI16 offset,
  PLAT_UI8 *p_buffer,
  PLAT_UI16 length,
  PLAT_UI16 chunk_size,
  stse_cmd_protection_t protection)
{
  stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  PLAT_UI16 remaning_length = length;
  PLAT_UI16 chunk_length = 0;
  PLAT_UI16 chunk_offset = offset;
  union
  {
    stsafea_read_option_t stsafea;
    stsafel_read_option_t stsafel;
  } read_option;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Check buffer validity when length is strictly positive */
  if ((length > 0U) && (p_buffer == NULL))
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set read option with all zero */
  memset(&read_option, 0, sizeof(read_option));

  /* Perform chunk-by-chunk read until all requested bytes are read */
  do
  {
    /* Determine size of current chunk based on remaining length and configured chunk size */
    if ((chunk_size != 0) && (remaning_length > chunk_size))
    {
      /* Limit current transfer to maximum chunk size */
      chunk_length = chunk_size;
    }
    else
    {
      /* Transfer all remaining bytes in this chunk */
      chunk_length = remaning_length;
    }

    /* Dispatch read command according to target device family */
    switch (p_stse->device_type)
    {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
      case STSAFE_L010:
        /* Execute data zone read on STSAFE-L010 device */
        ret = stsafel_read_data_zone(
                p_stse,
                zone,
                read_option.stsafel,
                chunk_offset,
                p_buffer + (chunk_offset) - offset,
                chunk_length,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
      case STSAFE_A100:
      case STSAFE_A110:
      case STSAFE_A120:
        /* Execute data zone read on STSAFE-A device */
        ret = stsafea_read_data_zone(
                p_stse,
                zone,
                read_option.stsafea,
                chunk_offset,
                p_buffer + (chunk_offset) - offset,
                chunk_length,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
      default:
        /* Unsupported or unrecognized secure element device */
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
    /* Check if transfer was successful before updating offsets */
    if (ret != STSE_OK)
    {
      /* Return error code immediately upon failure */
      return ret;
    }
    /* Decrement remaining length and advance chunk offset for next iteration */
    remaning_length = (remaning_length - chunk_length);
    chunk_offset = (chunk_offset + chunk_length);
  } while (remaning_length > 0);

  /* Return overall operation status code */
  return ret;
}

stse_return_code_t stse_data_storage_update_data_zone(
  stse_handle_t *p_stse,
  PLAT_UI32 zone,
  PLAT_UI16 offset,
  const PLAT_UI8 *p_buffer,
  PLAT_UI16 length,
  stse_zone_update_atomicity_t atomicity,
  stse_cmd_protection_t protection)
{

  stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  union
  {
    stsafea_update_option_t stsafea;
    stsafel_update_option_t stsafel;
  } update_option;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Check buffer validity when length is strictly positive */
  if ((length > 0U) && (p_buffer == NULL))
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set update option with all zero for both members of the union as they act similarly */
  memset(&update_option, 0, sizeof(update_option));

  /* Dispatch update command according to target device family */
  switch (p_stse->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      /* Execute data zone update on STSAFE-L010 device */
      ret = stsafel_update_data_zone(
              p_stse,
              zone,
              update_option.stsafel,
              offset,
              p_buffer,
              length,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      /* Set atomicity option for STSAFE-A device */
      update_option.stsafea.atomicity = atomicity;
      /* Execute data zone update on STSAFE-A device */
      ret = stsafea_update_data_zone(
              p_stse,
              zone,
              update_option.stsafea,
              offset,
              p_buffer,
              length,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      /* Unsupported or unrecognized secure element device */
      ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
      break;
  }

  /* Return STSE status code */
  return ret;
}

stse_return_code_t stse_data_storage_decrement_counter_zone(
  stse_handle_t *p_stse,
  PLAT_UI32 zone,
  PLAT_UI32 amount,
  PLAT_UI16 offset,
  PLAT_UI8 *p_buffer,
  PLAT_UI16 length,
  PLAT_UI32 *new_counter_value,
  stse_cmd_protection_t protection)
{
  stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  union
  {
    stsafea_decrement_option_t stsafea;
    stsafel_decrement_option_t stsafel;
  } decrement_option;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Validate output parameter pointer */
  if (new_counter_value == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Check associated data buffer validity if length is non-zero */
  if ((length > 0U) && (p_buffer == NULL))
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set decrement option with all zero for both members of the union as they act similarly */
  memset(&decrement_option, 0, sizeof(decrement_option));

  /* Dispatch counter decrement command according to target device family */
  switch (p_stse->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      /* Execute counter zone decrement on STSAFE-L010 device */
      ret = stsafel_decrement_counter_zone(
              p_stse,
              zone,
              decrement_option.stsafel,
              amount,
              offset,
              p_buffer,
              length,
              new_counter_value,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      /* Execute counter zone decrement on STSAFE-A device */
      ret = stsafea_decrement_counter_zone(
              p_stse,
              zone,
              decrement_option.stsafea,
              amount,
              offset,
              p_buffer,
              length,
              new_counter_value,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      /* Unsupported or unrecognized secure element device */
      ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
      break;
  }

  /* Return STSE status code */
  return ret;
}

stse_return_code_t stse_data_storage_read_counter_zone(
  stse_handle_t *p_stse,
  PLAT_UI32 zone,
  PLAT_UI16 offset,
  PLAT_UI8 *p_buffer,
  PLAT_UI16 length,
  PLAT_UI16 chunk_size,
  PLAT_UI32 *p_counter_value,
  stse_cmd_protection_t protection)
{
  stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  PLAT_UI16 remaning_length = length;
  PLAT_UI16 chunk_length = 0;
  PLAT_UI16 chunk_offset = offset;
  union
  {
    stsafea_read_option_t stsafea;
    stsafel_read_option_t stsafel;
  } read_option;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Validate output counter pointer */
  if (p_counter_value == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Check buffer validity when length is strictly positive */
  if ((length > 0U) && (p_buffer == NULL))
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set read option with all zero for both members of the union as they act similarly */
  memset(&read_option, 0, sizeof(read_option));

  /* Perform chunk-by-chunk read until all requested bytes are read or error occurs */
  do
  {
    /* Calculate size of current chunk based on remaining length and chunk size limit */
    if ((chunk_size != 0) && (remaning_length > chunk_size))
    {
      /* Limit chunk size to specified maximum */
      chunk_length = chunk_size;
    }
    else
    {
      /* Read entire remaining length */
      chunk_length = remaning_length;
    }

    /* Dispatch read counter command according to target device family */
    switch (p_stse->device_type)
    {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
      case STSAFE_L010:
        /* Execute counter zone read on STSAFE-L010 device */
        ret = stsafel_read_counter_zone(
                p_stse,
                zone,
                read_option.stsafel,
                chunk_offset,
                p_buffer + (chunk_offset) - offset,
                chunk_length,
                p_counter_value,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
      case STSAFE_A100:
      case STSAFE_A110:
      case STSAFE_A120:
        /* Execute counter zone read on STSAFE-A device */
        ret = stsafea_read_counter_zone(
                p_stse,
                zone,
                read_option.stsafea,
                chunk_offset,
                p_buffer + (chunk_offset) - offset,
                chunk_length,
                p_counter_value,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
      default:
        /* Unsupported or unrecognized secure element device */
        ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
        break;
    }

    /* Update remaining length and chunk offset for next iteration */
    remaning_length = (remaning_length - chunk_length);
    chunk_offset = (chunk_offset + chunk_length);
  } while ((remaning_length > 0) && (ret == STSE_OK));

  /* Return overall operation status code */
  return ret;
}

stse_return_code_t stse_data_storage_change_read_access_condition(
  stse_handle_t *p_stse,
  PLAT_UI32 zone,
  stse_zone_ac_t ac,
  stse_ac_change_right_t ac_change_right,
  stse_cmd_protection_t protection)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  volatile stse_return_code_t ret = STSE_API_INVALID_PARAMETER;

  stsafea_read_option_t options;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set access condition change flag */
  options.change_ac_indicator = STSE_AC_CHANGE;
  /* Clear unused filler field */
  options.filler = 0;
  /* Set target read access condition */
  options.new_read_ac = ac;
  /* Set access condition modification rights */
  options.new_read_ac_change_right = ac_change_right;

  /* Read zone with length 0 to apply new access condition without reading data */
  ret = stsafea_read_data_zone(
          p_stse,
          zone,
          options,
          0x00,
          NULL,
          0x00,
          protection);

  /* Return status code from STSAFE-A operation */
  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_data_storage_change_update_access_condition(stse_handle_t *p_stse,
                                                                    PLAT_UI32 zone,
                                                                    stse_zone_ac_t ac,
                                                                    stse_ac_change_right_t ac_change_right,
                                                                    PLAT_UI16 offset,
                                                                    PLAT_UI8 *p_buffer,
                                                                    PLAT_UI16 length,
                                                                    stse_zone_update_atomicity_t atomicity,
                                                                    stse_cmd_protection_t protection)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  volatile stse_return_code_t ret = STSE_API_INVALID_PARAMETER;
  stsafea_update_option_t options;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set access condition change flag */
  options.change_ac_indicator = STSE_AC_CHANGE;
  /* Clear unused filler field */
  options.filler = 0;
  /* Set target update access condition */
  options.new_update_ac = ac;
  /* Set access condition modification rights */
  options.new_update_ac_change_right = ac_change_right;
  /* Set update atomicity requirement */
  options.atomicity = atomicity;

  /* Perform update zone command to apply new access condition and optionally update data */
  ret = stsafea_update_data_zone(
          p_stse,
          zone,
          options,
          offset,
          p_buffer,
          length,
          protection);

  /* Return status code from STSAFE-A operation */
  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_data_storage_change_decrement_access_condition(stse_handle_t *p_stse,
                                                                       PLAT_UI32 zone,
                                                                       stse_zone_ac_t ac,
                                                                       stse_ac_change_right_t ac_change_right,
                                                                       PLAT_UI32 amount,
                                                                       PLAT_UI16 offset,
                                                                       PLAT_UI8 *p_buffer,
                                                                       PLAT_UI16 length,
                                                                       PLAT_UI32 *new_counter_value,
                                                                       stse_cmd_protection_t protection)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stsafea_decrement_option_t options;

  /* Check STSE handler pointer validity */
  if (p_stse == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Set access condition change flag */
  options.change_ac_indicator = STSE_AC_CHANGE;
  /* Clear unused filler field */
  options.filler = 0;
  /* Set target decrement access condition */
  options.new_decrement_ac = ac;
  /* Set access condition modification rights */
  options.new_decrement_ac_change_right = ac_change_right;

  /* Execute decrement command with updated access condition and update counter value */
  return stsafea_decrement_counter_zone(p_stse,
                                        zone,
                                        options,
                                        amount,
                                        offset,
                                        p_buffer,
                                        length,
                                        new_counter_value,
                                        protection);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
