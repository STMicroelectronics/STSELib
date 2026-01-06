/* user_settings.h
 *
 * Sample wolfSSL configuration for STSELib platform integration.
 * Customize based on your target platform and requirements.
 *
 * Copyright 2025 wolfSSL Inc.
 */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Platform Configuration
 * ============================================================================ */

/* Uncomment for specific platforms */
/* #define WOLFSSL_LINUX */
/* #define FREERTOS */
/* #define WOLFSSL_ZEPHYR */

/* ============================================================================
 * Math Library Configuration
 * ============================================================================ */

/* Use Single Precision (SP) math for ARM platforms - faster */
#if defined(__ARM_ARCH) || defined(__arm__) || defined(__aarch64__)
    #define WOLFSSL_SP
    #define WOLFSSL_SP_ARM64_ASM
    #define WOLFSSL_SP_MATH_ALL
    #define SP_INT_BITS 4096
#else
    /* Use Fast Math for other platforms */
    #define USE_FAST_MATH
    #define TFM_TIMING_RESISTANT
#endif

/* ============================================================================
 * Hash Algorithm Support
 * ============================================================================ */

/* Required for STSE */
#define WOLFSSL_SHA256
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512

/* Optional - if using SHA-1 certificates */
/* #define HAVE_SHA1 */

/* ============================================================================
 * AES Configuration
 * ============================================================================ */

/* Required for STSE session encryption */
#define HAVE_AES_CBC
#define WOLFSSL_AES_DIRECT      /* Required for ECB mode */
#define HAVE_AESGCM             /* Optional: for additional modes */

/* Required for NIST Key Wrap */
#define HAVE_AES_KEYWRAP

/* ============================================================================
 * ECC Configuration
 * ============================================================================ */

/* Core ECC support */
#define HAVE_ECC
#define ECC_TIMING_RESISTANT
#define TFM_ECC256
#define TFM_ECC384

/* NIST Curves (always recommended) */
#define HAVE_ECC256             /* P-256 - most common */
#define HAVE_ECC384             /* P-384 */
/* #define HAVE_ECC521 */       /* P-521 - uncomment if needed */

/* Brainpool Curves (optional) */
/* #define HAVE_ECC_BRAINPOOL */

/* Curve25519/Ed25519 (optional but recommended) */
#define HAVE_CURVE25519
#define HAVE_ED25519

/* ECC key import/export support */
#define HAVE_ECC_KEY_IMPORT
#define HAVE_ECC_KEY_EXPORT

/* ============================================================================
 * HMAC/HKDF Configuration
 * ============================================================================ */

/* Required for key derivation */
#define HAVE_HKDF
#define WOLFSSL_SHA256

/* ============================================================================
 * Random Number Generator
 * ============================================================================ */

/* Use system RNG (Linux /dev/urandom) */
#define HAVE_HASHDRBG

/* For embedded systems, you may need to provide custom seed:
 * #define CUSTOM_RAND_GENERATE_SEED my_seed_function
 * #define CUSTOM_RAND_GENERATE_BLOCK my_random_function
 */

/* Use STSAFE as RNG source (optional - for seeding wolfSSL RNG) */
/* #define USE_STSAFE_RNG_SEED */

/* ============================================================================
 * Memory Configuration
 * ============================================================================ */

/* For resource-constrained systems */
/* #define WOLFSSL_SMALL_STACK */
/* #define WOLFSSL_SMALL_STACK_CACHE */

/* Static memory allocation (no malloc) */
/* #define WOLFSSL_STATIC_MEMORY */

/* ============================================================================
 * Debugging (disable for production)
 * ============================================================================ */

/* #define DEBUG_WOLFSSL */
/* #define WOLFSSL_DEBUG_MEMORY */

/* ============================================================================
 * Feature Exclusions (to reduce code size)
 * ============================================================================ */

/* Disable features not needed for STSE platform support */
#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_DES3
#define NO_RABBIT
#define NO_HC128
#define NO_PSK
#define NO_PWDBASED

/* Disable SSL/TLS if only using wolfCrypt */
/* #define WOLFCRYPT_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */

