/**
  ******************************************************************************
  * @file    stsafea_ecc.c
  * @author  CS Application Team
  * @brief   STSAFE-A services for ECC (source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

stse_return_code_t stsafea_start_volatile_kek_session(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *host_ecdhe_public_key)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (host_ecdhe_public_key == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
  PLAT_UI8 rsp_header;

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);

  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  /* Prepare START VOLATILE KEK SESSION Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_curve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
  if (key_type == STSE_ECC_KT_CURVE25519)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = host_ecdhe_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
  }
  else
#endif /* STSE_CONF_ECC_CURVE_25519 */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&cmd_frame, &e_point_representation_id);
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = host_ecdhe_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
    e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = host_ecdhe_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
  }

  /* [ALGORITHM ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_algorithm_id, 1, &algorithm_id);

  /* Prepare START VOLATILE KEK SESSION RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_start_volatile_kek_session_authenticated(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t ecdhe_key_type,
  PLAT_UI8 *host_ecdhe_public_key,
  stse_hash_algorithm_t hash_algo,
  PLAT_UI8 signature_public_key_slot_number,
  stse_ecc_key_type_t signature_key_type,
  PLAT_UI8 *p_signature)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_START_VOLATILE_KEK_SESSION
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (host_ecdhe_public_key == NULL || p_signature == NULL ||
      ecdhe_key_type >= STSE_ECC_KT_INVALID || signature_key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);

  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);

  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  PLAT_UI8 kdf_algorithm_id = STSAFEA_ALGORITHM_ID_KEK_UNWRAPPING;
  PLAT_UI8 filler_1_byte = 0;

  PLAT_UI8 p_empty_hash_algo_id[STSAFEA_GENERIC_LENGTH_SIZE] = {0x00, 0x00};
  STSE_FRAME_ELEMENT_ALLOCATE(e_hash_algo_id, STSAFEA_GENERIC_LENGTH_SIZE, p_empty_hash_algo_id);

  /* Divide Signature length By 2 to get R or S length */
  PLAT_UI16 signature_r_s_length = stse_ecc_info_table[signature_key_type].signature_size >> 1;
  PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(signature_r_s_length),
    UI16_B0(signature_r_s_length),
  };

  PLAT_UI8 rsp_header = 0;

  /* Prepare START VOLATILE KEK SESSION Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);

  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_curve_id,
                                   stse_ecc_info_table[ecdhe_key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[ecdhe_key_type].curve_id);

#ifdef STSE_CONF_ECC_CURVE_25519
  if (ecdhe_key_type == STSE_ECC_KT_CURVE25519)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = host_ecdhe_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
  }
  else
#endif /* STSE_CONF_ECC_CURVE_25519 */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&cmd_frame, &e_point_representation_id);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);

    e_public_key_first_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = host_ecdhe_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);

    e_public_key_second_element.length = stse_ecc_info_table[ecdhe_key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = host_ecdhe_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
  }

  /* [KDF ALGORITHM ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_kdf_algorithm_id, 1, &kdf_algorithm_id);
  /* [FILLER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_filler, 1, &filler_1_byte);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_public_key_slot_number, 1,
                                   &signature_public_key_slot_number);

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (signature_key_type != STSE_ECC_KT_ED25519)
#endif /* STSE_CONF_ECC_EDWARD_25519 */
  {
    e_hash_algo_id.length = STSAFEA_HASH_ALGO_ID_SIZE;
    e_hash_algo_id.p_data = (PLAT_UI8 *)&stsafea_hash_info_table[hash_algo].id;
  }
  /* [HASH ALGORITHM] */
  stse_frame_push_element(&cmd_frame, &e_hash_algo_id);

  /* [SIGNATURE R LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE R] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r, signature_r_s_length, p_signature);
  /* [SIGNATURE S LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE S] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s, signature_r_s_length, p_signature + signature_r_s_length);

  /* Prepare START VOLATILE KEK SESSION RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, 1, &rsp_header);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_stop_volatile_kek_session(
  stse_handle_t *p_stse)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_STOP_VOLATILE_KEK_SESSION
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  PLAT_UI8 rsp_header;

  /* Prepare STOP VOLATILE KEK SESSION Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);

  /* Prepare STOP VOLATILE KEK SESSION RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_ecc_verify_signature(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *p_public_key,
  const PLAT_UI8 *p_signature,
  const PLAT_UI8 *p_message,
  PLAT_UI16 message_length,
  PLAT_UI8 eddsa_variant,
  PLAT_UI8 *p_signature_validity)
{
  stse_return_code_t ret;
  PLAT_UI8 cmd_header = STSAFEA_CMD_VERIFY_SIGNATURE;

  PLAT_UI8 subject = 0x00;

  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };

  /* Signature elements */
  PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_public_key == NULL || p_signature == NULL ||
      p_message == NULL || p_signature_validity == NULL ||
      key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Public key elements */
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  /* Hash elements*/
#ifdef STSE_CONF_ECC_EDWARD_25519
  STSE_FRAME_ELEMENT_ALLOCATE(e_ed_dsa_variant, 1, &eddsa_variant);
