/**
  ******************************************************************************
  * @file    stsafea_aes.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for symmetric key cryptography (source)
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
#include <string.h>

#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_frame_transfer.h"


#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_aes_ecb_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  /* ECB mode response is header + ciphertext with no tag/counter metadata. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* - Check stsafe-a handler initialization */

  if ((pPlaintext_message == NULL) || (pEncrypted_message == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MESSAGE]  */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Sub-command distinguisher selects symmetric-cipher operation branch. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eCmd_header,
                                   STSAFEA_HEADER_SIZE,
                                   &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  /* Plaintext length must match frame element length declared here. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_length, pPlaintext_message);

  /* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE]  */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_length, pEncrypted_message);

  /* - Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

#ifdef STSE_CONF_USE_HOST_SESSION
  if (ret != STSE_OK)
  {
    /* Defensive clear avoids exposing stale bytes on failed secure transfer. */
    memset(pEncrypted_message, 0, message_length);
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */
  return ret;
}

stse_ReturnCode_t stsafea_aes_ecb_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pPlaintext_message)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  /* Response is plain message bytes only, keyed by same slot selector. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pPlaintext_message == NULL) || (pEncrypted_message == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ENCRYPTED MESSAGE]  */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Slot selection determines device-side symmetric key used for decrypt. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eCmd_header,
                                   STSAFEA_HEADER_SIZE,
                                   &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  /* Ciphertext size equals expected plaintext output size for ECB mode. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_length, pEncrypted_message);

  /* - Prepare RSP Frame : [HEADER] [PLAIN TEXT MESSAGE]  */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_length, pPlaintext_message);

  /* - Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

#ifdef STSE_CONF_USE_HOST_SESSION
  if (ret != STSE_OK)
  {
    /* Defensive clear avoids leaking prior stack/heap data to caller path. */
    memset(pPlaintext_message, 0, message_length);
  }
#endif /* defined(STSE_CONF_USE_HOST_SESSION) */
  return ret;
}

stse_ReturnCode_t stsafea_aes_ccm_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pEncrypted_authentication_tag,
  PLAT_UI8 counter_presence,
  PLAT_UI32 *pCounter)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;
  /* Device can return counter-presence indication independently from request. */
  PLAT_UI8 received_counter_presence = 0;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pNonce == NULL)
      || (pAssociated_data == NULL && associated_data_length != 0)
      || (pAssociated_data != NULL && associated_data_length == 0)
      || (pPlaintext_message == NULL && message_length != 0)
      || (pPlaintext_message != NULL && message_length == 0)
      || (pEncrypted_message == NULL && message_length != 0)
      || (pEncrypted_message != NULL && message_length == 0)
      || (pEncrypted_authentication_tag == NULL && authentication_tag_length != 0)
      || (pEncrypted_authentication_tag != NULL && authentication_tag_length == 0))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  if ((counter_presence == 1) && (pCounter == NULL))
  {
    /* Counter output buffer is required when caller asks for counter return. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ASSOCIATED DATA LENGTH] ...
  *                       ... [ASSOCIATED DATA MESSAGE] [MESSAGE LENGTH] [MESSAGE] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Nonce is fixed-size for this CCM one-shot API variant. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eCmd_header,
                                   STSAFEA_HEADER_SIZE,
                                   &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eNonce, STSAFEA_NONCE_SIZE, pNonce);
  /* Associated-data length can be zero when only message encryption is needed. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE(eAssociated_data, associated_data_length, pAssociated_data);
  if (associated_data_length != 0)
  {
    /* Optional AAD field is omitted from frame when length is zero. */
    stse_frame_push_element(&CmdFrame, &eAssociated_data);
  }
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eMessage_length,
                                   STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* Message payload can be empty for AAD-only authentication updates. */
  STSE_FRAME_ELEMENT_ALLOCATE(ePlaintext_message, message_length, pPlaintext_message);
  if (message_length != 0)
  {
    /* Existing behavior gates plaintext push on associated_data_length check. */
    stse_frame_push_element(&CmdFrame, &ePlaintext_message);
  }

  /* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE] [TAG LENGTH] [COUNTER PRES.] [COUNTER VAL] */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_length, pEncrypted_message);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eAuthentication_tag, authentication_tag_length,
                                   pEncrypted_authentication_tag);
  /* Counter presence is always returned, even if caller ignores counter bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounter_presence, 1, &received_counter_presence);
  STSE_FRAME_ELEMENT_ALLOCATE(eCounter, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)pCounter);
  if (counter_presence != 0)
  {
    /* Counter bytes are returned only when slot configuration exposes counter. */
    stse_frame_push_element(&RspFrame, &eCounter);
  }

  /* Protocol length fields are serialized big-endian on the wire. */
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (counter_presence != 0)
  {
    /* Restore caller-native order after transfer for counter interpretation. */
    stse_frame_element_swap_byte_order(&eCounter);
  }

  return ret;
}

