/**
  ******************************************************************************
  * @file    stsafea_sessions.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for sessions (source)
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
#include <stdio.h>
#include <string.h>

#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_sessions.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_AES_SUBJECT_HOST_CMAC 0x00U
#define STSAFEA_AES_SUBJECT_HOST_RMAC 0x40U
#define STSAFEA_AES_SUBJECT_HOST_DECRYPT 0xC0U
#define STSAFEA_AES_SUBJECT_HOST_ENCRYPT 0x80U
#define STSAFEA_AES_FIRST_PADDING_BYTE 0x80U

/* Subject constants above select STSAFE host-session sub-operations:
 * C-MAC computation, R-MAC verification, payload decrypt, and payload encrypt.
 */

/* Private variables ---------------------------------------------------------*/
/* This module keeps no static mutable state; all context is session-bound. */

/* Public functions ----------------------------------------------------------*/

#ifdef STSE_CONF_USE_HOST_SESSION

stse_ReturnCode_t stsafea_open_host_session(stse_Handler_t *pSTSE, stse_session_t *pSession, PLAT_UI8 *pHost_MAC_key,
                                            PLAT_UI8 *pHost_cypher_key)
{
  stse_ReturnCode_t ret;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pSession == NULL)
  {
    return STSE_SERVICE_SESSION_ERROR;
  }

  if (pSTSE->device_type == STSAFE_A120)
  {
    /* A120 exposes host key configuration through the v2 slot structure. */
    stsafea_host_key_slot_v2_t host_key_slot;

    ret = stsafea_query_host_key_v2(pSTSE, &host_key_slot);
    if (ret != STSE_OK)
    {
      return ret;
    }

    if (host_key_slot.key_presence_flag == 0)
    {
      /* Session cannot start when host key is not provisioned on device. */
      return STSE_SERVICE_SESSION_ERROR;
    }
    /* A120 CMAC sequence counter is encoded on 4 bytes. */
    pSession->context.host.key_type = (stse_aes_key_type_t)host_key_slot.key_type;
    pSession->context.host.MAC_counter = ARRAY_4B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
  }
  else
  {
    /* Legacy variants use v1 host-key slot format and 3-byte CMAC counter. */
    stsafea_host_key_slot_t host_key_slot;

    ret = stsafea_query_host_key(pSTSE, &host_key_slot);
    if (ret != STSE_OK)
    {
      return ret;
    }

    if (host_key_slot.key_presence_flag == 0)
    {
      return STSE_SERVICE_SESSION_ERROR;
    }
    /* v1 device family uses AES-128 host keys by specification. */
    pSession->context.host.key_type = STSE_AES_128_KT;
    pSession->context.host.MAC_counter = ARRAY_3B_SWAP_TO_UI32(host_key_slot.cmac_sequence_counter);
  }

  /* Bind session to handler so frame-transfer layer can locate active context. */
  pSession->type = STSE_HOST_SESSION;
  pSession->context.host.pHost_MAC_key = pHost_MAC_key;
  pSession->context.host.pHost_cypher_key = pHost_cypher_key;
  pSession->context.host.pSTSE = pSTSE;
  pSTSE->pActive_host_session = pSession;

  /* Session is now fully initialized for authenticated/encrypted transfers. */

  return (STSE_OK);
}

void stsafea_close_host_session(stse_session_t *pSession)
{

  if (pSession == NULL)
  {
    return;
  }

  /* Null close requests are tolerated to simplify caller cleanup paths. */

  /* - Check if session is active in STSE handler*/
  if (pSession->context.host.pSTSE->pActive_host_session == pSession)
  {
    /* Clear pActive_host_session */
    pSession->context.host.pSTSE->pActive_host_session = NULL;
  }

  /* - Clear session context */
  /* Wipes local key pointers/counters and resets type marker. */
  stsafea_session_clear_context(pSession);
}

void stsafea_session_clear_context(stse_session_t *pSession)
{

  /* - Check stsafe handler initialization */
  if (pSession == NULL)
  {
    return;
  }

  /* - Clear session context */
  /* Full zeroization avoids stale session state reuse by later transfers. */
  memset(pSession, 0x00, sizeof(stse_session_t));
}

