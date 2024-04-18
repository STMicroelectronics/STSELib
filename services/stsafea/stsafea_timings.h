 /******************************************************************************
 * \file	stsafea_timings.h
 * \brief   STSAFE-A timings definitions
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

#ifndef STSAFE_TIMINGS_H
#define STSAFE_TIMINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"

/*! \defgroup stsafea_symmetric_key_slots timings
 *  \ingroup stsafea_services
 *  @{
 */

extern const PLAT_UI16 stsafea_cmd_timings[4][30];
extern const PLAT_UI16 stsafea_extended_cmd_timings[4][27];
extern const PLAT_UI16 stsafea_boot_time[4];
extern const PLAT_UI16 stsafea_wakeup_time[4];

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFE_TIMINGS_H */
