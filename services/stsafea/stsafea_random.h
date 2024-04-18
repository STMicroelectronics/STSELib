/*!
 ******************************************************************************
 * \file	stsafea_random.h
 * \brief   Random services for STSAFE
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

#ifndef STSAFEA_RANDOM_H
#define STSAFEA_RANDOM_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_random Random
 *  \ingroup stsafea_services
 *  @{
 */

#define STSAFEA_MAXIMUM_RNG_SIZE 0xFF /*!< STSAFE-A1xx maximum random number generation size */

/**
 * \brief 			STSAFEA generate random service
 * \details 		This service format and send/receive the generate random command/response
 * \param[in]		pSTSE 			Pointer to target STSecureElement device
 * \param[in,out] 	pRandom 		Pointer to random buffer
 * \param[in,out] 	random_size 	Random size
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_generate_random(
		stse_Handler_t *pSTSE,
		PLAT_UI8 *pRandom,
		PLAT_UI8 random_size
);

/** \}*/

#endif /*STSAFEA_RANDOM_H*/
