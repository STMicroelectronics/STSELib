# wolfSSL/wolfCrypt Platform Implementation {#stse_platform_wolfssl}

The `stse_platform_crypto_wolfssl.c` file provides a complete cryptographic platform implementation for the STSecureElement library using wolfSSL/wolfCrypt as the cryptographic provider. This implementation serves as an alternative to the STM32 CMOX library for platforms using wolfSSL.

## Overview

[wolfSSL](https://www.wolfssl.com/) is a lightweight, portable, C-language-based SSL/TLS library with an embedded cryptographic library (wolfCrypt). This platform implementation enables STSELib integration on any system where wolfSSL is available, including:

- Linux/POSIX systems
- Embedded systems (ARM Cortex-M, RISC-V, etc.)
- RTOS environments (FreeRTOS, Zephyr, etc.)
- Custom microcontroller platforms

## Features Supported

| Category | Functions | wolfSSL API Used |
|----------|-----------|------------------|
| **Hash** | `stse_platform_hash_compute` | `wc_Sha256Hash`, `wc_Sha384Hash` |
| **AES-ECB** | `stse_platform_aes_ecb_enc` | `wc_AesEncryptDirect` |
| **AES-CBC** | `stse_platform_aes_cbc_enc`, `stse_platform_aes_cbc_dec` | `wc_AesCbcEncrypt`, `wc_AesCbcDecrypt` |
| **AES-CMAC** | `stse_platform_aes_cmac_*` | Manual RFC 4493 implementation using `wc_AesCbcEncrypt` |
| **HMAC/HKDF** | `stse_platform_hmac_sha256_*` | `wc_HmacSetKey`, `wc_HmacUpdate`, `wc_HmacFinal` |
| **ECC Verify** | `stse_platform_ecc_verify` | `wc_ecc_verify_hash` |
| **ECC KeyGen** | `stse_platform_ecc_generate_key_pair` | `wc_ecc_make_key_ex`, `wc_curve25519_make_key`, `wc_ed25519_make_key` |
| **ECC Sign** | `stse_platform_ecc_sign` | `wc_ecc_sign_hash`, `wc_ed25519_sign_msg` |
| **ECDH** | `stse_platform_ecc_ecdh` | `wc_ecc_shared_secret`, `wc_curve25519_shared_secret` |
| **Key Wrap** | `stse_platform_nist_kw_encrypt` | `wc_AesKeyWrap` |

## ECC Curve Support

| STSE Key Type | wolfSSL Curve ID | Notes |
|---------------|------------------|-------|
| `STSE_ECC_KT_NIST_P_256` | `ECC_SECP256R1` | Default curve |
| `STSE_ECC_KT_NIST_P_384` | `ECC_SECP384R1` | |
| `STSE_ECC_KT_NIST_P_521` | `ECC_SECP521R1` | Requires `HAVE_ECC521` |
| `STSE_ECC_KT_BP_P_256` | `ECC_BRAINPOOLP256R1` | Requires `HAVE_ECC_BRAINPOOL` |
| `STSE_ECC_KT_BP_P_384` | `ECC_BRAINPOOLP384R1` | Requires `HAVE_ECC_BRAINPOOL` |
| `STSE_ECC_KT_BP_P_512` | `ECC_BRAINPOOLP512R1` | Requires `HAVE_ECC_BRAINPOOL` |
| `STSE_ECC_KT_CURVE25519` | `ECC_X25519` | Requires `HAVE_CURVE25519` |
| `STSE_ECC_KT_ED25519` | `ECC_ED25519` | Requires `HAVE_ED25519` |

## Configuration

### wolfSSL Build Requirements

The wolfSSL library must be built with the following features enabled:

```bash
./configure --enable-ecc --enable-sha384 --enable-sha512 \
            --enable-aescbc --enable-cmac --enable-keywrap \
            --enable-curve25519 --enable-ed25519 \
            --enable-brainpool
```

Or define in `user_settings.h`:

```c
#define WOLFSSL_USER_SETTINGS

/* ECC Support */
#define HAVE_ECC
#define TFM_ECC256
#define ECC_TIMING_RESISTANT

/* Hash Support */
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512

/* AES Support */
#define HAVE_AES_CBC
#define WOLFSSL_AES_DIRECT
#define HAVE_AES_KEYWRAP

/* HMAC/HKDF Support */
#define HAVE_HKDF

/* Optional: Additional curves */
#define HAVE_ECC_BRAINPOOL
#define HAVE_CURVE25519
#define HAVE_ED25519
```

### STSE Configuration

In your `stse_conf.h`, enable the required features:

```c
/* Hash algorithms */
#define STSE_CONF_HASH_SHA_256
#define STSE_CONF_HASH_SHA_384

/* ECC curves */
#define STSE_CONF_ECC_NIST_P_256
#define STSE_CONF_ECC_NIST_P_384

/* Session/Key establishment features */
#define STSE_CONF_USE_HOST_KEY_ESTABLISHMENT
#define STSE_CONF_USE_HOST_SESSION
```

## Implementation Details

### AES-CMAC Implementation

The AES-CMAC implementation follows RFC 4493 using wolfSSL's AES-CBC primitive. Subkeys K1 and K2 are generated according to the specification, and the streaming init/append/finish API is supported for processing data in chunks.

### Signature Format Conversion

STSE uses raw R||S signature format, while wolfSSL uses DER encoding. The implementation handles conversion using:
- `wc_ecc_rs_raw_to_sig()` - Convert R||S to DER for verification
- `wc_ecc_sig_to_rs()` - Convert DER to R||S after signing

### Public Key Format

Public keys are expected in raw X||Y coordinate format (uncompressed, without the 0x04 prefix). The implementation uses `wc_ecc_import_unsigned()` and `wc_ecc_export_public_raw()` for conversion.

## Usage Example

```c
#include "stselib.h"
#include "stse_conf.h"

/* Include wolfSSL with user settings */
#define WOLFSSL_USER_SETTINGS
#include "user_settings.h"
#include <wolfssl/wolfcrypt/settings.h>

int main(void)
{
    stse_Handler_t stse_handler;
    stse_ReturnCode_t ret;

    /* Initialize STSE (calls stse_platform_crypto_init internally) */
    ret = stse_init(&stse_handler, STSAFE_A120, 1, 0x20);
    if (ret != STSE_OK) {
        return -1;
    }

    /* Now use STSE APIs - platform crypto is ready */
    /* ... */

    return 0;
}
```

## File Location

The reference implementation is located at:
- `examples/wolfssl/stse_platform_crypto_wolfssl.c`

Copy this file to your platform directory and include it in your build.

## License

The wolfSSL platform implementation is provided under the wolfSSL license. wolfSSL is dual-licensed under GPLv2 and a commercial license. For commercial use, please contact wolfSSL Inc.

## References

- [wolfSSL Documentation](https://www.wolfssl.com/documentation/)
- [wolfSSL GitHub](https://github.com/wolfSSL/wolfssl)
- [RFC 4493 - AES-CMAC](https://tools.ietf.org/html/rfc4493)
- [RFC 5869 - HKDF](https://tools.ietf.org/html/rfc5869)
- [NIST SP 800-38F - Key Wrap](https://csrc.nist.gov/publications/detail/sp/800-38f/final)
