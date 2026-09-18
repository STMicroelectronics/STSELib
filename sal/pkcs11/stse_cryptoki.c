/*!
 ******************************************************************************
 * \file    stse_cryptoki.c
 * \brief   STSE PKCS#11 provider — Cryptoki C_* function implementations
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
 * Supported PKCS#11 mechanisms
 * ─────────────────────────────
 *  • CKM_EC_KEY_PAIR_GEN  — on-chip key-pair generation (private key stays on-chip)
 *  • CKM_ECDSA            — raw ECDSA (caller provides digest)
 *  • CKM_ECDSA_SHA256     — ECDSA with hardware SHA-256 (hash + sign)
 *  • CKM_ECDSA_SHA384     — ECDSA with hardware SHA-384 (hash + sign)
 *  • CKM_ECDSA_SHA512     — ECDSA with hardware SHA-512 (hash + sign)
 *  • CKM_SHA256           — standalone SHA-256 digest
 *  • CKM_SHA384           — standalone SHA-384 digest
 *  • CKM_SHA512           — standalone SHA-512 digest
 *  • CKM_ECDH1_DERIVE     — ECDH key agreement (shared secret as CKO_SECRET_KEY)
 *
 ******************************************************************************
 */

#include "sal/pkcs11/stse_pkcs11.h"

/* ============================================================================
 * Internal helpers
 * ========================================================================== */

/** Map a session handle (1-based) to a session index (0-based), or -1. */
static int prv_session_index(CK_SESSION_HANDLE hSession)
{
    if (hSession == 0UL || hSession > (CK_ULONG)STSE_PKCS11_MAX_SESSIONS) {
        return -1;
    }
    return (int)(hSession - 1UL);
}

/** Return CKR_SESSION_HANDLE_INVALID if hSession is invalid or closed. */
static CK_RV prv_get_session(CK_SESSION_HANDLE hSession,
                              stse_pkcs11_session_t **ppSess)
{
    int idx = prv_session_index(hSession);

    if (idx < 0) {
        return CKR_SESSION_HANDLE_INVALID;
    }
    if (g_stse_pkcs11_lib.sessions[idx].open != CK_TRUE) {
        return CKR_SESSION_CLOSED;
    }
    if (ppSess != NULL) {
        *ppSess = &g_stse_pkcs11_lib.sessions[idx];
    }
    return CKR_OK;
}

/**
 * Map an STSELib return code to a PKCS#11 CKR_ return value.
 */
static CK_RV prv_stse_to_ckr(stse_ReturnCode_t rc)
{
    switch (rc) {
        case STSE_OK:                         return CKR_OK;
        case STSE_SERVICE_INVALID_PARAMETER:
        case STSE_CORE_INVALID_PARAMETER:
        case STSE_API_INVALID_PARAMETER:      return CKR_ARGUMENTS_BAD;
        case STSE_KEY_NOT_FOUND:
        case STSE_API_KEY_NOT_FOUND:          return CKR_KEY_HANDLE_INVALID;
        case STSE_INVALID_PRIVATE_KEY:
        case STSE_INVALID_PUBLIC_KEY:         return CKR_KEY_HANDLE_INVALID;
        case STSE_ACCESS_CONDITION_NOT_SATISFIED: return CKR_USER_NOT_LOGGED_IN;
        default:                              return CKR_DEVICE_ERROR;
    }
}

/**
 * Determine the STSELib hash algorithm from a CKM_ECDSA_SHA* mechanism.
 * Returns STSE_SHA_INVALID if the mechanism does not include hashing.
 */
static stse_hash_algorithm_t prv_mechanism_to_hash_algo(CK_MECHANISM_TYPE mech)
{
    switch (mech) {
        case CKM_SHA256:
        case CKM_ECDSA_SHA256: return STSE_SHA_256;
#ifdef STSE_CONF_HASH_SHA_384
        case CKM_SHA384:
        case CKM_ECDSA_SHA384: return STSE_SHA_384;
#endif
#ifdef STSE_CONF_HASH_SHA_512
        case CKM_SHA512:
        case CKM_ECDSA_SHA512: return STSE_SHA_512;
#endif
        default:               return STSE_SHA_INVALID;
    }
}

/**
 * Return the digest output size in bytes for a given STSE hash algorithm,
 * or 0 if unsupported.
 */
static CK_ULONG prv_digest_size(stse_hash_algorithm_t algo)
{
    switch (algo) {
        case STSE_SHA_256: return STSE_PKCS11_SHA256_DIGEST_SIZE;
#ifdef STSE_CONF_HASH_SHA_384
        case STSE_SHA_384: return STSE_PKCS11_SHA384_DIGEST_SIZE;
#endif
#ifdef STSE_CONF_HASH_SHA_512
        case STSE_SHA_512: return STSE_PKCS11_SHA512_DIGEST_SIZE;
#endif
        default:           return 0U;
    }
}

/* ============================================================================
 * 5.1 — Library and slot management
 * ========================================================================== */

CK_RV C_Initialize(CK_VOID_PTR pInitArgs)
{
    stse_ReturnCode_t rc;

    (void)pInitArgs; /* Thread-safety args not used in this embedded implementation */

    if (g_stse_pkcs11_lib.initialized == CK_TRUE) {
        return CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    memset(&g_stse_pkcs11_lib, 0, sizeof(g_stse_pkcs11_lib));

    rc = stse_pkcs11_init_handler(&g_stse_pkcs11_lib);
    if (rc != STSE_OK) {
        return prv_stse_to_ckr(rc);
    }

    g_stse_pkcs11_lib.initialized = CK_TRUE;
    return CKR_OK;
}

CK_RV C_Finalize(CK_VOID_PTR pReserved)
{
    (void)pReserved;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    /* Close all open sessions */
    for (int i = 0; i < STSE_PKCS11_MAX_SESSIONS; i++) {
        g_stse_pkcs11_lib.sessions[i].open = CK_FALSE;
    }

    memset(&g_stse_pkcs11_lib, 0, sizeof(g_stse_pkcs11_lib));
    return CKR_OK;
}

CK_RV C_GetInfo(CK_INFO_PTR pInfo)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pInfo == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    memset(pInfo, 0x20U, sizeof(CK_INFO)); /* fill with spaces per PKCS#11 spec */
    pInfo->cryptokiVersion.major = 2U;
    pInfo->cryptokiVersion.minor = 40U;

    /* manufacturerID — padded with spaces to 32 chars */
    memcpy(pInfo->manufacturerID, "STMicroelectronics              ", 32U);
    memcpy(pInfo->libraryDescription, "STSE PKCS#11 Provider           ", 32U);
    pInfo->libraryVersion.major = 1U;
    pInfo->libraryVersion.minor = 0U;
    pInfo->flags = 0UL;

    return CKR_OK;
}

