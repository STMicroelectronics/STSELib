/*!
 ******************************************************************************
 * \file	stsafea_patch.c
 * \brief   Patch services for STSAFE-A
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

#include <services/stsafea/stsafea_patch.h>

stse_ReturnCode_t stsafe_get_patch_update_command_counter(
		stse_Handler_t * pSTSAFE,
		PLAT_UI8 pPatch_update_command_counter)
{
	(void)pSTSAFE;
	(void)pPatch_update_command_counter;
	return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_ReturnCode_t stsafe_patch_start(
		stse_Handler_t * pSTSAFE)
{
	(void)pSTSAFE;
	return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_ReturnCode_t stsafe_patch_update(
		stse_Handler_t * pSTSAFE)
{
	(void)pSTSAFE;
	return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}

stse_ReturnCode_t stsafe_patch_finalize(
		stse_Handler_t * pSTSAFE)
{
	(void)pSTSAFE;
	return STSE_SERVICE_INVALID_PARAMETER; /* TODO */
}
