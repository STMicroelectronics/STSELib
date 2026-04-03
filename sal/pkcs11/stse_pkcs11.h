/*!
 ******************************************************************************
 * \file    stse_pkcs11.h
 * \brief   STSE PKCS#11 provider — internal types and configuration (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_PKCS11_H
#define STSE_PKCS11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#include "sal/pkcs11/pkcs11.h"
#include "stselib.h"

/*!
 * \defgroup stse_pkcs11 STSE PKCS#11 Provider
 * \brief    Standard PKCS#11 Cryptoki shim layer for STSAFE-A secure elements.
 * \details  This layer maps PKCS#11 Cryptoki calls to STSELib services,
 *           enabling STSAFE-A110/A120 to act as a plug-and-play PKCS#11 token
 *           for applications such as OpenSSL, StrongSwan, and OpenSSH.
 * @{
 */

/* ---------------------------------------------------------------------------
 * Limits and handle encoding
 * --------------------------------------------------------------------------*/

/** Maximum number of private-key slots supported */
#define STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS   8U
/** Maximum number of data zones that may hold X.509 certificates */
#define STSE_PKCS11_MAX_CERT_ZONES          8U
/** Maximum concurrent PKCS#11 sessions */
#define STSE_PKCS11_MAX_SESSIONS            1U
/** Maximum total objects (2 keys per slot + certificates) */
#define STSE_PKCS11_MAX_OBJECTS \
    ((STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS * 2U) + STSE_PKCS11_MAX_CERT_ZONES)

/** Base handle for CKO_PRIVATE_KEY objects (handle = base + slot_number) */
#define STSE_PKCS11_PRIVKEY_HANDLE_BASE     0x00000001UL
/** Base handle for CKO_PUBLIC_KEY objects (handle = base + slot_number) */
#define STSE_PKCS11_PUBKEY_HANDLE_BASE      0x00000100UL
/** Base handle for CKO_CERTIFICATE objects (handle = base + zone_index) */
#define STSE_PKCS11_CERT_HANDLE_BASE        0x00000200UL

/** Encode a private-key handle from a hardware slot number */
#define STSE_PKCS11_PRIVKEY_HANDLE(slot) \
    ((CK_OBJECT_HANDLE)(STSE_PKCS11_PRIVKEY_HANDLE_BASE + (CK_ULONG)(slot)))
/** Encode a public-key handle from a hardware slot number */
#define STSE_PKCS11_PUBKEY_HANDLE(slot) \
    ((CK_OBJECT_HANDLE)(STSE_PKCS11_PUBKEY_HANDLE_BASE + (CK_ULONG)(slot)))
/** Encode a certificate handle from a data-zone index */
#define STSE_PKCS11_CERT_HANDLE(zone) \
    ((CK_OBJECT_HANDLE)(STSE_PKCS11_CERT_HANDLE_BASE + (CK_ULONG)(zone)))

/** True if @p h is a private-key handle */
#define STSE_PKCS11_IS_PRIVKEY(h) \
    ((h) >= STSE_PKCS11_PRIVKEY_HANDLE_BASE && (h) < STSE_PKCS11_PUBKEY_HANDLE_BASE)
/** True if @p h is a public-key handle */
#define STSE_PKCS11_IS_PUBKEY(h) \
    ((h) >= STSE_PKCS11_PUBKEY_HANDLE_BASE && (h) < STSE_PKCS11_CERT_HANDLE_BASE)
/** True if @p h is a certificate handle */
#define STSE_PKCS11_IS_CERT(h) \
    ((h) >= STSE_PKCS11_CERT_HANDLE_BASE && \
     (h) < (STSE_PKCS11_CERT_HANDLE_BASE + (CK_ULONG)STSE_PKCS11_MAX_CERT_ZONES))

/** Decode the hardware slot number from a private-key handle */
#define STSE_PKCS11_HANDLE_TO_PRIVKEY_SLOT(h) \
    ((uint8_t)((h) - STSE_PKCS11_PRIVKEY_HANDLE_BASE))
/** Decode the hardware slot number from a public-key handle */
#define STSE_PKCS11_HANDLE_TO_PUBKEY_SLOT(h) \
    ((uint8_t)((h) - STSE_PKCS11_PUBKEY_HANDLE_BASE))
/** Decode the data-zone index from a certificate handle */
#define STSE_PKCS11_HANDLE_TO_ZONE(h) \
    ((uint8_t)((h) - STSE_PKCS11_CERT_HANDLE_BASE))

