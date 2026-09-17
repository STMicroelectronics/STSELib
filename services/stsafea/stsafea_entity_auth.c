/**
  ******************************************************************************
  * @file    stsafea_entity_auth.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for entity authentication (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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

stse_ReturnCode_t stsafea_generate_challenge(
  stse_Handler_t *pSTSE,
  PLAT_UI8 challenge_size,
  PLAT_UI8 *pChallenge)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_GENERATE_CHALLENGE
  };
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return STSE_SERVICE_HANDLER_NOT_INITIALISED;
  }

  if ((pChallenge == NULL) || (challenge_size < STSE_EDDSA_CHALLENGE_SIZE))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eChallenge, STSE_EDDSA_CHALLENGE_SIZE, pChallenge);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_verify_entity_signature(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *pSignature,
  PLAT_UI8 *pSignature_validity)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] =
  {
    STSAFEA_EXTENDED_COMMAND_PREFIX,
    STSAFEA_EXTENDED_CMD_VERIFY_ENTITY_SIGNATURE
  };

  /* - Check stsafe handler initialization */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pSignature == NULL || pSignature_validity == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 filler = 0x00;
  STSE_FRAME_ELEMENT_ALLOCATE(eFiller, 1, &filler);

  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  stse_frame_push_element(&CmdFrame, &eFiller);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);

  /* Signature elements */
  PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_R, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   pSignature);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   pSignature_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   pSignature + (stse_ecc_info_table[key_type].signature_size >> 1));

  PLAT_UI8 rsp_header;
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eSignature_validity, 1, pSignature_validity);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (ret != STSE_OK)
  {
    *pSignature_validity = STSAFEA_FALSE;
  }

  return (ret);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