stse_ReturnCode_t stsafea_aes_ccm_encrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 Nonce_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 total_associated_data_length,
  PLAT_UI32 total_message_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pCounter_presence,
  PLAT_UI32 *pCounter)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_ENCRYPT};

  PLAT_UI8 rsp_header;
  /* Alternate buffers keep API tolerant when caller omits counter outputs. */
  PLAT_UI8 alt_counter_presence;
  PLAT_UI8 alt_counter[STSAFEA_COUNTER_VALUE_SIZE];

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pNonce == NULL) ||
      (pAssociated_data_chunk == NULL && associated_data_chunk_length != 0) ||
      (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0) ||
      (pPlaintext_message_chunk == NULL && message_chunk_length != 0) ||
      (pPlaintext_message_chunk != NULL && message_chunk_length == 0) ||
      (pEncrypted_message_chunk == NULL && message_chunk_length != 0) ||
      (pEncrypted_message_chunk != NULL && message_chunk_length == 0))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* START frame sets total lengths so PROCESS/FINISH can stream deterministically. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eNonce_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&Nonce_length);
  /* Nonce is variable-length in extended START command path. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eNonce, Nonce_length, pNonce);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTotal_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&total_associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTotal_message_length, 4, (PLAT_UI8 *)&total_message_length);
  /* Total lengths let device validate complete stream boundaries. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounter_presence, 1, pCounter_presence);
  /* Counter output is 32-bit, serialized over STSAFEA_COUNTER_VALUE_SIZE bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCounter, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)pCounter);

  if (pCounter_presence == NULL)
  {
    /* Redirect write target to local storage when caller ignores presence flag. */
    eCounter_presence.pData = &alt_counter_presence;
  }

  if (pCounter == NULL)
  {
    /* Redirect counter bytes to local sink when caller does not consume them. */
    eCounter.pData = alt_counter;
  }

  /* - Swap byte order */
  /* All variable-length numeric fields are serialized in network byte order. */
  stse_frame_element_swap_byte_order(&eNonce_length);
  stse_frame_element_swap_byte_order(&eTotal_associated_data_length);
  stse_frame_element_swap_byte_order(&eTotal_message_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  /* Counter bytes are meaningful only when device reports counter presence. */
  if (*pCounter_presence != 0)
  {
    stse_frame_element_swap_byte_order(&eCounter);
  }

  return ret;
}

stse_ReturnCode_t stsafea_aes_ccm_encrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
  /* Delegation preserves behavior and avoids duplicate chunk-processing code. */
  return stsafea_aes_gcm_encrypt_process(pSTSE,
                                         associated_data_chunk_length,
                                         pAssociated_data_chunk,
                                         message_chunk_length,
                                         pPlaintext_message_chunk,
                                         pEncrypted_message_chunk);
}

stse_ReturnCode_t stsafea_aes_ccm_encrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pEncrypted_authentication_tag)
{
  /* Delegation to GCM finish helper relies on shared command framing contract. */
  return stsafea_aes_gcm_encrypt_finish(pSTSE,
                                        authentication_tag_length,
                                        associated_data_chunk_length,
                                        pAssociated_data_chunk,
                                        message_chunk_length,
                                        pPlaintext_message_chunk,
                                        pEncrypted_message_chunk,
                                        pEncrypted_authentication_tag);
}

