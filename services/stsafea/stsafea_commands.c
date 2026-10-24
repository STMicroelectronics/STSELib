/**
  ******************************************************************************
  * @file    stsafea_commands.c
  * @author  CS Application Team
  * @brief   STSAFE-A command services (source)
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

#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_timings.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/* --------------------------------------------------------------------------
 * Query command authorization records count from STSAFE-A device
 * -------------------------------------------------------------------------- */
stse_return_code_t stsafea_get_command_authorization_records_count(stse_handle_t *p_stse, PLAT_UI8 *p_command_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
  PLAT_UI8 rsp_header;
  PLAT_UI8 table_cr;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Prepare QUERY Frame to read command authorization configuration */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] Command header for QUERY */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [TAG] Subject tag for command authorization table */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, &tag);

  /* Prepare QUERY RSP Frame to receive authorization table change rights and count */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] Response header byte */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [CHANGE RIGHTS] Access conditions for updating the authorization configuration */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_cr, 1, &table_cr);
  /* [COMMAND COUNT] Total count of authorization records present on device */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_command_count, 1, (PLAT_UI8 *)p_command_count);

  /* Execute raw frame transfer with dedicated QUERY command execution delay */
  return stsafea_frame_raw_transfer(p_stse,
                                    &cmd_frame,
                                    &rsp_frame,
                                    stsafea_cmd_timings[p_stse->device_type][cmd_header]);
}

/* --------------------------------------------------------------------------
 * Retrieve command access condition table from STSAFE-A device
 * -------------------------------------------------------------------------- */
stse_return_code_t stsafea_get_command_ac_table(stse_handle_t *p_stse,
                                                PLAT_UI8 total_command_count,
                                                stse_cmd_authorization_cr_t *p_change_rights,
                                                stse_cmd_authorization_record_t *p_record_table)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
  PLAT_UI8 rsp_header = 0;
  PLAT_UI8 raw_data[total_command_count * sizeof(stse_cmd_authorization_record_t)];
  PLAT_UI8 record_index = 0;
  PLAT_UI8 record_array_pos = 0;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Prepare QUERY Frame to query authorization table */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] Command header for QUERY */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, 1, &cmd_header);
  /* [TAG] Subject tag for command authorization table */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_tag, 1, &tag);

  /* Prepare QUERY RSP Frame to receive authorization records */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] Response header byte */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);
  /* [CHANGE RIGHTS] Table change rights field */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_cr, sizeof(stse_cmd_authorization_cr_t), (PLAT_UI8 *)p_change_rights);
  /* [RECORD COUNT] Number of returned authorization records */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_record_count, 1, &record_index);
  /* [RECORD TABLE] Raw byte stream containing all authorization records */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_record_table,
                                   total_command_count * sizeof(stse_cmd_authorization_record_t), raw_data);

  /* Transfer query frame and receive response */
  ret = stsafea_frame_raw_transfer(p_stse,
                                   &cmd_frame,
                                   &rsp_frame,
                                   stsafea_cmd_timings[p_stse->device_type][cmd_header]);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Parse serialized raw authorization records from device response into structured table */
  for (record_index = 0; record_index < total_command_count; record_index++)
  {
    /* Extract primary command header code */
    p_record_table[record_index].header = raw_data[record_array_pos++];
    /* Check for extended command header indicator (0x1F) */
    if (p_record_table[record_index].header == 0x1F)
    {
      /* Read secondary extended command header byte */
      p_record_table[record_index].extended_header = raw_data[record_array_pos++];
    }
    else
    {
      /* Not an extended command: clear extended header field */
      p_record_table[record_index].extended_header = 0;
    }
    /* Read command access condition bits */
    p_record_table[record_index].command_ac = (stse_cmd_access_conditions_t)raw_data[record_array_pos++];
    /* Extract command payload host encryption requirement flag (bit 1) */
    p_record_table[record_index].host_encryption_flags.cmd = (raw_data[record_array_pos] & 0x02) >> 1;
    /* Extract response payload host encryption requirement flag (bit 0) */
    p_record_table[record_index].host_encryption_flags.rsp = (raw_data[record_array_pos++] & 0x01);
  }

  return ret;
}

/* --------------------------------------------------------------------------
 * Update cached personalization info from device configuration
 * -------------------------------------------------------------------------- */
