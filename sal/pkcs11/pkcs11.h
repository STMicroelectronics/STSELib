/*!
 ******************************************************************************
 * \file    pkcs11.h
 * \brief   Bundled PKCS#11 v2.40 Cryptoki types, constants and function list
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
 * Based on OASIS PKCS #11 Cryptographic Token Interface Base Specification
 * Version 2.40, Committee Specification 01, 14 April 2015.
 *
 ******************************************************************************
 */

#ifndef PKCS11_H
#define PKCS11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---------------------------------------------------------------------------
 * Fundamental data types
 * --------------------------------------------------------------------------*/

typedef unsigned char      CK_BYTE;
typedef unsigned char      CK_CHAR;
typedef unsigned char      CK_UTF8CHAR;
typedef unsigned char      CK_BBOOL;
typedef unsigned long int  CK_ULONG;
typedef signed long int    CK_LONG;
typedef CK_ULONG           CK_FLAGS;
typedef CK_ULONG           CK_RV;

typedef void              *CK_VOID_PTR;
typedef void             **CK_VOID_PTR_PTR;
typedef CK_BYTE           *CK_BYTE_PTR;
typedef CK_CHAR           *CK_CHAR_PTR;
typedef CK_UTF8CHAR       *CK_UTF8CHAR_PTR;
typedef CK_ULONG          *CK_ULONG_PTR;

/* Boolean values */
#define CK_TRUE     1
#define CK_FALSE    0

/* Invalid handle */
#define CK_INVALID_HANDLE   0UL

/* Special ULONG values */
#define CK_UNAVAILABLE_INFORMATION  (~0UL)
#define CK_EFFECTIVELY_INFINITE     0UL

/* ---------------------------------------------------------------------------
 * Versioning
 * --------------------------------------------------------------------------*/

typedef struct CK_VERSION {
    CK_BYTE major;
    CK_BYTE minor;
} CK_VERSION;

/* ---------------------------------------------------------------------------
 * Object handles and IDs
 * --------------------------------------------------------------------------*/

typedef CK_ULONG  CK_SLOT_ID;
typedef CK_ULONG  CK_SESSION_HANDLE;
typedef CK_ULONG  CK_OBJECT_HANDLE;
typedef CK_ULONG  CK_MECHANISM_TYPE;
typedef CK_ULONG  CK_ATTRIBUTE_TYPE;
typedef CK_ULONG  CK_OBJECT_CLASS;
typedef CK_ULONG  CK_KEY_TYPE;
typedef CK_ULONG  CK_CERTIFICATE_TYPE;
typedef CK_ULONG  CK_USER_TYPE;
typedef CK_ULONG  CK_STATE;
typedef CK_ULONG  CK_HW_FEATURE_TYPE;

typedef CK_SLOT_ID         *CK_SLOT_ID_PTR;
typedef CK_SESSION_HANDLE  *CK_SESSION_HANDLE_PTR;
typedef CK_OBJECT_HANDLE   *CK_OBJECT_HANDLE_PTR;
typedef CK_MECHANISM_TYPE  *CK_MECHANISM_TYPE_PTR;

/* ---------------------------------------------------------------------------
 * Information structures
 * --------------------------------------------------------------------------*/

typedef struct CK_INFO {
    CK_VERSION   cryptokiVersion;
    CK_UTF8CHAR  manufacturerID[32];
    CK_FLAGS     flags;
    CK_UTF8CHAR  libraryDescription[32];
    CK_VERSION   libraryVersion;
} CK_INFO;
typedef CK_INFO *CK_INFO_PTR;

typedef struct CK_SLOT_INFO {
    CK_UTF8CHAR  slotDescription[64];
    CK_UTF8CHAR  manufacturerID[32];
    CK_FLAGS     flags;
    CK_VERSION   hardwareVersion;
    CK_VERSION   firmwareVersion;
} CK_SLOT_INFO;
typedef CK_SLOT_INFO *CK_SLOT_INFO_PTR;

typedef struct CK_TOKEN_INFO {
    CK_UTF8CHAR  label[32];
    CK_UTF8CHAR  manufacturerID[32];
    CK_UTF8CHAR  model[16];
    CK_CHAR      serialNumber[16];
    CK_FLAGS     flags;
    CK_ULONG     ulMaxSessionCount;
    CK_ULONG     ulSessionCount;
    CK_ULONG     ulMaxRwSessionCount;
    CK_ULONG     ulRwSessionCount;
    CK_ULONG     ulMaxPinLen;
    CK_ULONG     ulMinPinLen;
    CK_ULONG     ulTotalPublicMemory;
    CK_ULONG     ulFreePublicMemory;
    CK_ULONG     ulTotalPrivateMemory;
    CK_ULONG     ulFreePrivateMemory;
    CK_VERSION   hardwareVersion;
    CK_VERSION   firmwareVersion;
    CK_CHAR      utcTime[16];
} CK_TOKEN_INFO;
typedef CK_TOKEN_INFO *CK_TOKEN_INFO_PTR;

typedef struct CK_SESSION_INFO {
    CK_SLOT_ID  slotID;
    CK_STATE    state;
    CK_FLAGS    flags;
    CK_ULONG    ulDeviceError;
} CK_SESSION_INFO;
typedef CK_SESSION_INFO *CK_SESSION_INFO_PTR;

typedef struct CK_ATTRIBUTE {
    CK_ATTRIBUTE_TYPE  type;
    CK_VOID_PTR        pValue;
    CK_ULONG           ulValueLen;
} CK_ATTRIBUTE;
typedef CK_ATTRIBUTE *CK_ATTRIBUTE_PTR;

typedef struct CK_MECHANISM {
    CK_MECHANISM_TYPE  mechanism;
    CK_VOID_PTR        pParameter;
    CK_ULONG           ulParameterLen;
} CK_MECHANISM;
typedef CK_MECHANISM *CK_MECHANISM_PTR;

typedef struct CK_MECHANISM_INFO {
    CK_ULONG  ulMinKeySize;
    CK_ULONG  ulMaxKeySize;
    CK_FLAGS  flags;
} CK_MECHANISM_INFO;
typedef CK_MECHANISM_INFO *CK_MECHANISM_INFO_PTR;

typedef struct CK_DATE {
    CK_CHAR  year[4];
    CK_CHAR  month[2];
    CK_CHAR  day[2];
} CK_DATE;

/* ---------------------------------------------------------------------------
 * ECDH derive parameters
 * --------------------------------------------------------------------------*/

typedef struct CK_ECDH1_DERIVE_PARAMS {
    CK_ULONG     kdf;
    CK_ULONG     ulSharedDataLen;
    CK_BYTE_PTR  pSharedData;
    CK_ULONG     ulPublicDataLen;
    CK_BYTE_PTR  pPublicData;
} CK_ECDH1_DERIVE_PARAMS;
typedef CK_ECDH1_DERIVE_PARAMS *CK_ECDH1_DERIVE_PARAMS_PTR;

/* ---------------------------------------------------------------------------
 * Flags — Slot
 * --------------------------------------------------------------------------*/

#define CKF_TOKEN_PRESENT       0x00000001UL
#define CKF_REMOVABLE_DEVICE    0x00000002UL
#define CKF_HW_SLOT             0x00000004UL

/* ---------------------------------------------------------------------------
 * Flags — Token
 * --------------------------------------------------------------------------*/

#define CKF_RNG                     0x00000001UL
#define CKF_WRITE_PROTECTED         0x00000002UL
#define CKF_LOGIN_REQUIRED          0x00000004UL
#define CKF_USER_PIN_INITIALIZED    0x00000008UL
#define CKF_RESTORE_KEY_NOT_NEEDED  0x00000020UL
#define CKF_CLOCK_ON_TOKEN          0x00000040UL
#define CKF_PROTECTED_AUTHENTICATION_PATH 0x00000100UL
#define CKF_DUAL_CRYPTO_OPERATIONS  0x00000200UL
#define CKF_TOKEN_INITIALIZED       0x00000400UL
#define CKF_SECONDARY_AUTHENTICATION 0x00000800UL
#define CKF_USER_PIN_COUNT_LOW      0x00010000UL
#define CKF_USER_PIN_FINAL_TRY      0x00020000UL
#define CKF_USER_PIN_LOCKED         0x00040000UL
#define CKF_USER_PIN_TO_BE_CHANGED  0x00080000UL
#define CKF_SO_PIN_COUNT_LOW        0x00100000UL
#define CKF_SO_PIN_FINAL_TRY        0x00200000UL
#define CKF_SO_PIN_LOCKED           0x00400000UL
#define CKF_SO_PIN_TO_BE_CHANGED    0x00800000UL

