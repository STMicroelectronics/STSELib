/**
  ******************************************************************************
  * @file    stsafea_mac.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for message Authentication Code - MAC (source)
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_mac.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_cmac_hmac_compute(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 *p_message,
  PLAT_UI8 message_length,
  PLAT_UI8 *p_mac,
  PLAT_UI8 mac_length)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_MAC;
  PLAT_UI8 sub_command_distinguisher = 0x03;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_mac == NULL) || (p_message == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare GENERATE MAC Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [CMD DISTINGUISHER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_sub_command_distinguisher, 1, &sub_command_distinguisher);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [MAC LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_mac_length, 1, &mac_length);
  /* [MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message, message_length, p_message);

  /* Prepare GENERATE MAC RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [MAC] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_mac, mac_length, p_mac);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_cmac_hmac_verify(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI8 *p_mac,
  PLAT_UI8 mac_length,
  PLAT_UI8 *p_message,
  PLAT_UI8 message_length,
  PLAT_UI8 *p_verification_result)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_MAC;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_mac == NULL) || (p_message == NULL) || (p_verification_result == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare VERIFY MAC Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [CMD DISTINGUISHER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_sub_command_distinguisher, 1, &sub_command_distinguisher);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &slot_number);
  /* [MAC LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_mac_length, 1, &mac_length);
  /* [MAC] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_mac, mac_length, p_mac);
  /* [MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message, message_length, p_message);

  /* Prepare VERIFY MAC RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [VERIFICATION RESULT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_verification_result, 1, p_verification_result);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_aes_gmac_compute(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *p_authentication_tag)
{
  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  return (stsafea_aes_gcm_encrypt(
            p_stse,
            slot_number,
            authentication_tag_length,
            IV_length,
            p_iv,
            associated_data_length,
            p_associated_data,
            0,
            NULL,
            NULL,
            p_authentication_tag));
}

stse_return_code_t stsafea_aes_gmac_verify(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *p_iv,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *p_associated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *p_authentication_tag,
  PLAT_UI8 *p_verification_result)
{
  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  return (stsafea_aes_gcm_decrypt(p_stse,
                                  slot_number,
                                  authentication_tag_length,
                                  IV_length,
                                  p_iv,
                                  associated_data_length,
                                  p_associated_data,
                                  0,
                                  NULL,
                                  p_authentication_tag,
                                  p_verification_result,
                                  NULL));
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
