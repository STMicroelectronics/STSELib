/**
  ******************************************************************************
  * @file    stsafea_wrap_unwrap.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for wrap & unwrap (source)
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
#include "services/stsafea/stsafea_wrap_unwrap.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_wrap_payload(stse_handle_t *p_stse,
                                        PLAT_UI8 wrap_key_slot,
                                        PLAT_UI8 *p_payload,
                                        PLAT_UI16 payload_size,
                                        PLAT_UI8 *p_wrapped_payload,
                                        PLAT_UI16 wrapped_payload_size)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_WRAP_LOCAL_ENVELOPE;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_payload == NULL) || (p_wrapped_payload == NULL) ||
      (payload_size > STSAFEA_WRAP_LOCAL_ENVELOPE_MAX_PAYLOAD_SIZE) || (payload_size == 0) ||
      (wrapped_payload_size != (payload_size + STSAFEA_LOCAL_ENVELOPE_HEADER_SIZE)))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  /* Prepare WRAP LOCAL ENVELOPE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &wrap_key_slot);
  /* [PAYLOAD] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_payload, payload_size, p_payload);

  /* Prepare WRAP LOCAL ENVELOPE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [WRAPPED PAYLOAD] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_wrapped, wrapped_payload_size, p_wrapped_payload);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_unwrap_payload(stse_handle_t *p_stse,
                                          PLAT_UI8 wrap_key_slot,
                                          PLAT_UI8 *p_wrapped_payload,
                                          PLAT_UI16 wrapped_payload_size,
                                          PLAT_UI8 *p_payload,
                                          PLAT_UI16 payload_size)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if ((p_payload == NULL) || (p_wrapped_payload == NULL) ||
      (wrapped_payload_size > STSAFEA_UNWRAP_LOCAL_ENVELOPE_MAX_SIZE) ||
      (wrapped_payload_size < STSAFEA_LOCAL_ENVELOPE_HEADER_SIZE) ||
      (wrapped_payload_size != (payload_size + STSAFEA_LOCAL_ENVELOPE_HEADER_SIZE)))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  /* Prepare UNWRAP LOCAL ENVELOPE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, 1, &wrap_key_slot);
  /* [WRAPPED PAYLOAD] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_payload, wrapped_payload_size, p_wrapped_payload);

  /* Prepare UNWRAP LOCAL ENVELOPE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PAYLOAD] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_wrapped, payload_size, p_payload);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