/* ---------------------------------------------------------------------------
 * Flags — Session
 * --------------------------------------------------------------------------*/

#define CKF_RW_SESSION      0x00000002UL
#define CKF_SERIAL_SESSION  0x00000004UL

/* ---------------------------------------------------------------------------
 * Flags — Mechanism
 * --------------------------------------------------------------------------*/

#define CKF_HW                  0x00000001UL
#define CKF_ENCRYPT             0x00000100UL
#define CKF_DECRYPT             0x00000200UL
#define CKF_DIGEST              0x00000400UL
#define CKF_SIGN                0x00000800UL
#define CKF_SIGN_RECOVER        0x00001000UL
#define CKF_VERIFY              0x00002000UL
#define CKF_VERIFY_RECOVER      0x00004000UL
#define CKF_GENERATE            0x00008000UL
#define CKF_GENERATE_KEY_PAIR   0x00010000UL
#define CKF_WRAP                0x00020000UL
#define CKF_UNWRAP              0x00040000UL
#define CKF_DERIVE              0x00080000UL
#define CKF_EC_F_P              0x00100000UL
#define CKF_EC_F_2M             0x00200000UL
#define CKF_EC_ECPARAMETERS     0x00400000UL
#define CKF_EC_NAMEDCURVE       0x00800000UL
#define CKF_EC_UNCOMPRESS       0x01000000UL
#define CKF_EC_COMPRESS         0x02000000UL
#define CKF_EXTENSION           0x80000000UL

/* ---------------------------------------------------------------------------
 * Object classes — CKO_
 * --------------------------------------------------------------------------*/

#define CKO_DATA                0x00000000UL
#define CKO_CERTIFICATE         0x00000001UL
#define CKO_PUBLIC_KEY          0x00000002UL
#define CKO_PRIVATE_KEY         0x00000003UL
#define CKO_SECRET_KEY          0x00000004UL
#define CKO_HW_FEATURE          0x00000005UL
#define CKO_DOMAIN_PARAMETERS   0x00000006UL
#define CKO_MECHANISM           0x00000007UL
#define CKO_OTP_KEY             0x00000008UL
#define CKO_VENDOR_DEFINED      0x80000000UL

/* ---------------------------------------------------------------------------
 * Key types — CKK_
 * --------------------------------------------------------------------------*/

#define CKK_RSA             0x00000000UL
#define CKK_DSA             0x00000001UL
#define CKK_DH              0x00000002UL
#define CKK_EC              0x00000003UL
#define CKK_GENERIC_SECRET  0x00000010UL
#define CKK_AES             0x0000001FUL
#define CKK_VENDOR_DEFINED  0x80000000UL

/* ---------------------------------------------------------------------------
 * Certificate types — CKC_
 * --------------------------------------------------------------------------*/

#define CKC_X_509           0x00000000UL
#define CKC_X_509_ATTR_CERT 0x00000001UL
#define CKC_WTLS            0x00000002UL
#define CKC_VENDOR_DEFINED  0x80000000UL

/* ---------------------------------------------------------------------------
 * Attribute types — CKA_
 * --------------------------------------------------------------------------*/

#define CKA_CLASS                   0x00000000UL
#define CKA_TOKEN                   0x00000001UL
#define CKA_PRIVATE                 0x00000002UL
#define CKA_LABEL                   0x00000003UL
#define CKA_APPLICATION             0x00000010UL
#define CKA_VALUE                   0x00000011UL
#define CKA_OBJECT_ID               0x00000012UL
#define CKA_CERTIFICATE_TYPE        0x00000080UL
#define CKA_ISSUER                  0x00000081UL
#define CKA_SERIAL_NUMBER           0x00000082UL
#define CKA_AC_ISSUER               0x00000083UL
#define CKA_OWNER                   0x00000084UL
#define CKA_ATTR_TYPES              0x00000085UL
#define CKA_TRUSTED                 0x00000086UL
#define CKA_CERTIFICATE_CATEGORY    0x00000087UL
#define CKA_JAVA_MIDP_SECURITY_DOMAIN 0x00000088UL
#define CKA_URL                     0x00000089UL
#define CKA_HASH_OF_SUBJECT_PUBLIC_KEY 0x0000008AUL
#define CKA_HASH_OF_ISSUER_PUBLIC_KEY  0x0000008BUL
#define CKA_NAME_HASH_ALGORITHM     0x0000008CUL
#define CKA_CHECK_VALUE             0x00000090UL

