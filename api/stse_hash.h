/*!
 ******************************************************************************
 * \file	stse_hash.h
 * \brief   STSE Hash API set (header)
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

#ifndef STSE_HASH_H
#define STSE_HASH_H

/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_hash.h"

/** \defgroup 	stse_hash 	STSE Hash
 *  \ingroup 	stse_api
 *  \brief		STSE Hash (SHA2/SHA3) API set
 *  \details  	The Hash API set provides high level functions for Hash features 	\n
 *  			Supported algorithms listed below:									\n
 *  				- SHA256														\n
 *  				- SHA384														\n
 *  				- SHA512														\n
 *  				- SHA3-256														\n
 *  				- SHA3-384														\n
 *  				- SHA3-512
 *  @{
 */

/**
 * \brief 			STSE start hash API
 * \details 		This API use the STSE to start a hash processing
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		sha_algorithm	\ref stse_hash_algorithm_t SHA algorithm
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_start_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t 	sha_algorithm,
		PLAT_UI8				*pMessage,
		PLAT_UI16 				message_size);

/**
 * \brief 			STSE process hash API
 * \details 		This API use the STSE to continue a hash processing
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_process_hash(
		stse_Handler_t * pSTSE,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size);

/**
 * \brief 			STSE start hash API
 * \details 		This API use the STSE to finish a hash processing
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \param[out] 		pDigest			Pointer to digest buffer
 * \param[out]		pDigest_size	Digest buffer length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_finish_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t sha_algorithm,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size,
		PLAT_UI8* pDigest,
		PLAT_UI16* pDigest_size);

/**
 * \brief 			STSE compute hash API
 * \details 		This API use the STSE to compute a hash in one command
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		sha_algorithm	\ref stse_hash_algorithm_t SHA algorithm
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \param[out] 		pDigest			Pointer to digest buffer
 * \param[out]		pDigest_size	Digest buffer length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_compute_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t sha_algorithm,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size,
		PLAT_UI8* pDigest,
		PLAT_UI16* pDigest_size);

/** @}*/

#endif /*STSE_HASH_H*/