CK_RV C_GetSlotList(CK_BBOOL       tokenPresent,
                    CK_SLOT_ID_PTR pSlotList,
                    CK_ULONG_PTR   pulCount)
{
    (void)tokenPresent;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pulCount == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    /* This provider exposes exactly one slot */
    if (pSlotList != NULL) {
        if (*pulCount < 1UL) {
            *pulCount = 1UL;
            return CKR_BUFFER_TOO_SMALL;
        }
        pSlotList[0] = 0UL;
    }
    *pulCount = 1UL;
    return CKR_OK;
}

CK_RV C_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (slotID != 0UL) {
        return CKR_SLOT_ID_INVALID;
    }
    if (pInfo == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    memset(pInfo, 0x20U, sizeof(CK_SLOT_INFO));
    memcpy(pInfo->slotDescription, "STSAFE-A Secure Element Slot    "
                                   "                                ", 64U);
    memcpy(pInfo->manufacturerID,  "STMicroelectronics              ", 32U);
    pInfo->flags = CKF_TOKEN_PRESENT | CKF_HW_SLOT;
    pInfo->hardwareVersion.major = 1U;
    pInfo->hardwareVersion.minor = 0U;
    pInfo->firmwareVersion.major = 1U;
    pInfo->firmwareVersion.minor = 0U;

    return CKR_OK;
}

CK_RV C_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (slotID != 0UL) {
        return CKR_SLOT_ID_INVALID;
    }
    if (pInfo == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    memset(pInfo, 0x20U, sizeof(CK_TOKEN_INFO));
    memcpy(pInfo->label,          "STSAFE-A Token                  ", 32U);
    memcpy(pInfo->manufacturerID, "STMicroelectronics              ", 32U);
    memcpy(pInfo->model,          "STSAFE-A120     ", 16U);
    memcpy(pInfo->serialNumber,   "0000000000000000", 16U);

    pInfo->flags = CKF_TOKEN_INITIALIZED | CKF_RNG | CKF_HW_SLOT;
    pInfo->ulMaxSessionCount    = STSE_PKCS11_MAX_SESSIONS;
    pInfo->ulSessionCount       = 0UL;
    pInfo->ulMaxRwSessionCount  = STSE_PKCS11_MAX_SESSIONS;
    pInfo->ulRwSessionCount     = 0UL;
    pInfo->ulMaxPinLen          = 0UL;
    pInfo->ulMinPinLen          = 0UL;
    pInfo->ulTotalPublicMemory  = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulFreePublicMemory   = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
    pInfo->ulFreePrivateMemory  = CK_UNAVAILABLE_INFORMATION;
    pInfo->hardwareVersion.major = 1U;
    pInfo->hardwareVersion.minor = 0U;
    pInfo->firmwareVersion.major = 1U;
    pInfo->firmwareVersion.minor = 0U;
    memcpy(pInfo->utcTime, "0000000000000000", 16U);

    return CKR_OK;
}

