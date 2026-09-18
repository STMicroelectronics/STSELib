\page pkcs11_sal PKCS#11 SAL — Usage Guide

# PKCS#11 SAL — Usage Guide

## Overview

The STSE PKCS#11 provider layer (`sal/pkcs11/`) maps the standard PKCS#11 v2.40
Cryptoki API to the STSELib services. This allows a STSAFE‑A110 or STSAFE‑A120
device to act as a standard hardware security token for high-level applications
such as **OpenSSL**, **StrongSwan**, **OpenSSH**, and **PKCS#11-aware TLS stacks**
without any application-specific changes.

```
Application (OpenSSL / StrongSwan / OpenSSH / mbedTLS)
          │  standard PKCS#11 v2.40 Cryptoki calls  │
          ▼                                          │
  ┌─────────────────────────────────────────────────┤
  │   sal/pkcs11/stse_cryptoki.c                    │
  │   (C_Initialize, C_Sign, C_GenerateKeyPair …)   │
  ├─────────────────────────────────────────────────┤
  │   sal/pkcs11/stse_pkcs11.c                      │
  │   (object enumeration, format conversion)       │
  ├─────────────────────────────────────────────────┤
  │   STSELib Core — API / Services / Core layers   │
  └─────────────────────────────────────────────────┘
          │  I2C / ST1Wire  │
          ▼                 ▼
       STSAFE-A110 / STSAFE-A120
```

---

## Build integration

### 1. Enable the PKCS#11 provider in `stse_conf.h`

Add the following define to your project's `stse_conf.h` **alongside** the usual
STSAFE-A device and algorithm enables:

```c
/* Enable the PKCS#11 provider layer */
#define STSE_CONF_PKCS11_SUPPORT

/* The PKCS#11 layer requires at least STSAFE-A support and SHA-256 */
#define STSE_CONF_STSAFE_A_SUPPORT
#define STSE_CONF_ECC_NIST_P_256
#define STSE_CONF_HASH_SHA_256
```

Optional algorithms automatically expand the set of supported PKCS#11 mechanisms:

| `stse_conf.h` define        | PKCS#11 mechanism(s) added          |
|-----------------------------|-------------------------------------|
| `STSE_CONF_ECC_NIST_P_256`  | `CKM_EC_KEY_PAIR_GEN`, `CKM_ECDSA`, `CKM_ECDSA_SHA256`, `CKM_ECDH1_DERIVE` |
| `STSE_CONF_ECC_NIST_P_384`  | P-384 variants of the above         |
| `STSE_CONF_ECC_NIST_P_521`  | P-521 variants of the above         |
| `STSE_CONF_HASH_SHA_256`    | `CKM_SHA256`, `CKM_ECDSA_SHA256`    |
| `STSE_CONF_HASH_SHA_384`    | `CKM_SHA384`, `CKM_ECDSA_SHA384`    |
| `STSE_CONF_HASH_SHA_512`    | `CKM_SHA512`, `CKM_ECDSA_SHA512`    |

### 2. Add source files to your build

Add the following files to your build system (CMake, Makefile, IDE project, …):

```
sal/pkcs11/stse_pkcs11.c
sal/pkcs11/stse_cryptoki.c
```

No external PKCS#11 SDK is needed — `sal/pkcs11/pkcs11.h` is a self-contained
bundled header.

---

## Hardware configuration

Before calling `C_Initialize()`, optionally supply a `stse_pkcs11_config_t`
structure with your hardware parameters. If you skip this step the defaults
(address `0x20`, bus 0, 400 kHz, device type `STSAFE_A120`) are used.

```c
#include "stselib.h"   /* STSE_CONF_PKCS11_SUPPORT must be set */

void app_init(void)
{
    /* 1. (Optional) Set hardware parameters */
    stse_pkcs11_config_t cfg = {
        .i2c_addr        = 0x20,       /* 7-bit I2C address of the STSAFE-A */
        .bus_id          = 0,          /* Host I2C bus index                 */
        .bus_speed       = 400,        /* Bus speed in kHz                   */
        .device_type     = STSAFE_A120,

        /* Map data zones to X.509 DER certificate objects.
         * cert_zone_count = 0 means no certificate objects are exposed.     */
        .cert_zone_count = 1,
        .cert_zone_indices = { 0 },    /* Zone 0 holds the device certificate */
    };
    stse_pkcs11_set_config(&cfg);

    /* 2. Initialise the Cryptoki library */
    CK_RV rv = C_Initialize(NULL);
    if (rv != CKR_OK) {
        /* handle error */
    }
}
```

---

## Supported PKCS#11 mechanisms