/* Buffer size for the largest EC point (P-521: 0x04 || 66 || 66 = 133 bytes) */
#define STSE_PKCS11_MAX_EC_POINT_SIZE   134U
/* Buffer size for the largest raw X||Y coordinates (P-521: 66 + 66 = 132 bytes) */
#define STSE_PKCS11_MAX_COORD_PAIR_SIZE 132U
/* Buffer size for the largest raw signature (P-521: 66 + 66 = 132 bytes) */
#define STSE_PKCS11_MAX_SIGNATURE_SIZE  132U
/* Output size of SHA-256 */
#define STSE_PKCS11_SHA256_DIGEST_SIZE   32U
/* Output size of SHA-384 */
#define STSE_PKCS11_SHA384_DIGEST_SIZE   48U
/* Output size of SHA-512 */
#define STSE_PKCS11_SHA512_DIGEST_SIZE   64U
/* Maximum DER OID size for EC named-curve params */
#define STSE_PKCS11_MAX_EC_PARAMS_SIZE   11U

/* ---------------------------------------------------------------------------
 * Configuration structure
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_config_t
 * \brief  Hardware layout and I2C communication parameters for the token.
 *
 * Applications should populate this structure and call
 * stse_pkcs11_set_config() before C_Initialize().
 */
typedef struct stse_pkcs11_config_t {
    uint8_t         i2c_addr;       /*!< I2C device address (7-bit, un-shifted) */
    uint8_t         bus_id;         /*!< Host I2C bus identifier */
    uint16_t        bus_speed;      /*!< I2C bus speed in kHz (e.g. 100 or 400) */
    stse_device_t   device_type;    /*!< STSAFE-A device variant (STSAFE_A110, STSAFE_A120, …) */
    uint8_t         cert_zone_count;/*!< Number of data zones that contain X.509 DER certificates */
    /*!< Array of data-zone indices holding X.509 DER certificates */
    uint8_t         cert_zone_indices[STSE_PKCS11_MAX_CERT_ZONES];
} stse_pkcs11_config_t;

/* ---------------------------------------------------------------------------
 * Internal object descriptor
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_object_t
 * \brief  Lightweight descriptor for a PKCS#11 object backed by STSAFE-A.
 */
typedef struct stse_pkcs11_object_t {
    CK_OBJECT_CLASS      obj_class;    /*!< CKO_PRIVATE_KEY, CKO_PUBLIC_KEY, or CKO_CERTIFICATE */
    CK_OBJECT_HANDLE     handle;       /*!< PKCS#11 handle assigned to this object */
    uint8_t              slot_or_zone; /*!< Private/public key slot, or data-zone index */
    stse_ecc_key_type_t  key_type;     /*!< ECC curve (meaningful for key objects only) */
    CK_BBOOL             present;      /*!< CK_TRUE if a key has been generated in this slot */
} stse_pkcs11_object_t;

/* ---------------------------------------------------------------------------
 * Cryptographic operation context
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_op_ctx_t
 * \brief  Tracks a single in-progress C_SignInit / C_DigestInit operation.
 */
typedef struct stse_pkcs11_op_ctx_t {
    CK_BBOOL             active;       /*!< CK_TRUE while an operation is in progress */
    CK_MECHANISM_TYPE    mechanism;    /*!< Active CKM_* mechanism */
    CK_OBJECT_HANDLE     key_handle;   /*!< Key handle supplied to C_SignInit */
    uint8_t              slot_number;  /*!< Resolved hardware private-key slot */
    stse_ecc_key_type_t  key_type;     /*!< Resolved ECC curve for this key */
} stse_pkcs11_op_ctx_t;

/* ---------------------------------------------------------------------------
 * C_FindObjects context
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_find_ctx_t
 * \brief  State machine for multi-call C_FindObjectsInit / C_FindObjects /
 *         C_FindObjectsFinal.
 */
typedef struct stse_pkcs11_find_ctx_t {
    CK_BBOOL         active;                                 /*!< CK_TRUE while a search is active */
    CK_OBJECT_HANDLE found[STSE_PKCS11_MAX_OBJECTS];        /*!< Handles of matching objects */
    CK_ULONG         found_count;                           /*!< Total number of matches */
    CK_ULONG         find_pos;                              /*!< Next index to return in C_FindObjects */
} stse_pkcs11_find_ctx_t;

