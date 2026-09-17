/**
  ******************************************************************************
  * @file    stsafea_mac.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for message Authentication Code - MAC (source)
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

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_mac.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_cmac_hmac_compute(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 *pMessage,
  PLAT_UI8 message_length,
  PLAT_UI8 *pMac,
  PLAT_UI8 mac_length)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_MAC;
  PLAT_UI8 sub_command_distinguisher = 0x03;
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pMac == NULL) || (pMessage == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* The MAC command uses an explicit one-byte MAC length field, allowing
   * callers to request truncated tags when policy permits it. */

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MAC LENGTH] [MESSAGE] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMac_length, 1, &mac_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_length, pMessage);

  /* - Prepare RSP Frame : [HEADER] [MAC] */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eMac, mac_length, pMac);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_cmac_hmac_verify(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 *pMac,
  PLAT_UI8 mac_length,
  PLAT_UI8 *pMessage,
  PLAT_UI8 message_length,
  PLAT_UI8 *pVerification_result)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_MAC;
  PLAT_UI8 sub_command_distinguisher = 0x02;
  PLAT_UI8 rsp_header;

  /* - Check stsafe handler initialization */

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pMac == NULL) || (pMessage == NULL) || (pVerification_result == NULL))
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Verification sends both message and candidate MAC to the device.
   * The response returns a dedicated status byte instead of a recomputed tag. */

  /* - Prepare CMD Frame : [HEADER] [CMD DISTINGUISHER] [SLOT] [MAC LENGTH] [MESSAGE] */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSub_command_distinguisher, 1, &sub_command_distinguisher);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eSlot_number, 1, &slot_number);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMac_length, 1, &mac_length);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMac, mac_length, pMac);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_length, pMessage);

  /* - Prepare RSP Frame : [HEADER] [VERIFICATION RESULT] */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eVerification_result, 1, pVerification_result);

  /* - Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_aes_gmac_compute(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pAuthentication_tag)
{
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* GMAC is the tag-only mode of GCM: no plaintext payload is provided,
   * only associated data contributes to the authentication tag. */

  return (stsafea_aes_gcm_encrypt(
            pSTSE,
            slot_number,
            authentication_tag_length,
            IV_length,
            pIV,
            associated_data_length,
            pAssociated_data,
            0,
            NULL,
            NULL,
            pAuthentication_tag));
}

stse_ReturnCode_t stsafea_aes_gmac_verify(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result)
{
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Verification reuses GCM decrypt with zero payload; the returned status byte
   * indicates whether the provided authentication tag matches the computed one. */

  return (stsafea_aes_gcm_decrypt(pSTSE,
                                  slot_number,
                                  authentication_tag_length,
                                  IV_length,
                                  pIV,
                                  associated_data_length,
                                  pAssociated_data,
                                  0,
                                  NULL,
                                  pAuthentication_tag,
                                  pVerification_result,
                                  NULL));
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
