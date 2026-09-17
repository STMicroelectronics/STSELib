/**
  ******************************************************************************
  * @file    stse_aes.c
  * @author  CS Application Team
  * @brief   STSE AES API set (sources)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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

#include "api/stse_aes.h"

/* AES API layer role:
 * - Validate high-level handler pointer presence.
 * - Route operation to STSAFE-A service implementation when enabled.
 * - Return a consistent incompatible-device code when support is disabled.
 */

/* API wrapper for one-shot AES-ECB encryption.
 * Forwards slot selection and plaintext length/bytes to service layer.
 */
stse_ReturnCode_t stse_aes_ecb_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* ECB APIs are available only when STSAFE-A backend is compiled in. */
  if (pSTSE == NULL)
  {
    /* API-level null handler mapping keeps return-code namespace consistent. */
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Service performs detailed parameter and transport validation. */
  return stsafea_aes_ecb_encrypt(
           pSTSE,
           slot_number,
           message_length,
           pPlaintext_message,
           pEncrypted_message);
#else
  /* Build-time fallback when target does not include STSAFE-A services. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for one-shot AES-ECB decryption.
 * Mirrors encrypt path while routing to DECRYPT service command.
 */
stse_ReturnCode_t stse_aes_ecb_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pPlaintext_message)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Decrypt wrapper preserves exact parameter contract of service entrypoint. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Ciphertext/plaintext buffers are checked downstream by service layer. */
  return stsafea_aes_ecb_decrypt(
           pSTSE,
           slot_number,
           message_length,
           pEncrypted_message,
           pPlaintext_message);
#else
  /* Keeping fallback local to each wrapper avoids hidden macro side effects. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for one-shot AES-CCM encryption.
 * Supports associated data, payload encryption, tag output, and optional counter.
 */
stse_ReturnCode_t stse_aes_ccm_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 associated_data_length, PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length, PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pEncrypted_authentication_tag,
  PLAT_UI8 counter_presence, PLAT_UI32 *pCounter)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* CCM one-shot path can request optional counter reporting from device. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* This call forwards CCM tuple: nonce, AAD, message, and tag requirements. */
  return stsafea_aes_ccm_encrypt(
           pSTSE,
           slot_number,
           authentication_tag_length,
           pNonce,
           associated_data_length, pAssociated_data,
           message_length,
           pPlaintext_message,
           pEncrypted_message,
           pEncrypted_authentication_tag,
           counter_presence, pCounter);
#else
  /* Device-type incompatibility is surfaced before any service call attempt. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming START phase.
 * Initializes device context using total lengths and first chunks.
 */
stse_ReturnCode_t stse_aes_ccm_encrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 Nonce_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 total_associated_data_length,
  PLAT_UI32 total_message_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pCounter_presence,
  PLAT_UI32 *pCounter)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* START wrapper forwards total lengths to initialize streaming crypto state. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* START phase can return counter metadata depending on slot configuration. */
  return stsafea_aes_ccm_encrypt_start(
           pSTSE,
           slot_number,
           Nonce_length,
           pNonce,
           total_associated_data_length,
           total_message_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk,
           pCounter_presence,
           pCounter);
#else
  /* Unified fallback code keeps API behavior predictable across platforms. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming PROCESS phase.
 * Feeds intermediate AAD/message chunks and obtains ciphertext chunks.
 */
stse_ReturnCode_t stse_aes_ccm_encrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* PROCESS wrappers intentionally contain no chunk pre-validation logic. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Chunk ordering and size validation are delegated to service implementation. */
  return stsafea_aes_ccm_encrypt_process(
           pSTSE,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk);
#else
  /* Unsupported backend path exits immediately with explicit API status. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming FINISH phase.
 * Finalizes encryption stream and retrieves authentication tag bytes.
 */
