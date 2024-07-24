/*!
 ******************************************************************************
 * \file	stse_generic_typedef.c
 * \brief   STSE Generic typedefs (source)
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

#include "core/stse_generic_typedef.h"

const stse_ecc_info_t stse_ecc_info_table[STSE_ECC_NUMBER_OF_CURVES] = {
	{	/* NIST P_256 */
		.curve_id_total_length = (STSE_NIST_P_256_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_NIST_P_256_ID_VALUE_SIZE), UI16_B0(STSE_NIST_P_256_ID_VALUE_SIZE)},
								STSE_NIST_P_256_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_256_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_NIST_P_256_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_NIST_P_256_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_NIST_P_256_SIGNATURE_R_VALUE_SIZE + STSE_NIST_P_256_SIGNATURE_S_VALUE_SIZE)
	},
	{ 	/* NIST P_384 */
		.curve_id_total_length = (STSE_NIST_P_384_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_NIST_P_384_ID_VALUE_SIZE), UI16_B0(STSE_NIST_P_384_ID_VALUE_SIZE)},
								STSE_NIST_P_384_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_384_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_NIST_P_384_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_NIST_P_384_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_NIST_P_384_SIGNATURE_R_VALUE_SIZE + STSE_NIST_P_384_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* NIST P_521 */
		.curve_id_total_length = (STSE_NIST_P_521_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_NIST_P_521_ID_VALUE_SIZE), UI16_B0(STSE_NIST_P_521_ID_VALUE_SIZE)},
								STSE_NIST_P_521_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE + STSE_NIST_P_521_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_NIST_P_521_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_NIST_P_521_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_NIST_P_521_SIGNATURE_R_VALUE_SIZE + STSE_NIST_P_521_SIGNATURE_S_VALUE_SIZE)
	},
	{ 	/* BRAINPOOL P_256 */
		.curve_id_total_length = (STSE_BRAINPOOL_P_256_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_BRAINPOOL_P_256_ID_VALUE_SIZE), UI16_B0(STSE_BRAINPOOL_P_256_ID_VALUE_SIZE)},
								STSE_BRAINPOOL_P_256_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_256_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_BRAINPOOL_P_256_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_BRAINPOOL_P_256_SIGNATURE_R_VALUE_SIZE + STSE_BRAINPOOL_P_256_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* BRAINPOOL P_384 */
		.curve_id_total_length = (STSE_BRAINPOOL_P_384_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_BRAINPOOL_P_384_ID_VALUE_SIZE), UI16_B0(STSE_BRAINPOOL_P_384_ID_VALUE_SIZE)},
								STSE_BRAINPOOL_P_384_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_384_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_BRAINPOOL_P_384_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_BRAINPOOL_P_384_SIGNATURE_R_VALUE_SIZE + STSE_BRAINPOOL_P_384_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* BRAINPOOL P_512 */
		.curve_id_total_length = (STSE_BRAINPOOL_P_512_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_BRAINPOOL_P_512_ID_VALUE_SIZE), UI16_B0(STSE_BRAINPOOL_P_512_ID_VALUE_SIZE)},
								STSE_BRAINPOOL_P_512_ID_VALUE},
		.coordinate_or_key_size  	= (STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE + STSE_BRAINPOOL_P_512_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSE_BRAINPOOL_P_512_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSE_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSE_BRAINPOOL_P_512_SIGNATURE_R_VALUE_SIZE + STSE_BRAINPOOL_P_512_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* X25519 */
		.curve_id_total_length = (STSE_X25519_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_X25519_ID_VALUE_SIZE), UI16_B0(STSE_X25519_ID_VALUE_SIZE)},
								STSE_X25519_ID_VALUE},
		.coordinate_or_key_size  	= STSE_X25519_PUBLIC_KEY_VALUE_SIZE,
		.public_key_size  	= STSE_X25519_PUBLIC_KEY_VALUE_SIZE,
		.private_key_size 	= STSE_X25519_PRIVATE_KEY_SIZE,
		.shared_secret_size = STSE_X25519_SHARED_SECRET_VALUE_SIZE,
		.signature_size		= 0
	},
	{	/* ED25519 */
		.curve_id_total_length = (STSE_ED25519_ID_VALUE_SIZE + STSE_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSE_ED25519_ID_VALUE_SIZE), UI16_B0(STSE_ED25519_ID_VALUE_SIZE)},
								STSE_ED25519_ID_VALUE},
		.coordinate_or_key_size  	= STSE_ED25519_PUBLIC_KEY_VALUE_SIZE,
		.public_key_size  	= STSE_ED25519_PUBLIC_KEY_VALUE_SIZE,
		.private_key_size 	= STSE_ED25519_PRIVATE_KEY_SIZE,
		.shared_secret_size = 0,
		.signature_size		= (STSE_ED25519_SIGNATURE_R_VALUE_SIZE + STSE_ED25519_SIGNATURE_S_VALUE_SIZE)
	}
};