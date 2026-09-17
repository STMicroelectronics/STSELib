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

stse_ReturnCode_t stsafea_get_command_count(stse_Handler_t *pSTSE, PLAT_UI8 *pCommand_count)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
  PLAT_UI8 rsp_header;
  PLAT_UI8 table_cr;

  /* Query returns authorization-table control rights plus the number of records. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &tag);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCR, 1, &table_cr);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCommand_count, 1, (PLAT_UI8 *)pCommand_count);

  /*- Perform Transfer*/
  return stsafea_frame_raw_transfer(pSTSE,
                                    &CmdFrame,
                                    &RspFrame,
                                    stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_get_command_AC_table(stse_Handler_t *pSTSE,
                                               PLAT_UI8 total_command_count,
                                               stse_cmd_authorization_CR_t *pChange_rights,
                                               stse_cmd_authorization_record_t *pRecord_table)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
  PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_COMMAND_AUTHORIZATION_CONFIG;
  PLAT_UI8 rsp_header = 0;
  PLAT_UI8 raw_data[total_command_count * sizeof(stse_cmd_authorization_record_t)];
  PLAT_UI8 record_index = 0;
  PLAT_UI8 record_array_pos = 0;

  /* Raw response is parsed manually because entries are variadic due to optional
   * extended-header byte for vendor-specific commands. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, 1, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eTag, 1, &tag);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eCR, sizeof(stse_cmd_authorization_CR_t), (PLAT_UI8 *)pChange_rights);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRecordCount, 1, &record_index);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame,
                                   eRecordTable,
                                   total_command_count * sizeof(stse_cmd_authorization_record_t),
                                   raw_data);

  /*- Perform Transfer*/
  ret = stsafea_frame_raw_transfer(pSTSE,
                                   &CmdFrame,
                                   &RspFrame,
                                   stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
  if (ret != STSE_OK)
  {
    return ret;
  }

  for (record_index = 0; record_index < total_command_count; record_index++)
  {
    /* Table entries are a compact byte stream: [header][opt ext header][AC][enc flags].
     * Parse sequentially and normalize into fixed record fields. */
    pRecord_table[record_index].header = raw_data[record_array_pos++];
    if (pRecord_table[record_index].header == 0x1F)
    {
      /* Header 0x1F indicates extended command namespace and consumes one extra byte. */
      pRecord_table[record_index].extended_header = raw_data[record_array_pos++];
    }
    else
    {
      pRecord_table[record_index].extended_header = 0;
    }
    pRecord_table[record_index].command_AC = (stse_cmd_access_conditions_t)raw_data[record_array_pos++];
    /* Bit1=cmd encryption flag, Bit0=response encryption flag. */
    pRecord_table[record_index].host_encryption_flags.cmd = (raw_data[record_array_pos] & 0x02) >> 1;
    pRecord_table[record_index].host_encryption_flags.rsp = (raw_data[record_array_pos++] & 0x01);
  }

  return ret;
}

stse_ReturnCode_t stsafea_perso_info_update(stse_Handler_t *pSTSE)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 total_command_count = 0;
  stse_cmd_authorization_CR_t change_rights;

  /* Refresh of personalization cache is idempotent and can be called at session
   * startup to align host-side policy decisions with current device settings. */

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  pSTSE->perso_info.cmd_encryption_status = 0;
  pSTSE->perso_info.rsp_encryption_status = 0;
  pSTSE->perso_info.ext_cmd_encryption_status = 0;
  pSTSE->perso_info.ext_rsp_encryption_status = 0;
  /* Default AC bits to '0b01' pattern (host-protected policy placeholder)
   * until actual command table values are fetched from the device. */
  pSTSE->perso_info.cmd_AC_status = 0x5555555555555555;
  pSTSE->perso_info.ext_cmd_AC_status = 0x5555555555555555;

  ret = stsafea_get_command_count(pSTSE, &total_command_count);
  if (ret != STSE_OK)
  {
    return ret;
  }

  stse_cmd_authorization_record_t record_table[total_command_count];

  ret = stsafea_get_command_AC_table(pSTSE,
                                     total_command_count,
                                     &change_rights,
                                     record_table);
  if (ret != STSE_OK)
  {
    return ret;
  }

  for (PLAT_UI8 i = 0; i < total_command_count; i++)
  {
    /* Persist authorization/encryption metadata in fast bitfields used by
     * the frame-transfer layer when selecting protection policies. */
    if (record_table[i].extended_header == 0)
    {
      stsafea_perso_info_set_cmd_AC(&pSTSE->perso_info, record_table[i].header, record_table[i].command_AC);
      stsafea_perso_info_set_cmd_encrypt_flag(&pSTSE->perso_info, record_table[i].header,
                                              record_table[i].host_encryption_flags.cmd);
      stsafea_perso_info_set_rsp_encrypt_flag(&pSTSE->perso_info, record_table[i].header,
                                              record_table[i].host_encryption_flags.rsp);
    }
    else
    {
      stsafea_perso_info_set_ext_cmd_AC(&pSTSE->perso_info,
                                        record_table[i].extended_header,
                                        record_table[i].command_AC);
      stsafea_perso_info_set_ext_cmd_encrypt_flag(&pSTSE->perso_info, record_table[i].extended_header,
                                                  record_table[i].host_encryption_flags.cmd);
      stsafea_perso_info_set_ext_rsp_encrypt_flag(&pSTSE->perso_info, record_table[i].extended_header,
                                                  record_table[i].host_encryption_flags.rsp);
    }
  }

  return STSE_OK;
}