#define CKA_KEY_TYPE                0x00000100UL
#define CKA_SUBJECT                 0x00000101UL
#define CKA_ID                      0x00000102UL
#define CKA_SENSITIVE               0x00000103UL
#define CKA_ENCRYPT                 0x00000104UL
#define CKA_DECRYPT                 0x00000105UL
#define CKA_WRAP                    0x00000106UL
#define CKA_UNWRAP                  0x00000107UL
#define CKA_SIGN                    0x00000108UL
#define CKA_SIGN_RECOVER            0x00000109UL
#define CKA_VERIFY                  0x0000010AUL
#define CKA_VERIFY_RECOVER          0x0000010BUL
#define CKA_DERIVE                  0x0000010CUL
#define CKA_START_DATE              0x00000110UL
#define CKA_END_DATE                0x00000111UL
#define CKA_MODULUS                 0x00000120UL
#define CKA_MODULUS_BITS            0x00000121UL
#define CKA_PUBLIC_EXPONENT         0x00000122UL
#define CKA_PRIVATE_EXPONENT        0x00000123UL
#define CKA_PRIME_1                 0x00000124UL
#define CKA_PRIME_2                 0x00000125UL
#define CKA_EXPONENT_1              0x00000126UL
#define CKA_EXPONENT_2              0x00000127UL
#define CKA_COEFFICIENT             0x00000128UL
#define CKA_PUBLIC_KEY_INFO         0x00000129UL
#define CKA_PRIME                   0x00000130UL
#define CKA_SUBPRIME                0x00000131UL
#define CKA_BASE                    0x00000132UL
#define CKA_PRIME_BITS              0x00000133UL
#define CKA_SUBPRIME_BITS           0x00000134UL
#define CKA_VALUE_BITS              0x00000160UL
#define CKA_VALUE_LEN               0x00000161UL
#define CKA_EXTRACTABLE             0x00000162UL
#define CKA_LOCAL                   0x00000163UL
#define CKA_NEVER_EXTRACTABLE       0x00000164UL
#define CKA_ALWAYS_SENSITIVE        0x00000165UL
#define CKA_KEY_GEN_MECHANISM       0x00000166UL
#define CKA_MODIFIABLE              0x00000170UL
#define CKA_COPYABLE                0x00000171UL
#define CKA_DESTROYABLE             0x00000172UL
#define CKA_EC_PARAMS               0x00000180UL
#define CKA_EC_POINT                0x00000181UL
#define CKA_ALWAYS_AUTHENTICATE     0x00000202UL
#define CKA_WRAP_WITH_TRUSTED       0x00000210UL
#define CKA_VENDOR_DEFINED          0x80000000UL

/* ---------------------------------------------------------------------------
 * Mechanism types — CKM_
 * --------------------------------------------------------------------------*/

#define CKM_RSA_PKCS            0x00000001UL
#define CKM_RSA_9796            0x00000002UL
#define CKM_RSA_X_509           0x00000003UL
#define CKM_SHA1_RSA_PKCS       0x00000006UL
#define CKM_SHA256_RSA_PKCS     0x00000040UL
#define CKM_SHA384_RSA_PKCS     0x00000041UL
#define CKM_SHA512_RSA_PKCS     0x00000042UL
#define CKM_SHA1                0x00000220UL
#define CKM_SHA256              0x00000250UL
#define CKM_SHA384              0x00000260UL
#define CKM_SHA512              0x00000270UL
#define CKM_EC_KEY_PAIR_GEN     0x00001040UL
#define CKM_ECDSA               0x00001041UL
#define CKM_ECDSA_SHA1          0x00001042UL
#define CKM_ECDSA_SHA256        0x00001044UL
#define CKM_ECDSA_SHA384        0x00001045UL
#define CKM_ECDSA_SHA512        0x00001046UL
#define CKM_ECDH1_DERIVE        0x00001050UL
#define CKM_ECDH1_COFACTOR_DERIVE 0x00001051UL
#define CKM_ECMQV_DERIVE        0x00001052UL
#define CKM_VENDOR_DEFINED      0x80000000UL

/* ---------------------------------------------------------------------------
 * Key derivation functions — CKD_
 * --------------------------------------------------------------------------*/

#define CKD_NULL                0x00000001UL
#define CKD_SHA1_KDF            0x00000002UL
#define CKD_SHA256_KDF          0x00000007UL

