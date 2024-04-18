
#include "services/stsafea/stsafea_ecc.h"



const stsafea_ecc_info_t stsafea_ecc_info_table[STSAFEA_ECC_NUMBER_OF_CURVES] = {
	{	/* NIST P_256 */
		.curve_id_total_length = (STSAFEA_NIST_P_256_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_NIST_P_256_ID_VALUE_SIZE), UI16_B0(STSAFEA_NIST_P_256_ID_VALUE_SIZE)},
								STSAFEA_NIST_P_256_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_NIST_P_256_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_NIST_P_256_X_COORDINATE_VALUE_SIZE + STSAFEA_NIST_P_256_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_NIST_P_256_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_NIST_P_256_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_NIST_P_256_SIGNATURE_R_VALUE_SIZE + STSAFEA_NIST_P_256_SIGNATURE_S_VALUE_SIZE)
	},
	{ 	/* NIST P_384 */
		.curve_id_total_length = (STSAFEA_NIST_P_384_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_NIST_P_384_ID_VALUE_SIZE), UI16_B0(STSAFEA_NIST_P_384_ID_VALUE_SIZE)},
								STSAFEA_NIST_P_384_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_NIST_P_384_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_NIST_P_384_X_COORDINATE_VALUE_SIZE + STSAFEA_NIST_P_384_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_NIST_P_384_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_NIST_P_384_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_NIST_P_384_SIGNATURE_R_VALUE_SIZE + STSAFEA_NIST_P_384_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* NIST P_521 */
		.curve_id_total_length = (STSAFEA_NIST_P_521_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_NIST_P_521_ID_VALUE_SIZE), UI16_B0(STSAFEA_NIST_P_521_ID_VALUE_SIZE)},
								STSAFEA_NIST_P_521_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_NIST_P_521_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_NIST_P_521_X_COORDINATE_VALUE_SIZE + STSAFEA_NIST_P_521_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_NIST_P_521_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_NIST_P_521_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_NIST_P_521_SIGNATURE_R_VALUE_SIZE + STSAFEA_NIST_P_521_SIGNATURE_S_VALUE_SIZE)
	},
	{ 	/* BRAINPOOL P_256 */
		.curve_id_total_length = (STSAFEA_BRAINPOOL_P_256_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_BRAINPOOL_P_256_ID_VALUE_SIZE), UI16_B0(STSAFEA_BRAINPOOL_P_256_ID_VALUE_SIZE)},
								STSAFEA_BRAINPOOL_P_256_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE + STSAFEA_BRAINPOOL_P_256_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_BRAINPOOL_P_256_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_BRAINPOOL_P_256_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_BRAINPOOL_P_256_SIGNATURE_R_VALUE_SIZE + STSAFEA_BRAINPOOL_P_256_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* BRAINPOOL P_384 */
		.curve_id_total_length = (STSAFEA_BRAINPOOL_P_384_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_BRAINPOOL_P_384_ID_VALUE_SIZE), UI16_B0(STSAFEA_BRAINPOOL_P_384_ID_VALUE_SIZE)},
								STSAFEA_BRAINPOOL_P_384_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE + STSAFEA_BRAINPOOL_P_384_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_BRAINPOOL_P_384_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_BRAINPOOL_P_384_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_BRAINPOOL_P_384_SIGNATURE_R_VALUE_SIZE + STSAFEA_BRAINPOOL_P_384_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* BRAINPOOL P_512 */
		.curve_id_total_length = (STSAFEA_BRAINPOOL_P_512_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_BRAINPOOL_P_512_ID_VALUE_SIZE), UI16_B0(STSAFEA_BRAINPOOL_P_512_ID_VALUE_SIZE)},
								STSAFEA_BRAINPOOL_P_512_ID_VALUE},
		.coordinate_or_key_size  	= (STSAFEA_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE),
		.public_key_size  	= (STSAFEA_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE + STSAFEA_BRAINPOOL_P_512_Y_COORDINATE_VALUE_SIZE),
		.private_key_size 	= (STSAFEA_BRAINPOOL_P_512_PRIVATE_KEY_SIZE),
		.shared_secret_size = (STSAFEA_BRAINPOOL_P_512_X_COORDINATE_VALUE_SIZE),
		.signature_size		= (STSAFEA_BRAINPOOL_P_512_SIGNATURE_R_VALUE_SIZE + STSAFEA_BRAINPOOL_P_512_SIGNATURE_S_VALUE_SIZE)
	},
	{	/* X25519 */
		.curve_id_total_length = (STSAFEA_X25519_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_X25519_ID_VALUE_SIZE), UI16_B0(STSAFEA_X25519_ID_VALUE_SIZE)},
								STSAFEA_X25519_ID_VALUE},
		.coordinate_or_key_size  	= STSAFEA_X25519_PUBLIC_KEY_VALUE_SIZE,
		.public_key_size  	= STSAFEA_X25519_PUBLIC_KEY_VALUE_SIZE,
		.private_key_size 	= STSAFEA_X25519_PRIVATE_KEY_SIZE,
		.shared_secret_size = STSAFEA_X25519_SHARED_SECRET_VALUE_SIZE,
		.signature_size		= 0
	},
	{	/* ED25519 */
		.curve_id_total_length = (STSAFEA_ED25519_ID_VALUE_SIZE + STSAFEA_ECC_CURVE_ID_LENGTH_SIZE),
		.curve_id 		  	= {{UI16_B1(STSAFEA_ED25519_ID_VALUE_SIZE), UI16_B0(STSAFEA_ED25519_ID_VALUE_SIZE)},
								STSAFEA_ED25519_ID_VALUE},
		.coordinate_or_key_size  	= STSAFEA_ED25519_PUBLIC_KEY_VALUE_SIZE,
		.public_key_size  	= STSAFEA_ED25519_PUBLIC_KEY_VALUE_SIZE,
		.private_key_size 	= STSAFEA_ED25519_PRIVATE_KEY_SIZE,
		.shared_secret_size = 0,
		.signature_size		= (STSAFEA_ED25519_SIGNATURE_R_VALUE_SIZE + STSAFEA_ED25519_SIGNATURE_S_VALUE_SIZE)
	}
};