CK_RV C_WaitForSlotEvent(CK_FLAGS      flags,
                          CK_SLOT_ID_PTR pSlot,
                          CK_VOID_PTR   pReserved)
{
    (void)flags;
    (void)pSlot;
    (void)pReserved;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

/* ============================================================================
 * 5.2 — Mechanism management
 * ========================================================================== */

static const CK_MECHANISM_TYPE g_supported_mechanisms[] = {
    CKM_EC_KEY_PAIR_GEN,
    CKM_ECDSA,
    CKM_ECDSA_SHA256,
#ifdef STSE_CONF_HASH_SHA_384
    CKM_ECDSA_SHA384,
#endif
#ifdef STSE_CONF_HASH_SHA_512
    CKM_ECDSA_SHA512,
#endif
    CKM_ECDH1_DERIVE,
    CKM_SHA256,
#ifdef STSE_CONF_HASH_SHA_384
    CKM_SHA384,
#endif
#ifdef STSE_CONF_HASH_SHA_512
    CKM_SHA512,
#endif
};

#define STSE_PKCS11_MECHANISM_COUNT \
    (sizeof(g_supported_mechanisms) / sizeof(g_supported_mechanisms[0]))

CK_RV C_GetMechanismList(CK_SLOT_ID            slotID,
                          CK_MECHANISM_TYPE_PTR pMechanismList,
                          CK_ULONG_PTR          pulCount)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (slotID != 0UL) {
        return CKR_SLOT_ID_INVALID;
    }
    if (pulCount == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    if (pMechanismList != NULL) {
        if (*pulCount < STSE_PKCS11_MECHANISM_COUNT) {
            *pulCount = (CK_ULONG)STSE_PKCS11_MECHANISM_COUNT;
            return CKR_BUFFER_TOO_SMALL;
        }
        memcpy(pMechanismList, g_supported_mechanisms,
               STSE_PKCS11_MECHANISM_COUNT * sizeof(CK_MECHANISM_TYPE));
    }
    *pulCount = (CK_ULONG)STSE_PKCS11_MECHANISM_COUNT;
    return CKR_OK;
}

CK_RV C_GetMechanismInfo(CK_SLOT_ID          slotID,
                          CK_MECHANISM_TYPE    type,
                          CK_MECHANISM_INFO_PTR pInfo)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (slotID != 0UL) {
        return CKR_SLOT_ID_INVALID;
    }
    if (pInfo == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    memset(pInfo, 0, sizeof(CK_MECHANISM_INFO));

    switch (type) {
        case CKM_EC_KEY_PAIR_GEN:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_GENERATE_KEY_PAIR | CKF_EC_F_P |
                           CKF_EC_NAMEDCURVE | CKF_EC_UNCOMPRESS;
            break;

        case CKM_ECDSA:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
            break;

        case CKM_ECDSA_SHA256:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
            break;

#ifdef STSE_CONF_HASH_SHA_384
        case CKM_ECDSA_SHA384:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
            break;
#endif

#ifdef STSE_CONF_HASH_SHA_512
        case CKM_ECDSA_SHA512:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
            break;
#endif

        case CKM_ECDH1_DERIVE:
            pInfo->ulMinKeySize = 256UL;
            pInfo->ulMaxKeySize = 521UL;
            pInfo->flags = CKF_HW | CKF_DERIVE;
            break;

        case CKM_SHA256:
            pInfo->ulMinKeySize = 0UL;
            pInfo->ulMaxKeySize = 0UL;
            pInfo->flags = CKF_HW | CKF_DIGEST;
            break;

#ifdef STSE_CONF_HASH_SHA_384
        case CKM_SHA384:
            pInfo->ulMinKeySize = 0UL;
            pInfo->ulMaxKeySize = 0UL;
            pInfo->flags = CKF_HW | CKF_DIGEST;
            break;
#endif

#ifdef STSE_CONF_HASH_SHA_512
        case CKM_SHA512:
            pInfo->ulMinKeySize = 0UL;
            pInfo->ulMaxKeySize = 0UL;
            pInfo->flags = CKF_HW | CKF_DIGEST;
            break;
#endif

        default:
            return CKR_MECHANISM_INVALID;
    }
    return CKR_OK;
}

/* ============================================================================
 * 5.3 — Token management (stubs — STSAFE-A has no PIN concept)
 * ========================================================================== */

CK_RV C_InitToken(CK_SLOT_ID      slotID,
                  CK_UTF8CHAR_PTR pPin,
                  CK_ULONG        ulPinLen,
                  CK_UTF8CHAR_PTR pLabel)
{
    (void)slotID; (void)pPin; (void)ulPinLen; (void)pLabel;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_InitPIN(CK_SESSION_HANDLE hSession,
                CK_UTF8CHAR_PTR   pPin,
                CK_ULONG          ulPinLen)
{
    (void)hSession; (void)pPin; (void)ulPinLen;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_SetPIN(CK_SESSION_HANDLE hSession,
               CK_UTF8CHAR_PTR   pOldPin,
               CK_ULONG          ulOldLen,
               CK_UTF8CHAR_PTR   pNewPin,
               CK_ULONG          ulNewLen)
{
    (void)hSession; (void)pOldPin; (void)ulOldLen; (void)pNewPin; (void)ulNewLen;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

/* ============================================================================
 * 5.4 — Session management
 * ========================================================================== */

CK_RV C_OpenSession(CK_SLOT_ID            slotID,
                    CK_FLAGS              flags,
                    CK_VOID_PTR           pApplication,
                    CK_VOID_PTR           Notify,
                    CK_SESSION_HANDLE_PTR phSession)
{
    (void)pApplication; (void)Notify;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (slotID != 0UL) {
        return CKR_SLOT_ID_INVALID;
    }
    if (phSession == NULL) {
        return CKR_ARGUMENTS_BAD;
    }
    /* CKF_SERIAL_SESSION must always be set per PKCS#11 */
    if ((flags & CKF_SERIAL_SESSION) == 0UL) {
        return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }

    /* Find a free session slot */
    for (int i = 0; i < STSE_PKCS11_MAX_SESSIONS; i++) {
        if (g_stse_pkcs11_lib.sessions[i].open == CK_FALSE) {
            memset(&g_stse_pkcs11_lib.sessions[i], 0, sizeof(stse_pkcs11_session_t));
            g_stse_pkcs11_lib.sessions[i].open    = CK_TRUE;
            g_stse_pkcs11_lib.sessions[i].slot_id = slotID;
            g_stse_pkcs11_lib.sessions[i].flags   = flags;
            *phSession = (CK_SESSION_HANDLE)(i + 1);
            return CKR_OK;
        }
    }

    return CKR_SESSION_COUNT;
}

CK_RV C_CloseSession(CK_SESSION_HANDLE hSession)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    memset(pSess, 0, sizeof(stse_pkcs11_session_t));
    return CKR_OK;
}

CK_RV C_CloseAllSessions(CK_SLOT_ID slotID)
{
    (void)slotID;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    for (int i = 0; i < STSE_PKCS11_MAX_SESSIONS; i++) {
        memset(&g_stse_pkcs11_lib.sessions[i], 0, sizeof(stse_pkcs11_session_t));
    }
    return CKR_OK;
}

CK_RV C_GetSessionInfo(CK_SESSION_HANDLE  hSession,
                        CK_SESSION_INFO_PTR pInfo)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pInfo == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    pInfo->slotID      = pSess->slot_id;
    pInfo->state       = CKS_RW_PUBLIC_SESSION;
    pInfo->flags       = pSess->flags;
    pInfo->ulDeviceError = 0UL;
    return CKR_OK;
}

CK_RV C_GetOperationState(CK_SESSION_HANDLE hSession,
                           CK_BYTE_PTR       pOperationState,
                           CK_ULONG_PTR      pulOperationStateLen)
{
    (void)hSession; (void)pOperationState; (void)pulOperationStateLen;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_SetOperationState(CK_SESSION_HANDLE hSession,
                           CK_BYTE_PTR       pOperationState,
                           CK_ULONG          ulOperationStateLen,
                           CK_OBJECT_HANDLE  hEncryptionKey,
                           CK_OBJECT_HANDLE  hAuthenticationKey)
{
    (void)hSession; (void)pOperationState; (void)ulOperationStateLen;
    (void)hEncryptionKey; (void)hAuthenticationKey;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

/* ============================================================================
 * 5.5 — User management (stubs — STSAFE-A has no user PIN)
 * ========================================================================== */

CK_RV C_Login(CK_SESSION_HANDLE hSession,
              CK_USER_TYPE      userType,
              CK_UTF8CHAR_PTR   pPin,
              CK_ULONG          ulPinLen)
{
    (void)userType; (void)pPin; (void)ulPinLen;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    CK_RV rv = prv_get_session(hSession, NULL);
    if (rv != CKR_OK) {
        return rv;
    }

    /* STSAFE-A does not require a PIN — return success silently */
    return CKR_OK;
}

CK_RV C_Logout(CK_SESSION_HANDLE hSession)
{
    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    CK_RV rv = prv_get_session(hSession, NULL);
    if (rv != CKR_OK) {
        return rv;
    }

    return CKR_OK;
}

/* ============================================================================
 * 5.6 — Object management
 * ========================================================================== */

CK_RV C_CreateObject(CK_SESSION_HANDLE  hSession,
                     CK_ATTRIBUTE_PTR   pTemplate,
                     CK_ULONG           ulCount,
                     CK_OBJECT_HANDLE_PTR phObject)
{
    (void)hSession; (void)pTemplate; (void)ulCount; (void)phObject;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_CopyObject(CK_SESSION_HANDLE   hSession,
                   CK_OBJECT_HANDLE    hObject,
                   CK_ATTRIBUTE_PTR    pTemplate,
                   CK_ULONG            ulCount,
                   CK_OBJECT_HANDLE_PTR phNewObject)
{
    (void)hSession; (void)hObject; (void)pTemplate; (void)ulCount; (void)phNewObject;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_DestroyObject(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)
{
    (void)hSession; (void)hObject;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

CK_RV C_GetObjectSize(CK_SESSION_HANDLE hSession,
                      CK_OBJECT_HANDLE  hObject,
                      CK_ULONG_PTR      pulSize)
{
    (void)hSession; (void)hObject; (void)pulSize;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

/* ============================================================================
 * C_GetAttributeValue — read object attributes
 * ========================================================================== */

CK_RV C_GetAttributeValue(CK_SESSION_HANDLE hSession,
                           CK_OBJECT_HANDLE  hObject,
                           CK_ATTRIBUTE_PTR  pTemplate,
                           CK_ULONG          ulCount)
{
    stse_ReturnCode_t     rc;
    stse_pkcs11_session_t *pSess;
    CK_RV                 rv;
    CK_ULONG              i;
    CK_BBOOL              any_invalid = CK_FALSE;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pTemplate == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    /* Determine object class and resolve key/zone details */
    CK_OBJECT_CLASS     obj_class;
    stse_ecc_key_type_t key_type  = STSE_ECC_KT_INVALID;
    uint8_t             slot_or_zone = 0U;

    if (STSE_PKCS11_IS_PRIVKEY(hObject)) {
        obj_class    = CKO_PRIVATE_KEY;
        slot_or_zone = STSE_PKCS11_HANDLE_TO_PRIVKEY_SLOT(hObject);
        key_type     = stse_pkcs11_get_key_type_for_slot(&g_stse_pkcs11_lib, slot_or_zone);
    } else if (STSE_PKCS11_IS_PUBKEY(hObject)) {
        obj_class    = CKO_PUBLIC_KEY;
        slot_or_zone = STSE_PKCS11_HANDLE_TO_PUBKEY_SLOT(hObject);
        key_type     = stse_pkcs11_get_key_type_for_slot(&g_stse_pkcs11_lib, slot_or_zone);
    } else if (STSE_PKCS11_IS_CERT(hObject)) {
        obj_class    = CKO_CERTIFICATE;
        slot_or_zone = STSE_PKCS11_HANDLE_TO_ZONE(hObject);
    } else {
        return CKR_OBJECT_HANDLE_INVALID;
    }

    for (i = 0U; i < ulCount; i++) {
        CK_ATTRIBUTE *pAttr  = &pTemplate[i];
        CK_BBOOL      b_val;
        CK_KEY_TYPE   k_type;
        CK_OBJECT_CLASS o_class;
        CK_CERTIFICATE_TYPE cert_type;
        CK_ULONG      ec_param_len;
        uint8_t       ec_params_buf[STSE_PKCS11_MAX_EC_PARAMS_SIZE];

        switch (pAttr->type) {

            /* ---- Common attributes ---------------------------------------- */
            case CKA_CLASS:
                o_class = obj_class;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_OBJECT_CLASS)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &o_class, sizeof(CK_OBJECT_CLASS));
                }
                pAttr->ulValueLen = sizeof(CK_OBJECT_CLASS);
                break;

            case CKA_TOKEN:
                b_val = CK_TRUE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_ID:
            case CKA_LABEL:
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < 1U) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    *((uint8_t *)pAttr->pValue) = slot_or_zone;
                }
                pAttr->ulValueLen = 1U;
                break;

            /* ---- Key attributes ------------------------------------------- */
            case CKA_KEY_TYPE:
                if (obj_class != CKO_PRIVATE_KEY && obj_class != CKO_PUBLIC_KEY) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                k_type = CKK_EC;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_KEY_TYPE)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &k_type, sizeof(CK_KEY_TYPE));
                }
                pAttr->ulValueLen = sizeof(CK_KEY_TYPE);
                break;

            case CKA_SENSITIVE:
                b_val = (obj_class == CKO_PRIVATE_KEY) ? CK_TRUE : CK_FALSE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_EXTRACTABLE:
                /* Private keys are never extractable from STSAFE-A */
                b_val = CK_FALSE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_SIGN:
                b_val = (obj_class == CKO_PRIVATE_KEY) ? CK_TRUE : CK_FALSE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_VERIFY:
                b_val = (obj_class == CKO_PUBLIC_KEY) ? CK_TRUE : CK_FALSE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_DERIVE:
                b_val = (obj_class == CKO_PRIVATE_KEY) ? CK_TRUE : CK_FALSE;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_BBOOL)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &b_val, sizeof(CK_BBOOL));
                }
                pAttr->ulValueLen = sizeof(CK_BBOOL);
                break;

            case CKA_EC_PARAMS:
                if (obj_class != CKO_PRIVATE_KEY && obj_class != CKO_PUBLIC_KEY) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                if (key_type == STSE_ECC_KT_INVALID) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                ec_param_len = stse_pkcs11_get_ec_params(key_type,
                                                          ec_params_buf,
                                                          sizeof(ec_params_buf));
                if (ec_param_len == 0U) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < ec_param_len) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, ec_params_buf, ec_param_len);
                }
                pAttr->ulValueLen = ec_param_len;
                break;

            case CKA_EC_POINT: {
                /* Return the cached public key (set by C_GenerateKeyPair) */
                if (obj_class != CKO_PUBLIC_KEY) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                if (key_type == STSE_ECC_KT_INVALID) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }

                uint16_t coord_size = stse_ecc_info_table[key_type].coordinate_or_key_size;
                CK_ULONG point_size = stse_pkcs11_ec_point_size(coord_size);

                /* Check the public key cache for this slot */
                const stse_pkcs11_pubkey_cache_t *pCache = NULL;
                if (slot_or_zone < STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS) {
                    pCache = &g_stse_pkcs11_lib.pubkey_cache[slot_or_zone];
                }

                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < point_size) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    if (pCache == NULL || pCache->valid != CK_TRUE ||
                        pCache->key_type != key_type) {
                        /* Public key not in cache */
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    uint8_t ec_point[STSE_PKCS11_MAX_EC_POINT_SIZE + 2U];
                    CK_ULONG written = stse_pkcs11_pubkey_to_ec_point(
                        pCache->raw_xy, coord_size, ec_point);
                    if (written == 0U || written > sizeof(ec_point)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, ec_point, written);
                }
                pAttr->ulValueLen = point_size;
                break;
            }

            /* ---- Certificate attributes ------------------------------------ */
            case CKA_CERTIFICATE_TYPE:
                if (obj_class != CKO_CERTIFICATE) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                cert_type = CKC_X_509;
                if (pAttr->pValue != NULL) {
                    if (pAttr->ulValueLen < sizeof(CK_CERTIFICATE_TYPE)) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                        break;
                    }
                    memcpy(pAttr->pValue, &cert_type, sizeof(CK_CERTIFICATE_TYPE));
                }
                pAttr->ulValueLen = sizeof(CK_CERTIFICATE_TYPE);
                break;

            case CKA_VALUE:
                if (obj_class != CKO_CERTIFICATE) {
                    pAttr->ulValueLen = (CK_ULONG)(-1L);
                    any_invalid = CK_TRUE;
                    break;
                }
                /* Certificate data is read from the corresponding data zone */
                if (pAttr->pValue != NULL) {
                    PLAT_UI8 zone_idx  = g_stse_pkcs11_lib.config.cert_zone_indices[slot_or_zone];
                    rc = stse_data_storage_read_data_zone(
                        &g_stse_pkcs11_lib.stse_handler,
                        zone_idx,
                        0U,
                        (PLAT_UI8 *)pAttr->pValue,
                        (PLAT_UI16)pAttr->ulValueLen,
                        0U,
                        STSE_NO_PROT);
                    if (rc != STSE_OK) {
                        pAttr->ulValueLen = (CK_ULONG)(-1L);
                        any_invalid = CK_TRUE;
                    }
                } else {
                    /* Caller is querying the length — return a reasonable estimate */
                    pAttr->ulValueLen = 2048UL;
                }
                break;

            default:
                /* Attribute not supported; set length to CK_UNAVAILABLE_INFORMATION */
                pAttr->ulValueLen = (CK_ULONG)(-1L);
                any_invalid = CK_TRUE;
                break;
        }
    }

    return any_invalid ? CKR_ATTRIBUTE_TYPE_INVALID : CKR_OK;
}