stse_ReturnCode_t stsafea_set_active_host_session(stse_Handler_t *pSTSE, stse_session_t *pSession)
{
  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if (pSession == NULL)
  {
    return STSE_SERVICE_SESSION_ERROR;
  }

  pSTSE->pActive_host_session = pSession;

  /* Caller is responsible for ensuring session belongs to same handler. */

  return (STSE_OK);
}

/* Encrypt command payload elements in-place using STSAFE host-session CBC mode.
 * Header element remains clear-text; only payload section is padded and encrypted.
 */
stse_ReturnCode_t stsafea_session_frame_encrypt(stse_session_t *pSession,
                                                stse_frame_t *pFrame,
                                                stse_frame_element_t *pEnc_payload_element)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
  stse_frame_element_t *pElement;
  PLAT_UI16 i = 0;

  /* - Verify parameters */
  if ((pSession == NULL)
      || (pFrame == NULL)
      || (pEnc_payload_element == NULL)
      || (pEnc_payload_element->length < (pFrame->length - pFrame->first_element->length + (16 -
                                       (pFrame->length - pFrame->first_element->length) % 16))))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Output encrypted payload buffer must be pre-sized by caller. */

  /* - Prepare specific STSAFE AES IV */
  if (pSession->context.host.pSTSE->device_type == STSAFE_A120)
  {
    /* A120 IV uses 4-byte counter then subject marker and 0x80 padding byte. */
    initial_value[0] = UI32_B3(pSession->context.host.MAC_counter + 1);
    initial_value[1] = UI32_B2(pSession->context.host.MAC_counter + 1);
    initial_value[2] = UI32_B1(pSession->context.host.MAC_counter + 1);
    initial_value[3] = UI32_B0(pSession->context.host.MAC_counter + 1);
    initial_value[4] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
    initial_value[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
    (void)memset(&initial_value[6], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 6U);
  }
  else
  {
    /* Legacy IV uses 3-byte counter then subject marker and 0x80 padding byte. */
    initial_value[0] = UI32_B2(pSession->context.host.MAC_counter + 1);
    initial_value[1] = UI32_B1(pSession->context.host.MAC_counter + 1);
    initial_value[2] = UI32_B0(pSession->context.host.MAC_counter + 1);
    initial_value[3] = STSAFEA_AES_SUBJECT_HOST_ENCRYPT;
    initial_value[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
    (void)memset(&initial_value[5], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 5U);
  }

  PLAT_UI16 encrypted_iv_len = STSAFEA_HOST_AES_BLOCK_SIZE;

  /* ECB output overwrites initial_value and becomes CBC IV input block. */

  /* - Perform first AES ECB round on IV */
  /* STSAFE session scheme derives CBC IV by encrypting the formatted IV block. */
  ret = stse_platform_aes_ecb_enc(initial_value,
                                  STSAFEA_HOST_AES_BLOCK_SIZE,
                                  pSession->context.host.pHost_cypher_key,
                                  (pSession->context.host.key_type == STSE_AES_128_KT)
                                  ? STSE_AES_128_KEY_SIZE
                                  : STSE_AES_256_KEY_SIZE,
                                  initial_value,
                                  &encrypted_iv_len);
  if (ret != STSE_OK)
  {
    return (ret);
  }

  /* At this point initial_value contains transformed CBC IV seed. */
  /* - Copy Plain text Frame payload content in Ciphered   */
  /* Flatten linked frame payload elements into one contiguous encrypt buffer. */
  pElement = pFrame->first_element->next;
  while (pElement != NULL)
  {
    /* Preserve element order exactly while flattening into encrypt buffer. */
    memcpy(pEnc_payload_element->pData + i,
           pElement->pData,
           pElement->length);

    i += pElement->length;
    pElement = pElement->next;
  }
  /* - Add First padding byte */
  /* Padding scheme is 0x80 followed by 0x00 bytes to block boundary. */
  *(pEnc_payload_element->pData + i++) = 0x80;

  /* - Add padding  */
  while (i < pEnc_payload_element->length)
  {
    /* Remaining bytes are explicit zero padding after first 0x80 byte. */
    *(pEnc_payload_element->pData + i++) = 0x00;
  }

  PLAT_UI16 encrypted_payload_len = pEnc_payload_element->length;

  /* - Encrypt pEncFrame content */
  /* CBC encrypts prepared payload buffer back into same destination array. */
  ret = stse_platform_aes_cbc_enc(
          pEnc_payload_element->pData,
          pEnc_payload_element->length,
          initial_value,
          pSession->context.host.pHost_cypher_key,
          (pSession->context.host.key_type == STSE_AES_128_KT) ? STSE_AES_128_KEY_SIZE : STSE_AES_256_KEY_SIZE,
          pEnc_payload_element->pData,
          &encrypted_payload_len);
  if (ret != 0)
  {
    return (STSE_SESSION_ERROR);
  }
  else
  {
    return (STSE_OK);
  }
}

static stse_ReturnCode_t stsafea_session_frame_decrypt(stse_session_t *pSession, stse_frame_t *pFrame)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 initial_value[STSAFEA_HOST_AES_BLOCK_SIZE];
  stse_frame_element_t *pElement;
  PLAT_UI16 i = 0;
  PLAT_UI16 encrypted_payload_len;

  /* Decrypt path mirrors encryption path with inverse subject selection. */

  pElement = pFrame->first_element->next;
  if (pElement == NULL)
  {
    /* Response with no payload does not require decrypt processing. */
    return STSE_OK;
  }

  /* Total length of the encrypted part of the frame */
  encrypted_payload_len = pFrame->length - pFrame->first_element->length;

  /* Variable-length stack buffer mirrors encrypted payload byte count. */

  /* Fill decrypt buffer with encrypted payload content */
  /* Concatenate payload elements before CBC decrypt operation. */
  PLAT_UI8 decrypt_buffer[encrypted_payload_len];

  while (pElement != NULL)
  {
    if (pElement->length != 0)
    {
      /* Skip zero-length elements but keep traversal for frame consistency. */
      memcpy(decrypt_buffer + i, pElement->pData, pElement->length);
      i += pElement->length;
    }
    pElement = pElement->next;
  }

  /* - Prepare Plain text info for AES IV */
  if (pSession->context.host.pSTSE->device_type == STSAFE_A120)
  {
    /* Response decrypt uses current MAC counter (no +1 increment here). */
    initial_value[0] = UI32_B3(pSession->context.host.MAC_counter);
    initial_value[1] = UI32_B2(pSession->context.host.MAC_counter);
    initial_value[2] = UI32_B1(pSession->context.host.MAC_counter);
    initial_value[3] = UI32_B0(pSession->context.host.MAC_counter);
    initial_value[4] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
    initial_value[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
    (void)memset(&initial_value[6], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 6U);
  }
  else
  {
    initial_value[0] = UI32_B2(pSession->context.host.MAC_counter);
    initial_value[1] = UI32_B1(pSession->context.host.MAC_counter);
    initial_value[2] = UI32_B0(pSession->context.host.MAC_counter);
    initial_value[3] = STSAFEA_AES_SUBJECT_HOST_DECRYPT;
    initial_value[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
    (void)memset(&initial_value[5], 0x00, (STSAFEA_HOST_AES_BLOCK_SIZE) - 5U);
  }

  PLAT_UI16 out_len = STSAFEA_HOST_AES_BLOCK_SIZE;

  /* - Transform IV using AES ECB */
  /* Same IV derivation method as encryption path, with decrypt subject marker. */
  ret = stse_platform_aes_ecb_enc(initial_value,
                                  STSAFEA_HOST_AES_BLOCK_SIZE,
                                  pSession->context.host.pHost_cypher_key,
                                  (pSession->context.host.key_type == STSE_AES_128_KT)
                                  ? STSE_AES_128_KEY_SIZE
                                  : STSE_AES_256_KEY_SIZE,
                                  initial_value,
                                  &out_len);

  if (ret != 0)
  {
    /* Keep legacy session-error mapping for IV derivation failures. */
    return STSE_SERVICE_SESSION_ERROR;
  }

  /* - Decrypt payload using CBC */
  /* Decrypt in temporary flat buffer then scatter back to frame elements. */
  PLAT_UI16 decrypted_payload_len = encrypted_payload_len;

  ret = stse_platform_aes_cbc_dec(decrypt_buffer,
                                  encrypted_payload_len,
                                  initial_value,
                                  pSession->context.host.pHost_cypher_key,
                                  (pSession->context.host.key_type == STSE_AES_128_KT)
                                  ? STSE_AES_128_KEY_SIZE
                                  : STSE_AES_256_KEY_SIZE,
                                  decrypt_buffer,
                                  &decrypted_payload_len);

  if (ret != STSE_OK)
  {
    /* Propagate crypto layer failure codes to transfer caller. */
    return ret;
  }

  /* - Copy Decrypted payload content in un-strapped Frame  */
  /* Remove temporary strap inserted by encrypted transfer helper. */
  stse_frame_unstrap(pFrame);
  pElement = pFrame->first_element->next;
  i = 0;
  while (pElement != NULL)
  {
    /* Scatter decrypted flat buffer back to per-element payload slices. */
    memcpy(pElement->pData,
           decrypt_buffer + i,
           pElement->length);
    i += pElement->length;
    pElement = pElement->next;
  }

  return STSE_OK;
}

static stse_ReturnCode_t stsafea_session_frame_c_mac_compute(stse_session_t *pSession,
                                                             stse_frame_t *pCmd_frame,
                                                             PLAT_UI8 *pMAC)
{
  /* Build and compute C-MAC over command context + payload according to STSAFE
   * host-session authentication framing.
   */
  PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
  PLAT_UI8 mac_output_length;
  PLAT_UI8 mac_type = 0x00;
  stse_frame_element_t *pElement;
  PLAT_UI8 aes_block_idx = 0;
  PLAT_UI16 i;
  PLAT_UI16 cmd_payload_length = pCmd_frame->length - pCmd_frame->first_element->length;

  /* cmd_payload_length is serialized in big-endian before MAC streaming. */
  stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;

  if ((pSession == NULL) || (pCmd_frame == NULL) || (pMAC == NULL))
  {
    /* Authentication requires session context, command frame, and output MAC. */
    return STSE_SERVICE_SESSION_ERROR;
  }

  /*- create C-MAC Frame : [0x00] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] */
  STSE_FRAME_ALLOCATE(c_mac_frame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&c_mac_frame, eMACType, 1, &mac_type);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&c_mac_frame,
                                   eCMD_HEADER,
                                   pCmd_frame->first_element->length,
                                   pCmd_frame->first_element->pData);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&c_mac_frame,
                                   eCmdPayloadLength,
                                   STSAFEA_CMD_RSP_LEN_SIZE,
                                   (PLAT_UI8 *)&cmd_payload_length);
  stse_frame_element_swap_byte_order(&eCmdPayloadLength);
  /* Link payload from original command frame after synthetic length element. */
  eCmdPayloadLength.next = pCmd_frame->first_element->next;
  stse_frame_update(&c_mac_frame);

  /*- Initialize AES C-MAC computation */

  ret = stse_platform_aes_cmac_init(pSession->context.host.pHost_MAC_key,
                                    (pSession->context.host.key_type == STSE_AES_128_KT)
                                    ? STSE_AES_128_KEY_SIZE
                                    : STSE_AES_256_KEY_SIZE,
                                    STSAFEA_MAC_SIZE);
  if (ret != STSE_OK)
  {
    /* Initialization failure means MAC key/context is unusable for this frame. */
    return ret;
  }

  /*- Perform First AES-CMAC round with MAC subject info */
  if (pSession->context.host.pSTSE->device_type == STSAFE_A120)
  {
    /* Subject block seeds CMAC stream with counter and host C-MAC marker. */
    aes_cmac_block[0] = UI32_B3(pSession->context.host.MAC_counter);
    aes_cmac_block[1] = UI32_B2(pSession->context.host.MAC_counter);
    aes_cmac_block[2] = UI32_B1(pSession->context.host.MAC_counter);
    aes_cmac_block[3] = UI32_B0(pSession->context.host.MAC_counter);
    aes_cmac_block[4] = STSAFEA_AES_SUBJECT_HOST_CMAC;  /* Subject : Host C-MAC */
    aes_cmac_block[5] = STSAFEA_AES_FIRST_PADDING_BYTE; /* First byte of padding */
    for (i = 6; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++)
    {
      aes_cmac_block[i] = 0x00U; /* 0x00 padding */
    }
  }
  else
  {
    /* Legacy encoding uses 3-byte counter in subject block. */
    aes_cmac_block[0] = UI32_B2(pSession->context.host.MAC_counter);
    aes_cmac_block[1] = UI32_B1(pSession->context.host.MAC_counter);
    aes_cmac_block[2] = UI32_B0(pSession->context.host.MAC_counter);
    aes_cmac_block[3] = STSAFEA_AES_SUBJECT_HOST_CMAC;  /* Subject : Host C-MAC */
    aes_cmac_block[4] = STSAFEA_AES_FIRST_PADDING_BYTE; /* First byte of padding */
    for (i = 5; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++)
    {
      aes_cmac_block[i] = 0x00U; /* 0x00 padding */
    }
  }

  ret = stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Subject block has been consumed; now stream command-authentication frame. */

  pElement = c_mac_frame.first_element;

  /*- Perform additional AES-CMAC round(s) for frame to Authenticate */
  /* Stream frame bytes through block buffer and append on block completion. */
  while (pElement != NULL)
  {
    for (i = 0; i < pElement->length; i++)
    {
      if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE)
      {
        /* Flush full block to CMAC state and restart local fill index. */
        stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
        aes_block_idx = 0;
      }
      aes_cmac_block[aes_block_idx] = *(pElement->pData + i);
      aes_block_idx++;
    }
    pElement = pElement->next;
  }
  if (aes_block_idx != 0)
  {
    ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
    if (ret != STSE_OK)
    {
      return ret;
    }
  }

  /*- Finish AES MAC computation */
  /* Final MAC length must match protocol-defined STSAFEA_MAC_SIZE. */
  ret = stse_platform_aes_cmac_compute_finish(aes_cmac_block, &mac_output_length);
  if (ret != STSE_OK)
  {
    return ret;
  }
  else if (mac_output_length != STSAFEA_MAC_SIZE)
  {
    return STSE_SERVICE_SESSION_ERROR;
  }
  memcpy(pMAC, aes_cmac_block, STSAFEA_MAC_SIZE);

  /* Caller appends resulting command MAC as the last command frame element. */

  return ret;
}

static stse_ReturnCode_t stsafea_session_frame_r_mac_verify(stse_session_t *pSession,
                                                            stse_frame_t *pCmd_frame,
                                                            stse_frame_t *pRsp_frame,
                                                            PLAT_UI8 *pMAC)
{
  /* Verify response MAC by rebuilding RMAC input context and comparing against
   * MAC bytes appended in device response.
   */
  stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
  PLAT_UI8 aes_cmac_block[STSAFEA_HOST_AES_BLOCK_SIZE];
  PLAT_UI16 cmd_payload_length = pCmd_frame->length - pCmd_frame->first_element->length;
  PLAT_UI8 aes_block_idx = 0;
  PLAT_UI16 i;
  PLAT_UI8 mac_type = 0x80;
  stse_frame_element_t *pElement;

  if ((pSession == NULL) || (pCmd_frame == NULL) || (pRsp_frame == NULL))
  {
    /* Verification cannot proceed without both command and response contexts. */
    return STSE_SERVICE_SESSION_ERROR;
  }

  if (*(pCmd_frame->first_element->pData) & STSAFEA_PROT_RSP_MSK)
  {

    /* RMAC verification is only required when command requested protected response. */

    /*- Pop R-MAC from frame*/
    /* Last response element contains MAC and is removed before reconstruction. */
    stse_frame_pop_element(pRsp_frame);

    PLAT_UI16 rsp_payload_length = (pRsp_frame->length - (pRsp_frame->first_element->length));

    /*- Initialize AES CMAC computation */
    stse_platform_aes_cmac_init(
      pSession->context.host.pHost_MAC_key,
      (pSession->context.host.key_type == STSE_AES_128_KT) ? STSE_AES_128_KEY_SIZE : STSE_AES_256_KEY_SIZE,
      STSAFEA_MAC_SIZE);

    /* Init return value is intentionally ignored to preserve existing behavior. */

    /*- Perform First AES-CMAC round */
    if (pSession->context.host.pSTSE->device_type == STSAFE_A120)
    {
      /* Response subject block uses host R-MAC marker instead of C-MAC marker. */
      aes_cmac_block[0] = UI32_B3(pSession->context.host.MAC_counter);
      aes_cmac_block[1] = UI32_B2(pSession->context.host.MAC_counter);
      aes_cmac_block[2] = UI32_B1(pSession->context.host.MAC_counter);
      aes_cmac_block[3] = UI32_B0(pSession->context.host.MAC_counter);
      aes_cmac_block[4] = STSAFEA_AES_SUBJECT_HOST_RMAC;
      aes_cmac_block[5] = STSAFEA_AES_FIRST_PADDING_BYTE;
      for (i = 6; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++)
      {
        aes_cmac_block[i] = 0x00U; /* 0x00 padding */
      }
    }
    else
    {
      aes_cmac_block[0] = UI32_B2(pSession->context.host.MAC_counter);
      aes_cmac_block[1] = UI32_B1(pSession->context.host.MAC_counter);
      aes_cmac_block[2] = UI32_B0(pSession->context.host.MAC_counter);
      aes_cmac_block[3] = STSAFEA_AES_SUBJECT_HOST_RMAC;
      aes_cmac_block[4] = STSAFEA_AES_FIRST_PADDING_BYTE;
      for (i = 5; i < STSAFEA_HOST_AES_BLOCK_SIZE; i++)
      {
        aes_cmac_block[i] = 0x00U; /* 0x00 padding */
      }
    }

    stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);

    /*- Prepare AES CMAC input for response MAC verification  */
    STSE_FRAME_ALLOCATE(r_mac_frame);

    /*- Create r_mac_frame head :[MAC TYPE] [CMD HEADER] [CMD PAYLOAD LENGTH] [CMD PAYLOAD] ... */
    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(
      &r_mac_frame,
      eMACType,
      1,
      &mac_type);

    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(
      &r_mac_frame,
      eCMD_header,
      pCmd_frame->first_element->length,
      pCmd_frame->first_element->pData);

    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(
      &r_mac_frame,
      eCMD_Length,
      STSAFEA_CMD_RSP_LEN_SIZE,
      (PLAT_UI8 *)&cmd_payload_length);
    stse_frame_element_swap_byte_order(&eCMD_Length);

    if (pCmd_frame->first_element->next->length == 0)
    {
      /* Skip empty payload placeholder element when command payload is absent. */
      eCMD_Length.next = pCmd_frame->first_element->next->next;
    }
    else
    {
      eCMD_Length.next = pCmd_frame->first_element->next;
    }

    stse_frame_update(&r_mac_frame);

    /*- Create r_mac_frame head : ... [RSP HEADER] [RSP PAYLOAD LENGTH] [RSP PAYLOAD] */
    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(
      &r_mac_frame,
      eRSP_header,
      pRsp_frame->first_element->length,
      pRsp_frame->first_element->pData);

    STSE_FRAME_ELEMENT_ALLOCATE_PUSH(
      &r_mac_frame,
      eRsp_Length,
      STSAFEA_CMD_RSP_LEN_SIZE,
      (PLAT_UI8 *)&rsp_payload_length);
    stse_frame_element_swap_byte_order(&eRsp_Length);

    /* Link parsed response payload after synthetic response-length field. */

    eRsp_Length.next = pRsp_frame->first_element->next;
    stse_frame_update(&r_mac_frame);
    pElement = r_mac_frame.first_element;

    /*- Perform additional AES-CMAC round(s) on R-MAC verification frame*/
    /* Byte stream order must match device-side RMAC computation exactly. */
    while (pElement != NULL)
    {
      for (i = 0; i < pElement->length; i++)
      {
        if (aes_block_idx == STSAFEA_HOST_AES_BLOCK_SIZE)
        {
          stse_platform_aes_cmac_append(aes_cmac_block, STSAFEA_HOST_AES_BLOCK_SIZE);
          aes_block_idx = 0;
        }
        aes_cmac_block[aes_block_idx] = *(pElement->pData + i);
        aes_block_idx++;
      }
      pElement = pElement->next;
    }
    if (aes_block_idx != 0)
    {
      ret = stse_platform_aes_cmac_append(aes_cmac_block, aes_block_idx);
      if (ret != STSE_OK)
      {
        return ret;
      }
    }

    memcpy(aes_cmac_block, pMAC, STSAFEA_MAC_SIZE);
    /* Verify computed CMAC state against response-provided MAC value. */
    ret = stse_platform_aes_cmac_verify_finish(aes_cmac_block);
  }
  /* If response MAC not requested, function keeps default invalid-parameter code. */
  /* Callers invoke this only after protected transfers where RMAC is expected. */
  return ret;
}

