 /******************************************************************************
 * \file	stsafel_timings.c
 * \brief   STSAFE-L Timings definitions
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "services/stsafel/stsafel_commands.h"

const PLAT_UI16 stsafel_maximum_command_length[STSAFEL_PRODUCT_COUNT] = {
    750, /*!< STSAFE-L Maximum command length (bytes) */
};
