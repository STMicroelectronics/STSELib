/******************************************************************************
 * \file	stse_session.c
 * \brief   STSELib api for session manager (source)
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
#include <string.h>

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"

/* Public functions ----------------------------------------------------------*/
void stse_session_erase_context(stse_session_t *pSession) {
    /* - Check stsafe handler initialization */
    if (pSession == NULL) {
        return;
    }

    /*Todo validate correct erase */
    memset(pSession, 0x00, sizeof(stse_session_t));
}

stse_ReturnCode_t stse_set_active_session(stse_Handler_t *pSTSE, stse_session_t *pSession) {
    if (pSTSE == NULL) {
        return STSE_CORE_HANDLER_NOT_INITIALISED;
    }

    if (pSession == NULL) {
        return STSE_CORE_SESSION_ERROR;
    }

    pSTSE->pActive_host_session = pSession;

    return (STSE_OK);
}