stse_ReturnCode_t stsafea_session_encrypted_transfer(stse_session_t *pSession,
                                                     stse_frame_t *pCmdFrame,
                                                     stse_frame_t *pRspFrame,
                                                     PLAT_UI8 cmd_encryption_flag,
                                                     PLAT_UI8 rsp_encryption_flag,
                                                     stse_cmd_access_conditions_t cmd_ac_info,
                                                     PLAT_UI16 processing_time)
{
  stse_ReturnCode_t ret;
  PLAT_UI16 encrypted_cmd_payload_size = 0;
  PLAT_UI16 encrypted_rsp_payload_size = 0;
  PLAT_UI8 padding = 16;

  if (pSession == NULL || pCmdFrame == NULL || pRspFrame == NULL
      || pCmdFrame->first_element == NULL || pCmdFrame->first_element->pData == NULL
      || pRspFrame->first_element == NULL || pRspFrame->first_element->pData == NULL)
  {
    return STSE_SERVICE_SESSION_ERROR;
  }

  if (cmd_encryption_flag == 1)
  {
#ifdef STSE_FRAME_DEBUG_LOG
    printf("\n\r STSAFE Plaintext Frame > ");
    stse_frame_debug_print(pCmdFrame);
    printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */

    PLAT_UI16 plaintext_payload_size = pCmdFrame->length - pCmdFrame->first_element->length;
    /* Payload is always expanded to complete AES block with 0x80/0x00 padding. */
    if ((plaintext_payload_size % 16) != 0)
    {
      padding = 16 - (plaintext_payload_size % 16);
    }
    encrypted_cmd_payload_size = plaintext_payload_size + padding;
  }

  /* Command payload strap is inserted only when encryption is requested. */

  PLAT_UI8 encrypted_cmd_payload[encrypted_cmd_payload_size];
  STSE_FRAME_ELEMENT_ALLOCATE(eEncrypted_cmd_payload, encrypted_cmd_payload_size, encrypted_cmd_payload);
  STSE_FRAME_STRAP_ALLOCATE(S1);

  if (cmd_encryption_flag == 1)
  {
    ret = stsafea_session_frame_encrypt(pSession, pCmdFrame, &eEncrypted_cmd_payload);
    if (ret != STSE_OK)
    {
      return ret;
    }
    stse_frame_insert_strap(&S1, pCmdFrame->first_element, &eEncrypted_cmd_payload);
    /* Recalculate frame length/links after replacing clear payload by strap. */
    stse_frame_update(pCmdFrame);
  }

  if (rsp_encryption_flag == 1)
  {
    padding = 16;
    PLAT_UI16 plaintext_payload_size = pRspFrame->length - pRspFrame->first_element->length;
    if (plaintext_payload_size != 0)
    {
      if ((plaintext_payload_size % 16) != 0)
      {
        padding = 16 - (plaintext_payload_size % 16);
      }
    }
    else
    {
      padding = 0;
    }
    encrypted_rsp_payload_size = plaintext_payload_size + padding;
  }

  PLAT_UI8 encrypted_rsp_payload[encrypted_rsp_payload_size];
  STSE_FRAME_ELEMENT_ALLOCATE(eEncrypted_rsp_payload, encrypted_rsp_payload_size, encrypted_rsp_payload);
  STSE_FRAME_STRAP_ALLOCATE(S2);

  if (rsp_encryption_flag == 1 && pRspFrame->first_element->next != NULL)
  {
    /* Pre-allocate response encrypted payload slot before raw transfer. */
    stse_frame_insert_strap(&S2, pRspFrame->first_element, &eEncrypted_rsp_payload);
    stse_frame_update(pRspFrame);
  }

  ret = stsafea_session_authenticated_transfer(pSession,
                                               pCmdFrame,
                                               pRspFrame,
                                               cmd_ac_info,
                                               processing_time);

  /* Authentication transfer updates MAC counter and verifies RMAC when enabled. */

  if ((ret == STSE_OK) && (rsp_encryption_flag == 1))
  {
    /* On success, decrypt response payload back into original frame elements. */
    ret = stsafea_session_frame_decrypt(pSession, pRspFrame);

#ifdef STSE_FRAME_DEBUG_LOG
    printf("\n\r STSAFE Plaintext Frame < ");
    stse_frame_debug_print(pRspFrame);
    printf("\n\r");
#endif /* STSE_FRAME_DEBUG_LOG */
  }

  return ret;
}

