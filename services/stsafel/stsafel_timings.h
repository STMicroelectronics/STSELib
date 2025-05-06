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
#include "services/stsafel/stsafel_commands.h"


#define STSAFEL_EXEC_TIME_DEFAULT       500U /*!< STSAFE-L default command processing time (used when specific time == 0) */

/*! \defgroup stsafel_timings STSAFE-L Timings
 *  \ingroup stsafel_services
 *  @{
 */

extern const PLAT_UI16 stsafel_cmd_timings[STSAFEL_PRODUCT_COUNT][STSAFEL_MAX_CMD_COUNT];
extern const PLAT_UI16 stsafel_boot_time[STSAFEL_PRODUCT_COUNT];
extern const PLAT_UI16 stsafel_wakeup_time[STSAFEL_PRODUCT_COUNT];

/**
 * \brief 		Get the command execution timing
 * \details 	Get the command execution timing
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	command_code 		Command code
 * \return PLAT_UI16 : specific command execution time
 */
PLAT_UI16 stsafel_cmd_exec_duration(
    stse_Handler_t * pSTSE,
    stsafel_cmd_code_t command_code);

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSAFEL_TIMINGS_H */
