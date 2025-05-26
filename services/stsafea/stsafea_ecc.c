/*!
 ******************************************************************************
 * \file	stsafea_ecc.c
 * \brief   ECC services for STSAFE-A
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 *in the root directory of this software component. If no LICENSE file comes
 *with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t stsafea_start_volatile_KEK_session(stse_Handler_t *pSTSE,
						     stse_ecc_key_type_t key_type,
						     PLAT_UI8 *host_ecdhe_public_key)
{
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 ext_cmd_header = STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (host_ecdhe_public_key == NULL || key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
	PLAT_UI8 rsp_header;

	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
		UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eExt_cmd_header, STSAFEA_HEADER_SIZE,
					 &ext_cmd_header);

	stse_frame_element_allocate_push(&CmdFrame, eCurve_id,
					 stse_ecc_info_table[key_type].curve_id_total_length,
					 (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
	if (key_type == STSE_ECC_KT_CURVE25519) {
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	} else
#endif
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
		ePublic_key_second_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData =
			host_ecdhe_public_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_element_allocate_push(&CmdFrame, eAlgorithm_id, 1, &algorithm_id);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_start_volatile_KEK_session_authenticated(
	stse_Handler_t *pSTSE, stse_ecc_key_type_t ecdhe_key_type, PLAT_UI8 *host_ecdhe_public_key,
	stse_hash_algorithm_t hash_algo, PLAT_UI8 signature_public_key_slot_number,
	stse_ecc_key_type_t signature_key_type, PLAT_UI8 *pSignature)
{
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_extended_header = STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (host_ecdhe_public_key == NULL || pSignature == NULL ||
	    ecdhe_key_type == STSE_ECC_KT_INVALID || signature_key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
		UI16_B0(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	PLAT_UI8 kdf_algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
	PLAT_UI8 filler_1_byte = 0;

	PLAT_UI8 pEmpty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
	stse_frame_element_allocate(eHash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE,
				    pEmpty_hash_algo_id);

	/* Divide Signature length By 2 to get R or S length */
	PLAT_UI16 signature_R_S_length =
		stse_ecc_info_table[signature_key_type].signature_size >> 1;
	PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(signature_R_S_length),
		UI16_B0(signature_R_S_length),
	};

	PLAT_UI8 rsp_header = 0;

	/* FRAME : . */
	stse_frame_allocate(CmdFrame);

	/* FRAME : [HEADER] [EXT HEADER] */
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, 1, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_extended_header, 1, &cmd_extended_header);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] */
	stse_frame_element_allocate_push(&CmdFrame, eCurve_id,
					 stse_ecc_info_table[ecdhe_key_type].curve_id_total_length,
					 (PLAT_UI8 *)&stse_ecc_info_table[ecdhe_key_type].curve_id);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] */
#ifdef STSE_CONF_ECC_CURVE_25519
	if (ecdhe_key_type == STSE_ECC_KT_CURVE25519) {
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	} else
#endif
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length =
			stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = host_ecdhe_public_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length =
			stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData =
			host_ecdhe_public_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER]
	 * [SIGNATURE KEY SLOT] */
	stse_frame_element_allocate_push(&CmdFrame, eKdf_algorithm_id, 1, &kdf_algorithm_id);
	stse_frame_element_allocate_push(&CmdFrame, eFiller, 1, &filler_1_byte);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_public_key_slot_number, 1,
					 &signature_public_key_slot_number);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER]
	 * [SIGNATURE KEY SLOT] [HASH ALGO] */
#ifdef STSE_CONF_ECC_EDWARD_25519
	if (signature_key_type != STSE_ECC_KT_ED25519)
#endif
	{
		eHash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
		eHash_algo_id.pData = (PLAT_UI8 *)&stsafea_hash_info_table[hash_algo].id;
	}
	stse_frame_push_element(&CmdFrame, &eHash_algo_id);

	/* FRAME : [HEADER] [EXT HEADER] [CURVE ID] [PUBLIC KEY] [KDF ID] [FILLER]
	 * [SIGNATURE KEY SLOT] [HASH ALGO] [SIGNATURE] */
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R, signature_R_S_length, pSignature);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S, signature_R_S_length,
					 pSignature + signature_R_S_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, 1, &rsp_header);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_stop_volatile_KEK_session(stse_Handler_t *pSTSE)
{
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_extended_header = STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSubject_tag, 1, &cmd_extended_header);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_verify_signature(stse_Handler_t *pSTSE, stse_ecc_key_type_t key_type,
					       PLAT_UI8 *pPublic_key, PLAT_UI8 *pSignature,
					       PLAT_UI8 *pMessage, PLAT_UI16 message_length,
					       PLAT_UI8 eddsa_variant,
					       PLAT_UI8 *pSignature_validity)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_SIGNATURE;

	PLAT_UI8 subject = 0x00;

	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
		UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
	};

	/* Signature elements */
	PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
		UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
	};

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pPublic_key == NULL || pSignature == NULL || pMessage == NULL ||
	    pSignature_validity == NULL || key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	/* Public key elements */
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	/* Hash elements*/
#ifdef STSE_CONF_ECC_EDWARD_25519
	stse_frame_element_allocate(eEdDSA_variant, 1, &eddsa_variant);
