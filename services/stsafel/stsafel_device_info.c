/**
  ******************************************************************************
  * @file    stsafel_device_info.c
  * @author  CS Application Team
  * @brief   STSAFE-L services for device info (source)
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
#include "services/stsafel/stsafel_device_info.h"
#include "services/stsafel/stsafel_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

stse_return_code_t stsafel_get_device_uid(
  stse_handle_t *p_stse,
  PLAT_UI8 *p_device_uid)
{
  return stsafel_get_data(
           p_stse,
           STSAFEL_INFO_UNIQUE_IDENTIFIER,
           0,
           NULL,
           STSAFEL_UID_LENGTH,
           p_device_uid);
}

stse_return_code_t stsafel_get_device_traceability(
  stse_handle_t *p_stse,
  stsafel_device_traceability_t *p_device_traceability)
{
  return stsafel_get_data(
           p_stse,
           STSAFEL_INFO_TRACEABILITY_DATA,
           0,
           NULL,
           sizeof(stsafel_device_traceability_t),
           (PLAT_UI8 *)p_device_traceability);
}

stse_return_code_t stsafel_get_data(
  stse_handle_t *p_stse,
  stsafel_device_info_tag_t tag,
  PLAT_UI16 additional_data_length,
  PLAT_UI8 *p_additional_data,
  PLAT_UI16 device_info_expected_length,
  PLAT_UI8 *p_device_info)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_GET_DATA;
  PLAT_UI8 rsp_header;

  /* Check stsafel handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if ((p_additional_data == NULL && additional_data_length != 0) ||
      (p_device_info == NULL))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Prepare GET DATA Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, (PLAT_UI8 *)&tag);
  /* [ADDITIONAL DATA] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_additional_data, additional_data_length, p_additional_data);

  /* Prepare GET DATA RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);
  /* [DEVICE INFO] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_device_info, device_info_expected_length, p_device_info);

  return stsafel_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafel_put_data(
  stse_handle_t *p_stse,
  stsafel_device_info_tag_t tag,
  PLAT_UI16 device_info_length,
  PLAT_UI8 *p_device_info)
{
  PLAT_UI8 cmd_header = STSAFEL_CMD_PUT_DATA;
  PLAT_UI8 rsp_header;

  /* Check stsafel handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Check service parameters */
  if (p_device_info == NULL)
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /* Prepare PUT DATA Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);
  /* [TAG] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, (PLAT_UI8 *)&tag);
  /* [DEVICE INFO] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_device_info, device_info_length, p_device_info);

  /* Prepare PUT DATA RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);

  return stsafel_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