/* ---------------------------------------------------------------------------
 * User types — CKU_
 * --------------------------------------------------------------------------*/

#define CKU_SO              0UL
#define CKU_USER            1UL
#define CKU_CONTEXT_SPECIFIC 2UL

/* ---------------------------------------------------------------------------
 * Session states — CKS_
 * --------------------------------------------------------------------------*/

#define CKS_RO_PUBLIC_SESSION   0UL
#define CKS_RO_USER_FUNCTIONS   1UL
#define CKS_RW_PUBLIC_SESSION   2UL
#define CKS_RW_USER_FUNCTIONS   3UL
#define CKS_RW_SO_FUNCTIONS     4UL

/* ---------------------------------------------------------------------------
 * Return values — CKR_
 * --------------------------------------------------------------------------*/

#define CKR_OK                              0x00000000UL
#define CKR_CANCEL                          0x00000001UL
#define CKR_HOST_MEMORY                     0x00000002UL
#define CKR_SLOT_ID_INVALID                 0x00000003UL
#define CKR_GENERAL_ERROR                   0x00000005UL
#define CKR_FUNCTION_FAILED                 0x00000006UL
#define CKR_ARGUMENTS_BAD                   0x00000007UL
#define CKR_NO_EVENT                        0x00000008UL
#define CKR_NEED_TO_CREATE_THREADS          0x00000009UL
#define CKR_CANT_LOCK                       0x0000000AUL
#define CKR_ATTRIBUTE_READ_ONLY             0x00000010UL
#define CKR_ATTRIBUTE_SENSITIVE             0x00000011UL
#define CKR_ATTRIBUTE_TYPE_INVALID          0x00000012UL
#define CKR_ATTRIBUTE_VALUE_INVALID         0x00000013UL
#define CKR_ACTION_PROHIBITED               0x0000001BUL
#define CKR_DATA_INVALID                    0x00000020UL
#define CKR_DATA_LEN_RANGE                  0x00000021UL
#define CKR_DEVICE_ERROR                    0x00000030UL
#define CKR_DEVICE_MEMORY                   0x00000031UL
#define CKR_DEVICE_REMOVED                  0x00000032UL
#define CKR_ENCRYPTED_DATA_INVALID          0x00000040UL
#define CKR_ENCRYPTED_DATA_LEN_RANGE        0x00000041UL
#define CKR_FUNCTION_CANCELED               0x00000050UL
#define CKR_FUNCTION_NOT_PARALLEL           0x00000051UL
#define CKR_FUNCTION_NOT_SUPPORTED          0x00000054UL
#define CKR_KEY_HANDLE_INVALID              0x00000060UL
#define CKR_KEY_SIZE_RANGE                  0x00000062UL
#define CKR_KEY_TYPE_INCONSISTENT           0x00000063UL
#define CKR_KEY_NOT_NEEDED                  0x00000064UL
#define CKR_KEY_CHANGED                     0x00000065UL
#define CKR_KEY_NEEDED                      0x00000066UL
#define CKR_KEY_INDIGESTIBLE                0x00000067UL
#define CKR_KEY_FUNCTION_NOT_PERMITTED      0x00000068UL
#define CKR_KEY_NOT_WRAPPABLE               0x00000069UL
#define CKR_KEY_UNEXTRACTABLE               0x0000006AUL
#define CKR_MECHANISM_INVALID               0x00000070UL
#define CKR_MECHANISM_PARAM_INVALID         0x00000071UL
#define CKR_OBJECT_HANDLE_INVALID           0x00000082UL
#define CKR_OPERATION_ACTIVE                0x00000090UL
#define CKR_OPERATION_NOT_INITIALIZED       0x00000091UL
#define CKR_PIN_INCORRECT                   0x000000A0UL
#define CKR_PIN_INVALID                     0x000000A1UL
#define CKR_PIN_LEN_RANGE                   0x000000A2UL
#define CKR_PIN_EXPIRED                     0x000000A3UL
#define CKR_PIN_LOCKED                      0x000000A4UL
#define CKR_SESSION_CLOSED                  0x000000B0UL
#define CKR_SESSION_COUNT                   0x000000B1UL
#define CKR_SESSION_HANDLE_INVALID          0x000000B3UL
#define CKR_SESSION_PARALLEL_NOT_SUPPORTED  0x000000B4UL
#define CKR_SESSION_READ_ONLY               0x000000B5UL
#define CKR_SESSION_EXISTS                  0x000000B6UL
#define CKR_SESSION_READ_ONLY_EXISTS        0x000000B7UL
#define CKR_SESSION_READ_WRITE_SO_EXISTS    0x000000B8UL
#define CKR_SIGNATURE_INVALID               0x000000C0UL
#define CKR_SIGNATURE_LEN_RANGE             0x000000C1UL
#define CKR_TEMPLATE_INCOMPLETE             0x000000D0UL
#define CKR_TEMPLATE_INCONSISTENT           0x000000D1UL
#define CKR_TOKEN_NOT_PRESENT               0x000000E0UL
#define CKR_TOKEN_NOT_RECOGNIZED            0x000000E1UL
#define CKR_TOKEN_WRITE_PROTECTED           0x000000E2UL
#define CKR_UNWRAPPING_KEY_HANDLE_INVALID   0x000000F0UL
#define CKR_UNWRAPPING_KEY_SIZE_RANGE       0x000000F1UL
#define CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT 0x000000F2UL
#define CKR_USER_ALREADY_LOGGED_IN          0x00000100UL
#define CKR_USER_NOT_LOGGED_IN              0x00000101UL
#define CKR_USER_PIN_NOT_INITIALIZED        0x00000102UL
#define CKR_USER_TYPE_INVALID               0x00000103UL
#define CKR_USER_ANOTHER_ALREADY_LOGGED_IN  0x00000104UL
#define CKR_USER_TOO_MANY_TYPES             0x00000105UL
#define CKR_WRAPPED_KEY_INVALID             0x00000110UL
#define CKR_WRAPPED_KEY_LEN_RANGE           0x00000112UL
#define CKR_WRAPPING_KEY_HANDLE_INVALID     0x00000113UL
#define CKR_WRAPPING_KEY_SIZE_RANGE         0x00000114UL
#define CKR_WRAPPING_KEY_TYPE_INCONSISTENT  0x00000115UL
#define CKR_RANDOM_SEED_NOT_SUPPORTED       0x00000120UL
#define CKR_RANDOM_NO_RNG                   0x00000121UL
#define CKR_DOMAIN_PARAMS_INVALID           0x00000130UL
#define CKR_CURVE_NOT_SUPPORTED             0x00000140UL
#define CKR_BUFFER_TOO_SMALL                0x00000150UL
#define CKR_SAVED_STATE_INVALID             0x00000160UL
#define CKR_INFORMATION_SENSITIVE           0x00000170UL
#define CKR_STATE_UNSAVEABLE                0x00000180UL
#define CKR_CRYPTOKI_NOT_INITIALIZED        0x00000190UL
#define CKR_CRYPTOKI_ALREADY_INITIALIZED    0x00000191UL
#define CKR_MUTEX_BAD                       0x000001A0UL
#define CKR_MUTEX_NOT_LOCKED                0x000001A1UL
#define CKR_NEW_PIN_MODE                    0x000001B0UL
#define CKR_NEXT_OTP                        0x000001B1UL
#define CKR_EXCEEDED_MAX_ITERATIONS         0x000001B5UL
#define CKR_FIPS_SELF_TEST_FAILED           0x000001B6UL
#define CKR_LIBRARY_LOAD_FAILED             0x000001B7UL
#define CKR_PIN_TOO_WEAK                    0x000001B8UL
#define CKR_PUBLIC_KEY_INVALID              0x000001B9UL
#define CKR_FUNCTION_REJECTED               0x00000200UL
#define CKR_VENDOR_DEFINED                  0x80000000UL