#endif /* STSE_CONF_ECC_EDWARD_25519 */

  PLAT_UI8 rsp_header;

  /* Prepare VERIFY SIGNATURE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SUBJECT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_subject, 1, &subject);
  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_curve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = (PLAT_UI8 *)p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
  }
  else
#endif /* STSE_CONF_ECC_EDWARD_25519 */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&cmd_frame, &e_point_representation_id);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);

    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = (PLAT_UI8 *)p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);

    e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = (PLAT_UI8 *)p_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
  }

  /* [SIGNATURE R LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE R] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_r, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   (PLAT_UI8 *)p_signature);
  /* [SIGNATURE S LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE S] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_signature_s, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   (PLAT_UI8 *)p_signature + (stse_ecc_info_table[key_type].signature_size >> 1));

#ifdef STSE_CONF_ECC_EDWARD_25519
  if (key_type == STSE_ECC_KT_ED25519)
  {
    /* [EDDSA VARIANT] */
    stse_frame_push_element(&cmd_frame, &e_ed_dsa_variant);
  }
#endif /* STSE_CONF_ECC_EDWARD_25519 */

  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* [MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message, message_length, (PLAT_UI8 *)p_message);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_message_length);

  /* Prepare VERIFY SIGNATURE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [VERIFICATION RESULT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_validity, 1, p_signature_validity);

  ret = stsafea_frame_transfer(p_stse,
                               &cmd_frame,
                               &rsp_frame);

  if (ret != STSE_OK)
  {
    *p_signature_validity = STSAFEA_FALSE;
  }

  return ret;
}

stse_return_code_t stsafea_ecc_generate_signature(
  stse_handle_t *p_stse,
  PLAT_UI8 slot_number,
  stse_ecc_key_type_t key_type,
  const PLAT_UI8 *p_message,
  PLAT_UI16 message_length,
  PLAT_UI8 *p_signature)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_GENERATE_SIGNATURE;

  PLAT_UI8 rsp_header;
  /* Signature elements */
  PLAT_UI8 p_signature_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].signature_size >> 1),
    UI16_B0(stse_ecc_info_table[key_type].signature_size >> 1),
  };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_message == NULL || p_signature == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Prepare GENERATE SIGNATURE Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE, &slot_number);
  /* [MESSAGE LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message_length, STSAFEA_GENERIC_LENGTH_SIZE,
                                   (PLAT_UI8 *)&message_length);
  /* [MESSAGE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_message, message_length, (PLAT_UI8 *)p_message);
  /* Swap length byte order to be compliant with STSAFE-A convention */
  stse_frame_element_swap_byte_order(&e_message_length);

  /* Prepare GENERATE SIGNATURE RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [SIGNATURE R LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_r_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE R] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_r, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   p_signature);
  /* [SIGNATURE S LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_s_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_signature_length_element);
  /* [SIGNATURE S] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_signature_s, (stse_ecc_info_table[key_type].signature_size >> 1),
                                   p_signature + (stse_ecc_info_table[key_type].signature_size >> 1));

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_ecc_establish_shared_secret(
  stse_handle_t *p_stse,
  PLAT_UI8 private_key_slot_number,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 *p_public_key,
  PLAT_UI8 *p_shared_secret)
{
  PLAT_UI8 cmd_header = STSAFEA_CMD_ESTABLISH_KEY;

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_public_key == NULL || p_shared_secret == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  /* Public key elements */
  PLAT_UI8 point_representation_id = STSE_NIST_BRAINPOOL_POINT_REPRESENTATION_ID;
  STSE_FRAME_ELEMENT_ALLOCATE(e_point_representation_id, 1, &point_representation_id);
  PLAT_UI8 p_public_key_length_element[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].coordinate_or_key_size),
    UI16_B0(stse_ecc_info_table[key_type].coordinate_or_key_size),
  };
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_first_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_length_second_element, STSE_ECC_GENERIC_LENGTH_SIZE,
                              p_public_key_length_element);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_first_element, 0, NULL);
  STSE_FRAME_ELEMENT_ALLOCATE(e_public_key_second_element, 0, NULL);

  PLAT_UI8 rsp_header;
  PLAT_UI8 p_shared_secret_length[STSE_ECC_GENERIC_LENGTH_SIZE] =
  {
    UI16_B1(stse_ecc_info_table[key_type].shared_secret_size),
    UI16_B0(stse_ecc_info_table[key_type].shared_secret_size),
  };

  /* Prepare ESTABLISH KEY Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_HEADER_SIZE, &cmd_header);
  /* [SLOT] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_private_key_slot_number, STSAFEA_SLOT_NUMBER_ID_SIZE,
                                   &private_key_slot_number);

#if defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519)
  uint8_t is_supported_key = 0;
#ifdef STSE_CONF_ECC_CURVE_25519
  is_supported_key |= (key_type == STSE_ECC_KT_CURVE25519);
#endif /* STSE_CONF_ECC_CURVE_25519 */
#ifdef STSE_CONF_ECC_EDWARD_25519
  is_supported_key |= (key_type == STSE_ECC_KT_ED25519);