void stsafea_perso_info_get_cmd_AC(stse_perso_info_t *pPerso, PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t *pProtection)
{
  /* AC is packed as 2-bit fields indexed by command code. */
  /* Bits decode to stse_cmd_access_conditions_t domain values. */
  *pProtection = (stse_cmd_access_conditions_t)((pPerso->cmd_AC_status >> (command_code + command_code)) & 0x03);
}

void stsafea_perso_info_get_ext_cmd_AC(stse_perso_info_t *pPerso, PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t *pProtection)
{
  /* Extended command AC follows same packing strategy as base commands. */
  /* Command index is used directly as bitfield slot selector. */
  *pProtection = (stse_cmd_access_conditions_t)((pPerso->ext_cmd_AC_status >> (command_code + command_code)) & 0x03);
}

void stsafea_perso_info_get_cmd_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 *pEnc_flag)
{
  /* One bit per command indicates whether command payload must be encrypted. */
  /* Returned value is normalized to 0 or 1. */
  *pEnc_flag = ((pPerso->cmd_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_rsp_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 *pEnc_flag)
{
  /* One bit per command indicates whether response payload must be encrypted. */
  /* Getter avoids recomputing table lookups during frame-transfer decisions. */
  *pEnc_flag = ((pPerso->rsp_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_ext_cmd_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 *pEnc_flag)
{
  /* Extended command encryption flags are kept in a dedicated bitmap. */
  /* Separation prevents collisions with base command code space. */
  *pEnc_flag = ((pPerso->ext_cmd_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_get_ext_rsp_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 *pEnc_flag)
{
  /* Extended response encryption flags mirror extended command indexing. */
  /* Caller uses this bit to decide whether to unwrap extended responses. */
  *pEnc_flag = ((pPerso->ext_rsp_encryption_status >> command_code) & 0x01);
}

void stsafea_perso_info_set_cmd_AC(stse_perso_info_t *pPerso, PLAT_UI8 command_code,
                                   stse_cmd_access_conditions_t protection)
{
  /* Two bits per command code are packed in cmd_AC_status. */
  PLAT_UI8 offset = command_code + command_code;
  pPerso->cmd_AC_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
  pPerso->cmd_AC_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

void stsafea_perso_info_set_ext_cmd_AC(stse_perso_info_t *pPerso, PLAT_UI8 command_code,
                                       stse_cmd_access_conditions_t protection)
{
  /* Extended commands use a dedicated packed bitmap with same 2-bit encoding. */
  PLAT_UI8 offset = command_code + command_code;
  pPerso->ext_cmd_AC_status &= (PLAT_UI64) ~(((PLAT_UI64)0x03) << offset);
  pPerso->ext_cmd_AC_status |= (PLAT_UI64)((PLAT_UI64)protection << offset);
}

void stsafea_perso_info_set_cmd_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Setter writes a single command bit in the command-encryption bitmap. */
  /* Non-zero input is treated as enable and written at command_code position. */
  if (enc_flag)
  {
    pPerso->cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    /* Clear target bit when encryption is disabled for this command. */
    /* Bit clear uses the same shifted position as set path for symmetry. */
    pPerso->cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

void stsafea_perso_info_set_rsp_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Setter writes a single command bit in the response-encryption bitmap. */
  /* This bitmap is consumed only for response-side policy checks. */
  if (enc_flag)
  {
    pPerso->rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    /* Clear target bit when response encryption is disabled. */
    pPerso->rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

void stsafea_perso_info_set_ext_cmd_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Extended-command setter mirrors base-command bit write semantics. */
  /* Extended headers have their own command code domain and bitmap storage. */
  if (enc_flag)
  {
    pPerso->ext_cmd_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    /* Clear bit when command payload protection is not required. */
    pPerso->ext_cmd_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

void stsafea_perso_info_set_ext_rsp_encrypt_flag(stse_perso_info_t *pPerso, PLAT_UI8 command_code, PLAT_UI8 enc_flag)
{
  /* Extended-response setter updates the matching per-command policy bit. */
  /* Keeping response policy explicit avoids coupling with command policy bits. */
  if (enc_flag)
  {
    pPerso->ext_rsp_encryption_status |= (PLAT_UI32)(enc_flag << command_code);
  }
  else
  {
    /* Clear bit when response encryption is not requested for that command. */
    pPerso->ext_rsp_encryption_status &= (PLAT_UI32) ~(enc_flag << command_code);
  }
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
