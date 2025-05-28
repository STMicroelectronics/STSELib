/*!
 ******************************************************************************
 * \file	stse_mac.c
 * \brief   STSE MAC API set (sources)
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
 *****************************************************************************/

#include "api/stse_mac.h"

stse_ReturnCode_t stse_cmac_hmac_compute(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 * pMessage,
		PLAT_UI8 message_length,
		PLAT_UI8 * pMac,
		PLAT_UI8 mac_length)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_cmac_hmac_compute(pSTSE, slot_number, pMessage, message_length, pMac, mac_length);
}

stse_ReturnCode_t stse_cmac_hmac_verify(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 * pMac,
		PLAT_UI8 mac_length,
		PLAT_UI8 * pMessage,
		PLAT_UI8 message_length,
		PLAT_UI8 * verification_result)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_cmac_hmac_verify(pSTSE, slot_number, pMac, mac_length, pMessage, message_length, verification_result);
}

stse_ReturnCode_t stse_aes_gmac_compute(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pAuthentication_tag)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_gmac_compute(pSTSE, slot_number, IV_length, pIV, associated_data_length, pAssociated_data, authentication_tag_length, pAuthentication_tag);
}

stse_ReturnCode_t stse_aes_gmac_verify(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_gmac_verify(pSTSE, slot_number, IV_length, pIV, associated_data_length, pAssociated_data, authentication_tag_length, pAuthentication_tag, pVerification_result);
}
