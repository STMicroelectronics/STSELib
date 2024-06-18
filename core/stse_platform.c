 /******************************************************************************
 * \file	stselib_crypto_platform.h
 * \brief   STSecureElement cryptographic platform file
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


#include "core/stse_platform.h"
#include "services/stsafea/stsafea_hash.h"

/************************************************************
 *                STSAFE CRYPTO Global variables
 **************************************************************/


/************************************************************
 *                STSAFE CRYPTO HAL
 **************************************************************/

__WEAK stse_ReturnCode_t stse_platform_hmac_sha256_compute(PLAT_UI8 *pSalt, PLAT_UI16 salt_length,
											 PLAT_UI8 *pInput_keying_material, PLAT_UI16 input_keying_material_length,
											 PLAT_UI8 *pInfo, PLAT_UI16 info_length,
											 PLAT_UI8 *pOutput_keying_material, PLAT_UI16 output_keying_material_length)
{
	stse_ReturnCode_t retval;
	PLAT_UI8 pPseudorandom_key[STSAFEA_SHA_256_HASH_SIZE];


	/* Extract pseudo-random key from input keying material */
	retval = stse_platform_hmac_sha256_extract(pSalt,
												 salt_length,
												 pInput_keying_material,
												 input_keying_material_length,
												 pPseudorandom_key,
												 STSAFEA_SHA_256_HASH_SIZE);

	if(retval != 0)
	{
		memset(pPseudorandom_key, 0, STSAFEA_SHA_256_HASH_SIZE);
		return retval;
	}

	/* Expand output key from pseudo-random key */
	retval = stse_platform_hmac_sha256_expand(pPseudorandom_key,
													STSAFEA_SHA_256_HASH_SIZE,
													pInfo,
													info_length,
													pOutput_keying_material,
													output_keying_material_length);

	/* Pseudo-random key no more needed, cleanup */
	memset(pPseudorandom_key, 0, STSAFEA_SHA_256_HASH_SIZE);

	return retval;
}
