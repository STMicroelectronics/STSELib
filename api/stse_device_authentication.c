/*!
 ******************************************************************************
 * \file	stse_device_authentication.c
 * \brief   STSAFE-Axxx Middleware Authentication API (sources)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <api/stse_ecc.h>
#include <api/stse_device_authentication.h>

#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_subparsing.h"
#include "certificate/stse_certificate_crypto.h"

#define STSAFE_CERTIFICATE_ZONE_0 		0U
#define CERTIFICATE_SIZE_OFFSET_BYTES 	2U
#define CERTIFICATE_SIZE_LENGTH 		2U
#define CERTIFICATE_OFFSET_BYTES		0U
#define CERTIFICATE_DEVICE_ID_OFFSET	15U
#define CERTIFICATE_DEVICE_ID_SIZE		11U

/* Static functions declaration ----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

stse_ReturnCode_t stse_get_device_id(stse_Handler_t * pSTSE, PLAT_UI8* device_id)
{
	volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	ret = stse_data_storage_read_data_zone(
			pSTSE,							/* STSAFE handler */
			STSAFE_CERTIFICATE_ZONE_0,		/* Zone = 0 */
			CERTIFICATE_DEVICE_ID_OFFSET,	/* X bytes offset */
			device_id,						/* Returned certificate size */
			CERTIFICATE_DEVICE_ID_SIZE,		/* Certificate size length */
			0,								/* No maximum chunck size (No chunck at all) */
			STSAFEA_NO_PROT);				/* No protection */

	return ret;
}

stse_ReturnCode_t stse_get_device_certificate_size(stse_Handler_t * pSTSE, PLAT_UI16 * certificate_size)
{
	volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;
	PLAT_UI8 certificate_size_ui8[2];

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}


	ret = stse_data_storage_read_data_zone(
			pSTSE,							/* STSAFE handler */
			STSAFE_CERTIFICATE_ZONE_0,		/* Zone = 0 */
			CERTIFICATE_SIZE_OFFSET_BYTES,	/* 2 bytes offset */
			certificate_size_ui8,			/* Returned certificate size */
			CERTIFICATE_SIZE_LENGTH,		/* Certificate size length 2 bytes */
			0,								/* No maximum chunck size (No chunck at all) */
			STSAFEA_NO_PROT);				/* No protection */

	if(ret != STSE_OK)
	{
		return( ret );
	}

	*certificate_size = (((uint16_t)certificate_size_ui8[0]) << 8) + ((uint16_t)certificate_size_ui8[1]) + 4U;


	return STSE_OK;
}

stse_ReturnCode_t stse_get_device_certificate(stse_Handler_t * pSTSE, PLAT_UI16 certificate_size, PLAT_UI8* stsafe_certificate)
{
	volatile stse_ReturnCode_t ret = STSE_API_INVALID_PARAMETER;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	ret = stse_data_storage_read_data_zone(
			pSTSE,							/* STSAFE handler */
			STSAFE_CERTIFICATE_ZONE_0,		/* Zone = 0 */
			CERTIFICATE_OFFSET_BYTES,		/* 0 bytes offset */
			stsafe_certificate,				/* Returned certificate size */
			certificate_size,				/* Certificate size length */
			255,								/* No maximum chunck size (No chunck at all) */
			STSAFEA_NO_PROT);				/* No protection */

	if(ret != STSE_OK)
	{
		return( ret );
	}

	return STSE_OK;
}

stse_ReturnCode_t stse_device_authenticate(
		stse_Handler_t * pSTSE,
		const PLAT_UI8 *pRoot_CA_certificate,
		const PLAT_UI8 *pCertificate_chain,
		PLAT_UI16 certificate_chain_size,
		PLAT_UI8 priv_key_slot_number)
{
	stse_ReturnCode_t ret;
	stse_certificate_t leaf_certificate;


	ret = stse_certificate_parse_chain(
			(uint8_t*)pRoot_CA_certificate,
			(uint8_t*)pCertificate_chain,
			certificate_chain_size,
			&leaf_certificate);

	if (ret != STSE_OK)
	{
		return STSE_UNEXPECTED_ERROR;
	}

	stse_ecc_key_type_t key_type = stse_certificate_get_key_type(&leaf_certificate);

	PLAT_UI16 signature_size = stsafea_ecc_info_table[key_type].signature_size;
	PLAT_UI8 signature[signature_size];

	PLAT_UI16 challenge_size = stsafea_ecc_info_table[key_type].private_key_size;
	PLAT_UI8 challenge[challenge_size];

	/* Generate a challenge with Random Number */
	for (int i=0; i<challenge_size; i++)
	{
		challenge[i] = (PLAT_UI8)(stse_platform_Random()&0xFF);
	}

	/* Generate Signature for the Random Number */
	ret = stse_ecc_generate_signature(
		pSTSE, 					/* STSAFE handler */
		priv_key_slot_number, 	/* Slot number */
		key_type,
		challenge, 		 		/* Random number to sign */
		challenge_size, 		/* random number size in bytes */
		signature); 			/* returned signature */

	if (ret != STSE_OK)
	{
		return ret;
	}

	/* Verify The Signature of the Random Number*/
	ret = stse_certificate_verify_signature(
			&leaf_certificate,
			challenge,
			challenge_size,
			signature, (signature_size >> 1),
			&signature[signature_size >> 1], (signature_size >> 1));

	if(ret != STSE_OK)
	{
		return STSE_API_INVALID_SIGNATURE;
	}
	return STSE_OK;
}
