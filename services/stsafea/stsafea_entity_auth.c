/**
  ******************************************************************************
  * @file    stsafea_entity_auth.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for entity authentication (source)
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

#include "services/stsafea/stsafea_entity_auth.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_generate_challenge(
  stse_handle_t *p_stse,
  PLAT_UI8 challenge_size,
  PLAT_UI8 *p_challenge)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE
                                                 };
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_challenge == NULL) || (challenge_size < STSE_EDDSA_CHALLENGE_SIZE))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Prepare GENERATE CHALLENGE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* Prepare GENERATE CHALLENGE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [CHALLENGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_challenge, STSE_EDDSA_CHALLENGE_SIZE, p_challenge);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_verify_entity_signature(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *p_signature,
  PLAT_UI8 *p_signature_validity)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_signature == NULL || p_signature_validity == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 filler = 0x00;
  STSE_FRAME_ELEMENT_ALLOCATE(e_filler, 1, &filler);

  /* Prepare VERIFY ENTITY SIGNATURE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [FILLER] */
  stse_frame_push_element(&cmd_frame, &e_filler);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  /* Signature elements */
  PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  /* [SIGNATURE R LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE R] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   p_signature);
  /* [SIGNATURE S LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE S] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   p_signature + (stse_ecc_info_table[key_type].signature_size >> 1));

  PLAT_UI8 rsp_header;
  /* Prepare VERIFY ENTITY SIGNATURE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [VERIFICATION RESULT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_validity, 1, p_signature_validity);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

  if (ret != STSE_OK)
  {
    *p_signature_validity = STSAFEA_FALSE;
  }

  return (ret);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
