/**
  ******************************************************************************
  * @file    stsafea_aes_gcm.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for AES GCM symmetric key cryptography (source)
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

#include "services/stsafea/stsafea_aes_gcm.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_aes_gcm_encrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_plaintext_message,
  PLAT_UI8 *p_encrypted_message,
  PLAT_UI8 *p_authentication_tag)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_iv == NULL || IV_length == 0) ||
      (p_associated_data == NULL && p_plaintext_message == NULL) ||
      (p_associated_data == NULL && associated_data_length != 0) ||
      (p_associated_data != NULL && associated_data_length == 0) ||
      (p_plaintext_message == NULL && message_length != 0) ||
      (p_plaintext_message != NULL && message_length == 0) ||
      (p_encrypted_message == NULL && p_plaintext_message != NULL) ||
      (p_authentication_tag == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare ENCRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [CMD DISTINGUISHER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_sub_command_distinguisher, 1, &sub_command_distinguisher);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [IV LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  /* [IV] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv, IV_length, p_iv);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_length, p_associated_data);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* [PLAINTEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_plaintext_message, message_length, p_plaintext_message);

  /* Prepare ENCRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_encrypted_message, message_length, p_encrypted_message);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_authentication_tag, authentication_tag_length, p_authentication_tag);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_iv_length);
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_encrypt_start(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_plaintext_message_chunk,
  PLAT_UI8 *p_encrypted_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_ENCRYPT};
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_iv == NULL || IV_length == 0) || (p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0) || (p_plaintext_message_chunk == NULL
          && message_chunk_length != 0) || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_plaintext_message_chunk == NULL && p_plaintext_message_chunk != NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare START ENCRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [IV LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  /* [IV] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv, IV_length, p_iv);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [PLAINTEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Prepare START ENCRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_iv_length);
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_encrypt_process(
  stse_handle_t *p_stse,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_plaintext_message_chunk,
  PLAT_UI8 *p_encrypted_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_PROCESS_ENCRYPT
                                                 };
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (p_plaintext_message_chunk == NULL && message_chunk_length != 0)
      || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_encrypted_message_chunk == NULL && p_plaintext_message_chunk != NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare PROCESS ENCRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [PLAINTEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Prepare PROCESS ENCRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_encrypt_finish(
  stse_handle_t *p_stse,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_plaintext_message_chunk,
  PLAT_UI8 *p_encrypted_message_chunk,
  PLAT_UI8 *p_authentication_tag)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_FINISH_ENCRYPT
                                                 };
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (p_plaintext_message_chunk == NULL && message_chunk_length != 0)
      || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_encrypted_message_chunk == NULL && p_plaintext_message_chunk != NULL)
      || (p_authentication_tag == NULL && authentication_tag_length == 0))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare FINISH ENCRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [PLAINTEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Prepare FINISH ENCRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_authentication_tag, authentication_tag_length, p_authentication_tag);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_decrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_encrypted_message,
  PLAT_UI8 *p_authentication_tag,
  PLAT_UI8 *p_verification_result,
  PLAT_UI8 *p_plaintext_message)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_iv == NULL || IV_length == 0)
      || (p_associated_data == NULL && p_encrypted_message == NULL)
      || (p_associated_data == NULL && associated_data_length != 0)
      || (p_associated_data != NULL && associated_data_length == 0)
      || (p_encrypted_message == NULL && message_length != 0)
      || (p_encrypted_message != NULL && message_length == 0)
      || (p_plaintext_message == NULL && p_encrypted_message != NULL)
      || (p_authentication_tag == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare DECRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [CMD DISTINGUISHER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_sub_command_distinguisher, 1, &sub_command_distinguisher);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [IV LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  /* [IV] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv, IV_length, p_iv);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_length, p_associated_data);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_encrypted_message, message_length, p_encrypted_message);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_authentication_tag, authentication_tag_length, p_authentication_tag);

  /* Prepare DECRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [VERIFICATION RESULT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_verification_result, 1, p_verification_result);
  /* [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_plaintext_message, message_length, p_plaintext_message);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_iv_length);
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_decrypt_start(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_encrypted_message_chunk,
  PLAT_UI8 *p_plaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_DECRYPT};
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_iv == NULL || IV_length == 0) || (p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0) || (p_plaintext_message_chunk == NULL
          && message_chunk_length != 0) || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_encrypted_message_chunk == NULL && p_plaintext_message_chunk != NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare START DECRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [IV LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv_length, STSAFEA_GENERIC_LENGTH_SIZE, (PLAT_UI8 *)&IV_length);
  /* [IV] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_iv, IV_length, p_iv);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);

  /* Prepare START DECRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_iv_length);
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_decrypt_process(
  stse_handle_t *p_stse,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_encrypted_message_chunk,
  PLAT_UI8 *p_plaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_PROCESS_DECRYPT
                                                 };
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (p_plaintext_message_chunk == NULL && message_chunk_length != 0)
      || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_encrypted_message_chunk == NULL && p_plaintext_message_chunk != NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare PROCESS DECRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);

  /* Prepare PROCESS DECRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gcm_decrypt_finish(
  stse_handle_t *p_stse,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *p_associated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *p_encrypted_message_chunk,
  PLAT_UI8 *p_authentication_tag,
  PLAT_UI8 *p_verification_result,
  PLAT_UI8 *p_plaintext_message_chunk)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_FINISH_DECRYPT
                                                 };
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_associated_data_chunk == NULL && associated_data_chunk_length != 0)
      || (p_associated_data_chunk != NULL && associated_data_chunk_length == 0)
      || (p_plaintext_message_chunk == NULL && message_chunk_length != 0)
      || (p_plaintext_message_chunk != NULL && message_chunk_length == 0)
      || (p_encrypted_message_chunk == NULL && p_plaintext_message_chunk != NULL)
      || (p_authentication_tag == NULL && authentication_tag_length == 0)
      || (p_verification_result == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare FINISH DECRYPT Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [ASSOCIATED DATA LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&associated_data_chunk_length);
  /* [ASSOCIATED DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_associated_data, associated_data_chunk_length,
                                   p_associated_data_chunk);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_chunk_length);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_encrypted_message, message_chunk_length, p_encrypted_message_chunk);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_authentication_tag, authentication_tag_length, p_authentication_tag);

  /* Prepare FINISH DECRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [VERIFICATION RESULT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_verification_result, 1, p_verification_result);
  /* [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_plaintext_message, message_chunk_length, p_plaintext_message_chunk);

  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_associated_data_length);
  stse_frame_element_swap_byte_order(&e_message_length);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */

