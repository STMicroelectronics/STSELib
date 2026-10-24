/**
  ******************************************************************************
  * @file    stsafel_ecc.c
  * @author  CS Application Team
  * @brief   STSAFE-L services for ECC (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_ecc.h"
#include "services/stsafel/stsafel_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_return_code_t stsafel_ecc_generate_signature(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *p_challenge,
  PLAT_UI16 challenge_length,
  PLAT_UI8 *p_signature)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_GENERATE_SIGNATURE;
  PLAT_UI8 rsp_header;
  PLAT_UI8 internal_data_subject = STSAFEL_ECC_SIGNATURE_SUBJECT_NONE;

  /* Check stsafel handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((challenge_length != STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH) ||
      (p_challenge == NULL) ||
      (p_signature == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Prepare GENERATE SIGNATURE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  /* [CHALLENGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_challenge, challenge_length, (PLAT_UI8 *)p_challenge);
  /* [INTERNAL DATA SUBJECT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_internal_data_subject, 1, &internal_data_subject);

  /* Prepare GENERATE SIGNATURE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  /* [SIGNATURE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature, stse_ecc_info_table[key_type].signature_size, p_signature);

  return stsafel_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
