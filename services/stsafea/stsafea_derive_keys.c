/**
  ******************************************************************************
  * @file    stsafea_derive_keys.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for derive keys command (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "services/stsafea/stsafea_derive_keys.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_derive_keys(
  stse_handle_t *p_stse,
  stsafea_hkdf_input_key_t *p_input_key,
  PLAT_UI8 extract_flag,
  PLAT_UI8 expand_flag,
  stsafea_hkdf_salt_t *p_salt,
  stsafea_hkdf_info_t *p_info,
  stsafea_hkdf_okm_description_t *p_okm_map,
  PLAT_UI8 okm_count,
  stsafea_hkdf_output_t *p_output)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_DERIVE_KEYS};
  PLAT_UI8 rsp_header;
  PLAT_UI8 hkdf_flags = 0;
  stsafea_hash_algorithm_identifier_t hash_algo = {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_256};

  /* -- Frame Element Declarations -- */
  stse_frame_element_t e_cmd_header;

  /* Input Key Elements */
  stse_frame_element_t e_input_source;
  stse_frame_element_t e_input_mode, e_input_length, e_input_value; /* For Command source */
  stse_frame_element_t e_input_slot;                            /* For Slot source */

  /* HKDF Param Elements */
  stse_frame_element_t e_hkdf_flags, e_hash_algo;

  /* Salt Elements */
  stse_frame_element_t e_salt_source;
  stse_frame_element_t e_salt_length, e_salt_value; /* For Command source */
  stse_frame_element_t e_salt_slot;               /* For Slot source */

  /* Info Elements */
  stse_frame_element_t e_info_length, e_info_value;

  /* Response Elements */
  stse_frame_element_t e_rsp_header, e_rsp_prk_slot;

  /* -- Auxiliary Buffers -- */
  PLAT_UI8 input_len_buf[2];
  PLAT_UI8 salt_len_buf[2];
  PLAT_UI8 info_len_buf[2];

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if (p_input_key == NULL || p_output == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }
  if (expand_flag && (p_okm_map == NULL || p_output->derived_keys == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  stse_frame_element_t e_okm_desc_length[okm_count], e_okm_destination[okm_count];
  stse_frame_element_t e_okm_data1[okm_count], e_okm_data2[okm_count];
  stse_frame_element_t e_rsp_okm_data[okm_count];

  PLAT_UI8 okm_desc_len_buf[okm_count][2];
  PLAT_UI8 okm_key_len_buf[okm_count][2];

  /* Prepare DERIVE KEYS Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);

  /* [HEADER] */
  e_cmd_header.length = STSAFEA_EXT_HEADER_SIZE;
  e_cmd_header.p_data = cmd_header;
  stse_frame_push_element(&cmd_frame, &e_cmd_header);

  /* [INPUT KEY SOURCE] */
  e_input_source.length = 1;
  e_input_source.p_data = &p_input_key->source;
  stse_frame_push_element(&cmd_frame, &e_input_source);

  if (p_input_key->source == STSAFEA_KEY_SOURCE_COMMAND)
  {
    input_len_buf[0] = (PLAT_UI8)(p_input_key->command.length >> 8);
    input_len_buf[1] = (PLAT_UI8)(p_input_key->command.length);

    e_input_mode.length = 1;
    e_input_mode.p_data = (PLAT_UI8 *)&p_input_key->command.mode_of_operation;

    e_input_length.length = 2;
    e_input_length.p_data = input_len_buf;

    e_input_value.length = p_input_key->command.length;
    e_input_value.p_data = p_input_key->command.data;

    /* [INPUT KEY MODE] */
    stse_frame_push_element(&cmd_frame, &e_input_mode);
    /* [INPUT KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_input_length);
    /* [INPUT KEY VALUE] */
    stse_frame_push_element(&cmd_frame, &e_input_value);
  }
  else
  {
    e_input_slot.length = 1;
    e_input_slot.p_data = &p_input_key->symmkey.slot_number;
    /* [INPUT KEY SLOT] */
    stse_frame_push_element(&cmd_frame, &e_input_slot);
  }

  if (extract_flag)
  {
    hkdf_flags |= (1 << STSAFEA_HKDF_FLAG_EXTRACT_POS);
  }
  if (expand_flag)
  {
    hkdf_flags |= (1 << STSAFEA_HKDF_FLAG_EXPAND_POS);
  }

  e_hkdf_flags.length = 1;
  e_hkdf_flags.p_data = &hkdf_flags;

  e_hash_algo.length = STSAFEA_HASH_ALGO_ID_SIZE;
  e_hash_algo.p_data = (PLAT_UI8 *)&hash_algo;

  /* [HKDF FLAGS] */
  stse_frame_push_element(&cmd_frame, &e_hkdf_flags);
  /* [HASH ALGORITHM] */
  stse_frame_push_element(&cmd_frame, &e_hash_algo);

  if (extract_flag)
  {
    if (p_salt == NULL)
    {
      return STSE_SERVICE_INVALID_PARAMETER;
    }

    e_salt_source.length = 1;
    e_salt_source.p_data = &p_salt->source;
    /* [SALT SOURCE] */
    stse_frame_push_element(&cmd_frame, &e_salt_source);

    if (p_salt->source == STSAFEA_KEY_SOURCE_COMMAND)
    {
      salt_len_buf[0] = (PLAT_UI8)(p_salt->command.length >> 8);
      salt_len_buf[1] = (PLAT_UI8)(p_salt->command.length);

      e_salt_length.length = 2;
      e_salt_length.p_data = salt_len_buf;
      /* [SALT LENGTH] */
      stse_frame_push_element(&cmd_frame, &e_salt_length);

      if (p_salt->command.length > 0 && p_salt->command.data != NULL)
      {
        e_salt_value.length = p_salt->command.length;
        e_salt_value.p_data = p_salt->command.data;
        /* [SALT VALUE] */
        stse_frame_push_element(&cmd_frame, &e_salt_value);
      }
    }
    else
    {
      e_salt_slot.length = 1;
      e_salt_slot.p_data = &p_salt->symmkey.slot_number;
      /* [SALT SLOT] */
      stse_frame_push_element(&cmd_frame, &e_salt_slot);
    }
  }

  if (expand_flag)
  {
    PLAT_UI16 info_len = (p_info != NULL) ? p_info->length : 0;
    info_len_buf[0] = (PLAT_UI8)(info_len >> 8);
    info_len_buf[1] = (PLAT_UI8)(info_len);

    e_info_length.length = 2;
    e_info_length.p_data = info_len_buf;
    /* [INFO LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_info_length);

    if (info_len > 0 && p_info != NULL && p_info->data != NULL)
    {
      e_info_value.length = info_len;
      e_info_value.p_data = p_info->data;
      /* [INFO VALUE] */
      stse_frame_push_element(&cmd_frame, &e_info_value);
    }
  }

  if (expand_flag)
  {
    for (PLAT_I32 i = 0; i < okm_count; i++)
    {
      stsafea_hkdf_okm_description_t *p_desc = &p_okm_map[i];
      PLAT_UI16 desc_len;

      /* Calculate Lengths and Pointers */
      if (p_desc->destination == STSAFEA_KEY_SOURCE_RESPONSE)
      {
        desc_len = 3; /* Dest(1) + KeyLen(2) */
        okm_desc_len_buf[i][0] = (PLAT_UI8)(desc_len >> 8);
        okm_desc_len_buf[i][1] = (PLAT_UI8)(desc_len);

        okm_key_len_buf[i][0] = (PLAT_UI8)(p_desc->response.key_length >> 8);
        okm_key_len_buf[i][1] = (PLAT_UI8)(p_desc->response.key_length);

        e_okm_desc_length[i].length = 2;
        e_okm_desc_length[i].p_data = okm_desc_len_buf[i];

        e_okm_destination[i].length = 1;
        e_okm_destination[i].p_data = (PLAT_UI8 *)&p_desc->destination;

        e_okm_data1[i].length = 2;
        e_okm_data1[i].p_data = okm_key_len_buf[i];

        e_okm_data2[i].length = 0;
        e_okm_data2[i].p_data = NULL;
      }
      else
      {
        if (p_desc->symmkey.key_info == NULL)
        {
          return STSE_SERVICE_INVALID_PARAMETER;
        }

        desc_len = 1 + p_desc->symmkey.key_info->info_length;
        okm_desc_len_buf[i][0] = (PLAT_UI8)(desc_len >> 8);
        okm_desc_len_buf[i][1] = (PLAT_UI8)(desc_len);

        e_okm_desc_length[i].length = 2;
        e_okm_desc_length[i].p_data = okm_desc_len_buf[i];

        e_okm_destination[i].length = 1;
        e_okm_destination[i].p_data = (PLAT_UI8 *)&p_desc->destination;

        e_okm_data1[i].length = 1;
        e_okm_data1[i].p_data = (PLAT_UI8 *)&p_desc->symmkey.key_info->lock_indicator;

        e_okm_data2[i].length = (p_desc->symmkey.key_info->info_length > 1) ?
                                (p_desc->symmkey.key_info->info_length - 1) : 0;
        e_okm_data2[i].p_data = (e_okm_data2[i].length > 0) ?
                                (PLAT_UI8 *)(&p_desc->symmkey.key_info->lock_indicator) + 1 : NULL;
      }
    }

    /* Push Elements */
    for (PLAT_I32 i = 0; i < okm_count; i++)
    {
      /* [OKM DESC LENGTH] */
      stse_frame_push_element(&cmd_frame, &e_okm_desc_length[i]);
      /* [OKM DESTINATION] */
      stse_frame_push_element(&cmd_frame, &e_okm_destination[i]);
      /* [OKM DATA 1] */
      stse_frame_push_element(&cmd_frame, &e_okm_data1[i]);
      if (e_okm_data2[i].length > 0)
      {
        /* [OKM DATA 2] */
        stse_frame_push_element(&cmd_frame, &e_okm_data2[i]);
      }
    }
  }

  /* Prepare DERIVE KEYS RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  e_rsp_header.length = STSAFEA_HEADER_SIZE;
  e_rsp_header.p_data = &rsp_header;
  stse_frame_push_element(&rsp_frame, &e_rsp_header);

  if (extract_flag && !expand_flag)
  {
    e_rsp_prk_slot.length = 1;
    e_rsp_prk_slot.p_data = &p_output->prk_slot;
    /* [PRK SLOT] */
    stse_frame_push_element(&rsp_frame, &e_rsp_prk_slot);
  }

  if (expand_flag)
  {
    for (PLAT_I32 i = 0; i < okm_count; i++)
    {
      if (p_okm_map[i].destination == STSAFEA_KEY_SOURCE_RESPONSE)
      {
        p_output->derived_keys[i].response.length = p_okm_map[i].response.key_length;
        e_rsp_okm_data[i].length = p_output->derived_keys[i].response.length;
        e_rsp_okm_data[i].p_data = p_output->derived_keys[i].response.data;
      }
      else
      {
        e_rsp_okm_data[i].length = 1;
        e_rsp_okm_data[i].p_data = &p_output->derived_keys[i].symmkey.slot_number;
      }
      /* [OKM DATA] */
      stse_frame_push_element(&rsp_frame, &e_rsp_okm_data[i]);
    }
  }

  ret = stsafea_frame_transfer(p_stse, &cmd_frame, &rsp_frame);

  return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