/* ---------------------------------------------------------------------------
 * Session state
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_session_t
 * \brief  Per-session state (one session supported at a time).
 */
typedef struct stse_pkcs11_session_t {
    CK_BBOOL                 open;        /*!< CK_TRUE while the session is open */
    CK_SLOT_ID               slot_id;     /*!< PKCS#11 slot ID for this session */
    CK_FLAGS                 flags;       /*!< Session flags (CKF_RW_SESSION etc.) */
    stse_pkcs11_op_ctx_t     sign_ctx;    /*!< State of an in-progress sign operation */
    stse_pkcs11_op_ctx_t     digest_ctx;  /*!< State of an in-progress digest operation */
    stse_pkcs11_find_ctx_t   find_ctx;    /*!< State of an in-progress find operation */
} stse_pkcs11_session_t;

/* ---------------------------------------------------------------------------
 * Public key cache entry
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_pubkey_cache_t
 * \brief  Cached public key (raw X||Y) for one private-key slot.
 *
 * Populated by C_GenerateKeyPair() and used by
 * C_GetAttributeValue(CKA_EC_POINT) to avoid re-issuing a generate command.
 */
typedef struct stse_pkcs11_pubkey_cache_t {
    CK_BBOOL            valid;                              /*!< CK_TRUE if this entry is populated */
    stse_ecc_key_type_t key_type;                          /*!< Curve of the cached key */
    uint8_t             raw_xy[STSE_PKCS11_MAX_COORD_PAIR_SIZE]; /*!< X||Y coordinate bytes */
} stse_pkcs11_pubkey_cache_t;

/* ---------------------------------------------------------------------------
 * Global library state
 * --------------------------------------------------------------------------*/

/*!
 * \struct stse_pkcs11_lib_t
 * \brief  Singleton holding the full state of the PKCS#11 provider.
 */
typedef struct stse_pkcs11_lib_t {
    CK_BBOOL             initialized;   /*!< CK_TRUE after C_Initialize succeeds */
    stse_Handler_t       stse_handler;  /*!< STSELib device handler */
    stse_pkcs11_config_t config;        /*!< Active hardware configuration */
    /*!< Open sessions (only index 0 is used in this implementation) */
    stse_pkcs11_session_t sessions[STSE_PKCS11_MAX_SESSIONS];
    uint8_t              key_slot_count; /*!< Number of private-key slots discovered */
    /*!< Per-slot information returned by the device */
    stsafea_private_key_slot_information_t key_slot_info[STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS];
    /*!< Cached public keys populated on C_GenerateKeyPair */
    stse_pkcs11_pubkey_cache_t pubkey_cache[STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS];
} stse_pkcs11_lib_t;

/* ---------------------------------------------------------------------------
 * Internal API
 * --------------------------------------------------------------------------*/

/*!
 * \brief  Override the default hardware configuration before C_Initialize().
 * \param[in]  pConfig  Pointer to the caller-supplied configuration structure.
 */
void stse_pkcs11_set_config(const stse_pkcs11_config_t *pConfig);

/*!
 * \brief  Retrieve the current hardware configuration.
 * \return Pointer to the active \ref stse_pkcs11_config_t.
 */
const stse_pkcs11_config_t *stse_pkcs11_get_config(void);

/*!
 * \brief  Initialise the STSELib handler from the active configuration and
 *         query the device for its private-key table.
 * \param[in,out]  pLib  Pointer to the library state.
 * \return \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise.
 */
stse_ReturnCode_t stse_pkcs11_init_handler(stse_pkcs11_lib_t *pLib);

/*!
 * \brief  Test whether an object matches all attributes in a search template.
 * \param[in]  pObj       Object to test.
 * \param[in]  pTemplate  Array of CK_ATTRIBUTE search criteria.
 * \param[in]  ulCount    Number of attributes in the template.
 * \return CK_TRUE if every attribute matches; CK_FALSE otherwise.
 */
CK_BBOOL stse_pkcs11_object_matches_template(
    const stse_pkcs11_object_t *pObj,
    const CK_ATTRIBUTE_PTR      pTemplate,
    CK_ULONG                    ulCount);