stse_ReturnCode_t stse_aes_ccm_encrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pEncrypted_authentication_tag)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* FINISH wrappers collect authentication tags and final chunk outputs. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* FINISH also accepts residual AAD/message chunk segments if present. */
  return stsafea_aes_ccm_encrypt_finish(
           pSTSE,
           authentication_tag_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk,
           pEncrypted_authentication_tag);
#else
  /* Compile-time guard prevents dangling references to missing services. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for one-shot AES-CCM decryption.
 * Returns plaintext and verification result indicating tag validity.
 */
stse_ReturnCode_t stse_aes_ccm_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pEncrypted_authentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Decrypt one-shot wrappers deliver verification result back to caller. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Verification_result must be inspected by caller before trusting plaintext. */
  return stsafea_aes_ccm_decrypt(
           pSTSE,
           slot_number,
           authentication_tag_length,
           pNonce,
           associated_data_length,
           pAssociated_data,
           message_length,
           pEncrypted_message,
           pEncrypted_authentication_tag,
           pVerification_result,
           pPlaintext_message);
#else
  /* Same fallback code is used by all wrappers for caller-side simplicity. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for one-shot AES-GCM encryption.
 * Handles variable IV length and optional associated data bytes.
 */
stse_ReturnCode_t stse_aes_gcm_encrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pPlaintext_message,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pAuthentication_tag)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM one-shot encrypt supports variable IV sizes through service API. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Delegates frame serialization and endian conversions to service layer. */
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
#else
  /* Incompatible-device code allows runtime feature probing by applications. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming START phase (decryption).
 * Initializes decrypt context with nonce and total ciphertext budget.
 */
stse_ReturnCode_t stse_aes_ccm_decrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 Nonce_length,
  PLAT_UI8 *pNonce,
  PLAT_UI16 total_associated_data_length,
  PLAT_UI16 total_ciphertext_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* CCM decrypt streaming start initializes authentication/check state. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* First decrypt chunk can contain both AAD and encrypted payload parts. */
  return stsafea_aes_ccm_decrypt_start(
           pSTSE,
           slot_number,
           Nonce_length,
           pNonce,
           total_associated_data_length,
           total_ciphertext_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pPlaintext_message_chunk);
#else
  /* Wrapper fallback remains intentionally branch-local for readability. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming PROCESS phase (decryption).
 * Continues authenticated decrypt over subsequent chunks.
 */
stse_ReturnCode_t stse_aes_ccm_decrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Decrypt PROCESS wrappers propagate chunk boundaries as provided by caller. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Streaming order constraints are preserved by passing chunks unchanged. */
  return stsafea_aes_ccm_decrypt_process(
           pSTSE,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pPlaintext_message_chunk);
#else
  /* No-op path for unsupported builds returns deterministic status code. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-CCM streaming FINISH phase (decryption).
 * Supplies final tag and receives verification status with final plaintext chunk.
 */
stse_ReturnCode_t stse_aes_ccm_decrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* Decrypt FINISH wrappers combine tag check and plaintext tail retrieval. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Result code combines transport outcome and cryptographic verification path. */
  return stsafea_aes_ccm_decrypt_finish(
           pSTSE,
           authentication_tag_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pAuthentication_tag,
           pVerification_result,
           pPlaintext_message_chunk);
#else
  /* Fallback keeps API binary compatible even without STSAFE-A integration. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming START phase (encryption).
 * Binds IV and first chunks to device-side stream state.
 */
stse_ReturnCode_t stse_aes_gcm_encrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM START wrappers seed stream state and return first encrypted chunk. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Wrapper keeps API and service signatures aligned without data mutation. */
  return stsafea_aes_gcm_encrypt_start(
           pSTSE,
           slot_number,
           IV_length,
           pIV,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk);
#else
  /* Explicit incompatible return avoids ambiguous generic error reporting. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming PROCESS phase (encryption).
 * Processes middle chunks until stream reaches finish step.
 */
stse_ReturnCode_t stse_aes_gcm_encrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM PROCESS wrappers are thin pass-throughs by design. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Chunk lengths are forwarded exactly to preserve stream framing semantics. */
  return stsafea_aes_gcm_encrypt_process(
           pSTSE,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk);
