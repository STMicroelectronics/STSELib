/*!
 ******************************************************************************
 * \file    stse_derive_keys.h
 * \brief   STSE Derive Keys API Layer (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2025 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************/

#ifndef STSE_DERIVE_KEYS_H
#define STSE_DERIVE_KEYS_H

#include "services/stsafea/stsafea_derive_keys.h"

/*! \defgroup stse_derive_keys STSE Derive Keys
 * \ingroup stse_api
 * \brief      STSE derive keys API set
 * \details    The derive keys API set provides high level functions to derive keys in STSE devices.
 * @{
 */

/**
 * @brief Derive a single key from a master key stored in a slot.
 * \details A wrapper for Extract+Expand. Derives a key using optional salt and context,
 * returning the result to the host buffer.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   master_slot  Slot containing the master key (IKM).
 * \param[in]   pSalt        Salt data (Optional, can be NULL).
 * \param[in]   salt_length  Length of salt.
 * \param[in]   pContext     Context/Info string (Optional, can be NULL).
 * \param[in]   context_len  Length of context.
 * \param[out]  pOutput_key  Buffer for the derived key (Pre-allocated).
 * \param[in]   key_length   Desired derived key length.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_derive_key.dox
 */
stse_ReturnCode_t stse_derive_key(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length);

/**
 * @brief Simplest HKDF derivation using a context with explicit length.
 * \details Derives a key from a master slot using the provided context.
 * Uses a default empty salt.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   master_slot  Slot containing the master key.
 * \param[in]   pContext     Context/Info data.
 * \param[in]   context_len  Length of the context data.
 * \param[out]  pOutput_key  Buffer for the derived key.
 * \param[in]   key_length   Desired derived key length.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_derive_key_simple.dox
 */
stse_ReturnCode_t stse_derive_key_simple(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length);

/**
 * @brief Perform HKDF-Extract only and store PRK in a slot.
 * \details Creates a session/context-specific PRK inside the secure element.
 * This PRK can be used for subsequent Expand operations.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   master_slot  Slot containing the master key.
 * \param[in]   pSalt        Salt data.
 * \param[in]   salt_length  Length of salt.
 * \param[out]  pPrk_slot    Output: Slot number where PRK was stored.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_key_extract.dox
 */
stse_ReturnCode_t stse_derive_key_extract(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pPrk_slot);

/**
 * @brief Perform HKDF-Expand only from an existing PRK slot.
 * \details Derives a key from a previously extracted PRK using the provided Context/Info.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   prk_slot     Slot containing the PRK (must be 32 bytes).
 * \param[in]   pContext     Context/Info data.
 * \param[in]   context_len  Length of context.
 * \param[out]  pOutput_key  Buffer for the derived key.
 * \param[in]   key_length   Desired derived key length.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_key_expand.dox
 */
stse_ReturnCode_t stse_derive_key_expand(
    stse_Handler_t *pSTSE,
    PLAT_UI8 prk_slot,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length);

/**
 * @brief Derive encryption and MAC keys for a session.
 * \details Convenience function:
 * 1. Extract PRK from master using session_id.
 * 2. Expand "ENC" -> Encryption Key.
 * 3. Expand "MAC" -> Authentication Key.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   master_slot  Slot containing master key.
 * \param[in]   session_id   Session identifier (salt).
 * \param[out]  pEnc_key     Buffer for encryption key.
 * \param[in]   enc_key_len  Length of encryption key.
 * \param[out]  pMac_key     Buffer for MAC key.
 * \param[in]   mac_key_len  Length of MAC key.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_session_keys.dox
 */
stse_ReturnCode_t stse_derive_session_keys(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI32 session_id,
    PLAT_UI8 *pEnc_key,
    PLAT_UI16 enc_key_len,
    PLAT_UI8 *pMac_key,
    PLAT_UI16 mac_key_len);

/**
 * @brief Derive a key and store it securely in a slot.
 * \details Performs Extract+Expand but routes result to an internal slot.
 * The key is therefore never exposed.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   master_slot  Slot containing master key.
 * \param[in]   pSalt        Salt data.
 * \param[in]   salt_length  Length of salt.
 * \param[in]   pContext     Context/Info data.
 * \param[in]   context_len  Length of context.
 * \param[in]   pKey_info    Configuration for the destination slot.
 * \param[out]  pOutput_slot Output: Slot number where key was created.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_key_to_slot.dox
 */
stse_ReturnCode_t stse_derive_key_to_slot(
    stse_Handler_t *pSTSE,
    PLAT_UI8 master_slot,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    stsafe_output_key_description_information_t *pKey_info,
    PLAT_UI8 *pOutput_slot);

/**
 * @brief Efficiently derive multiple keys from a PRK in one command.
 * \details Uses HKDF-Expand to generate up to 32 keys at once.
 * Note: Uses the first context string for the entire batch.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   prk_slot     Slot containing PRK.
 * \param[in]   pContexts    Array of context strings.
 * \param[in]   pContext_lens Array of context lengths.
 * \param[out]  pOutput_keys  Array of output buffers.
 * \param[in]   pKey_lengths  Array of requested key lengths.
 * \param[in]   num_keys     Number of keys to derive.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_key_expand_multiple.dox
 */
stse_ReturnCode_t stse_derive_key_expand_multiple(
    stse_Handler_t *pSTSE,
    PLAT_UI8 prk_slot,
    PLAT_UI8 **pContexts,
    PLAT_UI16 *pContext_lens,
    PLAT_UI8 **pOutput_keys,
    PLAT_UI16 *pKey_lengths,
    PLAT_UI8 num_keys);

/**
 * @brief Derive a key using raw Input Key Material (IKM).
 * \details Use when the source key is not stored in the secure element.
 * The IKM is passed in the command payload and is not stored.
 * \param[in]   pSTSE        Pointer to STSE Handler.
 * \param[in]   pIkm         Pointer to Input Key Material.
 * \param[in]   ikm_length   Length of IKM.
 * \param[in]   pSalt        Salt data.
 * \param[in]   salt_length  Length of salt.
 * \param[in]   pContext     Context/Info data.
 * \param[in]   context_len  Length of context.
 * \param[out]  pOutput_key  Buffer for derived key.
 * \param[in]   key_length   Length of derived key.
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise.
 * \details 	\include{doc} stse_derive_key_from_ikm.dox
 */
stse_ReturnCode_t stse_derive_key_from_ikm(
    stse_Handler_t *pSTSE,
    PLAT_UI8 *pIkm,
    PLAT_UI16 ikm_length,
    PLAT_UI8 *pSalt,
    PLAT_UI16 salt_length,
    PLAT_UI8 *pContext,
    PLAT_UI16 context_len,
    PLAT_UI8 *pOutput_key,
    PLAT_UI16 key_length);

    
#endif /* STSE_DERIVE_KEYS_H */

/*! @}*/