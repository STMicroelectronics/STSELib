/**
  ******************************************************************************
  * @file    stsafea_aes_ecb.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for AES ECB symmetric key cryptography (source)
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

#include "services/stsafea/stsafea_aes_ecb.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_aes_ecb_encrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_plaintext_message,
  PLAT_UI8 *p_encrypted_message)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_ENCRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_plaintext_message == NULL) || (p_encrypted_message == NULL))
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
  /* [PLAINTEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_plaintext_message, message_length, p_plaintext_message);

  /* Prepare ENCRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_encrypted_message, message_length, p_encrypted_message);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

#ifdef STSE_CONF_USE_HOST_SESSION
  if (ret != STSE_OK)
  {
    memset(p_encrypted_message, 0, message_length);
  }
#endif /* STSE_CONF_USE_HOST_SESSION */
  return ret;
}

stse_return_code_t stsafea_aes_ecb_decrypt(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_encrypted_message,
  PLAT_UI8 *p_plaintext_message)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_DECRYPT;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_plaintext_message == NULL) || (p_encrypted_message == NULL))
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
  /* [ENCRYPTED MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_encrypted_message, message_length, p_encrypted_message);

  /* Prepare DECRYPT RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PLAIN TEXT MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_plaintext_message, message_length, p_plaintext_message);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

#ifdef STSE_CONF_USE_HOST_SESSION
  if (ret != STSE_OK)
  {
    memset(p_plaintext_message, 0, message_length);
  }
#endif /* STSE_CONF_USE_HOST_SESSION */
  return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */

