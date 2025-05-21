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
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSAFE_TIMINGS_H
#define STSAFE_TIMINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "services/stsafea/stsafea_commands.h"

#define STSAFEA_EXEC_TIME_DEFAULT                                                                  \
	500U /*!< STSAFE default command processing time (used when specific time ==               \
		0) */

/*! \defgroup stsafea_timings STSAFEA timings
 *  \ingroup stsafea_services
 *  @{
 */

extern const PLAT_UI16 stsafea_cmd_timings[STSAFEA_PRODUCT_COUNT][STSAFEA_MAX_CMD_COUNT];
extern const PLAT_UI16 stsafea_extended_cmd_timings[STSAFEA_PRODUCT_COUNT]
						   [STSAFEA_MAX_EXT_CMD_COUNT];
extern const PLAT_UI16 stsafea_boot_time[STSAFEA_PRODUCT_COUNT];
extern const PLAT_UI16 stsafea_wakeup_time[STSAFEA_PRODUCT_COUNT];

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFE_TIMINGS_H */
