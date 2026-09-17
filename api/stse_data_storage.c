/**
  ******************************************************************************
  * @file    stse_data_storage.c
  * @author  CS Application Team
  * @brief   STSE secure storage API (sources)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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

stse_ReturnCode_t stse_data_storage_get_total_partition_count(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *total_partition_count)
{

  stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

  /* Partition count query is routed only to STSAFE-A because STSAFE-L does not
   * expose the same partitioning table model in this API surface. */
  /* Output count is consumed by subsequent table-fetch API to size receive buffers. */

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type != STSAFE_L010)
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    ret = stsafea_get_total_partition_count(pSTSE, total_partition_count);
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  return ret;
}

stse_ReturnCode_t stse_data_storage_get_partitioning_table(
  stse_Handler_t *pSTSE,
  PLAT_UI8 total_partition_count,
  stsafea_data_partition_record_t *pPartitioning_table,
  PLAT_UI16 partitioning_table_size)
{

  stse_ReturnCode_t ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;

  /* Caller provides count and destination size so service layer can validate
   * packed records before writing decoded partition metadata. */
  /* Wrapper keeps STSAFE-A framing details hidden from application code. */

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  if (pSTSE->device_type != STSAFE_L010)
  {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    ret = stsafea_get_data_partitions_configuration(pSTSE,
                                                    total_partition_count,
                                                    pPartitioning_table,
                                                    partitioning_table_size);
#ifdef STSE_CONF_STSAFE_L_SUPPORT
  }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

  return ret;
}

