/**
  ******************************************************************************
  * @file    stsafea_derive_keys.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for derive keys command (source)
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

#include "services/stsafea/stsafea_derive_keys.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_derive_keys(
  stse_Handler_t *pSTSE,
  stsafea_hkdf_input_key_t *pInput_key,
  PLAT_UI8 extract_flag,
  PLAT_UI8 expand_flag,
  stsafea_hkdf_salt_t *pSalt,
  stsafea_hkdf_info_t *pInfo,
  stsafea_hkdf_okm_description_t *pOkm_map,
  PLAT_UI8 okm_count,
  stsafea_hkdf_output_t *pOutput)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_DERIVE_KEYS};
  PLAT_UI8 rsp_header;
  PLAT_UI8 hkdf_flags = 0;
  stsafea_hash_algorithm_identifier_t hash_algo = {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_256};

  /* -- Frame Element Declarations -- */
  stse_frame_element_t eCmdHeader;

  /* Input Key Elements */
  stse_frame_element_t eInputSource;
  stse_frame_element_t eInputMode, eInputLength, eInputValue; /* For Command source */
  stse_frame_element_t eInputSlot;                            /* For Slot source */

  /* HKDF Param Elements */
  stse_frame_element_t eHkdfFlags, eHashAlgo;

  /* Salt Elements */
  stse_frame_element_t eSaltSource;
  stse_frame_element_t eSaltLength, eSaltValue; /* For Command source */
  stse_frame_element_t eSaltSlot;               /* For Slot source */

  /* Info Elements */
  stse_frame_element_t eInfoLength, eInfoValue;

  /* Response Elements */
  stse_frame_element_t eRspHeader, eRspPrkSlot;

  /* -- Auxiliary Buffers -- */
  PLAT_UI8 input_len_buf[2];
  PLAT_UI8 salt_len_buf[2];
  PLAT_UI8 info_len_buf[2];

  /* 1. Parameter Validation */
  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }
  if (pInput_key == NULL || pOutput == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }
  if (expand_flag && (pOkm_map == NULL || pOutput->derived_keys == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* 2. Loop Arrays & Buffer Allocation */
  stse_frame_element_t eOkmDescLength[okm_count], eOkmDestination[okm_count];
  stse_frame_element_t eOkmData1[okm_count], eOkmData2[okm_count];
  stse_frame_element_t eRspOkmData[okm_count];

  PLAT_UI8 okm_desc_len_buf[okm_count][2];
  PLAT_UI8 okm_key_len_buf[okm_count][2];

  /* Variable-length arrays are bounded by okm_count (validated by callers).
   * They keep per-key frame metadata contiguous during command construction. */

  /* Initialize Frames */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ALLOCATE(RspFrame);

  /* 3. Build Command Frame */
  /* Header */
  eCmdHeader.length = STSAFEA_EXT_HEADER_SIZE;
  eCmdHeader.pData = cmd_header;
  stse_frame_push_element(&CmdFrame, &eCmdHeader);

  /* -- Input Key -- */
  /* Input source selects whether IKM bytes are inlined in this command
   * or referenced through an existing symmetric key slot. */
  eInputSource.length = 1;
  eInputSource.pData = &pInput_key->source;
  stse_frame_push_element(&CmdFrame, &eInputSource);

  if (pInput_key->source == STSAFEA_KEY_SOURCE_COMMAND)
  {
    input_len_buf[0] = (PLAT_UI8)(pInput_key->command.length >> 8);
    input_len_buf[1] = (PLAT_UI8)(pInput_key->command.length);

    eInputMode.length = 1;
    eInputMode.pData = (PLAT_UI8 *)&pInput_key->command.mode_of_operation;

    eInputLength.length = 2;
    eInputLength.pData = input_len_buf;

    eInputValue.length = pInput_key->command.length;
    eInputValue.pData = pInput_key->command.data;

    stse_frame_push_element(&CmdFrame, &eInputMode);
    stse_frame_push_element(&CmdFrame, &eInputLength);
    stse_frame_push_element(&CmdFrame, &eInputValue);
  }
  else
  {
    eInputSlot.length = 1;
    eInputSlot.pData = &pInput_key->symmkey.slot_number;
    stse_frame_push_element(&CmdFrame, &eInputSlot);
  }

  /* -- HKDF Parameters -- */
  /* Flags encode Extract/Expand mode in the command byte expected by firmware.
   * Hash algorithm is fixed to SHA-256 for this command version. */
  if (extract_flag)
  {
    hkdf_flags |= (1 << STSAFEA_HKDF_FLAG_EXTRACT_POS);
  }
  if (expand_flag)
  {
    hkdf_flags |= (1 << STSAFEA_HKDF_FLAG_EXPAND_POS);
  }

  eHkdfFlags.length = 1;
  eHkdfFlags.pData = &hkdf_flags;

  eHashAlgo.length = STSAFEA_HASH_ALGO_ID_SIZE;
  eHashAlgo.pData = (PLAT_UI8 *)&hash_algo;

  stse_frame_push_element(&CmdFrame, &eHkdfFlags);
  stse_frame_push_element(&CmdFrame, &eHashAlgo);

  /* -- Salt (Conditional) -- */
  if (extract_flag)
  {
    if (pSalt == NULL)
    {
      return STSE_SERVICE_INVALID_PARAMETER;
    }

    eSaltSource.length = 1;
    eSaltSource.pData = &pSalt->source;
    stse_frame_push_element(&CmdFrame, &eSaltSource);

    if (pSalt->source == STSAFEA_KEY_SOURCE_COMMAND)
    {
      /* Command-sourced salt is serialized as [length(2B), value(NB)]. */
      salt_len_buf[0] = (PLAT_UI8)(pSalt->command.length >> 8);
      salt_len_buf[1] = (PLAT_UI8)(pSalt->command.length);

      eSaltLength.length = 2;
      eSaltLength.pData = salt_len_buf;
      stse_frame_push_element(&CmdFrame, &eSaltLength);

      if (pSalt->command.length > 0 && pSalt->command.data != NULL)
      {
        eSaltValue.length = pSalt->command.length;
        eSaltValue.pData = pSalt->command.data;
        stse_frame_push_element(&CmdFrame, &eSaltValue);
      }
    }
    else
    {
      /* Slot-sourced salt carries only the slot identifier. */
      eSaltSlot.length = 1;
      eSaltSlot.pData = &pSalt->symmkey.slot_number;
      stse_frame_push_element(&CmdFrame, &eSaltSlot);
    }
  }

  /* -- Info (Conditional) -- */
  if (expand_flag)
  {
    PLAT_UI16 info_len = (pInfo != NULL) ? pInfo->length : 0;
    /* Info is optional in HKDF-Expand; zero length is encoded explicitly so
     * the device keeps command parsing deterministic across modes. */
    info_len_buf[0] = (PLAT_UI8)(info_len >> 8);
    info_len_buf[1] = (PLAT_UI8)(info_len);

    eInfoLength.length = 2;
    eInfoLength.pData = info_len_buf;
    stse_frame_push_element(&CmdFrame, &eInfoLength);

    if (info_len > 0 && pInfo != NULL && pInfo->data != NULL)
    {
      eInfoValue.length = info_len;
      eInfoValue.pData = pInfo->data;
      stse_frame_push_element(&CmdFrame, &eInfoValue);
    }
  }

  /* -- OKM Map (Conditional) -- */
  if (expand_flag)
  {
    for (PLAT_UI8 i = 0; i < okm_count; i++)
    {
      stsafea_hkdf_okm_description_t *pDesc = &pOkm_map[i];
      PLAT_UI16 desc_len;

      /* Build one descriptor per derived key destination.
       * Response destination encodes key length only.
       * Symmkey destination encodes full key-info payload for slot creation. */
      if (pDesc->destination == STSAFEA_KEY_SOURCE_RESPONSE)
      {
        desc_len = 3; /* Dest(1) + KeyLen(2) */
        okm_desc_len_buf[i][0] = (PLAT_UI8)(desc_len >> 8);
        okm_desc_len_buf[i][1] = (PLAT_UI8)(desc_len);

        okm_key_len_buf[i][0] = (PLAT_UI8)(pDesc->response.key_length >> 8);
        okm_key_len_buf[i][1] = (PLAT_UI8)(pDesc->response.key_length);

        eOkmDescLength[i].length = 2;
        eOkmDescLength[i].pData = okm_desc_len_buf[i];

        eOkmDestination[i].length = 1;
        eOkmDestination[i].pData = (PLAT_UI8 *)&pDesc->destination;

        eOkmData1[i].length = 2;
        eOkmData1[i].pData = okm_key_len_buf[i];

        eOkmData2[i].length = 0;
        eOkmData2[i].pData = NULL;
      }
      else
      {
        if (pDesc->symmkey.key_info == NULL)
        {
          return STSE_SERVICE_INVALID_PARAMETER;
        }

        desc_len = 1 + pDesc->symmkey.key_info->info_length;
        okm_desc_len_buf[i][0] = (PLAT_UI8)(desc_len >> 8);
        okm_desc_len_buf[i][1] = (PLAT_UI8)(desc_len);

        eOkmDescLength[i].length = 2;
        eOkmDescLength[i].pData = okm_desc_len_buf[i];

        eOkmDestination[i].length = 1;
        eOkmDestination[i].pData = (PLAT_UI8 *)&pDesc->destination;

        eOkmData1[i].length = 1;
        eOkmData1[i].pData = (PLAT_UI8 *)&pDesc->symmkey.key_info->lock_indicator;

        eOkmData2[i].length = (pDesc->symmkey.key_info->info_length > 1)
                              ? (pDesc->symmkey.key_info->info_length - 1)
                              : 0;
        eOkmData2[i].pData = (eOkmData2[i].length > 0)
                             ? (PLAT_UI8 *)(&pDesc->symmkey.key_info->lock_indicator) + 1
                             : NULL;
      }
    }

    /* Push Elements */
    for (PLAT_UI8 i = 0; i < okm_count; i++)
    {
      stse_frame_push_element(&CmdFrame, &eOkmDescLength[i]);
      stse_frame_push_element(&CmdFrame, &eOkmDestination[i]);
      stse_frame_push_element(&CmdFrame, &eOkmData1[i]);
      if (eOkmData2[i].length > 0)
      {
        stse_frame_push_element(&CmdFrame, &eOkmData2[i]);
      }
    }
  }

  /* 4. Build Response Frame */
  eRspHeader.length = STSAFEA_HEADER_SIZE;
  eRspHeader.pData = &rsp_header;
  stse_frame_push_element(&RspFrame, &eRspHeader);

  if (extract_flag && !expand_flag)
  {
    eRspPrkSlot.length = 1;
    eRspPrkSlot.pData = &pOutput->prk_slot;
    stse_frame_push_element(&RspFrame, &eRspPrkSlot);
  }

  if (expand_flag)
  {
    for (PLAT_UI8 i = 0; i < okm_count; i++)
    {
      /* Response parsing mirrors descriptor order used in command encoding. */
      if (pOkm_map[i].destination == STSAFEA_KEY_SOURCE_RESPONSE)
      {
        pOutput->derived_keys[i].response.length = pOkm_map[i].response.key_length;
        eRspOkmData[i].length = pOutput->derived_keys[i].response.length;
        eRspOkmData[i].pData = pOutput->derived_keys[i].response.data;
      }
      else
      {
        /* For slot output, firmware returns only the allocated slot number,
         * not key bytes, to preserve key non-exportability semantics. */
        eRspOkmData[i].length = 1;
        eRspOkmData[i].pData = &pOutput->derived_keys[i].symmkey.slot_number;
      }
      stse_frame_push_element(&RspFrame, &eRspOkmData[i]);
    }
  }

  /* 5. Transfer */
  ret = stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);

  return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
