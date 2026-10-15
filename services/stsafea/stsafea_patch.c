/*!
 ******************************************************************************
 * \file	stsafea_patch.c
 * \brief   STSAFE-A services for patch (source)
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

#include "services/stsafea/stsafea_patch.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafe_get_patch_update_command_counter(
    stse_handle_t *p_stsafe,
    PLAT_UI8 p_patch_update_command_counter) {
    (void)p_stsafe;
    (void)p_patch_update_command_counter;
    return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_return_code_t stsafe_patch_start(
    stse_handle_t *p_stsafe) {
    (void)p_stsafe;
    return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_return_code_t stsafe_patch_update(
    stse_handle_t *p_stsafe) {
    (void)p_stsafe;
    return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_return_code_t stsafe_patch_finalize(
    stse_handle_t *p_stsafe) {
    (void)p_stsafe;
    return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