stse_ReturnCode_t stsafea_start_volatile_KEK_session(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8* host_ecdhe_public_key)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(host_ecdhe_public_key == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 ext_cmd_header = STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION;
	PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
	PLAT_UI8 rsp_header;

	PLAT_UI8 point_representation_id = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
			UI16_B0(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eExt_cmd_header,STSAFEA_HEADER_SIZE,&ext_cmd_header);

	stse_frame_element_allocate_push(&CmdFrame,eCurve_id,
			stsafea_ecc_info_table[key_type].curve_id_total_length,
			(PLAT_UI8*)&stsafea_ecc_info_table[key_type].curve_id);

	if(key_type == STSE_ECC_KT_CURVE25519)
	{
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	}
	else
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
		ePublic_key_second_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = host_ecdhe_public_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_element_allocate_push(&CmdFrame,eAlgorithm_id,1,&algorithm_id);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION]
	);
	return ret;
}

stse_ReturnCode_t stsafea_start_volatile_KEK_session_authenticated(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t ecdhe_key_type,
		PLAT_UI8* host_ecdhe_public_key,
		stse_hash_algorithm_t hash_algo,
		PLAT_UI8 signature_public_key_slot_number,
		stse_ecc_key_type_t signature_key_type,
		PLAT_UI8* pSignature)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if((host_ecdhe_public_key == NULL)
	|| (pSignature 			  == NULL)
	|| (ecdhe_key_type 	   == STSE_ECC_KT_ED25519)
	|| (signature_key_type == STSE_ECC_KT_CURVE25519))
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_extended_header = STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION;

	PLAT_UI8 point_representation_id = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
			UI16_B0(stsafea_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	PLAT_UI8 kdf_algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
	PLAT_UI8 filler_1_byte = 0;

	PLAT_UI8 pEmpty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
	stse_frame_element_allocate(eHash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, pEmpty_hash_algo_id);

	/* Divide Signature length By 2 to get R or S length */
	PLAT_UI16 signature_R_S_length = stsafea_ecc_info_table[signature_key_type].signature_size>>1;
	PLAT_UI8 pSignature_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(signature_R_S_length),
			UI16_B0(signature_R_S_length),
	};

	PLAT_UI8 rsp_header = 0;

	/* FRAME : . */
	stse_frame_allocate(CmdFrame);

	/* FRAME : [HEADER] [EXT HEADER] */
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,1,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_extended_header,1,&cmd_extended_header);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] */
	stse_frame_element_allocate_push(&CmdFrame,eCurve_id,
			stsafea_ecc_info_table[ecdhe_key_type].curve_id_total_length,
			(PLAT_UI8*)&stsafea_ecc_info_table[ecdhe_key_type].curve_id);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] */
	if(ecdhe_key_type == STSE_ECC_KT_CURVE25519)
	{
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	}
	else
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length = stsafea_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length = stsafea_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = host_ecdhe_public_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER] [SIGNATURE KEY SLOT] */
	stse_frame_element_allocate_push(&CmdFrame, eKdf_algorithm_id, 1, &kdf_algorithm_id);
	stse_frame_element_allocate_push(&CmdFrame, eFiller, 1, &filler_1_byte);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_public_key_slot_number, 1, &signature_public_key_slot_number);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER] [SIGNATURE KEY SLOT] [HASH ALGO] */
	if(signature_key_type != STSE_ECC_KT_ED25519)
	{
		eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
		eHash_algo_id.pData = (PLAT_UI8*)&stsafea_hash_info_table[hash_algo].id;
	}
	stse_frame_push_element(&CmdFrame, &eHash_algo_id);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER] [SIGNATURE KEY SLOT] [HASH ALGO] [SIGNATURE] */
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R, signature_R_S_length, pSignature);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S, signature_R_S_length, pSignature + signature_R_S_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,1,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION]
	);
	return ret;
}