/*!
 * \brief  Enumerate all STSAFE-A objects and populate the find context with
 *         handles whose attributes match the supplied template.
 * \param[in]  pLib       Library state (contains key-slot information).
 * \param[out] pFind      Find context to populate.
 * \param[in]  pTemplate  Search template (may be NULL to match everything).
 * \param[in]  ulCount    Number of attributes in the template.
 * \return \ref CKR_OK on success.
 */
CK_RV stse_pkcs11_enumerate_objects(
    stse_pkcs11_lib_t      *pLib,
    stse_pkcs11_find_ctx_t *pFind,
    CK_ATTRIBUTE_PTR        pTemplate,
    CK_ULONG                ulCount);

/* ---------------------------------------------------------------------------
 * Format-conversion helpers
 * --------------------------------------------------------------------------*/

/*!
 * \brief  Build the DER-encoded ANSI X9.62 EC point (CKA_EC_POINT) from the
 *         raw X||Y coordinates returned by STSELib.
 * \details
 *   The output is a DER OCTET STRING wrapping the uncompressed point:
 *   \c 0x04 0x<length> 0x04 <X[coord_size]> <Y[coord_size]>
 *
 * \param[in]  pRaw_xy     Buffer containing X then Y coordinates,
 *                         each of @p coord_size bytes.
 * \param[in]  coord_size  Size of a single coordinate in bytes.
 * \param[out] pEc_point   Output buffer; must be at least
 *                         stse_pkcs11_ec_point_size(coord_size) bytes.
 * \return Number of bytes written to @p pEc_point.
 */
CK_ULONG stse_pkcs11_pubkey_to_ec_point(
    const uint8_t *pRaw_xy,
    uint16_t       coord_size,
    uint8_t       *pEc_point);

/*!
 * \brief  Return the encoded size of the DER EC point for a given
 *         coordinate size.
 * \param[in]  coord_size  Coordinate size in bytes.
 * \return Total bytes needed for the DER OCTET STRING EC point.
 */
CK_ULONG stse_pkcs11_ec_point_size(uint16_t coord_size);

/*!
 * \brief  Extract raw X||Y coordinates from a PKCS#11 EC point parameter.
 * \details
 *   Accepts both the raw uncompressed format \c 0x04 <X> <Y> and the DER
 *   OCTET STRING wrapped format \c 0x04 <len> 0x04 <X> <Y>.
 *
 * \param[in]  pEc_point   Input EC point buffer from the application.
 * \param[in]  point_len   Length of @p pEc_point in bytes.
 * \param[in]  coord_size  Expected coordinate size in bytes.
 * \param[out] pRaw_xy     Output buffer of at least (2 * @p coord_size) bytes
 *                         that will receive X followed by Y.
 * \return CK_TRUE on success; CK_FALSE if the format is unrecognised.
 */
CK_BBOOL stse_pkcs11_ec_point_to_xy(
    const uint8_t *pEc_point,
    CK_ULONG       point_len,
    uint16_t       coord_size,
    uint8_t       *pRaw_xy);

/*!
 * \brief  Build the DER-encoded namedCurve OID (CKA_EC_PARAMS) for a given
 *         STSE ECC key type.
 * \param[in]  key_type  ECC key type.
 * \param[out] pBuf      Output buffer; must be at least
 *                       STSE_PKCS11_MAX_EC_PARAMS_SIZE bytes.
 * \param[in]  buf_size  Size of @p pBuf in bytes.
 * \return Number of bytes written, or 0 if the curve is unsupported.
 */
CK_ULONG stse_pkcs11_get_ec_params(
    stse_ecc_key_type_t key_type,
    uint8_t            *pBuf,
    CK_ULONG            buf_size);

/*!
 * \brief  Look up the ECC key type associated with a private-key slot.
 * \param[in]  pLib         Library state containing the key-slot table.
 * \param[in]  slot_number  Hardware slot number to query.
 * \return The \ref stse_ecc_key_type_t for that slot, or
 *         STSE_ECC_KT_INVALID if not found.
 */
stse_ecc_key_type_t stse_pkcs11_get_key_type_for_slot(
    const stse_pkcs11_lib_t *pLib,
    uint8_t                  slot_number);

/* ---------------------------------------------------------------------------
 * Global singleton — defined in stse_pkcs11.c
 * --------------------------------------------------------------------------*/

/** Global library state used by all C_* functions. */
extern stse_pkcs11_lib_t g_stse_pkcs11_lib;

/** @}*/

#ifdef __cplusplus
}
#endif

#endif /* STSE_PKCS11_H */