stse_return_code_t stsafea_perso_info_update(stse_handle_t *p_stse)
{
  stse_return_code_t ret;
  PLAT_UI8 total_command_count = 0;
  stse_cmd_authorization_cr_t change_rights;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /* Reset all cached encryption status bitfields to unencrypted */
  p_stse->perso_info.cmd_encryption_status = 0;
  p_stse->perso_info.rsp_encryption_status = 0;
  p_stse->perso_info.ext_cmd_encryption_status = 0;
  p_stse->perso_info.ext_rsp_encryption_status = 0;
  /* Reset command access conditions to open access defaults (0x5555555555555555) */
  p_stse->perso_info.cmd_ac_status = 0x5555555555555555;
  p_stse->perso_info.ext_cmd_ac_status = 0x5555555555555555;

  /* Query the device for total number of configured command authorization records */
  ret = stsafea_get_command_authorization_records_count(p_stse, &total_command_count);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Allocate stack buffer for the authorization records */
  stse_cmd_authorization_record_t record_table[total_command_count];

  /* Retrieve all command authorization records from target device */
  ret = stsafea_get_command_ac_table(p_stse,
                                     total_command_count,
                                     &change_rights,
                                     record_table);
  if (ret != STSE_OK)
  {
    return ret;
  }

  /* Populate device personalization info cache from fetched records */
  for (PLAT_UI8 i = 0; i < total_command_count; i++)
  {
    /* Check if record corresponds to a standard or extended command */
    if (record_table[i].extended_header == 0)
    {
      /* Cache access condition for standard command */
      stsafea_perso_info_set_cmd_ac(&p_stse->perso_info, record_table[i].header, record_table[i].command_ac);
      /* Cache command encryption requirement flag */
      stsafea_perso_info_set_cmd_encrypt_flag(&p_stse->perso_info, record_table[i].header,
                                              record_table[i].host_encryption_flags.cmd);
      /* Cache response encryption requirement flag */
      stsafea_perso_info_set_rsp_encrypt_flag(&p_stse->perso_info, record_table[i].header,
                                              record_table[i].host_encryption_flags.rsp);
    }
    else
    {
      /* Cache access condition for extended command */
      stsafea_perso_info_set_ext_cmd_ac(&p_stse->perso_info, record_table[i].extended_header,
                                        record_table[i].command_ac);
      /* Cache extended command encryption requirement flag */
      stsafea_perso_info_set_ext_cmd_encrypt_flag(&p_stse->perso_info, record_table[i].extended_header,
                                                  record_table[i].host_encryption_flags.cmd);
      /* Cache extended response encryption requirement flag */
      stsafea_perso_info_set_ext_rsp_encrypt_flag(&p_stse->perso_info, record_table[i].extended_header,
                                                  record_table[i].host_encryption_flags.rsp);
    }
  }

  return STSE_OK;
}

/* --------------------------------------------------------------------------
 * Get access condition for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t *p_protection)
{
  /* Extract the 2-bit access condition field for the command code */
  *p_protection = (stse_cmd_access_conditions_t)((p_perso->cmd_ac_status >> (command_code + command_code)) & 0x03);
}

/* --------------------------------------------------------------------------
 * Get access condition for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_ext_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t *p_protection)
{
  /* Extract the 2-bit access condition field for the extended command code */
  *p_protection = (stse_cmd_access_conditions_t)((p_perso->ext_cmd_ac_status >> (command_code + command_code)) & 0x03);
}

/* --------------------------------------------------------------------------
 * Get command request encryption flag for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag)
{
  /* Extract the 1-bit encryption flag for the command code */
  *p_enc_flag = ((p_perso->cmd_encryption_status >> command_code) & 0x01);
}

/* --------------------------------------------------------------------------
 * Get response encryption flag for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 *p_enc_flag)
{
  /* Extract the 1-bit response encryption flag for the command code */
  *p_enc_flag = ((p_perso->rsp_encryption_status >> command_code) & 0x01);
}

/* --------------------------------------------------------------------------
 * Get command request encryption flag for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_ext_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                                 PLAT_UI8 *p_enc_flag)
{
  /* Extract the 1-bit command encryption flag for the extended command code */
  *p_enc_flag = ((p_perso->ext_cmd_encryption_status >> command_code) & 0x01);
}

/* --------------------------------------------------------------------------
 * Get response encryption flag for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_get_ext_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                                 PLAT_UI8 *p_enc_flag)
{
  /* Extract the 1-bit response encryption flag for the extended command code */
  *p_enc_flag = ((p_perso->ext_rsp_encryption_status >> command_code) & 0x01);
}

/* --------------------------------------------------------------------------
 * Set access condition for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t protection)
{
  PLAT_UI8 offset = command_code + command_code;
  /* Clear previous 2-bit protection value then set new protection level */
  p_perso->cmd_ac_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
  p_perso->cmd_ac_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

/* --------------------------------------------------------------------------
 * Set access condition for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_ext_cmd_ac(stse_perso_info_t *p_perso, PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t protection)
{
  PLAT_UI8 offset = command_code + command_code;
  /* Clear previous 2-bit protection value then set new protection level */
  p_perso->ext_cmd_ac_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
  p_perso->ext_cmd_ac_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

/* --------------------------------------------------------------------------
 * Set command request encryption flag for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Update 1-bit command encryption flag in status bitfield */
  if (enc_flag)
  {
    p_perso->cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    p_perso->cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

/* --------------------------------------------------------------------------
 * Set response encryption flag for standard command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Update 1-bit response encryption flag in status bitfield */
  if (enc_flag)
  {
    p_perso->rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    p_perso->rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

/* --------------------------------------------------------------------------
 * Set command request encryption flag for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_ext_cmd_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Update 1-bit extended command encryption flag in status bitfield */
  if (enc_flag)
  {
    p_perso->ext_cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    p_perso->ext_cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

/* --------------------------------------------------------------------------
 * Set response encryption flag for extended command code
 * -------------------------------------------------------------------------- */
void stsafea_perso_info_set_ext_rsp_encrypt_flag(stse_perso_info_t *p_perso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Update 1-bit extended response encryption flag in status bitfield */
  if (enc_flag)
  {
    p_perso->ext_rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    p_perso->ext_rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