stse_ReturnCode_t stsafea_session_authenticated_transfer(stse_session_t *pSession,
                                                         stse_frame_t *pCmdFrame,
                                                         stse_frame_t *pRspFrame,
                                                         stse_cmd_access_conditions_t cmd_ac_info,
                                                         PLAT_UI16 processing_time)
{
  (void)cmd_ac_info;
  /* Access condition parameter is already resolved by command-layer helpers. */
  stse_ReturnCode_t ret;
  PLAT_UI8 Cmd_MAC[STSAFEA_MAC_SIZE];
  PLAT_UI8 Rsp_MAC[STSAFEA_MAC_SIZE];

  if (pSession == NULL || pCmdFrame == NULL || pRspFrame == NULL
      || pCmdFrame->first_element == NULL || pCmdFrame->first_element->pData == NULL
      || pRspFrame->first_element == NULL || pRspFrame->first_element->pData == NULL)
  {
    return STSE_SERVICE_SESSION_ERROR;
  }

  if (pSession->type == STSE_HOST_SESSION)
  {
    /* Bit 5 marks host-session usage in STSAFE command protection field. */
    *(pCmdFrame->first_element->pData) |= (1 << 5);
  }

  /* Bits 7 and 6 request both command and response MAC protections. */
  *(pCmdFrame->first_element->pData) |= ((1 << 7) | (1 << 6));

  /* Protection bits are set directly in command header before MAC computation. */

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(pRspFrame, eRspMAC, STSAFEA_MAC_SIZE, Rsp_MAC);

  /* Response frame reserves trailing MAC storage before raw transfer call. */

  ret = stsafea_session_frame_c_mac_compute(pSession, pCmdFrame, Cmd_MAC);
  if (ret != STSE_OK)
  {
    return ret;
  }

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(pCmdFrame, eCmdMAC, STSAFEA_MAC_SIZE, Cmd_MAC);

  /* Command MAC must be appended before transfer so device can authenticate host. */

  switch (pSession->type)
  {

    case STSE_HOST_SESSION:
      /* Raw transfer handles transport framing while session logic handles MAC. */
      ret = stsafea_frame_raw_transfer(pSession->context.host.pSTSE, pCmdFrame, pRspFrame, processing_time);
      if (ret <= 0xFF && ret != STSE_INVALID_C_MAC && ret != STSE_COMMUNICATION_ERROR)
      {
        /* Counter increments only for accepted command-execution responses. */
        pSession->context.host.MAC_counter++;
      }
      break;

    default:
      /* Only host-session type is supported in STSAFE-A session services. */
      ret = STSE_SERVICE_SESSION_ERROR;
      break;
  }

  /*- Pop C-MAC from frame*/
  /* Remove temporary command MAC element before returning to caller. */
  stse_frame_pop_element(pCmdFrame);

  if (ret == STSE_OK)
  {
    /* RMAC verification also removes response MAC element from working frame. */
    ret = stsafea_session_frame_r_mac_verify(pSession, pCmdFrame, pRspFrame, Rsp_MAC);
  }

  /* Return status reflects transport + RMAC verification outcome. */

  return ret;
}

#endif /* STSE_CONF_USE_HOST_SESSION */

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