stse_ReturnCode_t stse_data_storage_read_data_zone(
  stse_Handler_t *pSTSE,
  PLAT_UI32 zone,
  PLAT_UI16 offset,
  PLAT_UI8 *pBuffer,
  PLAT_UI16 length,
  PLAT_UI16 chunk_size,
  stse_cmd_protection_t protection)
{
  stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
  PLAT_UI16 remaning_length = length;
  PLAT_UI16 chunk_length = 0;
  PLAT_UI16 chunk_offset = offset;

  /* Large zone reads are split into transport-sized chunks while keeping
   * a monotonically increasing offset into the caller destination buffer. */
  union
  {
    stsafea_read_option_t stsafea;
    stsafel_read_option_t stsafel;
  } read_option;

  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Set read option with all zero for both members of the union as they act similarly */
  memset(&read_option, 0, sizeof(read_option));

  /* - Perform read by chunk */
  do
  {
    /* chunk_size==0 means "single transfer" for whole remaining length. */
    if ((chunk_size != 0) && (remaning_length > chunk_size))
    {
      chunk_length = chunk_size;
    }
    else
    {
      chunk_length = remaning_length;
    }

    /* - Transfer command/response */
    switch (pSTSE->device_type)
    {
        /* Device family dispatch is centralized here; option union keeps signatures aligned. */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
      case STSAFE_L010:
        ret = stsafel_read_data_zone(
                pSTSE,
                zone,
                read_option.stsafel,
                chunk_offset,
                pBuffer + (chunk_offset) - offset,
                chunk_length,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
      case STSAFE_A100:
      case STSAFE_A110:
      case STSAFE_A120:
        ret = stsafea_read_data_zone(
                pSTSE,
                zone,
                read_option.stsafea,
                chunk_offset,
                pBuffer + (chunk_offset) - offset,
                chunk_length,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
      default:
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
    /* - Check if Transfer successful and format response data */
    if (ret != STSE_OK)
    {
      /* - (ERROR) Return communication error code */
      return ret;
    }

    /* - Decrement Length value */
    /* Update loop cursor for next chunk transfer. */
    remaning_length = (remaning_length - chunk_length);
    chunk_offset = (chunk_offset + chunk_length);

  } while (remaning_length > 0);

  /* - Return STSE Status code */
  return ret;
}

stse_ReturnCode_t stse_data_storage_update_data_zone(
  stse_Handler_t *pSTSE,
  PLAT_UI32 zone,
  PLAT_UI16 offset,
  PLAT_UI8 *pBuffer,
  PLAT_UI16 length,
  stse_zone_update_atomicity_t atomicity,
  stse_cmd_protection_t protection)
{

  stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

  /* Update path writes caller-provided bytes once, with optional atomicity semantics
   * interpreted by the selected backend implementation. */
  /* Read-modify-write policy is delegated to secure element command behavior. */
  union
  {
    stsafea_update_option_t stsafea;
    stsafel_update_option_t stsafel;
  } update_option;

  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Set update option with all zero for both members of the union as they act similarly */
  memset(&update_option, 0, sizeof(update_option));

  /* No chunking in this wrapper: caller controls write size and offset explicitly. */

  switch (pSTSE->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
      ret = stsafel_update_data_zone(
              pSTSE,
              zone,
              update_option.stsafel,
              offset,
              pBuffer,
              length,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
      ret = stsafea_update_data_zone(
              pSTSE,
              zone,
              update_option.stsafea,
              offset,
              pBuffer,
              length,
              protection);
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }

  /* - Return STSE Status code */
  return ret;
}

stse_ReturnCode_t stse_data_storage_decrement_counter_zone(
  stse_Handler_t *pSTSE,
  PLAT_UI32 zone,
  PLAT_UI32 amount,
  PLAT_UI16 offset,
  PLAT_UI8 *pBuffer,
  PLAT_UI16 length,
  PLAT_UI32 *new_counter_value,
  stse_cmd_protection_t protection)
{
  stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

  /* Counter decrement can optionally couple with associated data write/read in one
   * secure-element operation to preserve transactional consistency. */
  /* new_counter_value receives the post-decrement value reported by the device. */
  union
  {
    stsafea_decrement_option_t stsafea;
    stsafel_decrement_option_t stsafel;
  } decrement_option;

  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Set decrement option with all zero for both members of the union as they act similarly */
  memset(&decrement_option, 0, sizeof(decrement_option));

  /* Backend chooses command variant based on family while preserving API contract. */

  switch (pSTSE->device_type)
  {
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
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
      break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
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
      break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
    default:
      ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
  }

  /* - Return STSAFE Status code */
  return ret;
}

stse_ReturnCode_t stse_data_storage_read_counter_zone(
  stse_Handler_t *pSTSE,
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
  PLAT_UI16 chunk_length = 0;
  PLAT_UI16 chunk_offset = offset;

  /* Counter-zone reads stream optional associated data by chunks and return
   * the latest counter value observed in the same command family response. */
  union
  {
    stsafea_read_option_t stsafea;
    stsafel_read_option_t stsafel;
  } read_option;

  /* - Check STSAFE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* - Set read option with all zero for both members of the union as they act similarly */
  memset(&read_option, 0, sizeof(read_option));

  /*- Perform read by chunk */
  do
  {
    /* Each iteration computes the next transport payload length. */
    if ((chunk_size != 0) && (remaning_length > chunk_size))
    {
      chunk_length = chunk_size;
    }
    else
    {
      chunk_length = remaning_length;
    }

    switch (pSTSE->device_type)
    {
        /* Same multi-family dispatch model as read_data_zone wrapper. */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
      case STSAFE_L010:
        ret = stsafel_read_counter_zone(
                pSTSE,
                zone,
                read_option.stsafel,
                chunk_offset,
                pBuffer + (chunk_offset) - offset,
                chunk_length,
                pCounter_value,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
      case STSAFE_A100:
      case STSAFE_A110:
      case STSAFE_A120:
        ret = stsafea_read_counter_zone(
                pSTSE,
                zone,
                read_option.stsafea,
                chunk_offset,
                pBuffer + (chunk_offset) - offset,
                chunk_length,
                pCounter_value,
                protection);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
      default:
        ret = STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
    /* - Decrement Length value */
    /* Advance destination pointer index represented via chunk_offset delta. */
    remaning_length = (remaning_length - chunk_length);
    chunk_offset = (chunk_offset + chunk_length);
  } while ((remaning_length > 0) && (ret == STSE_OK));

  /* - Return STSAFE Status code */
  return ret;
}

stse_ReturnCode_t stse_data_storage_change_read_access_condition(
  stse_Handler_t *pSTSE,
  PLAT_UI32 zone,
  stse_zone_ac_t ac,
  stse_ac_change_right_t ac_change_right,
  stse_cmd_protection_t protection)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

  stsafea_read_option_t options;

  /* Access-condition update is encoded as a special read command with AC-change
   * indicator set, no payload buffer, and zero transfer length. */

  options.change_ac_indicator = STSE_AC_CHANGE;
  options.filler = 0;
  options.new_read_ac = ac;
  options.new_read_ac_change_right = ac_change_right;

  /* Filler byte remains zero to match command structure reserved field. */

  /*- Read Zone (single access) */
  ret = stsafea_read_data_zone(
          pSTSE,
          zone,
          options,
          0x00,
          NULL,
          0x00,
          protection);

  /* - Return STSE Status code */
  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_data_storage_change_update_access_condition(stse_Handler_t *pSTSE,
                                                                   PLAT_UI32 zone,
                                                                   stse_zone_ac_t ac,
                                                                   stse_ac_change_right_t ac_change_right,
                                                                   PLAT_UI16 offset,
                                                                   PLAT_UI8 *pBuffer,
                                                                   PLAT_UI16 length,
                                                                   stse_zone_update_atomicity_t atomicity,
                                                                   stse_cmd_protection_t protection)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
  stsafea_update_option_t options;

  /* AC update for write permissions is piggybacked on UPDATE command options. */
  /* This avoids a dedicated command variant while preserving access-right checks. */

  /*- Prepare update options */
  options.change_ac_indicator = STSE_AC_CHANGE;
  options.filler = 0;
  options.new_update_ac = ac;
  options.new_update_ac_change_right = ac_change_right;
  options.atomicity = atomicity;

  /* atomicity value is passed through unchanged so backend enforces selected mode. */

  /*- Update zone */
  ret = stsafea_update_data_zone(
          pSTSE,
          zone,
          options,
          offset,
          pBuffer,
          length,
          protection);

  /* - Return STSE Status code */
  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_data_storage_change_decrement_access_condition(stse_Handler_t *pSTSE,
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
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stsafea_decrement_option_t options;

  /* Decrement AC change is encoded in decrement options while preserving normal
   * amount/offset/data semantics of the counter operation itself. */
  /* Caller can update access rules and decrement counter within one authenticated flow. */

  options.change_ac_indicator = STSE_AC_CHANGE;
  options.filler = 0;
  options.new_decrement_ac = ac;
  options.new_decrement_ac_change_right = ac_change_right;

  /* Reserved filler byte is explicitly zeroed for deterministic command image. */

  /* - Check STSE handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  return stsafea_decrement_counter_zone(pSTSE,
                                        zone,
                                        options,
                                        amount,
                                        offset,
                                        pBuffer,
                                        length,
                                        new_counter_value,
                                        protection);
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
