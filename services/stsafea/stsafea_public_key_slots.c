/*!
 ******************************************************************************
 * \file	stsafea_public_key_slots.c
 * \brief   STSAFE Middleware services for genric public slots (source)
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
 *****************************************************************************/

#include "services/stsafea/stsafea_public_key_slots.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_ReturnCode_t
stsafea_query_generic_public_key_slots_count(stse_Handler_t *pSTSE,
					     PLAT_UI8 *pGeneric_public_key_slot_count)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pGeneric_public_key_slot_count == NULL) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_TABLE;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSubject_tag, 1, &subject_tag);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, eSymmetric_key_slot_count, 1,
					 pGeneric_public_key_slot_count);

	/*- Perform Transfer*/
	return stsafea_frame_raw_transfer(pSTSE, &CmdFrame, &RspFrame,
					  stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_query_generic_public_key_slot_info(
	stse_Handler_t *pSTSE, PLAT_UI8 slot_number, PLAT_UI8 *pPresence_flag,
	stsafea_generic_public_key_configuration_flags_t *pConfiguration_flags,
	stse_ecc_key_type_t *pKey_type)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pPresence_flag == NULL || pConfiguration_flags == NULL || pKey_type == NULL) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
	stsafea_ecc_curve_id_t curve_id;
	PLAT_UI8 rsp_header;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSubject_tag, 1, &subject_tag);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
					 &slot_number);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, ePresence_flag, 1, pPresence_flag);
	stse_frame_element_allocate_push(&RspFrame, eConfiguration_flags,
					 sizeof(stsafea_generic_public_key_configuration_flags_t),
					 (PLAT_UI8 *)pConfiguration_flags);
	stse_frame_element_allocate_push(&RspFrame, eCurve_id, sizeof(stsafea_ecc_curve_id_t),
					 (PLAT_UI8 *)&curve_id);

	/*- Perform Transfer*/
	ret = stsafea_frame_raw_transfer(pSTSE, &CmdFrame, &RspFrame,
					 stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

	if (ret != STSE_OK) {
		return ret;
	}

	if (*pPresence_flag == 1) {
		stse_ecc_key_type_t curve_id_index;
		PLAT_UI8 curve_id_total_length;
		*pKey_type = STSE_ECC_KT_INVALID;
		/*extract curve id length */
		curve_id_total_length = (*(eCurve_id.pData) << 8);
		curve_id_total_length += *(eCurve_id.pData + 1) + STSE_ECC_CURVE_ID_LENGTH_SIZE;
		/* Compare slot curve ID against each known curve ID to set the key type */
		for (curve_id_index = (stse_ecc_key_type_t)0;
		     (PLAT_I8)curve_id_index < (PLAT_I8)STSE_ECC_KT_INVALID; curve_id_index++) {
			/* First check of the ID length to speed-up the loop */
			if (curve_id_total_length ==
			    stse_ecc_info_table[curve_id_index].curve_id_total_length) {
				int diff;
				diff = memcmp(
					(PLAT_UI8 *)&stse_ecc_info_table[curve_id_index].curve_id,
					(PLAT_UI8 *)&curve_id,
					stse_ecc_info_table[curve_id_index].curve_id_total_length);
				if (diff == 0) {
					*pKey_type = curve_id_index;
					break;
				}
			}
		}
		/* If the comparison loop reach the end and pKey_type is always as
		 * initialized return error */
		if ((curve_id_index + 1) >= STSE_ECC_KT_INVALID ||
		    (PLAT_I8)*pKey_type >= (PLAT_I8)STSE_ECC_KT_INVALID) {
			return STSE_UNEXPECTED_ERROR;
		}
	}

	return ret;
}

