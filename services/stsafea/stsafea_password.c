/**
  ******************************************************************************
  * @file    stsafea_password.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for password (source)
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
#include "services/stsafea/stsafea_password.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_verify_password(
  stse_handle_t *p_stse,
  PLAT_UI8 *p_password_buffer,
  PLAT_UI8 password_length,
  PLAT_UI8 *p_verification_status,
  PLAT_UI8 *p_remaining_tries)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_PASSWORD;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((password_length != STSAFEA_PASSWORD_LENGTH))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Prepare VERIFY PASSWORD Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [PASSWORD] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_password, password_length, p_password_buffer);

  /* Prepare VERIFY PASSWORD RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [VERIFICATION STATUS] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_ver_stat, 1, p_verification_status);
  /* [REMAINING TRIES] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rem_tri, 1, p_remaining_tries);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_delete_password(stse_handle_t *p_stse)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_DELETE;
  PLAT_UI8 tag = STSAFEA_DELETE_TAG_PASSWORD;
  PLAT_UI8 rsp_header;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Prepare DELETE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, &tag);

  /* Prepare DELETE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);

  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