#endif /* STSE_CONF_ECC_EDWARD_25519 */

  if (is_supported_key)
  {
    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);
  }
  else
#endif /* defined(STSE_CONF_ECC_CURVE_25519) || defined(STSE_CONF_ECC_EDWARD_25519) */
  {
    /* [POINT REPRESENTATION ID] */
    stse_frame_push_element(&cmd_frame, &e_point_representation_id);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_first_element);
    e_public_key_first_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_first_element.p_data = p_public_key;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_first_element);

    /* [PUBLIC KEY LENGTH] */
    stse_frame_push_element(&cmd_frame, &e_public_key_length_second_element);
    e_public_key_second_element.length = stse_ecc_info_table[key_type].coordinate_or_key_size;
    e_public_key_second_element.p_data = p_public_key + e_public_key_first_element.length;
    /* [PUBLIC KEY] */
    stse_frame_push_element(&cmd_frame, &e_public_key_second_element);
  }

  /* Prepare ESTABLISH KEY RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [SHARED SECRET LENGTH] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_shared_secret_length, STSE_ECC_GENERIC_LENGTH_SIZE,
                                   p_shared_secret_length);
  /* [SHARED SECRET] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_shared_secret, stse_ecc_info_table[key_type].shared_secret_size,
                                   p_shared_secret);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

stse_return_code_t stsafea_ecc_decompress_public_key(
  stse_handle_t *p_stse,
  stse_ecc_key_type_t key_type,
  PLAT_UI8 point_representation_id,
  PLAT_UI8 *p_public_key_x,
  PLAT_UI8 *p_public_key_y)
{
  PLAT_UI8 cmd_header[STSAFEA_EXT_HEADER_SIZE] = {STSAFEA_EXTENDED_COMMAND_PREFIX,
                                                  STSAFEA_EXTENDED_CMD_DECOMPRESS_PUBLIC_KEY
                                                 };

  /* Check stsafe handler initialization */
  if (p_stse == NULL)
  {
    return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
  }

  /* Check service parameters */
  if (p_public_key_x == NULL || p_public_key_y == NULL || key_type >= STSE_ECC_KT_INVALID)
  {
    return (STSE_SERVICE_INVALID_PARAMETER);
  }

  PLAT_UI8 rsp_header;

  /* Prepare DECOMPRESS PUBLIC KEY Frame */
  STSE_FRAME_ALLOCATE(cmd_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_cmd_header, STSAFEA_EXT_HEADER_SIZE, cmd_header);
  /* [CURVE ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_curve_id,
                                   stse_ecc_info_table[key_type].curve_id_total_length,
                                   (PLAT_UI8 *)&stse_ecc_info_table[key_type].curve_id);
  /* [POINT REPRESENTATION ID] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_point_representation_id, 1, &point_representation_id);
  /* [PUBLIC KEY X COORDINATE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&cmd_frame, e_public_key_x_coordinate,
                                   stse_ecc_info_table[key_type].coordinate_or_key_size, p_public_key_x);

  /* Prepare DECOMPRESS PUBLIC KEY RSP Frame */
  STSE_FRAME_ALLOCATE(rsp_frame);
  /* [HEADER] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_rsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
  /* [PUBLIC KEY Y COORDINATE] */
  STSE_FRAME_ELEMENT_ALLOCATE_PUSH(&rsp_frame, e_public_key_y_coordinate,
                                   stse_ecc_info_table[key_type].coordinate_or_key_size, p_public_key_y);

  return stsafea_frame_transfer(p_stse,
                                &cmd_frame,
                                &rsp_frame);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
