/**
  ******************************************************************************
  * @file    stse_asymmetric_keys_management.c
  * @author  CS Application Team
  * @brief   STSE Asymmetric keys management API (sources)
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
#include <stddef.h>
#include <string.h>

#include "api/stse_asymmetric_keys_management.h"

stse_return_code_t stse_get_ecc_key_slots_count(
  stse_handle_t *p_stse,
  PLAT_UI8 *p_private_key_slot_count)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_return_code_t ret;

  /* Check STSE handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Validate output parameter pointer */
  if (p_private_key_slot_count == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  /* Query private key slot count from target device */
  ret = stsafea_query_private_key_slots_count(p_stse, p_private_key_slot_count);

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_get_ecc_key_slot_info(
  stse_handle_t *p_stse,
  PLAT_UI8 private_key_slot_number,
  PLAT_UI16 *p_global_usage_limit,
  stsafea_private_key_slot_information_t *private_key_slot_info)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  stse_return_code_t ret;

  /* Check STSE handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Validate output parameter pointer */
  if (private_key_slot_info == NULL)
  {
    return (STSE_API_INVALID_PARAMETER);
  }

  PLAT_UI8 private_key_slot_count;
  PLAT_UI8 slot_info_index;

  /* Query total number of private key slots available */
  ret = stsafea_query_private_key_slots_count(p_stse, &private_key_slot_count);

  /* Allocate table to receive all private key slot information */
  stsafea_private_key_slot_information_t private_key_table_info[private_key_slot_count];

  /* Query private key table from target device */
  ret = stsafea_query_private_key_table(p_stse, private_key_slot_count, p_global_usage_limit, private_key_table_info);

  if (ret != STSE_OK)
  {
    return (ret);
  }

  /* Search for the matching private key slot number */
  for (slot_info_index = 0; slot_info_index < private_key_slot_count; slot_info_index++)
  {
    if (private_key_table_info[slot_info_index].slot_number == private_key_slot_number)
    {
      /* Copy the found slot information to caller buffer */
      memcpy(private_key_slot_info,
             &private_key_table_info[slot_info_index],
             sizeof(stsafea_private_key_slot_information_t));
      break;
    }
  }

  /* Return key not found if slot index was not found in table */
  if (slot_info_index == private_key_slot_count)
  {
    return (STSE_API_KEY_NOT_FOUND);
  }

  return ret;
#else
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_return_code_t stse_sign_for_generic_public_key_slot(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t private_key_type,
  PLAT_UI8 *p_private_key,
  stse_hash_algorithm_t hash_algo,
  PLAT_UI16 payload_length,
  PLAT_UI8 *p_payload,
  PLAT_UI8 *p_signature)
{
  (void)p_stse;
#if defined(STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED) || \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED) ||   \
    defined(STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED)

  stse_return_code_t ret;
  PLAT_UI16 hash_length = stsafea_hash_info_table[hash_algo].length;
  PLAT_UI8 hash_data[hash_length];

  /* Validate input parameters and algorithm bounds */
  if (p_private_key == NULL || p_payload == NULL || p_signature == NULL ||
      private_key_type >= STSE_ECC_KT_INVALID || hash_algo >= STSE_SHA_INVALID)
  {
    return STSE_API_INVALID_PARAMETER;
  }

#ifdef STSE_CONF_ECC_EDWARD_25519
  /* Ed25519 signs the payload directly without pre-hashing */
  if (private_key_type != STSE_ECC_KT_ED25519)
  {
#endif /* STSE_CONF_ECC_EDWARD_25519 */
    /* Compute the hash of the payload buffer */
    ret = stse_platform_hash_compute(
            hash_algo,
            p_payload, payload_length,
            hash_data, &hash_length);

    if (ret != STSE_OK)
    {
      return (ret);
    }
#ifdef STSE_CONF_ECC_EDWARD_25519
  }
#endif /* STSE_CONF_ECC_EDWARD_25519 */

  /* Sign hash or payload using the platform ECC service */
  ret = stse_platform_ecc_sign(
          private_key_type,
          p_private_key,
#ifdef STSE_CONF_ECC_EDWARD_25519
          (private_key_type == STSE_ECC_KT_ED25519) ? p_payload : hash_data,
          (private_key_type == STSE_ECC_KT_ED25519) ? payload_length : hash_length,
#else
          hash_data,
          hash_length,
#endif /* STSE_CONF_ECC_EDWARD_25519 */
          p_signature);

  return (ret);
#else
  return STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_USE_HOST_KEY_PROVISIONING_WRAPPED_AUTHENTICATED ||
          STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT_AUTHENTICATED ||
          STSE_CONF_USE_SYMMETRIC_KEY_PROVISIONING_WRAPPED_AUTHENTICATED */
}