/* ---------------------------------------------------------------------------
 * CK_FUNCTION_LIST — function pointer typedefs
 * --------------------------------------------------------------------------*/

typedef CK_RV (*CK_C_Initialize)(CK_VOID_PTR);
typedef CK_RV (*CK_C_Finalize)(CK_VOID_PTR);
typedef CK_RV (*CK_C_GetInfo)(CK_INFO_PTR);
typedef CK_RV (*CK_C_GetFunctionList)(void **);
typedef CK_RV (*CK_C_GetSlotList)(CK_BBOOL, CK_SLOT_ID_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_GetSlotInfo)(CK_SLOT_ID, CK_SLOT_INFO_PTR);
typedef CK_RV (*CK_C_GetTokenInfo)(CK_SLOT_ID, CK_TOKEN_INFO_PTR);
typedef CK_RV (*CK_C_WaitForSlotEvent)(CK_FLAGS, CK_SLOT_ID_PTR, CK_VOID_PTR);
typedef CK_RV (*CK_C_GetMechanismList)(CK_SLOT_ID, CK_MECHANISM_TYPE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_GetMechanismInfo)(CK_SLOT_ID, CK_MECHANISM_TYPE, CK_MECHANISM_INFO_PTR);
typedef CK_RV (*CK_C_InitToken)(CK_SLOT_ID, CK_UTF8CHAR_PTR, CK_ULONG, CK_UTF8CHAR_PTR);
typedef CK_RV (*CK_C_InitPIN)(CK_SESSION_HANDLE, CK_UTF8CHAR_PTR, CK_ULONG);
typedef CK_RV (*CK_C_SetPIN)(CK_SESSION_HANDLE, CK_UTF8CHAR_PTR, CK_ULONG, CK_UTF8CHAR_PTR, CK_ULONG);
typedef CK_RV (*CK_C_OpenSession)(CK_SLOT_ID, CK_FLAGS, CK_VOID_PTR, CK_VOID_PTR, CK_SESSION_HANDLE_PTR);
typedef CK_RV (*CK_C_CloseSession)(CK_SESSION_HANDLE);
typedef CK_RV (*CK_C_CloseAllSessions)(CK_SLOT_ID);
typedef CK_RV (*CK_C_GetSessionInfo)(CK_SESSION_HANDLE, CK_SESSION_INFO_PTR);
typedef CK_RV (*CK_C_GetOperationState)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_SetOperationState)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_OBJECT_HANDLE, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_Login)(CK_SESSION_HANDLE, CK_USER_TYPE, CK_UTF8CHAR_PTR, CK_ULONG);
typedef CK_RV (*CK_C_Logout)(CK_SESSION_HANDLE);
typedef CK_RV (*CK_C_CreateObject)(CK_SESSION_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_CopyObject)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_DestroyObject)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_GetObjectSize)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE, CK_ULONG_PTR);
typedef CK_RV (*CK_C_GetAttributeValue)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_SetAttributeValue)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_FindObjectsInit)(CK_SESSION_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_FindObjects)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE_PTR, CK_ULONG, CK_ULONG_PTR);
typedef CK_RV (*CK_C_FindObjectsFinal)(CK_SESSION_HANDLE);
typedef CK_RV (*CK_C_EncryptInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_Encrypt)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_EncryptUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_EncryptFinal)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DecryptInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_Decrypt)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DecryptUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DecryptFinal)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DigestInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR);
typedef CK_RV (*CK_C_Digest)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DigestUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_DigestKey)(CK_SESSION_HANDLE, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_DigestFinal)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_SignInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_Sign)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_SignUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_SignFinal)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_SignRecoverInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_SignRecover)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_VerifyInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_Verify)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_VerifyUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_VerifyFinal)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_VerifyRecoverInit)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE);
typedef CK_RV (*CK_C_VerifyRecover)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DigestEncryptUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DecryptDigestUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_SignEncryptUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_DecryptVerifyUpdate)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_GenerateKey)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_GenerateKeyPair)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_ATTRIBUTE_PTR, CK_ULONG, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_WrapKey)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE, CK_OBJECT_HANDLE, CK_BYTE_PTR, CK_ULONG_PTR);
typedef CK_RV (*CK_C_UnwrapKey)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE, CK_BYTE_PTR, CK_ULONG, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_DeriveKey)(CK_SESSION_HANDLE, CK_MECHANISM_PTR, CK_OBJECT_HANDLE, CK_ATTRIBUTE_PTR, CK_ULONG, CK_OBJECT_HANDLE_PTR);
typedef CK_RV (*CK_C_SeedRandom)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_GenerateRandom)(CK_SESSION_HANDLE, CK_BYTE_PTR, CK_ULONG);
typedef CK_RV (*CK_C_GetFunctionStatus)(CK_SESSION_HANDLE);
typedef CK_RV (*CK_C_CancelFunction)(CK_SESSION_HANDLE);

