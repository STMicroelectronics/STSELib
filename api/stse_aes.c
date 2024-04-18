/*!
 ******************************************************************************
 * \file	stse_aes.c
 * \brief   STSE AES API set (sources)
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

#include "api/stse_aes.h"

stse_ReturnCode_t stse_aes_ecb_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_ecb_encrypt(
				pSTSE,
				slot_number,
				message_length,
				pPlaintext_message,
				pEncrypted_message);
}

stse_ReturnCode_t stse_aes_ecb_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pPlaintext_message)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_ecb_decrypt(
			pSTSE,
			slot_number,
			message_length,
			pEncrypted_message,
			pPlaintext_message);
}

stse_ReturnCode_t stse_aes_ccm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 *pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 *pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 *pPlaintext_message,
		PLAT_UI8 *pEncrypted_message,
		PLAT_UI8 *pAuthentication_tag,
		PLAT_UI8 *pCounter_presence,
		PLAT_UI32 *pCounter)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_ccm_encrypt(
			pSTSE,
			slot_number,
			authentication_tag_length,
			pNonce,
			associated_data_length,
			pAssociated_data,
			message_length,
			pPlaintext_message,
			pEncrypted_message,
			pAuthentication_tag,
			pCounter_presence,
			pCounter);
}

stse_ReturnCode_t stse_aes_ccm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI8 * pNonce,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_ccm_decrypt(
			pSTSE,
			slot_number,
			authentication_tag_length,
			pNonce,
			associated_data_length,
			pAssociated_data,
			message_length,
			pEncrypted_message,
			pAuthentication_tag,
			pVerification_result,
			pPlaintext_message);
}

stse_ReturnCode_t stse_aes_gcm_encrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pPlaintext_message,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_gcm_encrypt(
			pSTSE,
			slot_number,
			authentication_tag_length,
			IV_length,
			pIV,
			associated_data_length,
			pAssociated_data,
			message_length,
			pPlaintext_message,
			pEncrypted_message,
			pAuthentication_tag);
}

stse_ReturnCode_t stse_aes_gcm_decrypt(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		PLAT_UI8 authentication_tag_length,
		PLAT_UI16 IV_length,
		PLAT_UI8* pIV,
		PLAT_UI16 associated_data_length,
		PLAT_UI8 * pAssociated_data,
		PLAT_UI16 message_length,
		PLAT_UI8 * pEncrypted_message,
		PLAT_UI8 * pAuthentication_tag,
		PLAT_UI8 * pVerification_result,
		PLAT_UI8 * pPlaintext_message)
{
	if (pSTSE == NULL)
	{
		return( STSE_API_HANDLER_NOT_INITIALISED );
	}

	return stsafea_aes_gcm_decrypt(
			pSTSE,
			slot_number,
			authentication_tag_length,
			IV_length,
			pIV,
			associated_data_length,
			pAssociated_data,
			message_length,
			pEncrypted_message,
			pAuthentication_tag,
			pVerification_result,
			pPlaintext_message);
}
