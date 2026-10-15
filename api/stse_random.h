/*!
 ******************************************************************************
 * \file	stse_random.h
 * \brief   STSE Random API set (header)
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
 ******************************************************************************/

#ifndef STSE_RANDOM_H
#define STSE_RANDOM_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_random.h"

/** \defgroup 	stse_random 	STSE Random API set
 *  \ingroup 	stse_api
 *  \brief		STSE Random API set
 *  \details  	STSE Random number generation API set
 *  @{
 */

/**
 * \brief 			STSE generate random API
 * \details 		This API use the STSE to generate random number
 * \param[in]		p_stse 			Pointer to target STSecureElement device
 * \param[in,out] 	p_random 		Pointer to random buffer
 * \param[in]       random_size 	Random size
 * \return 			\ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_generate_random(
    stse_handle_t *p_stse,
    PLAT_UI8 *p_random,
    PLAT_UI16 random_size);

/** @}*/

#endif /*STSE_RANDOM_H*/