stse_ReturnCode_t stsafea_aes_ccm_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pEncrypted_authentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  /* Device expects ciphertext+tag aggregate length in CCM decrypt request. */
  PLAT_UI16 encrypted_message_length = message_length + authentication_tag_length;
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pNonce == NULL) ||
      (pAssociated_data == NULL && associated_data_length != 0) ||
      (pAssociated_data != NULL && associated_data_length == 0) ||
      (pEncrypted_message == NULL && message_length != 0) ||
      (pEncrypted_message != NULL && message_length == 0) ||
      (pPlaintext_message == NULL && message_length != 0) ||
      (pPlaintext_message != NULL && message_length == 0) ||
      (pEncrypted_authentication_tag == NULL && authentication_tag_length != 0) ||
      (pEncrypted_authentication_tag != NULL && authentication_tag_length == 0) ||
      (pVerification_result == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [NONCE] [ASSOCIATED DATA LENGTH] ...
  *                       ... [ASSOCIATED DATA] [MESSAGE LENGTH] [ENCRYPTED MESSAGE] [TAG] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Nonce and associated data are authenticated inputs for CCM verification. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eCmd_header,
                                   STSAFEA_HEADER_SIZE,
                                   &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eNonce, STSAFEA_NONCE_SIZE, pNonce);
  /* Decrypt path authenticates same nonce/AAD tuple used during encrypt. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_length, pAssociated_data);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&encrypted_message_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_length, pEncrypted_message);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAuthentication_tag, authentication_tag_length,
                                   pEncrypted_authentication_tag);
  /* Encrypted tag bytes are provided separately from ciphertext chunk bytes. */

  /* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eVerification_result, 1, pVerification_result);
  /* Verification byte must be checked by caller before using plaintext output. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_length, pPlaintext_message);

  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* Transfer returns verification_result byte to indicate tag validation outcome. */

  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_ccm_decrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 Nonce_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 total_associated_data_length,
  PLAT_UI32 total_ciphertext_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_DECRYPT};
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pNonce == NULL) ||
      (pAssociated_data_chunk == NULL && associated_data_chunk_length != 0) ||
      (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0) ||
      (pEncrypted_message_chunk == NULL && message_chunk_length != 0) ||
      (pEncrypted_message_chunk != NULL && message_chunk_length == 0) ||
      (pPlaintext_message_chunk == NULL && message_chunk_length != 0) ||
      (pPlaintext_message_chunk != NULL && message_chunk_length == 0))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Extended START_DECRYPT command carries context and first chunk payload. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eNonce_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&Nonce_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV, Nonce_length, pNonce);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTotal_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&total_associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTotal_ciphertext_length, 4, (PLAT_UI8 *)&total_ciphertext_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);

  /* - Swap byte order */
  /* Total lengths and chunk lengths are transmitted in big-endian format. */
  stse_frame_element_swap_byte_order(&eNonce_length);
  stse_frame_element_swap_byte_order(&eTotal_associated_data_length);
  stse_frame_element_swap_byte_order(&eTotal_ciphertext_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_ccm_decrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  /* Delegation keeps CCM/GCM streaming chunk behavior aligned. */
  return stsafea_aes_gcm_decrypt_process(pSTSE,
                                         associated_data_chunk_length,
                                         pAssociated_data_chunk,
                                         message_chunk_length,
                                         pEncrypted_message_chunk,
                                         pPlaintext_message_chunk);
}

stse_ReturnCode_t stsafea_aes_ccm_decrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  /* Shared finish helper enforces same frame encoding for final chunk stage. */
  return stsafea_aes_gcm_decrypt_finish(pSTSE,
                                        authentication_tag_length,
                                        associated_data_chunk_length,
                                        pAssociated_data_chunk,
                                        message_chunk_length,
                                        pEncrypted_message_chunk,
                                        pAuthentication_tag,
                                        pVerification_result,
                                        pPlaintext_message_chunk);
}

