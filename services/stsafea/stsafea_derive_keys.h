/*!
 ******************************************************************************
 * \file    stsafea_derive_keys.h
 * \brief   STSAFE Middleware services for derive keys command (header)
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

#ifndef STSAFEA_DERIVE_KEYS_H
#define STSAFEA_DERIVE_KEYS_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_derive_keys STSAFE-A Derive Keys
 *  \ingroup stsafea_services
 *  @{
 */

/* Defines for bit positions in the HKDF flags byte */
#define STSAFEA_HKDF_FLAG_EXTRACT_POS  (1)
#define STSAFEA_HKDF_FLAG_EXPAND_POS   (0)

/* Key/Salt source location codes */
#define STSAFEA_KEY_SOURCE_COMMAND   0x00  /*!< Source from command data */
#define STSAFEA_KEY_SOURCE_RESPONSE  0x01  /*!< Destination to response */
#define STSAFEA_KEY_SOURCE_SYMMKEY   0x02  /*!< Source/Destination symmetric key slot */

/*! STSAFE-A AES key information structure for derive key command output key description information. Doesn't contain slot number */
typedef struct stsafe_output_key_description_information_t {
    PLAT_UI16 info_length;                                        /*!< Structure length excluding this field */
    stsafea_symmetric_key_lock_indicator_t lock_indicator;        /*!< Key lock indicator */
    stsafea_symmetric_key_t type;                                 /*!< Key type */
    stsafea_symmetric_key_operation_mode_t mode_of_operation;     /*!< Mode of operation */
    stsafea_symmetric_key_usage_t usage;                          /*!< Key usage flags */
    union {
        struct {
            PLAT_UI8 auth_tag_length;             /*!< Authentication tag length (0, 4, 8, 16) */
            PLAT_UI8 counter_offset_in_nonce : 7; /*!< Counter offset in nonce (0-9) */
            PLAT_UI8 counter_presence : 1;        /*!< Counter presence indicator */
        } CCM;
        struct {
            PLAT_UI8 min_MAC_length;              /*!< Minimum MAC verification length */
        } CMAC;
        struct {
            PLAT_UI8 auth_tag_length;             /*!< Authentication tag length (4, 8, 12-16) */
        } GCM;
        struct {
            PLAT_UI8 allow_derived_key_to_host : 1; /*!< Flag: Allow derived keys to be returned to host */
            PLAT_UI8 filler : 7;                    /*!< Reserved (0b) */
            PLAT_UI8 generic_secret_key_length;     /*!< Key length (Generic Secret type only) */
        } HKDF;
        struct {
            PLAT_UI8 min_MAC_length;                /*!< Minimum MAC verification length */
            PLAT_UI8 generic_secret_key_length;     /*!< Key length (Generic Secret type only) */
        } HMAC;
    };
} stsafe_output_key_description_information_t;

/*! STSAFE-A HKDF Input Key structure */
typedef struct stsafea_hkdf_input_key_t {
    PLAT_UI8 source; /*!< Key source: STSAFEA_KEY_SOURCE_COMMAND or STSAFEA_KEY_SOURCE_SYMMKEY */
    union {
        struct {
            stsafea_symmetric_key_operation_mode_t mode_of_operation; /*!< Must be HKDF */
            PLAT_UI16 length;                                         /*!< Key length (16-32 bytes) */
            PLAT_UI8 *data;                                           /*!< Pointer to key data */
        } command;
        struct {
            PLAT_UI8 slot_number; /*!< Slot number in symmetric key table */
        } symmkey;
    };
} stsafea_hkdf_input_key_t;

/*! STSAFE-A HKDF Salt structure */
typedef struct stsafea_hkdf_salt_t {
    PLAT_UI8 source; /*!< Salt source: STSAFEA_KEY_SOURCE_COMMAND or STSAFEA_KEY_SOURCE_SYMMKEY */
    union {
        struct {
            PLAT_UI16 length; /*!< Salt length (0 = use default zeros) */
            PLAT_UI8 *data;   /*!< Pointer to salt data */
        } command;
        struct {
            PLAT_UI8 slot_number; /*!< Slot number containing salt */
        } symmkey;
    };
} stsafea_hkdf_salt_t;

/*! STSAFE-A HKDF Info structure */
typedef struct stsafea_hkdf_info_t {
    PLAT_UI16 length;  /*!< Info length (0 = no info) */
    PLAT_UI8 *data;    /*!< Pointer to info data */
} stsafea_hkdf_info_t;

/*! STSAFE-A HKDF Output Key Map (OKM) Description */
typedef struct stsafea_hkdf_okm_description_t {
    PLAT_UI8 destination; /*!< Destination: STSAFEA_KEY_SOURCE_RESPONSE or STSAFEA_KEY_SOURCE_SYMMKEY */
    union {
        struct {
            PLAT_UI16 key_length; /*!< Length of derived key */
        } response;
        struct {
            stsafe_output_key_description_information_t *key_info; /*!< Configuration for slot storage */
        } symmkey;
    };
} stsafea_hkdf_okm_description_t;

/*! STSAFE-A HKDF Derived Key Output Entry */
typedef struct stsafea_hkdf_derived_key_output_t {
    union {
        struct {
            PLAT_UI16 length; /*!< Length of derived key data */
            PLAT_UI8 *data;   /*!< Pointer to derived key buffer */
        } response;
        struct {
            PLAT_UI8 slot_number; /*!< Slot where key was stored */
        } symmkey;
    };
} stsafea_hkdf_derived_key_output_t;

/*! STSAFE-A HKDF Output structure */
typedef struct stsafea_hkdf_output_t {
    /* For Extract-only mode (Extract=1, Expand=0) */
    PLAT_UI8 prk_slot;  /*!< Slot number where PRK is stored */
    /* For Expand mode (Expand=1) */
    stsafea_hkdf_derived_key_output_t *derived_keys;  /*!< Array of derived key results */
} stsafea_hkdf_output_t;

/**
 * @brief Derive cryptographic keys using HKDF (HMAC-based Key Derivation Function).
 *
 * @details This function implements the HKDF algorithm as specified in RFC 5869.
 * It performs the Extract step, Expand step, or both in a single command.
 * The function supports deriving multiple keys (up to 32) from a single Input Key Material (IKM).
 * Each derived key can be routed to:
 * - **Response Buffer**: Returned to the host application in plaintext.
 * - **Secure Slot**: Stored directly in an internal symmetric key slot (never exposed).
 *
 * 
 *
 * @param[in]  pSTSE        Pointer to STSE Handler.
 * @param[in]  pInput_key   Configuration for the input key (IKM). 
 * - Can be raw bytes provided in the command.
 * - Can be an existing key in a symmetric slot.
 * @param[in]  extract_flag 1 to perform HKDF-Extract, 0 to skip.
 * @param[in]  expand_flag  1 to perform HKDF-Expand, 0 to skip.
 * @param[in]  pSalt        Salt configuration (Required if extract_flag=1, ignored otherwise).
 * If length is 0, a default salt of HashLen zeros is used.
 * @param[in]  pInfo        Context/Application specific info (Optional for Expand).
 * Can be NULL or length 0 if no info is needed.
 * @param[in]  pOkm_map     Array of output key descriptors defining destination and attributes.
 * (Required if expand_flag=1).
 * @param[in]  okm_count    Number of keys to derive (1 to 32).
 * @param[out] pOutput      Pointer to the output structure.
 * - **Extract-only**: <code>pOutput->prk_slot</code> will contain the slot number where the PRK was stored.
 * - **Expand**: <code>pOutput->derived_keys</code> array is populated.
 *
 * @return @ref STSE_OK on success ; @ref stse_ReturnCode_t error code otherwise
 *
 * @note **Operation Modes:**
 * | Mode | Extract | Expand | Description |
 * | :--- | :---: | :---: | :--- |
 * | **Extract-only** | 1 | 0 | Derives a Pseudo-Random Key (PRK) from IKM + Salt. The PRK is stored in an internal slot. |
 * | **Expand-only** | 0 | 1 | Derives keys from an existing PRK (must be 32 bytes) + Info. |
 * | **Full HKDF** | 1 | 1 | Performs Extract then Expand atomically. IKM + Salt + Info -> Derived Keys. |
 *
 * @note **Memory Management:**
 * For keys returned to the host ( <code>STSAFEA_KEY_SOURCE_RESPONSE</code>), the caller **must pre-allocate** buffers
 * in <code>pOutput->derived_keys[i].response.data</code> before calling this function.
 *
 * @warning **Slot Selection Logic:**
 * When deriving to a slot, the device automatically selects the destination:
 * 1. First available **Empty** slot with <code>Derived flag = 1</code>.
 * 2. Else, first available **Unlocked** slot with <code>Derived flag = 1</code>.
 * If no suitable slot is found, returns <code>STSE_BOUNDARY_EXCEEDED</code>.
 *
 * @warning **Input Key Constraints:**
 * If the input key is read from a slot:
 * - Type must be **Generic Secret**.
 * - Usage must include **Derive**.
 * - If you request output to **Response**, the input key must have **Allow Derived Key to Host** set.
 *
 * @see stse_hkdf_derive_key_simple
 * @see stse_hkdf_derive_session_keys
 */
stse_ReturnCode_t stsafea_derive_keys(
    stse_Handler_t *pSTSE,
    stsafea_hkdf_input_key_t *pInput_key,
    PLAT_UI8 extract_flag,
    PLAT_UI8 expand_flag,
    stsafea_hkdf_salt_t *pSalt,
    stsafea_hkdf_info_t *pInfo,
    stsafea_hkdf_okm_description_t *pOkm_map,
    PLAT_UI8 okm_count,
    stsafea_hkdf_output_t *pOutput
);

#endif /* STSAFEA_DERIVE_KEYS_H */

/** \}*/