| Mechanism                | Description                                      |
|--------------------------|--------------------------------------------------|
| `CKM_EC_KEY_PAIR_GEN`    | On-chip ECC key-pair generation (private key never leaves the device) |
| `CKM_ECDSA`              | Raw ECDSA — caller supplies the pre-computed digest |
| `CKM_ECDSA_SHA256`       | ECDSA with integrated SHA-256 hashing            |
| `CKM_ECDSA_SHA384`       | ECDSA with integrated SHA-384 hashing (requires `STSE_CONF_HASH_SHA_384`) |
| `CKM_ECDSA_SHA512`       | ECDSA with integrated SHA-512 hashing (requires `STSE_CONF_HASH_SHA_512`) |
| `CKM_SHA256`             | Hardware SHA-256 digest                          |
| `CKM_SHA384`             | Hardware SHA-384 digest (requires `STSE_CONF_HASH_SHA_384`) |
| `CKM_SHA512`             | Hardware SHA-512 digest (requires `STSE_CONF_HASH_SHA_512`) |
| `CKM_ECDH1_DERIVE`       | ECDH key agreement — shared secret computed on hardware |

---

## Object model

The token exposes **three types of PKCS#11 objects**, all backed by the
STSAFE-A non-volatile storage:

| PKCS#11 class      | Handle range    | Maps to                                |
|--------------------|-----------------|----------------------------------------|
| `CKO_PRIVATE_KEY`  | `0x001 – 0x0FF` | STSAFE-A private-key slot N            |
| `CKO_PUBLIC_KEY`   | `0x100 – 0x1FF` | Corresponding public half of slot N    |
| `CKO_CERTIFICATE`  | `0x200 – 0x2FF` | Data zone M (configured in `cert_zone_indices[]`) |

All private keys have `CKA_SENSITIVE = TRUE` and `CKA_EXTRACTABLE = FALSE`,
enforcing the hardware security boundary.

---

## Usage examples

### 1. Discover the token

```c
CK_SLOT_ID slot_list[1];
CK_ULONG   slot_count = 1;

C_GetSlotList(CK_TRUE, slot_list, &slot_count);
/* slot_list[0] == 0  (single slot representing the STSAFE-A) */
```

### 2. Open a session and enumerate key objects

```c
CK_SESSION_HANDLE hSession;
C_OpenSession(0, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL, NULL, &hSession);

/* Search for all private keys */
CK_OBJECT_CLASS privkey_class = CKO_PRIVATE_KEY;
CK_ATTRIBUTE search_tmpl[] = {
    { CKA_CLASS, &privkey_class, sizeof(privkey_class) }
};

C_FindObjectsInit(hSession, search_tmpl, 1);

CK_OBJECT_HANDLE handles[8];
CK_ULONG         found = 0;
C_FindObjects(hSession, handles, 8, &found);
C_FindObjectsFinal(hSession);

/* handles[0..found-1] are the private key handles */
```

### 3. Generate a key pair in slot 0

```c
CK_MECHANISM     mech = { CKM_EC_KEY_PAIR_GEN, NULL, 0 };

/* EC parameters OID for NIST P-256 (DER: 06 08 2A 86 48 CE 3D 03 01 07) */
static const CK_BYTE p256_oid[] = {
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07
};

CK_ATTRIBUTE pub_tmpl[] = {
    { CKA_EC_PARAMS, (void *)p256_oid, sizeof(p256_oid) }
};

/* CKA_ID = 0 means slot number 0 */
CK_BYTE      slot_id = 0;
CK_ATTRIBUTE priv_tmpl[] = {
    { CKA_ID, &slot_id, sizeof(slot_id) }
};

CK_OBJECT_HANDLE hPublic, hPrivate;
CK_RV rv = C_GenerateKeyPair(
    hSession, &mech,
    pub_tmpl,  1,
    priv_tmpl, 1,
    &hPublic, &hPrivate);
```

### 4. Sign data with ECDSA-SHA256

```c
CK_MECHANISM sign_mech = { CKM_ECDSA_SHA256, NULL, 0 };

/* hPrivate is the handle returned by C_GenerateKeyPair or found by C_FindObjects */
C_SignInit(hSession, &sign_mech, hPrivate);

CK_BYTE    data[]      = { /* application data to sign */ };
CK_BYTE    signature[64];  /* 32 + 32 bytes for P-256 R||S */
CK_ULONG   sig_len     = sizeof(signature);

CK_RV rv = C_Sign(hSession, data, sizeof(data), signature, &sig_len);
```

### 5. Digest data with SHA-256

```c
CK_MECHANISM hash_mech = { CKM_SHA256, NULL, 0 };
C_DigestInit(hSession, &hash_mech);

CK_BYTE  digest[32];
CK_ULONG digest_len = sizeof(digest);

CK_RV rv = C_Digest(hSession, data, sizeof(data), digest, &digest_len);
```

### 6. Read a certificate from a data zone

```c
/* CKA_ID = 0 selects the first configured certificate zone */
CK_OBJECT_CLASS cert_class = CKO_CERTIFICATE;
CK_BYTE         cert_id    = 0;
CK_ATTRIBUTE cert_search[] = {
    { CKA_CLASS, &cert_class, sizeof(cert_class) },
    { CKA_ID,    &cert_id,    sizeof(cert_id)    }
};

C_FindObjectsInit(hSession, cert_search, 2);

CK_OBJECT_HANDLE hCert;
CK_ULONG         cert_found = 0;
C_FindObjects(hSession, &hCert, 1, &cert_found);
C_FindObjectsFinal(hSession);

/* Retrieve the DER-encoded certificate bytes */
CK_BYTE    cert_der[2048];
CK_ATTRIBUTE cert_value[] = {
    { CKA_VALUE, cert_der, sizeof(cert_der) }
};
C_GetAttributeValue(hSession, hCert, cert_value, 1);
/* cert_der[0..cert_value[0].ulValueLen-1] contains the X.509 DER data */
```