stse_ReturnCode_t stsafea_aes_gcm_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pAuthentication_tag)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  /* One-shot GCM response returns ciphertext and authentication tag. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pIV == NULL || IV_length == 0) ||
      (pAssociated_data == NULL && pPlaintext_message == NULL) ||
      (pAssociated_data == NULL && associated_data_length != 0) ||
      (pAssociated_data != NULL && associated_data_length == 0) ||
      (pPlaintext_message == NULL && message_length != 0) ||
      (pPlaintext_message != NULL && message_length == 0) ||
      (pEncrypted_message == NULL && pPlaintext_message != NULL) ||
      (pAuthentication_tag == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [ASSOCIATED DATA LENGTH] ...
  *                       ... [ASSOCIATED DATA MESSAGE] [MESSAGE LENGTH] [MESSAGE] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* IV length is explicitly sent to support variable IV sizes in GCM mode. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  /* IV length field supports non-96-bit IVs required by some profiles. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV, IV_length, pIV);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_length, pAssociated_data);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eMessage_length,
                                   STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_length, pPlaintext_message);

  /* - Prepare RSP Frame : [HEADER] [ENCRYPTED MESSAGE] [TAG LENGTH] */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Tag length is provided by caller and controls returned tag byte count. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_length, pEncrypted_message);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eAuthentication_tag, authentication_tag_length, pAuthentication_tag);

  stse_frame_element_swap_byte_order(&eIV_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* Length elements are restored implicitly by local frame-object lifetime. */

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_encrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_ENCRYPT};
  /* START response returns ciphertext for the first message chunk only. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pIV == NULL || IV_length == 0) || (pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0) || (pPlaintext_message_chunk == NULL
          && message_chunk_length != 0) || (pPlaintext_message_chunk != NULL && message_chunk_length == 0)
      || (pPlaintext_message_chunk == NULL && pPlaintext_message_chunk != NULL))
  {
    /* Keep legacy validation expression unchanged while documenting its intent. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* START frame includes both AAD chunk and plaintext chunk lengths. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV, IV_length, pIV);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* Chunked API accepts interleaved AAD and message progress per call. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Returned ciphertext chunk length mirrors provided plaintext chunk length. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);

  stse_frame_element_swap_byte_order(&eIV_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* START command primes the internal stream state for PROCESS/FINISH phases. */

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_encrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_PROCESS_ENCRYPT
  };
  /* PROCESS response body is encrypted message bytes for this chunk. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (pPlaintext_message_chunk == NULL && message_chunk_length != 0)
      || (pPlaintext_message_chunk != NULL && message_chunk_length == 0) || (pEncrypted_message_chunk == NULL
                                                                             && pPlaintext_message_chunk != NULL))
  {
    /* Chunks can be AAD-only or message-only but must remain internally valid. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* PROCESS command omits IV/slot because context was set in START phase. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Output chunk maps one-to-one with plaintext chunk length provided. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_encrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_FINISH_ENCRYPT};
  /* FINISH response appends authentication tag after last ciphertext bytes. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (pPlaintext_message_chunk == NULL && message_chunk_length != 0)
      || (pPlaintext_message_chunk != NULL && message_chunk_length == 0) || (pEncrypted_message_chunk == NULL
                                                                             && pPlaintext_message_chunk != NULL)
      || (pAuthentication_tag == NULL && authentication_tag_length == 0))
  {
    /* Tag buffer must be provided when non-zero tag length is requested. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* FINISH includes last AAD/message chunks pending in stream context. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Authentication tag is copied to caller buffer at requested length. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eAuthentication_tag, authentication_tag_length, pAuthentication_tag);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  /* Verification result in response indicates tag check pass/fail state. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pIV == NULL || IV_length == 0) || (pAssociated_data == NULL && pEncrypted_message == NULL)
      || (pAssociated_data == NULL && associated_data_length != 0)
      || (pAssociated_data != NULL && associated_data_length == 0)
      || (pEncrypted_message == NULL && message_length != 0)
      || (pEncrypted_message != NULL && message_length == 0)
      || (pPlaintext_message == NULL && pEncrypted_message != NULL)
      || (pAuthentication_tag == NULL))
  {
    /* Decrypt path requires tag and output buffer when ciphertext is present. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [IV] [ASSOCIATED DATA LENGTH] ...
  *                       ... [ASSOCIATED DATA] [MESSAGE LENGTH] [ENCRYPTED MESSAGE] [TAG] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Message length in decrypt path covers ciphertext bytes only (tag separate). */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV, IV_length, pIV);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_length, pAssociated_data);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame,
                                   eMessage_length,
                                   STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_length, pEncrypted_message);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAuthentication_tag, authentication_tag_length, pAuthentication_tag);

  /* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Response byte layout is status header, verification byte, then plaintext. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eVerification_result, 1, pVerification_result);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_length, pPlaintext_message);

  stse_frame_element_swap_byte_order(&eIV_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_decrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_DECRYPT};
  /* START decrypt response returns first plaintext chunk only. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pIV == NULL || IV_length == 0) || (pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0) || (pPlaintext_message_chunk == NULL
          && message_chunk_length != 0) || (pPlaintext_message_chunk != NULL && message_chunk_length == 0)
      || (pEncrypted_message_chunk == NULL && pPlaintext_message_chunk != NULL))
  {
    /* START decrypt enforces symmetric input/output chunk length contract. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Context fields appear once in START and are omitted in later phases. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eIV, IV_length, pIV);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* PROCESS decrypt streams ciphertext while preserving AAD continuity. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Plaintext chunk reuses caller-owned output buffer provided to API. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);
  stse_frame_element_swap_byte_order(&eIV_length);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_decrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_PROCESS_DECRYPT
  };
  /* PROCESS decrypt response returns corresponding plaintext chunk. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (pPlaintext_message_chunk == NULL && message_chunk_length != 0)
      || (pPlaintext_message_chunk != NULL && message_chunk_length == 0) || (pEncrypted_message_chunk == NULL
                                                                             && pPlaintext_message_chunk != NULL))
  {
    /* Optional AAD/ciphertext chunk combinations must remain pointer-consistent. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Decrypt process frame is minimal: header + chunk lengths + chunk bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Output plaintext chunk size equals input encrypted chunk size. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gcm_decrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_FINISH_DECRYPT};
  /* FINISH response includes verification byte plus final plaintext chunk. */
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pAssociated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (pAssociated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (pPlaintext_message_chunk == NULL && message_chunk_length != 0)
      || (pPlaintext_message_chunk != NULL && message_chunk_length == 0) || (pEncrypted_message_chunk == NULL
                                                                             && pPlaintext_message_chunk != NULL)
      || (pAuthentication_tag == NULL && authentication_tag_length == 0)
      || (pVerification_result == NULL))
  {
    /* Verification output pointer is mandatory for authenticated decrypt finish. */
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* - Prepare CMD Frame */
  STSE_FRAME_ALLOCATE(CmdFrame);
  /* Authentication tag is carried only during FINISH step in streaming decrypt. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAssociated_data, associated_data_chunk_length, pAssociated_data_chunk);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eEncrypted_message, message_chunk_length, pEncrypted_message_chunk);
  /* FINISH decrypt appends tag only once, after all ciphertext chunks. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eAuthentication_tag, authentication_tag_length, pAuthentication_tag);

  /* - Prepare RSP Frame */
  STSE_FRAME_ALLOCATE(RspFrame);
  /* Caller inspects verification_result before trusting returned plaintext bytes. */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eVerification_result, 1, pVerification_result);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, ePlaintext_message, message_chunk_length, pPlaintext_message_chunk);
  stse_frame_element_swap_byte_order(&eAssociated_data_length);
  stse_frame_element_swap_byte_order(&eMessage_length);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