stse_ReturnCode_t stsafea_stop_volatile_KEK_session(
		stse_Handler_t * pSTSE)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_extended_header = STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject_tag,1,&cmd_extended_header);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION]
	);

	return ret;
}



/*!
 ******************************************************************************
 * NEW FUNCTIONS
 *
 *****************************************************************************/
stse_ReturnCode_t stsafea_ecc_verify_signature(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key,
		PLAT_UI8 *pSignature,
		PLAT_UI8 *pMessage,
		PLAT_UI16 message_length,
		PLAT_UI8 message_is_hashed,
		PLAT_UI8 *pSignature_validity)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key 			== NULL
	|| pSignature 			== NULL
	|| pMessage 			== NULL
	|| pSignature_validity 	== NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_SIGNATURE;
	PLAT_UI8 subject = 0x00;

	/* Public key elements */
	PLAT_UI8 point_representation_id = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);
	PLAT_UI8 pPublic_key_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
			UI16_B0(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	/* Signature elements */
	PLAT_UI8 pSignature_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].signature_size>>1),
			UI16_B0(stsafea_ecc_info_table[key_type].signature_size>>1),
	};

	/* Hash elements*/
	stse_frame_element_allocate(eEdDSA_variant, 1, &message_is_hashed);

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eSubject,1,&subject);
	stse_frame_element_allocate_push(&CmdFrame,eCurve_id,
			stsafea_ecc_info_table[key_type].curve_id_total_length,
			(PLAT_UI8*)&stsafea_ecc_info_table[key_type].curve_id);

	if(key_type == STSE_ECC_KT_ED25519)
	{
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	}
	else
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_element_allocate_push(&CmdFrame, eSignature_R_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R, (stsafea_ecc_info_table[key_type].signature_size>>1), pSignature);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S, (stsafea_ecc_info_table[key_type].signature_size>>1), pSignature + (stsafea_ecc_info_table[key_type].signature_size>>1));

	if(key_type == STSE_ECC_KT_ED25519)
	{
		stse_frame_push_element(&CmdFrame, &eEdDSA_variant);
	}

	stse_frame_element_allocate_push(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,(PLAT_UI8*)&message_length);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length,pMessage);
	stse_frame_element_swap_byte_order(&eMessage_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eSignature_validity,1,pSignature_validity);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_VERIFY_SIGNATURE]
	);

	if(ret != STSE_OK)
	{
		*pSignature_validity = STSAFEA_FALSE;
	}

	return ret;
}

stse_ReturnCode_t stsafea_ecc_generate_signature(
		stse_Handler_t * pSTSE,
		PLAT_UI8 slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pMessage,
		PLAT_UI16 message_length,
		PLAT_UI8 *pSignature)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pMessage   == NULL
	|| pSignature == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_SIGNATURE;

	PLAT_UI8 rsp_header;
	/* Signature elements */
	PLAT_UI8 pSignature_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].signature_size>>1),
			UI16_B0(stsafea_ecc_info_table[key_type].signature_size>>1),
	};

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,&slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,(PLAT_UI8*)&message_length);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length,pMessage);
	stse_frame_element_swap_byte_order(&eMessage_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eSignature_R_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&RspFrame,eSignature_R, (stsafea_ecc_info_table[key_type].signature_size>>1), pSignature);
	stse_frame_element_allocate_push(&RspFrame,eSignature_S_length, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&RspFrame,eSignature_S, (stsafea_ecc_info_table[key_type].signature_size>>1), pSignature + (stsafea_ecc_info_table[key_type].signature_size>>1));

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_cmd_timings[pSTSE->device_type][STSAFEA_CMD_GENERATE_SIGNATURE]
	);

	return ret;
}

