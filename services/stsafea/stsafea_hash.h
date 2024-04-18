/*!
 ******************************************************************************
 * \file	stsafea_hash.h
 * \brief   Hash services for STSAFE-A
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

#ifndef STSAFEA_HASH_H
#define STSAFEA_HASH_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"


/*! \defgroup stsafea_hash Hash
 *  \ingroup stsafea_services
 *  @{
 */

/*  ---------------------------------------------------------------------------------------
 *
 *						       SHA cryptographic algorithms definitions
 *
 *  ---------------------------------------------------------------------------------------
 */

#define STSAFEA_SHA_1_HASH_SIZE				20U		/*!< Size in bytes of a SHA1 digest */
#define STSAFEA_SHA_224_HASH_SIZE			28U		/*!< Size in bytes of a SHA224 digest */
#define STSAFEA_SHA_256_HASH_SIZE 			32U		/*!< Size in bytes of a SHA256 digest */
#define STSAFEA_SHA_384_HASH_SIZE 			48U		/*!< Size in bytes of a SHA384 digest */
#define STSAFEA_SHA_512_HASH_SIZE   		64U  	/*!< Size in bytes of a SHA512 digest */

#define STSAFEA_HASH_ALGO_ID_VALUE_SIZE 	9U		/*!< Max size in bytes of a hash algo ID */
#define STSAFEA_HASH_ALGO_ID_LENGTH_SIZE	2U		/*!<Size in bytes of a hash algo ID length field */
#define STSAFEA_HASH_ALGO_ID_SIZE 			(STSAFEA_HASH_ALGO_ID_LENGTH_SIZE + STSAFEA_HASH_ALGO_ID_VALUE_SIZE)

#define STSAFEA_SHA1_ALGO_ID_LENGTH			{0x00, 0x05}
#define STSAFEA_HASH_ALGO_ID_LENGTH			{0x00, 0x09}

#define STSAFEA_HASH_ALGO_ID_SHA_1			{0x2b, 0x0e, 0x03, 0x02, 0x1a,    0,    0,    0,    0}
#define STSAFEA_HASH_ALGO_ID_SHA_224		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04}
#define STSAFEA_HASH_ALGO_ID_SHA_256		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01}
#define STSAFEA_HASH_ALGO_ID_SHA_384		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02}
#define STSAFEA_HASH_ALGO_ID_SHA_512		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03}
#define STSAFEA_HASH_ALGO_ID_SHA3_256		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x08}
#define STSAFEA_HASH_ALGO_ID_SHA3_384		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x09}
#define STSAFEA_HASH_ALGO_ID_SHA3_512		{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0A}

/*!
 * \struct 	stsafea_hash_algorithm_identifier_t
 * \brief 	STSAFEA Hash identifier structure
 */
typedef struct stsafea_hash_algorithm_identifier_t {
	PLAT_UI8 length[STSAFEA_HASH_ALGO_ID_LENGTH_SIZE];
	PLAT_UI8 value[STSAFEA_HASH_ALGO_ID_VALUE_SIZE];
}PLAT_PACKED_STRUCT stsafea_hash_algorithm_identifier_t;

/*!
 * \struct 	stsafea_hash_info_t
 * \brief 	STSAFEA Hash informations structure
 */
typedef struct stsafea_hash_info_t {
	PLAT_UI16 length;
	stsafea_hash_algorithm_identifier_t id;
}PLAT_PACKED_STRUCT stsafea_hash_info_t;

extern const stsafea_hash_info_t stsafea_hash_info_table[];

/*  ---------------------------------------------------------------------------------------
 *
 *						       Exported SHA cryptographic services
 *
 *  ---------------------------------------------------------------------------------------
 */

/**
 * \brief 			STSAFEA start hash service
 * \details 		This service format and send/receive STSAFE-Axxx start hash command/response
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		sha_algorithm	\ref stse_hash_algorithm_t SHA algorithm
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_start_hash( stse_Handler_t * pSTSE,
		stse_hash_algorithm_t 	sha_algorithm,
		PLAT_UI8				*pMessage,
		PLAT_UI16 				message_size);

/**
 * \brief 			STSAFEA process hash service
 * \details 		This service format and send/receive STSAFE-Axxx process hash command/response
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_process_hash(
		stse_Handler_t * pSTSE,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size
);

/**
 * \brief 			STSAFEA start hash service
 * \details 		This service format and send/receive STSAFE-Axxx process hash command/response
 * \param[in]		pSTSE			Pointer to target SE handler
 * \param[in]		sha_algorithm	\ref stse_hash_algorithm_t sha_algorithm
 * \param[in] 		pMessage		Pointer to message buffer
 * \param[in]		message_size	Input message length in bytes
 * \param[out] 		pDigest			Pointer to digest buffer
 * \param[out]		pDigest_size	Digest buffer length in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_finish_hash(
		stse_Handler_t * pSTSE,
		stse_hash_algorithm_t sha_algorithm,
		PLAT_UI8* pMessage,
		PLAT_UI16 message_size,
		PLAT_UI8* pDigest,
		PLAT_UI16* pDigest_size
);

/** \}*/

#endif /*STSAFEA_HASH_H*/