/* ---------------------------------------------------------------------------
 * CK_FUNCTION_LIST
 * --------------------------------------------------------------------------*/

typedef struct CK_FUNCTION_LIST CK_FUNCTION_LIST;
typedef CK_FUNCTION_LIST       *CK_FUNCTION_LIST_PTR;
typedef CK_FUNCTION_LIST_PTR   *CK_FUNCTION_LIST_PTR_PTR;

struct CK_FUNCTION_LIST {
    CK_VERSION              version;
    CK_C_Initialize         C_Initialize;
    CK_C_Finalize           C_Finalize;
    CK_C_GetInfo            C_GetInfo;
    CK_C_GetFunctionList    C_GetFunctionList;
    CK_C_GetSlotList        C_GetSlotList;
    CK_C_GetSlotInfo        C_GetSlotInfo;
    CK_C_GetTokenInfo       C_GetTokenInfo;
    CK_C_GetMechanismList   C_GetMechanismList;
    CK_C_GetMechanismInfo   C_GetMechanismInfo;
    CK_C_InitToken          C_InitToken;
    CK_C_InitPIN            C_InitPIN;
    CK_C_SetPIN             C_SetPIN;
    CK_C_OpenSession        C_OpenSession;
    CK_C_CloseSession       C_CloseSession;
    CK_C_CloseAllSessions   C_CloseAllSessions;
    CK_C_GetSessionInfo     C_GetSessionInfo;
    CK_C_GetOperationState  C_GetOperationState;
    CK_C_SetOperationState  C_SetOperationState;
    CK_C_Login              C_Login;
    CK_C_Logout             C_Logout;
    CK_C_CreateObject       C_CreateObject;
    CK_C_CopyObject         C_CopyObject;
    CK_C_DestroyObject      C_DestroyObject;
    CK_C_GetObjectSize      C_GetObjectSize;
    CK_C_GetAttributeValue  C_GetAttributeValue;
    CK_C_SetAttributeValue  C_SetAttributeValue;
    CK_C_FindObjectsInit    C_FindObjectsInit;
    CK_C_FindObjects        C_FindObjects;
    CK_C_FindObjectsFinal   C_FindObjectsFinal;
    CK_C_EncryptInit        C_EncryptInit;
    CK_C_Encrypt            C_Encrypt;
    CK_C_EncryptUpdate      C_EncryptUpdate;
    CK_C_EncryptFinal       C_EncryptFinal;
    CK_C_DecryptInit        C_DecryptInit;
    CK_C_Decrypt            C_Decrypt;
    CK_C_DecryptUpdate      C_DecryptUpdate;
    CK_C_DecryptFinal       C_DecryptFinal;
    CK_C_DigestInit         C_DigestInit;
    CK_C_Digest             C_Digest;
    CK_C_DigestUpdate       C_DigestUpdate;
    CK_C_DigestKey          C_DigestKey;
    CK_C_DigestFinal        C_DigestFinal;
    CK_C_SignInit           C_SignInit;
    CK_C_Sign               C_Sign;
    CK_C_SignUpdate         C_SignUpdate;
    CK_C_SignFinal          C_SignFinal;
    CK_C_SignRecoverInit    C_SignRecoverInit;
    CK_C_SignRecover        C_SignRecover;
    CK_C_VerifyInit         C_VerifyInit;
    CK_C_Verify             C_Verify;
    CK_C_VerifyUpdate       C_VerifyUpdate;
    CK_C_VerifyFinal        C_VerifyFinal;
    CK_C_VerifyRecoverInit  C_VerifyRecoverInit;
    CK_C_VerifyRecover      C_VerifyRecover;
    CK_C_DigestEncryptUpdate  C_DigestEncryptUpdate;
    CK_C_DecryptDigestUpdate  C_DecryptDigestUpdate;
    CK_C_SignEncryptUpdate    C_SignEncryptUpdate;
    CK_C_DecryptVerifyUpdate  C_DecryptVerifyUpdate;
    CK_C_GenerateKey        C_GenerateKey;
    CK_C_GenerateKeyPair    C_GenerateKeyPair;
    CK_C_WrapKey            C_WrapKey;
    CK_C_UnwrapKey          C_UnwrapKey;
    CK_C_DeriveKey          C_DeriveKey;
    CK_C_SeedRandom         C_SeedRandom;
    CK_C_GenerateRandom     C_GenerateRandom;
    CK_C_GetFunctionStatus  C_GetFunctionStatus;
    CK_C_CancelFunction     C_CancelFunction;
    CK_C_WaitForSlotEvent   C_WaitForSlotEvent;
};

/* ---------------------------------------------------------------------------
 * Entry point
 * --------------------------------------------------------------------------*/

CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

#ifdef __cplusplus
}
#endif

#endif /* PKCS11_H */
