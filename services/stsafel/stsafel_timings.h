 /******************************************************************************
 * \file	stsafel_timings.h
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

#ifndef STSAFEL_TIMINGS_H
#define STSAFEL_TIMINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"

#define STSAFEL_PRODUCT_COUNT 1U
#define STSAFEL_MAX_CMD_COUNT 13U

/*! \defgroup stsafel_symmetric_key_slots timings
 *  \ingroup stsafel_services
 *  @{
 */

extern const PLAT_UI16 stsafel_cmd_timings[STSAFEL_PRODUCT_COUNT][STSAFEL_MAX_CMD_COUNT];
extern const PLAT_UI16 stsafel_boot_time[STSAFEL_PRODUCT_COUNT];
extern const PLAT_UI16 stsafel_wakeup_time[STSAFEL_PRODUCT_COUNT];

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFEL_TIMINGS_H */