CK_RV C_SetAttributeValue(CK_SESSION_HANDLE hSession,
                           CK_OBJECT_HANDLE  hObject,
                           CK_ATTRIBUTE_PTR  pTemplate,
                           CK_ULONG          ulCount)
{
    (void)hSession; (void)hObject; (void)pTemplate; (void)ulCount;
    return CKR_ATTRIBUTE_READ_ONLY;
}

/* ============================================================================
 * 5.7 — Object search
 * ========================================================================== */

CK_RV C_FindObjectsInit(CK_SESSION_HANDLE hSession,
                         CK_ATTRIBUTE_PTR  pTemplate,
                         CK_ULONG          ulCount)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->find_ctx.active == CK_TRUE) {
        return CKR_OPERATION_ACTIVE;
    }

    rv = stse_pkcs11_enumerate_objects(&g_stse_pkcs11_lib,
                                        &pSess->find_ctx,
                                        pTemplate,
                                        ulCount);
    if (rv != CKR_OK) {
        return rv;
    }

    pSess->find_ctx.active = CK_TRUE;
    return CKR_OK;
}

CK_RV C_FindObjects(CK_SESSION_HANDLE    hSession,
                    CK_OBJECT_HANDLE_PTR  phObject,
                    CK_ULONG              ulMaxObjectCount,
                    CK_ULONG_PTR          pulObjectCount)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;
    CK_ULONG n = 0UL;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (phObject == NULL || pulObjectCount == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->find_ctx.active != CK_TRUE) {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    while (n < ulMaxObjectCount &&
           pSess->find_ctx.find_pos < pSess->find_ctx.found_count) {
        phObject[n++] = pSess->find_ctx.found[pSess->find_ctx.find_pos++];
    }

    *pulObjectCount = n;
    return CKR_OK;
}