stse_ReturnCode_t stsafea_query_generic_public_key_slot_value(stse_Handler_t *pSTSE,
							      PLAT_UI8 slot_number,
							      stse_ecc_key_type_t key_type,
							      PLAT_UI8 *pPublic_key)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (key_type == STSE_ECC_KT_INVALID || pPublic_key == NULL) {
		return (STSE_SERVICE_INVALID_PARAMETER);
	}

	/* Allocate elements and buffers*/
	PLAT_UI8 subject_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_SLOT;
	PLAT_UI8 rsp_header;
	PLAT_UI8 presence_flag;
	stsafea_generic_public_key_configuration_flags_t configuration_flags;
	PLAT_UI8 pCurve_id[stse_ecc_info_table[key_type].curve_id_total_length];

	PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
	stse_frame_element_allocate(ePoint_representation_id, 1, &point_representation_id);

	PLAT_UI8 pPublic_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] = {
		UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
		UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size)};
	stse_frame_element_allocate(ePublic_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);
	stse_frame_element_allocate(ePublic_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
				    pPublic_key_length_element);

	stse_frame_element_allocate(ePublic_key_first_element, 0, NULL);
	stse_frame_element_allocate(ePublic_key_second_element, 0, NULL);

	/* Construct Cmd & Rsp frames */
	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eSubject_tag, 1, &subject_tag);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
					 &slot_number);

	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
	stse_frame_element_allocate_push(&RspFrame, ePresence_flag, 1, &presence_flag);
	stse_frame_element_allocate_push(&RspFrame, eConfiguration_flags,
					 sizeof(stsafea_generic_public_key_configuration_flags_t),
					 (PLAT_UI8 *)&configuration_flags);
	stse_frame_element_allocate_push(&RspFrame, eCurve_id,
					 stse_ecc_info_table[key_type].curve_id_total_length,
					 pCurve_id);

#ifdef STSE_CONF_ECC_EDWARD_25519
	if (key_type == STSE_ECC_KT_ED25519) {
		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);
		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);
	} else
#endif
	{
		stse_frame_push_element(&RspFrame, &ePoint_representation_id);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_first_element);

		ePublic_key_first_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_first_element.pData = pPublic_key;
		stse_frame_push_element(&RspFrame, &ePublic_key_first_element);

		stse_frame_push_element(&RspFrame, &ePublic_key_length_second_element);

		ePublic_key_second_element.length =
			stse_ecc_info_table[key_type].coordinate_or_key_size;
		ePublic_key_second_element.pData = pPublic_key + ePublic_key_first_element.length;
		stse_frame_push_element(&RspFrame, &ePublic_key_second_element);
	}

	/*- Perform Transfer*/
	return stsafea_frame_raw_transfer(pSTSE, &CmdFrame, &RspFrame,
					  stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_write_generic_ecc_public_key(stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
						       stse_ecc_key_type_t key_type,
						       PLAT_UI8 *pPublic_key)
{
	PLAT_UI8 cmd_header = STSAFEA_EXTENDED_COMMAND_PREFIX;
	PLAT_UI8 cmd_header_extended = STSAFEA_EXTENDED_CMD_WRITE_PUBLIC_KEY;

	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	if (pPublic_key == NULL || (PLAT_I8)key_type >= (PLAT_I8)STSE_ECC_KT_INVALID) {
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

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header_extended, STSAFEA_HEADER_SIZE,
					 &cmd_header_extended);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
					 &slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eCurve_id,
					 stse_ecc_info_table[key_type].curve_id_total_length,
					 (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#if defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519)
	uint8_t is_supported_key = 0;
#ifdef STSE_CONF_ECC_CURVE_25519
	is_supported_key |= (key_type == STSE_ECC_KT_CURVE25519);
#endif
#ifdef STSE_CONF_ECC_EDWARD_25519
	is_supported_key |= (key_type == STSE_ECC_KT_ED25519);
#endif

	if (is_supported_key) {
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

	/*- Perform Transfer*/
	return stsafea_frame_transfer(pSTSE, &CmdFrame, &RspFrame);
}

stse_ReturnCode_t stsafea_set_generic_public_slot_configuration_flag(
	stse_Handler_t *pSTSE, PLAT_UI8 slot_number,
	stsafea_generic_public_key_configuration_flags_t configuration_flags)
{
	PLAT_UI8 cmd_header = STSAFEA_CMD_PUT_ATTRIBUTE;

	/* - Check stsafe handler initialization */
	if (pSTSE == NULL) {
		return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
	}

	PLAT_UI8 attribute_tag = STSAFEA_SUBJECT_TAG_GENERIC_PUBLIC_KEY_CONFIGURATION_FLAGS;

	stse_frame_allocate(CmdFrame);
	stse_frame_element_allocate_push(&CmdFrame, eCmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
	stse_frame_element_allocate_push(&CmdFrame, eAttribute_tag, 1, &attribute_tag);
	stse_frame_element_allocate_push(&CmdFrame, eSlot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
					 &slot_number);
	stse_frame_element_allocate_push(&CmdFrame, eConfiguration_flags,
					 sizeof(stsafea_generic_public_key_configuration_flags_t),
					 (PLAT_UI8 *)&configuration_flags);

	PLAT_UI8 rsp_header;
	stse_frame_allocate(RspFrame);
	stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

	/*- Perform Transfer*/
	return stsafea_frame_raw_transfer(pSTSE, &CmdFrame, &RspFrame,
					  stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