#else
  /* Fallback path documents unsupported-feature behavior at API level. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming FINISH phase (encryption).
 * Emits final ciphertext and authentication tag.
 */
stse_ReturnCode_t stse_aes_gcm_encrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pPlaintext_message_chunk,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM FINISH wrappers return both tag and final encrypted chunk bytes. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Caller controls requested tag length through authentication_tag_length. */
  return stsafea_aes_gcm_encrypt_finish(
           pSTSE,
           authentication_tag_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pPlaintext_message_chunk,
           pEncrypted_message_chunk,
           pAuthentication_tag);
#else
  /* Caller can branch on incompatible status without inspecting device type. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for one-shot AES-GCM decryption.
 * Returns plaintext and an explicit verification-result status byte.
 */
stse_ReturnCode_t stse_aes_gcm_decrypt(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_length,
  PLAT_UI8 *pAssociated_data,
  PLAT_UI16 message_length,
  PLAT_UI8 *pEncrypted_message,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM one-shot decrypt exposes tag verification status via output pointer. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Verification output allows caller to gate plaintext acceptance policy. */
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
#else
  /* Wrapper-level fallback ensures consistent behavior in mixed device builds. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming START phase (decryption).
 * Starts authenticated decrypt context with IV and initial chunks.
 */
stse_ReturnCode_t stse_aes_gcm_decrypt_start(
  stse_Handler_t *pSTSE,
  PLAT_UI8 slot_number,
  PLAT_UI16 IV_length,
  PLAT_UI8 *pIV,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM decrypt START wrapper forwards IV/chunks untouched to service call. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Wrapper forwards encrypted and plaintext chunk pointers unmodified. */
  return stsafea_aes_gcm_decrypt_start(
           pSTSE,
           slot_number,
           IV_length,
           pIV,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pPlaintext_message_chunk);
#else
  /* Returning here avoids exposing unsupported command paths at runtime. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming PROCESS phase (decryption).
 * Continues chunked decrypt/authenticate sequence.
 */
stse_ReturnCode_t stse_aes_gcm_decrypt_process(
  stse_Handler_t *pSTSE,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM decrypt PROCESS wrapper continues authenticated stream processing. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* PROCESS phase can be invoked repeatedly before FINISH. */
  return stsafea_aes_gcm_decrypt_process(
           pSTSE,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pPlaintext_message_chunk);
#else
  /* Common fallback simplifies application-level feature gating. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

/* API wrapper for AES-GCM streaming FINISH phase (decryption).
 * Supplies final tag and retrieves both verification and plaintext tail.
 */
stse_ReturnCode_t stse_aes_gcm_decrypt_finish(
  stse_Handler_t *pSTSE,
  PLAT_UI8 authentication_tag_length,
  PLAT_UI16 associated_data_chunk_length,
  PLAT_UI8 *pAssociated_data_chunk,
  PLAT_UI16 message_chunk_length,
  PLAT_UI8 *pEncrypted_message_chunk,
  PLAT_UI8 *pAuthentication_tag,
  PLAT_UI8 *pVerification_result,
  PLAT_UI8 *pPlaintext_message_chunk)
{
#ifdef STSE_CONF_STSAFE_A_SUPPORT
  /* GCM decrypt FINISH wrapper returns verification flag and plaintext tail. */
  if (pSTSE == NULL)
  {
    return (STSE_API_HANDLER_NOT_INITIALISED);
  }

  /* Finish return value represents final authenticated-decrypt status. */
  return stsafea_aes_gcm_decrypt_finish(
           pSTSE,
           authentication_tag_length,
           associated_data_chunk_length,
           pAssociated_data_chunk,
           message_chunk_length,
           pEncrypted_message_chunk,
           pAuthentication_tag,
           pVerification_result,
           pPlaintext_message_chunk);
#else
  /* Final fallback branch matches all prior wrappers for uniform semantics. */
  return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
