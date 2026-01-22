# wolfSSL Platform Implementation for STSELib

This directory contains a reference implementation of the STSELib Platform Abstraction Layer (PAL) using wolfSSL/wolfCrypt as the cryptographic provider.

## Files

| File | Description |
|------|-------------|
| `stse_platform_crypto_wolfssl.c` | Complete crypto PAL implementation |
| `user_settings.h` | Sample wolfSSL configuration |

## Quick Start

1. **Copy files to your project:**
   ```bash
   cp stse_platform_crypto_wolfssl.c /your/project/platform/
   cp user_settings.h /your/project/
   ```

2. **Configure your build to include:**
   - wolfSSL library (with `WOLFSSL_USER_SETTINGS` defined)
   - STSELib headers
   - Your platform-specific I2C/delay implementations

3. **Build and link with wolfSSL:**
   ```bash
   gcc -DWOLFSSL_USER_SETTINGS -I. -I/path/to/wolfssl -I/path/to/STSELib \
       your_app.c stse_platform_crypto_wolfssl.c \
       -lwolfssl -o your_app
   ```

## Features Implemented

- ✅ SHA-256/SHA-384 hashing
- ✅ AES-ECB/CBC encryption/decryption
- ✅ AES-CMAC (streaming and one-shot)
- ✅ HMAC-SHA256 HKDF (extract/expand)
- ✅ ECC signature verification (NIST P-256/P-384, Brainpool, Ed25519)
- ✅ ECC key pair generation
- ✅ ECDSA/EdDSA signing
- ✅ ECDH key exchange (including Curve25519)
- ✅ NIST AES Key Wrap

## Requirements

### wolfSSL Configuration

Minimum required features in `user_settings.h` or configure:

```c
#define HAVE_ECC
#define HAVE_AES_CBC
#define WOLFSSL_AES_DIRECT
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
```

### Optional Features

For full curve support:
```c
#define HAVE_ECC_BRAINPOOL
#define HAVE_CURVE25519
#define HAVE_ED25519
#define HAVE_AES_KEYWRAP
```

## Testing

This implementation has been tested with:
- STSAFE-A120 on Raspberry Pi 5
- wolfSSL 5.x
- Linux I2C driver

## License

Copyright 2025 wolfSSL Inc. Licensed under GPLv2 or commercial license.
See https://www.wolfssl.com/license/ for details.
