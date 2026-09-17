/**
  ******************************************************************************
  * @file    stsafea_hash.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for hash (source)
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
#include "services/stsafea/stsafea_hash.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

const stsafea_hash_info_t stsafea_hash_info_table[] =
#if !defined(STSE_CONF_HASH_SHA_1) && !defined(STSE_CONF_HASH_SHA_224) \
  && !defined(STSE_CONF_HASH_SHA_256) && !defined(STSE_CONF_HASH_SHA_384) \
  && !defined(STSE_CONF_HASH_SHA_512) && !defined(STSE_CONF_HASH_SHA_3_256) \
  && !defined(STSE_CONF_HASH_SHA_3_384) && !defined(STSE_CONF_HASH_SHA_3_512)
{0};
#else
  {
#ifdef STSE_CONF_HASH_SHA_1
{
  STSAFEA_SHA_1_HASH_SIZE,
  {STSAFEA_SHA1_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_1},
},
#endif /* defined(STSE_CONF_HASH_SHA_1) */
#ifdef STSE_CONF_HASH_SHA_224
{
  STSAFEA_SHA_224_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_224},
},
#endif /* defined(STSE_CONF_HASH_SHA_224) */
#ifdef STSE_CONF_HASH_SHA_256
{
  STSAFEA_SHA_256_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_256},
},
#endif /* defined(STSE_CONF_HASH_SHA_256) */
#ifdef STSE_CONF_HASH_SHA_384
{
  STSAFEA_SHA_384_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_384},
},
#endif /* defined(STSE_CONF_HASH_SHA_384) */
#ifdef STSE_CONF_HASH_SHA_512
{
  STSAFEA_SHA_512_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_512},
},
#endif /* defined(STSE_CONF_HASH_SHA_512) */
#ifdef STSE_CONF_HASH_SHA_3_256
{
  STSAFEA_SHA_256_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_256},
},
#endif /* defined(STSE_CONF_HASH_SHA_3_256) */
#ifdef STSE_CONF_HASH_SHA_3_384
{
  STSAFEA_SHA_384_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_384},
},
#endif /* defined(STSE_CONF_HASH_SHA_3_384) */
#ifdef STSE_CONF_HASH_SHA_3_512
{
  STSAFEA_SHA_512_HASH_SIZE,
  {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_512}
}
#endif /* defined(STSE_CONF_HASH_SHA_3_512) */
  };
#endif /* !defined(STSE_CONF_HASH_SHA_1) && !defined(STSE_CONF_HASH_SHA_224)
          && !defined(STSE_CONF_HASH_SHA_256) */

#if defined(STSE_CONF_HASH_SHA_1) || defined(STSE_CONF_HASH_SHA_224) \
  || defined(STSE_CONF_HASH_SHA_256) || defined(STSE_CONF_HASH_SHA_384) \
  || defined(STSE_CONF_HASH_SHA_512) || defined(STSE_CONF_HASH_SHA_3_256) \
  || defined(STSE_CONF_HASH_SHA_3_384) || defined(STSE_CONF_HASH_SHA_3_512)

stse_ReturnCode_t stsafea_start_hash(
  stse_Handler_t *pSTSE,
  stse_hash_algorithm_t sha_algorithm,
  PLAT_UI8 *pMessage,
  PLAT_UI16 message_size)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_HASH};
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pMessage == NULL) || (message_size == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eHashAlgo, STSAFEA_HASH_ALGO_ID_SIZE,
                                   (PLAT_UI8 *)&stsafea_hash_info_table[sha_algorithm].id);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_size, pMessage);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_process_hash(
  stse_Handler_t *pSTSE,
  PLAT_UI8 *pMessage,
  PLAT_UI16 message_size)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_PROCESS_HASH};
  PLAT_UI8 rsp_header;

  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if ((pMessage == NULL) || (message_size == 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_size, pMessage);

  /*- Create Rsp frame and populate elements*/
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, 1, &rsp_header);

  /*- Perform Transfer*/
  return stsafea_frame_transfer(pSTSE,
                                &CmdFrame,
                                &RspFrame);
}

stse_ReturnCode_t stsafea_finish_hash(
  stse_Handler_t *pSTSE,
  stse_hash_algorithm_t sha_algorithm,
  PLAT_UI8 *pMessage,
  PLAT_UI16 message_size,
  PLAT_UI8 *pDigest,
  PLAT_UI16 *pDigest_size)
{
  stse_ReturnCode_t ret;
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_FINISH_HASH};
  PLAT_UI8 rsp_header;
  PLAT_UI8 digest_size_array[STSAFEA_GENERIC_LENGTH_SIZE];
  PLAT_UI16 expected_digest_size = stsafea_hash_info_table[sha_algorithm].length;

  /*- Verify Parameters */
  if (pSTSE == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  if (pDigest == NULL || pDigest_size == NULL || (pMessage != NULL && message_size == 0) || (pMessage == NULL
      && message_size != 0))
  {
    return STSE_SERVICE_INVALID_PARAMETER;
  }

  /*- Create CMD frame and populate elements */
  STSE_FRAME_ALLOCATE(CmdFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eCmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&CmdFrame, eMessage, message_size, pMessage);

  /*- Create Rsp frame and populate elements */
  STSE_FRAME_ALLOCATE(RspFrame);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eDigestSize, STSAFEA_GENERIC_LENGTH_SIZE, digest_size_array);
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&RspFrame, eDigest, expected_digest_size, pDigest);

  /*- Perform Transfer*/
  ret = stsafea_frame_transfer(pSTSE,
                               &CmdFrame,
                               &RspFrame);

  if (ret == STSE_OK)
  {
    *pDigest_size = ARRAY_2B_SWAP_TO_UI16(digest_size_array);
    *pDigest_size = expected_digest_size;
  }
  else
  {
    *pDigest_size = 0;
  }

  return (ret);
}

#endif /* defined(STSE_CONF_HASH_SHA_1) || defined(STSE_CONF_HASH_SHA_224) ||                                      \ */

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