stse_ReturnCode_t stsafea_ecc_establish_shared_secret(
		stse_Handler_t * pSTSE,
		PLAT_UI8 private_key_slot_number,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 *pPublic_key,
		PLAT_UI8 *pShared_secret)
{
	stse_ReturnCode_t ret;
	stse_cmd_access_conditions_t cmd_ac_info;
	PLAT_UI8 cmd_encryption_flag = 0;
	PLAT_UI8 rsp_encryption_flag = 0;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key	  == NULL
	|| pShared_secret == NULL
	|| key_type == STSE_ECC_KT_CURVE25519)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_CMD_ESTABLISH_KEY;

	/* Public key elements */
	PLAT_UI8 point_representation_id = STSAFEA_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);
	PLAT_UI8 pPublic_key_length_element[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
			UI16_B0(stsafea_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSAFEA_ECC_GENERIC_LENGTH_SIZE, pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	PLAT_UI8 rsp_header;
	PLAT_UI8 pShared_secret_length[STSAFEA_ECC_GENERIC_LENGTH_SIZE] = {
			UI16_B1(stsafea_ecc_info_table[key_type].shared_secret_size),
			UI16_B0(stsafea_ecc_info_table[key_type].shared_secret_size),
	};

	stsafea_perso_info_get_cmd_encrypt_flag(pSTSE->pPerso_info, cmd_header, &cmd_encryption_flag);
	stsafea_perso_info_get_rsp_encrypt_flag(pSTSE->pPerso_info, cmd_header, &rsp_encryption_flag);
	stsafea_perso_info_get_cmd_AC(pSTSE->pPerso_info, cmd_header, &cmd_ac_info);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,ePrivate_key_slot_number,STSAFEA_SLOT_NUMBER_ID_SIZE,&private_key_slot_number);

	if(key_type == STSE_ECC_KT_ED25519)
	{
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	}
	else
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
		ePublic_key_second_element.length = stsafea_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,eShared_secret_length,STSAFEA_ECC_GENERIC_LENGTH_SIZE,pShared_secret_length);
	stse_frame_element_allocate_push(&RspFrame,eShared_secret,stsafea_ecc_info_table[key_type].shared_secret_size,pShared_secret);

	/*- Perform Transfer*/
	if (cmd_encryption_flag || rsp_encryption_flag)
	{
		ret = stsafea_session_encrypted_transfer (pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_encryption_flag,
				rsp_encryption_flag,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else if (cmd_ac_info != STSE_CMD_AC_FREE) {
		ret = stsafea_session_authenticated_transfer( pSTSE->pActive_host_session,
				&CmdFrame,
				&RspFrame,
				cmd_ac_info,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	} else {
		/* - Perform Transfer*/
		ret = stse_frame_transfer(pSTSE,
				&CmdFrame,
				&RspFrame,
				stsafea_cmd_timings[pSTSE->device_type][cmd_header]
		);
	}

	return ret;
}

stse_ReturnCode_t stsafea_ecc_decompress_public_key(
		stse_Handler_t * pSTSE,
		stse_ecc_key_type_t key_type,
		PLAT_UI8 point_representation_id,
		PLAT_UI8 *pPublic_key_X,
		PLAT_UI8 *pPublic_key_Y)
{
	stse_ReturnCode_t ret;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL)
	{
		return( STSE_SERVICE_HANDLER_NOT_INITIALISED );
	}

	if(pPublic_key_X == NULL
	|| pPublic_key_Y == NULL)
	{
		return( STSE_SERVICE_INVALID_PARAMETER );
	}

	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_ext_header = STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY;

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_header,STSAFEA_HEADER_SIZE,&cmd_header);
	stse_frame_element_allocate_push(&CmdFrame,eCmd_ext_header,STSAFEA_HEADER_SIZE,&cmd_ext_header);
	stse_frame_element_allocate_push(&CmdFrame,eCurve_ID,
			stsafea_ecc_info_table[key_type].curve_id_total_length,(PLAT_UI8*)&stsafea_ecc_info_table[key_type].curve_id);
	stse_frame_element_allocate_push(&CmdFrame,ePoint_representation_id,1,&point_representation_id);
	stse_frame_element_allocate_push(&CmdFrame,ePublic_key_X_coordinate,stsafea_ecc_info_table[key_type].coordinate_or_key_size,pPublic_key_X);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame,eRsp_header,STSAFEA_HEADER_SIZE,&rsp_header);
	stse_frame_element_allocate_push(&RspFrame,ePublic_key_Y_coordinate,stsafea_ecc_info_table[key_type].coordinate_or_key_size,pPublic_key_Y);

	/*- Perform Transfer*/
	ret = stse_frame_transfer(pSTSE,
			&CmdFrame,
			&RspFrame,
			stsafea_extended_cmd_timings[pSTSE->device_type][STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY]
	);

	return ret;
}

/* Static functions definition -----------------------------------------------*/
