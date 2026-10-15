/*!
 ******************************************************************************
 * \file	stse_hash.c
 * \brief   STSE Hash API set (sources)
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
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "api/stse_hash.h"
#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_hash.h"

#if defined(STSE_CONF_HASH_SHA_1) || defined(STSE_CONF_HASH_SHA_224) ||                                      \
    defined(STSE_CONF_HASH_SHA_256) || defined(STSE_CONF_HASH_SHA_384) || defined(STSE_CONF_HASH_SHA_512) || \
    defined(STSE_CONF_HASH_SHA_3_256) || defined(STSE_CONF_HASH_SHA_3_384) || defined(STSE_CONF_HASH_SHA_3_512)

stse_return_code_t stse_compute_hash(
    stse_handle_t *p_stse,
    stse_hash_algorithm_t sha_algorithm,
    PLAT_UI8 *p_message,
    PLAT_UI16 message_size,
    PLAT_UI8 *p_digest,
    PLAT_UI16 *p_digest_size) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    stse_return_code_t ret;
    PLAT_UI16 remaining_length = message_size;

    /* - Check stsafe handler initialization */
    if (p_stse == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    if (p_message == NULL || p_digest == NULL || p_digest_size == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    PLAT_UI16 maximum_chunk_size = stsafea_maximum_frame_length[p_stse->device_type] - STSE_FRAME_CRC_SIZE - STSAFEA_CMD_EXTENSION_SIZE;

    message_size = ((remaining_length + STSAFEA_HASH_ALGO_ID_SIZE) > maximum_chunk_size) ? maximum_chunk_size - STSAFEA_HASH_ALGO_ID_SIZE : remaining_length;
    ret = stsafea_start_hash(p_stse, sha_algorithm, p_message, message_size);
    if (ret != STSE_OK) {
        return (ret);
    }
    remaining_length -= message_size;
    p_message += message_size;

    while (remaining_length > 0) {
        message_size = (remaining_length > maximum_chunk_size) ? maximum_chunk_size : remaining_length;
        ret = stsafea_process_hash(p_stse, p_message, message_size);
        if (ret != STSE_OK) {
            return (ret);
        }
        remaining_length -= message_size;
        p_message += message_size;
    }

    ret = stsafea_finish_hash(p_stse, sha_algorithm, NULL, 0, p_digest, p_digest_size);

    return ret;
#else
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

#endif /* STSE_CONF_HASH_SHA_1 || STSE_CONF_HASH_SHA_224 ||
          STSE_CONF_HASH_SHA_256 || STSE_CONF_HASH_SHA_384 || STSE_CONF_HASH_SHA_512 ||
          STSE_CONF_HASH_SHA_3_256 || STSE_CONF_HASH_SHA_3_384 || STSE_CONF_HASH_SHA_3_512 */