### 7. ECDH key agreement

```c
/* peer_public_key_point: uncompressed EC point 0x04 || X || Y of the peer */
CK_BYTE  peer_point[]  = { 0x04, /* X bytes… */, /* Y bytes… */ };

CK_ECDH1_DERIVE_PARAMS ecdh_params = {
    .kdf              = CKD_NULL,
    .ulSharedDataLen  = 0,
    .pSharedData      = NULL,
    .ulPublicDataLen  = sizeof(peer_point),
    .pPublicData      = peer_point,
};

CK_MECHANISM derive_mech = {
    CKM_ECDH1_DERIVE,
    &ecdh_params,
    sizeof(ecdh_params)
};

CK_OBJECT_HANDLE hDerived;
C_DeriveKey(hSession, &derive_mech, hPrivate, NULL, 0, &hDerived);
```

---

## Using with OpenSSL (libp11 / pkcs11-provider)

The PKCS#11 SAL can be loaded as a shared library by any PKCS#11-aware OpenSSL
engine or provider. The entry point required is `C_GetFunctionList`, which is
already implemented in `stse_cryptoki.c`.

**Example — TLS client using hardware-backed private key:**

```bash
# Load the shared library and reference the private key by its PKCS#11 URI
openssl s_client \
    -engine pkcs11 \
    -keyform engine \
    -key "pkcs11:token=STSAFE-A%20Token;object=0" \
    -connect example.com:443
```

The PKCS#11 URI scheme uses the object ID (`CKA_ID`) set during key generation
as the `object` field. Object ID `0` refers to hardware slot 0.

---

## Using with OpenSSH

Configure `~/.ssh/config` or `/etc/ssh/ssh_config` to load the PKCS#11 module:

```
Host myhost
    PKCS11Provider /path/to/libstse_pkcs11.so
```

OpenSSH will call `C_GetFunctionList` to discover the token, enumerate keys
using `C_FindObjects`, and sign the authentication challenge via `C_Sign`.

---

## Error handling

All `C_*` functions return a standard `CK_RV` value. The most relevant codes are:

| Code                           | Typical cause                                     |
|--------------------------------|---------------------------------------------------|
| `CKR_OK`                       | Success                                           |
| `CKR_CRYPTOKI_NOT_INITIALIZED` | `C_Initialize()` was not called                   |
| `CKR_DEVICE_ERROR`             | I2C communication failure with the STSAFE-A       |
| `CKR_KEY_HANDLE_INVALID`       | Slot number out of range or key not yet generated |
| `CKR_MECHANISM_INVALID`        | Mechanism not supported (check `stse_conf.h`)     |
| `CKR_BUFFER_TOO_SMALL`         | Output buffer shorter than the result; `*pulLen` updated with required size |
| `CKR_OPERATION_ACTIVE`         | `C_SignInit` / `C_DigestInit` called twice without finalising |
| `CKR_OPERATION_NOT_INITIALIZED`| `C_Sign` / `C_Digest` called without a prior `Init` |

---

## Configuration reference

The `stse_pkcs11_config_t` structure controls the hardware mapping:

```c
typedef struct stse_pkcs11_config_t {
    uint8_t       i2c_addr;                  /* 7-bit I2C address (default 0x20)  */
    uint8_t       bus_id;                    /* Host I2C bus index (default 0)     */
    uint16_t      bus_speed;                 /* kHz: 100 or 400 (default 400)      */
    stse_device_t device_type;               /* STSAFE_A110 or STSAFE_A120         */
    uint8_t       cert_zone_count;           /* Number of certificate zones (0-8)  */
    uint8_t       cert_zone_indices[8];      /* Data-zone indices for certificates  */
} stse_pkcs11_config_t;
```

Call `stse_pkcs11_set_config(&cfg)` before `C_Initialize()`.
Call `stse_pkcs11_get_config()` to inspect the active configuration at runtime.

---

## Limitations

- **Single session**: Only one concurrent PKCS#11 session is supported. If your
  middleware opens multiple sessions, serialise access externally.
- **No PIN / login**: `C_Login` succeeds silently — the STSAFE-A hardware does
  not use a software PIN.
- **No multi-part signing**: `C_SignUpdate` / `C_SignFinal` return
  `CKR_FUNCTION_NOT_SUPPORTED`. Supply the full message to `C_Sign`.
- **Public key cache**: `CKA_EC_POINT` is only available after
  `C_GenerateKeyPair` has been called in the current session, as the public key
  is cached in RAM. It is not persisted across power cycles or
  `C_Initialize` / `C_Finalize` calls.