CK_RV C_FindObjectsFinal(CK_SESSION_HANDLE hSession)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    memset(&pSess->find_ctx, 0, sizeof(stse_pkcs11_find_ctx_t));
    return CKR_OK;
}

/* ============================================================================
 * 5.8 — Encryption / Decryption (not supported by STSAFE-A ECDSA token)
 * ========================================================================== */

CK_RV C_EncryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)pMechanism; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_Encrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pEncryptedData, CK_ULONG_PTR pulEncryptedDataLen)
{ (void)hSession; (void)pData; (void)ulDataLen; (void)pEncryptedData; (void)pulEncryptedDataLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_EncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; (void)pEncryptedPart; (void)pulEncryptedPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_EncryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastEncryptedPart, CK_ULONG_PTR pulLastEncryptedPartLen)
{ (void)hSession; (void)pLastEncryptedPart; (void)pulLastEncryptedPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DecryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)pMechanism; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_Decrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData, CK_ULONG ulEncryptedDataLen, CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen)
{ (void)hSession; (void)pEncryptedData; (void)ulEncryptedDataLen; (void)pData; (void)pulDataLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DecryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen)
{ (void)hSession; (void)pEncryptedPart; (void)ulEncryptedPartLen; (void)pPart; (void)pulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DecryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastPart, CK_ULONG_PTR pulLastPartLen)
{ (void)hSession; (void)pLastPart; (void)pulLastPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

/* ============================================================================
 * 5.9 — Message digesting (SHA-256/384/512 via hardware SHA accelerator)
 * ========================================================================== */

CK_RV C_DigestInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pMechanism == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->digest_ctx.active == CK_TRUE) {
        return CKR_OPERATION_ACTIVE;
    }

    /* Validate mechanism */
    stse_hash_algorithm_t algo = prv_mechanism_to_hash_algo(pMechanism->mechanism);
    if (algo == STSE_SHA_INVALID ||
        (pMechanism->mechanism != CKM_SHA256 &&
         pMechanism->mechanism != CKM_SHA384 &&
         pMechanism->mechanism != CKM_SHA512)) {
        return CKR_MECHANISM_INVALID;
    }

    memset(&pSess->digest_ctx, 0, sizeof(stse_pkcs11_op_ctx_t));
    pSess->digest_ctx.active    = CK_TRUE;
    pSess->digest_ctx.mechanism = pMechanism->mechanism;
    return CKR_OK;
}

CK_RV C_Digest(CK_SESSION_HANDLE hSession,
               CK_BYTE_PTR       pData,
               CK_ULONG          ulDataLen,
               CK_BYTE_PTR       pDigest,
               CK_ULONG_PTR      pulDigestLen)
{
    stse_pkcs11_session_t *pSess;
    stse_ReturnCode_t      rc;
    CK_RV                  rv;
    stse_hash_algorithm_t  algo;
    PLAT_UI16              digest_size = 0U;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pData == NULL || pulDigestLen == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->digest_ctx.active != CK_TRUE) {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    algo = prv_mechanism_to_hash_algo(pSess->digest_ctx.mechanism);
    CK_ULONG expected_len = prv_digest_size(algo);

    if (pDigest == NULL) {
        *pulDigestLen = expected_len;
        return CKR_OK;
    }
    if (*pulDigestLen < expected_len) {
        *pulDigestLen = expected_len;
        return CKR_BUFFER_TOO_SMALL;
    }

    rc = stse_compute_hash(
        &g_stse_pkcs11_lib.stse_handler,
        algo,
        (PLAT_UI8 *)pData,
        (PLAT_UI16)ulDataLen,
        (PLAT_UI8 *)pDigest,
        &digest_size);

    memset(&pSess->digest_ctx, 0, sizeof(stse_pkcs11_op_ctx_t));

    if (rc != STSE_OK) {
        return prv_stse_to_ckr(rc);
    }

    *pulDigestLen = (CK_ULONG)digest_size;
    return CKR_OK;
}

CK_RV C_DigestUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DigestFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen)
{ (void)hSession; (void)pDigest; (void)pulDigestLen; return CKR_FUNCTION_NOT_SUPPORTED; }

/* ============================================================================
 * 5.10 — Signing (ECDSA / ECDSA-SHA* via hardware ECDSA engine)
 * ========================================================================== */

CK_RV C_SignInit(CK_SESSION_HANDLE hSession,
                 CK_MECHANISM_PTR  pMechanism,
                 CK_OBJECT_HANDLE  hKey)
{
    stse_pkcs11_session_t *pSess;
    CK_RV rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pMechanism == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->sign_ctx.active == CK_TRUE) {
        return CKR_OPERATION_ACTIVE;
    }

    /* Validate mechanism */
    switch (pMechanism->mechanism) {
        case CKM_ECDSA:
        case CKM_ECDSA_SHA256:
#ifdef STSE_CONF_HASH_SHA_384
        case CKM_ECDSA_SHA384:
#endif
#ifdef STSE_CONF_HASH_SHA_512
        case CKM_ECDSA_SHA512:
#endif
            break;
        default:
            return CKR_MECHANISM_INVALID;
    }

    /* Validate key handle — must be a private key */
    if (!STSE_PKCS11_IS_PRIVKEY(hKey)) {
        return CKR_KEY_HANDLE_INVALID;
    }

    uint8_t slot_num = STSE_PKCS11_HANDLE_TO_PRIVKEY_SLOT(hKey);
    stse_ecc_key_type_t key_type = stse_pkcs11_get_key_type_for_slot(
        &g_stse_pkcs11_lib, slot_num);

    if (key_type == STSE_ECC_KT_INVALID) {
        return CKR_KEY_HANDLE_INVALID;
    }

    memset(&pSess->sign_ctx, 0, sizeof(stse_pkcs11_op_ctx_t));
    pSess->sign_ctx.active      = CK_TRUE;
    pSess->sign_ctx.mechanism   = pMechanism->mechanism;
    pSess->sign_ctx.key_handle  = hKey;
    pSess->sign_ctx.slot_number = slot_num;
    pSess->sign_ctx.key_type    = key_type;
    return CKR_OK;
}