#endif

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSubject, 1, &subject);
	stse_frame_element_allocate_push(&CmdFrame, eCurve_id,
					 stse_ecc_info_table[key_type].curve_id_total_length,
					 (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_EDWARD_25519
	if (key_type == STSE_ECC_KT_ED25519) {
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	} else
#endif
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_element_allocate_push(&CmdFrame, eSignature_R_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_R,
					 (stse_ecc_info_table[key_type].signature_size >> 1),
					 pSignature);
	stse_frame_element_allocate_push(&CmdFrame, eSignature_S_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(
		&CmdFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size >> 1),
		pSignature + (stse_ecc_info_table[key_type].signature_size >> 1));

#ifdef STSE_CONF_ECC_EDWARD_25519
	if (key_type == STSE_ECC_KT_ED25519) {
		stse_frame_push_element(&CmdFrame, &eEdDSA_variant);
	}
#endif

	stse_frame_element_allocate_push(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
					 (PLAT_UI8 *)&message_length);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length, pMessage);
	stse_frame_element_swap_byte_order(&eMessage_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eSignature_validity, 1, pSignature_validity);

	/* - Perform Transfer*/
	ret = stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);

	if (ret != STSE_OK) {
		*pSignature_validity = STSAFEA_FALSE;
	}

	return ret;
}

stse_ReturnCode_t stsafea_ecc_generate_signature(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
						 stse_ecc_key_type_t key_type, PLAT_UI8 *pMessage,
						 PLAT_UI16 message_length, PLAT_UI8 *pSignature)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_SIGNATURE;

	PLAT_UI8 rsp_header;
	/* Signature elements */
	PLAT_UI8 pSignature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
		UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
	};

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pMessage == NULL || pSignature == NULL || key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
					 &slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eMessage_length, STSAFEA_GENERIC_LENGTH_SIZE,
					 (PLAT_UI8 *)&message_length);
	stse_frame_element_allocate_push(&CmdFrame, eMessage, message_length, pMessage);
	stse_frame_element_swap_byte_order(&eMessage_length);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eSignature_R_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(&RspFrame, eSignature_R,
					 (stse_ecc_info_table[key_type].signature_size >> 1),
					 pSignature);
	stse_frame_element_allocate_push(&RspFrame, eSignature_S_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pSignature_length_element);
	stse_frame_element_allocate_push(
		&RspFrame, eSignature_S, (stse_ecc_info_table[key_type].signature_size >> 1),
		pSignature + (stse_ecc_info_table[key_type].signature_size >> 1));

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_establish_shared_secret(stse_Handler_t *pSTSE,
						      PLAT_UI8 private_key_slot_number,
						      stse_ecc_key_type_t key_type,
						      PLAT_UI8 *pPublic_key,
						      PLAT_UI8 *pShared_secret)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_ESTABLISH_KEY;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pPublic_key == NULL || pShared_secret == NULL || key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	/* Public key elements */
	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);
	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
		UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
	};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	PLAT_UI8 rsp_header;
	PLAT_UI8 pShared_secret_length[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].shared_secret_size),
		UI16_B0(stse_ecc_info_table[key_type].shared_secret_size),
	};

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, ePrivate_key_slot_number,
					 STSAFEA_SLOT_NUMBER_ID_SIZE, &private_key_slot_number);

#ifdef STSE_CONF_ECC_EDWARD_25519
	if (key_type == STSE_ECC_KT_ED25519) {
		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);
	} else
#endif
	{
		stse_frame_push_element(&CmdFrame, &ePoint_representation_id);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&CmdFrame, &ePublic_key_first_element);

		stse_frame_push_element(&CmdFrame, &ePublic_key_length_second_element);
		ePublic_key_second_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&CmdFrame, &ePublic_key_second_element);
	}

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eShared_secret_length,
					 STSE_ECC_GENERIC_LENGTH_SIZE, pShared_secret_length);
	stse_frame_element_allocate_push(&RspFrame, eShared_secret,
					 stse_ecc_info_table[key_type].shared_secret_size,
					 pShared_secret);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_ecc_decompress_public_key(stse_Handler_t *pSTSE,
						    stse_ecc_key_type_t key_type,
						    PLAT_UI8 point_representation_id,
						    PLAT_UI8 *pPublic_key_X,
						    PLAT_UI8 *pPublic_key_Y)
{
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_ext_header = STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pPublic_key_X == NULL || pPublic_key_Y == NULL || key_type == STSE_ECC_KT_INVALID) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_ext_header, STSAFEA_HEADER_SIZE,
					 &cmd_ext_header);
	stse_frame_element_allocate_push(&CmdFrame, eCurve_ID,
					 stse_ecc_info_table[key_type].curve_id_total_length,
					 (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);
	stse_frame_element_allocate_push(&CmdFrame, ePoint_representation_id, 1,
					 &point_representation_id);
	stse_frame_element_allocate_push(&CmdFrame, ePublic_key_X_coordinate,
					 stse_ecc_info_table[key_type].coordinate_or_key_size,
					 pPublic_key_X);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, ePublic_key_Y_coordinate,
					 stse_ecc_info_table[key_type].coordinate_or_key_size,
					 pPublic_key_Y);

	/* - Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
