/*!
 ******************************************************************************
 * \file	stsafea_hash.c
 * \brief   STSAFE-A services for hash (source)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
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
#if !defined(STSE_CONF_HASH_SHA_1) && !defined(STSE_CONF_HASH_SHA_224) &&                                       \
    !defined(STSE_CONF_HASH_SHA_256) && !defined(STSE_CONF_HASH_SHA_384) && !defined(STSE_CONF_HASH_SHA_512) && \
    !defined(STSE_CONF_HASH_SHA_3_256) && !defined(STSE_CONF_HASH_SHA_3_384) && !defined(STSE_CONF_HASH_SHA_3_512)
    {0};
#else
    {
#ifdef STSE_CONF_HASH_SHA_1
        {
            STSAFEA_SHA_1_HASH_SIZE,
            {STSAFEA_SHA1_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_1},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_224
        {
            STSAFEA_SHA_224_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_224},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_256
        {
            STSAFEA_SHA_256_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_256},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_384
        {
            STSAFEA_SHA_384_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_384},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_512
        {
            STSAFEA_SHA_512_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA_512},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_3_256
        {
            STSAFEA_SHA_256_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_256},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_3_384
        {
            STSAFEA_SHA_384_HASH_SIZE,
            {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_384},
        },
#endif
#ifdef STSE_CONF_HASH_SHA_3_512
        {STSAFEA_SHA_512_HASH_SIZE,
         {STSAFEA_HASH_ALGO_ID_LENGTH, STSAFEA_HASH_ALGO_ID_SHA3_512}}
#endif
};
#endif

#if defined(STSE_CONF_HASH_SHA_1) || defined(STSE_CONF_HASH_SHA_224) ||                                      \
    defined(STSE_CONF_HASH_SHA_256) || defined(STSE_CONF_HASH_SHA_384) || defined(STSE_CONF_HASH_SHA_512) || \
    defined(STSE_CONF_HASH_SHA_3_256) || defined(STSE_CONF_HASH_SHA_3_384) || defined(STSE_CONF_HASH_SHA_3_512)

stse_return_code_t stsafea_start_hash(
    stse_handle_t *p_stse,
    stse_hash_algorithm_t sha_algorithm,
    PLAT_UI8 *p_message,
    PLAT_UI16 message_size) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_START_HASH};
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((p_message == NULL) || (message_size == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_hash_algo, STSAFEA_HASH_ALGO_ID_SIZE, (PLAT_UI8 *)&stsafea_hash_info_table[sha_algorithm].id);
    stse_frame_element_allocate_push(&cmd_frame, e_message, message_size, p_message);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_process_hash(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_message,
    PLAT_UI16 message_size) {
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_PROCESS_HASH};
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if ((p_message == NULL) || (message_size == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_message, message_size, p_message);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafea_finish_hash(
    stse_handle_t *p_stse,
    stse_hash_algorithm_t sha_algorithm,
    PLAT_UI8 *p_message,
    PLAT_UI16 message_size,
    PLAT_UI8 *p_digest,
    PLAT_UI16 *p_digest_size) {
    stse_return_code_t ret;
    PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX, STSAFEA_EXTENDED_CMD_FINISH_HASH};
    PLAT_UI8 rsp_header;
    PLAT_UI8 digest_size_array[STSAFEA_GENERIC_LENGTH_SIZE];
    PLAT_UI16 expected_digest_size = stsafea_hash_info_table[sha_algorithm].length;

    /*- Verify Parameters */
    if (p_stse == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    if (p_digest == NULL || p_digest_size == NULL || (p_message != NULL && message_size == 0) || (p_message == NULL && message_size != 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
    stse_frame_element_allocate_push(&cmd_frame, e_message, message_size, p_message);

    /*- Create Rsp frame and populate elements */
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&rsp_frame, e_digest_size, STSAFEA_GENERIC_LENGTH_SIZE, digest_size_array);
    stse_frame_element_allocate_push(&rsp_frame, e_digest, expected_digest_size, p_digest);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(p_stse,
                                 &cmd_frame,
                                 &rsp_frame);

    if (ret == STSE_OK) {
        *p_digest_size = ARRAY_2B_SWAP_TO_UI16(digest_size_array);
        *p_digest_size = expected_digest_size;
    } else {
        *p_digest_size = 0;
    }

    return (ret);
}

#endif

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