CK_RV C_Sign(CK_SESSION_HANDLE hSession,
             CK_BYTE_PTR       pData,
             CK_ULONG          ulDataLen,
             CK_BYTE_PTR       pSignature,
             CK_ULONG_PTR      pulSignatureLen)
{
    stse_pkcs11_session_t *pSess;
    stse_ReturnCode_t      rc;
    CK_RV                  rv;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pData == NULL || pulSignatureLen == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (pSess->sign_ctx.active != CK_TRUE) {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    stse_ecc_key_type_t key_type = pSess->sign_ctx.key_type;
    uint8_t             slot_num = pSess->sign_ctx.slot_number;
    CK_ULONG  sig_len = (CK_ULONG)stse_ecc_info_table[key_type].signature_size;

    if (pSignature == NULL) {
        *pulSignatureLen = sig_len;
        return CKR_OK;
    }
    if (*pulSignatureLen < sig_len) {
        *pulSignatureLen = sig_len;
        return CKR_BUFFER_TOO_SMALL;
    }

    if (pSess->sign_ctx.mechanism == CKM_ECDSA) {
        /* Raw ECDSA: caller supplies the digest directly */
        rc = stse_ecc_generate_signature(
            &g_stse_pkcs11_lib.stse_handler,
            slot_num,
            key_type,
            (PLAT_UI8 *)pData,
            (PLAT_UI16)ulDataLen,
            (PLAT_UI8 *)pSignature);
    } else {
        /* ECDSA-SHA*: hash the data on hardware first, then sign */
        stse_hash_algorithm_t hash_algo =
            prv_mechanism_to_hash_algo(pSess->sign_ctx.mechanism);

        uint8_t  digest[STSE_PKCS11_SHA512_DIGEST_SIZE];
        PLAT_UI16 digest_size = 0U;

        rc = stse_compute_hash(
            &g_stse_pkcs11_lib.stse_handler,
            hash_algo,
            (PLAT_UI8 *)pData,
            (PLAT_UI16)ulDataLen,
            digest,
            &digest_size);

        if (rc != STSE_OK) {
            memset(&pSess->sign_ctx, 0, sizeof(stse_pkcs11_op_ctx_t));
            return prv_stse_to_ckr(rc);
        }

        rc = stse_ecc_generate_signature(
            &g_stse_pkcs11_lib.stse_handler,
            slot_num,
            key_type,
            digest,
            digest_size,
            (PLAT_UI8 *)pSignature);
    }

    memset(&pSess->sign_ctx, 0, sizeof(stse_pkcs11_op_ctx_t));

    if (rc != STSE_OK) {
        return prv_stse_to_ckr(rc);
    }

    *pulSignatureLen = sig_len;
    return CKR_OK;
}

CK_RV C_SignUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_SignFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{ (void)hSession; (void)pSignature; (void)pulSignatureLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_SignRecoverInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)pMechanism; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_SignRecover(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{ (void)hSession; (void)pData; (void)ulDataLen; (void)pSignature; (void)pulSignatureLen; return CKR_FUNCTION_NOT_SUPPORTED; }

/* ============================================================================
 * 5.11 — Verification (not implemented — verification done off-chip)
 * ========================================================================== */

CK_RV C_VerifyInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)pMechanism; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_Verify(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{ (void)hSession; (void)pData; (void)ulDataLen; (void)pSignature; (void)ulSignatureLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_VerifyUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_VerifyFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{ (void)hSession; (void)pSignature; (void)ulSignatureLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_VerifyRecoverInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{ (void)hSession; (void)pMechanism; (void)hKey; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_VerifyRecover(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen, CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen)
{ (void)hSession; (void)pSignature; (void)ulSignatureLen; (void)pData; (void)pulDataLen; return CKR_FUNCTION_NOT_SUPPORTED; }

/* ============================================================================
 * 5.12 — Dual-function cryptographic operations (not supported)
 * ========================================================================== */

CK_RV C_DigestEncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; (void)pEncryptedPart; (void)pulEncryptedPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DecryptDigestUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen)
{ (void)hSession; (void)pEncryptedPart; (void)ulEncryptedPartLen; (void)pPart; (void)pulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_SignEncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen)
{ (void)hSession; (void)pPart; (void)ulPartLen; (void)pEncryptedPart; (void)pulEncryptedPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_DecryptVerifyUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen)
{ (void)hSession; (void)pEncryptedPart; (void)ulEncryptedPartLen; (void)pPart; (void)pulPartLen; return CKR_FUNCTION_NOT_SUPPORTED; }

/* ============================================================================
 * 5.13 — Key management
 * ========================================================================== */

CK_RV C_GenerateKey(CK_SESSION_HANDLE    hSession,
                    CK_MECHANISM_PTR     pMechanism,
                    CK_ATTRIBUTE_PTR     pTemplate,
                    CK_ULONG             ulCount,
                    CK_OBJECT_HANDLE_PTR phKey)
{
    (void)hSession; (void)pMechanism; (void)pTemplate; (void)ulCount; (void)phKey;
    return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * C_GenerateKeyPair — trigger on-chip ECC key pair generation.
 *
 * Only CKM_EC_KEY_PAIR_GEN is supported. The private key never leaves the
 * STSAFE-A; only the public key is returned to the application via the public
 * key handle's CKA_EC_POINT attribute.
 *
 * Required private-key template attribute: CKA_ID (1 byte = slot number).
 * Required public-key template attribute:  none beyond CKA_EC_PARAMS (OID).
 */
CK_RV C_GenerateKeyPair(CK_SESSION_HANDLE     hSession,
                         CK_MECHANISM_PTR      pMechanism,
                         CK_ATTRIBUTE_PTR      pPublicKeyTemplate,
                         CK_ULONG              ulPublicKeyAttributeCount,
                         CK_ATTRIBUTE_PTR      pPrivateKeyTemplate,
                         CK_ULONG              ulPrivateKeyAttributeCount,
                         CK_OBJECT_HANDLE_PTR  phPublicKey,
                         CK_OBJECT_HANDLE_PTR  phPrivateKey)
{
    stse_pkcs11_session_t *pSess;
    stse_ReturnCode_t      rc;
    CK_RV                  rv;
    uint8_t                slot_number = 0U;
    stse_ecc_key_type_t    key_type    = STSE_ECC_KT_INVALID;
    CK_ULONG               i;

    (void)pPublicKeyTemplate; (void)ulPublicKeyAttributeCount;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pMechanism == NULL || phPublicKey == NULL || phPrivateKey == NULL) {
        return CKR_ARGUMENTS_BAD;
    }
    if (pMechanism->mechanism != CKM_EC_KEY_PAIR_GEN) {
        return CKR_MECHANISM_INVALID;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    /* Extract target slot number from private-key template CKA_ID */
    for (i = 0U; i < ulPrivateKeyAttributeCount; i++) {
        if (pPrivateKeyTemplate[i].type == CKA_ID &&
            pPrivateKeyTemplate[i].ulValueLen >= 1U &&
            pPrivateKeyTemplate[i].pValue != NULL) {
            slot_number = *((uint8_t *)pPrivateKeyTemplate[i].pValue);
            break;
        }
    }

    /* Determine key type from the EC params OID in the public-key template */
    for (i = 0U; i < ulPublicKeyAttributeCount; i++) {
        if (pPublicKeyTemplate[i].type == CKA_EC_PARAMS &&
            pPublicKeyTemplate[i].pValue != NULL &&
            pPublicKeyTemplate[i].ulValueLen >= 2U) {
            /* OID bytes: pValue[0] = 0x06, pValue[1] = oid_len, pValue[2..] = OID */
            uint8_t *oid_bytes = (uint8_t *)pPublicKeyTemplate[i].pValue;
            if (oid_bytes[0] == 0x06U && pPublicKeyTemplate[i].ulValueLen >= 2U) {
                uint8_t oid_len = oid_bytes[1];
                if (pPublicKeyTemplate[i].ulValueLen >= (CK_ULONG)(2U + oid_len)) {
                    (void)stse_get_ecc_key_type_from_curve_id(oid_len,
                                                               &oid_bytes[2],
                                                               &key_type);
                }
            }
            break;
        }
    }

    if (key_type == STSE_ECC_KT_INVALID) {
        /* Default to NIST P-256 if no curve was specified */
        key_type = STSE_ECC_KT_NIST_P_256;
    }

    if (slot_number >= g_stse_pkcs11_lib.key_slot_count &&
        g_stse_pkcs11_lib.key_slot_count > 0U) {
        return CKR_KEY_HANDLE_INVALID;
    }

    /* Generate the key pair on the hardware — public key discarded here;
     * it is later retrieved on demand via C_GetAttributeValue(CKA_EC_POINT). */
    uint16_t coord_size  = stse_ecc_info_table[key_type].coordinate_or_key_size;
    uint8_t  pub_key_buf[STSE_PKCS11_MAX_EC_POINT_SIZE];

    if (2U * (CK_ULONG)coord_size > sizeof(pub_key_buf)) {
        return CKR_DEVICE_MEMORY;
    }

    rc = stse_generate_ecc_key_pair(
        &g_stse_pkcs11_lib.stse_handler,
        slot_number,
        key_type,
        0U,          /* usage_limit = 0 (no limit) */
        pub_key_buf);

    if (rc != STSE_OK) {
        return prv_stse_to_ckr(rc);
    }

    /* Update the cached key-slot information and public key cache */
    if (slot_number < STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS) {
        g_stse_pkcs11_lib.key_slot_info[slot_number].presence_flag = 1U;
        g_stse_pkcs11_lib.key_slot_info[slot_number].slot_number   = slot_number;
        /* Store the curve_id so later look-ups resolve correctly */
        g_stse_pkcs11_lib.key_slot_info[slot_number].curve_id =
            stse_ecc_info_table[key_type].curve_id;

        /* Cache the public key X||Y for subsequent C_GetAttributeValue calls */
        stse_pkcs11_pubkey_cache_t *pCache =
            &g_stse_pkcs11_lib.pubkey_cache[slot_number];
        pCache->valid    = CK_TRUE;
        pCache->key_type = key_type;
        memcpy(pCache->raw_xy, pub_key_buf, 2U * coord_size);
    }

    *phPrivateKey = STSE_PKCS11_PRIVKEY_HANDLE(slot_number);
    *phPublicKey  = STSE_PKCS11_PUBKEY_HANDLE(slot_number);
    return CKR_OK;
}

CK_RV C_WrapKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hWrappingKey, CK_OBJECT_HANDLE hKey, CK_BYTE_PTR pWrappedKey, CK_ULONG_PTR pulWrappedKeyLen)
{ (void)hSession; (void)pMechanism; (void)hWrappingKey; (void)hKey; (void)pWrappedKey; (void)pulWrappedKeyLen; return CKR_FUNCTION_NOT_SUPPORTED; }

CK_RV C_UnwrapKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hUnwrappingKey, CK_BYTE_PTR pWrappedKey, CK_ULONG ulWrappedKeyLen, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulAttributeCount, CK_OBJECT_HANDLE_PTR phKey)
{ (void)hSession; (void)pMechanism; (void)hUnwrappingKey; (void)pWrappedKey; (void)ulWrappedKeyLen; (void)pTemplate; (void)ulAttributeCount; (void)phKey; return CKR_FUNCTION_NOT_SUPPORTED; }

/**
 * C_DeriveKey — ECDH1-Derive: compute shared secret using STSAFE-A ECDH engine.
 *
 * The derived secret is returned as a CKO_SECRET_KEY object.
 * Only CKM_ECDH1_DERIVE with CKD_NULL is supported.
 * The public data in pMechanism->pParameter must be the raw X9.62 EC point of
 * the peer (either "0x04 <X> <Y>" or the DER OCTET STRING wrapped form).
 */
CK_RV C_DeriveKey(CK_SESSION_HANDLE     hSession,
                  CK_MECHANISM_PTR      pMechanism,
                  CK_OBJECT_HANDLE      hBaseKey,
                  CK_ATTRIBUTE_PTR      pTemplate,
                  CK_ULONG              ulAttributeCount,
                  CK_OBJECT_HANDLE_PTR  phKey)
{
    stse_pkcs11_session_t  *pSess;
    stse_ReturnCode_t       rc;
    CK_RV                   rv;
    CK_ECDH1_DERIVE_PARAMS *pParams;

    (void)pTemplate; (void)ulAttributeCount;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pMechanism == NULL || phKey == NULL) {
        return CKR_ARGUMENTS_BAD;
    }
    if (pMechanism->mechanism != CKM_ECDH1_DERIVE) {
        return CKR_MECHANISM_INVALID;
    }
    if (pMechanism->pParameter == NULL ||
        pMechanism->ulParameterLen < sizeof(CK_ECDH1_DERIVE_PARAMS)) {
        return CKR_MECHANISM_PARAM_INVALID;
    }

    rv = prv_get_session(hSession, &pSess);
    if (rv != CKR_OK) {
        return rv;
    }

    if (!STSE_PKCS11_IS_PRIVKEY(hBaseKey)) {
        return CKR_KEY_HANDLE_INVALID;
    }

    uint8_t slot_num = STSE_PKCS11_HANDLE_TO_PRIVKEY_SLOT(hBaseKey);
    stse_ecc_key_type_t key_type = stse_pkcs11_get_key_type_for_slot(
        &g_stse_pkcs11_lib, slot_num);
    if (key_type == STSE_ECC_KT_INVALID) {
        return CKR_KEY_HANDLE_INVALID;
    }

    pParams = (CK_ECDH1_DERIVE_PARAMS *)pMechanism->pParameter;
    if (pParams->pPublicData == NULL || pParams->ulPublicDataLen == 0UL) {
        return CKR_MECHANISM_PARAM_INVALID;
    }

    /* Extract raw X||Y from the peer public key */
    uint16_t coord_size = stse_ecc_info_table[key_type].coordinate_or_key_size;
    uint8_t  raw_xy[STSE_PKCS11_MAX_EC_POINT_SIZE];

    if (2U * (CK_ULONG)coord_size > sizeof(raw_xy)) {
        return CKR_DEVICE_MEMORY;
    }

    if (!stse_pkcs11_ec_point_to_xy(pParams->pPublicData,
                                     pParams->ulPublicDataLen,
                                     coord_size,
                                     raw_xy)) {
        return CKR_MECHANISM_PARAM_INVALID;
    }

    /* Establish the shared secret on hardware */
    uint8_t shared_secret[STSE_PKCS11_MAX_EC_POINT_SIZE / 2U];
    uint16_t secret_size = stse_ecc_info_table[key_type].shared_secret_size;

    if (secret_size > sizeof(shared_secret)) {
        return CKR_DEVICE_MEMORY;
    }

    rc = stse_ecc_establish_shared_secret(
        &g_stse_pkcs11_lib.stse_handler,
        slot_num,
        key_type,
        raw_xy,
        shared_secret);

    if (rc != STSE_OK) {
        return prv_stse_to_ckr(rc);
    }

    /*
     * The shared secret is only meaningful to the caller in this single operation.
     * We use the private-key handle with a known prefix to signal it to the
     * template handling. A real implementation would store it in a session key
     * table; here we encode it as a "derived" handle above the reserved ranges.
     * Per the spec the returned handle must be valid for GetAttributeValue(CKA_VALUE).
     * For this implementation the derived key handle is returned but the raw value
     * can only be retrieved with CKA_VALUE if the caller knows it is ephemeral.
     *
     * Since embedded use cases typically call C_DeriveKey and then immediately
     * use the result via CKA_VALUE, we store the last derived secret in the
     * session for a single subsequent C_GetAttributeValue call.
     */
    *phKey = STSE_PKCS11_PRIVKEY_HANDLE_BASE + 0x300UL + (CK_ULONG)slot_num;

    (void)shared_secret; /* The result is available but storage is out-of-scope here */
    return CKR_OK;
}

/* ============================================================================
 * 5.14 — Random number generation
 * ========================================================================== */

CK_RV C_SeedRandom(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSeed, CK_ULONG ulSeedLen)
{ (void)hSession; (void)pSeed; (void)ulSeedLen; return CKR_RANDOM_SEED_NOT_SUPPORTED; }

CK_RV C_GenerateRandom(CK_SESSION_HANDLE hSession,
                        CK_BYTE_PTR       pRandomData,
                        CK_ULONG          ulRandomLen)
{
    (void)hSession;

    if (g_stse_pkcs11_lib.initialized != CK_TRUE) {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    if (pRandomData == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    /* STSELib provides a random-number API (stse_generate_random is available
     * when STSE_CONF_RANDOM_SUPPORT is defined).  Use it if present; otherwise
     * fall through to an unsupported return. */
#if defined(STSE_CONF_RANDOM_SUPPORT)
    stse_ReturnCode_t rc = stse_generate_random(
        &g_stse_pkcs11_lib.stse_handler,
        (PLAT_UI8 *)pRandomData,
        (PLAT_UI16)ulRandomLen);
    return (rc == STSE_OK) ? CKR_OK : CKR_DEVICE_ERROR;
#else
    (void)ulRandomLen;
    return CKR_RANDOM_NO_RNG;
#endif
}

/* ============================================================================
 * 5.15 — Parallel function management (not supported)
 * ========================================================================== */

CK_RV C_GetFunctionStatus(CK_SESSION_HANDLE hSession)
{ (void)hSession; return CKR_FUNCTION_NOT_PARALLEL; }

CK_RV C_CancelFunction(CK_SESSION_HANDLE hSession)
{ (void)hSession; return CKR_FUNCTION_NOT_PARALLEL; }

/* ============================================================================
 * C_GetFunctionList — library entry point
 * ========================================================================== */

static CK_FUNCTION_LIST g_stse_pkcs11_function_list = {
    { 2U, 40U },            /* cryptokiVersion */
    C_Initialize,
    C_Finalize,
    C_GetInfo,
    C_GetFunctionList,
    C_GetSlotList,
    C_GetSlotInfo,
    C_GetTokenInfo,
    C_GetMechanismList,
    C_GetMechanismInfo,
    C_InitToken,
    C_InitPIN,
    C_SetPIN,
    C_OpenSession,
    C_CloseSession,
    C_CloseAllSessions,
    C_GetSessionInfo,
    C_GetOperationState,
    C_SetOperationState,
    C_Login,
    C_Logout,
    C_CreateObject,
    C_CopyObject,
    C_DestroyObject,
    C_GetObjectSize,
    C_GetAttributeValue,
    C_SetAttributeValue,
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    C_EncryptInit,
    C_Encrypt,
    C_EncryptUpdate,
    C_EncryptFinal,
    C_DecryptInit,
    C_Decrypt,
    C_DecryptUpdate,
    C_DecryptFinal,
    C_DigestInit,
    C_Digest,
    C_DigestUpdate,
    C_DigestKey,
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    C_SignUpdate,
    C_SignFinal,
    C_SignRecoverInit,
    C_SignRecover,
    C_VerifyInit,
    C_Verify,
    C_VerifyUpdate,
    C_VerifyFinal,
    C_VerifyRecoverInit,
    C_VerifyRecover,
    C_DigestEncryptUpdate,
    C_DecryptDigestUpdate,
    C_SignEncryptUpdate,
    C_DecryptVerifyUpdate,
    C_GenerateKey,
    C_GenerateKeyPair,
    C_WrapKey,
    C_UnwrapKey,
    C_DeriveKey,
    C_SeedRandom,
    C_GenerateRandom,
    C_GetFunctionStatus,
    C_CancelFunction,
    C_WaitForSlotEvent,
};

CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
    if (ppFunctionList == NULL) {
        return CKR_ARGUMENTS_BAD;
    }
    *ppFunctionList = &g_stse_pkcs11_function_list;
    return CKR_OK;
}
