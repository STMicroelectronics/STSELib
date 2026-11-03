# stse_platform_aes.c {#stse_platform_aes}

The `stse_platform_aes.c` file provides AES functions for the STSecureElement library, abstracting the platform-specific details of the AES cryptographic process.

## stse_platform_aes_cmac_init:

- **Purpose**: Initializes the AES CMAC computation.
- **Parameters**:
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `exp_tag_size`: Expected tag size.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the AES CMAC computation.

## stse_platform_aes_cmac_append:

- **Purpose**: Appends data to the AES CMAC computation.
- **Parameters**:
  - `p_input`: Pointer to the input data.
  - `length`: Length of the input data.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that appends data to the AES CMAC computation.

## stse_platform_aes_cmac_compute_finish:

- **Purpose**: Finishes the AES CMAC computation and generates the tag.
- **Parameters**:
  - `p_tag`: Pointer to the tag.
  - `p_tag_len`: Pointer to the length of the tag.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that finishes the AES CMAC computation and generates the tag.

## stse_platform_aes_cmac_verify_finish:

- **Purpose**: Verifies the AES CMAC tag.
- **Parameters**:
  - `p_tag`: Pointer to the tag.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_VERIFY_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that verifies the AES CMAC tag.

## stse_platform_aes_cmac_compute:

- **Purpose**: Computes the AES CMAC for the given payload.
- **Parameters**:
  - `p_payload`: Pointer to the payload.
  - `payload_length`: Length of the payload.
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `exp_tag_size`: Expected tag size.
  - `p_tag`: Pointer to the tag.
  - `p_tag_length`: Pointer to the length of the tag.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_VERIFY_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that computes the AES CMAC for the given payload.

## stse_platform_aes_cmac_verify:

- **Purpose**: Verifies the AES CMAC for the given payload.
- **Parameters**:
  - `p_payload`: Pointer to the payload.
  - `payload_length`: Length of the payload.
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `p_tag`: Pointer to the tag.
  - `tag_length`: Length of the tag.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CMAC_VERIFY_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that verifies the AES CMAC for the given payload.

## stse_platform_aes_cbc_enc:

- **Purpose**: Encrypts data using AES CBC mode.
- **Parameters**:
  - `p_plaintext`: Pointer to the plaintext.
  - `plaintext_length`: Length of the plaintext.
  - `p_initial_value`: Pointer to the initial value (IV).
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `p_encryptedtext`: Pointer to the encrypted text.
  - `p_encryptedtext_length`: Pointer to the length of the encrypted text.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that encrypts data using AES CBC mode.

## stse_platform_aes_cbc_dec:

- **Purpose**: Decrypts data using AES CBC mode.
- **Parameters**:
  - `p_encryptedtext`: Pointer to the encrypted text.
  - `encryptedtext_length`: Length of the encrypted text.
  - `p_initial_value`: Pointer to the initial value (IV).
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `p_plaintext`: Pointer to the plaintext.
  - `p_plaintext_length`: Pointer to the length of the plaintext.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_CBC_DECRYPT_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that decrypts data using AES CBC mode.

## stse_platform_aes_ecb_enc:

- **Purpose**: Encrypts data using AES ECB mode.
- **Parameters**:
  - `p_plaintext`: Pointer to the plaintext.
  - `plaintext_length`: Length of the plaintext.
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `p_encryptedtext`: Pointer to the encrypted text.
  - `p_encryptedtext_length`: Pointer to the length of the encrypted text.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that encrypts data using AES ECB mode.

## stse_platform_aes_ecb_dec:

- **Purpose**: Decrypts data using AES ECB mode.
- **Parameters**:
  - `p_encryptedtext`: Pointer to the encrypted text.
  - `encryptedtext_length`: Length of the encrypted text.
  - `p_key`: Pointer to the key.
  - `key_length`: Length of the key.
  - `p_plaintext`: Pointer to the plaintext.
  - `p_plaintext_length`: Pointer to the length of the plaintext.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_AES_ECB_DECRYPT_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that decrypts data using AES ECB mode.

## Implementation Example:

Please find below an example of the `stse_platform_aes` implementation for the STM32 CMOX library:

```c
/******************************************************************************
 * \file    stse_platform_aes.c
 * \brief   STSecureElement AES platform file
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "stse_conf.h"
#include "stselib.h"
#include "Middleware/STM32_Cryptographic/include/cmox_crypto.h"

cmox_mac_handle_t* p_mac_handler;
cmox_cmac_handle_t CMAC_Handler;

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION)

stse_return_code_t stse_platform_aes_cmac_init (const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI16 exp_tag_size)
{
    cmox_mac_retval_t retval;

    /* - Call CMAC constructor */
    p_mac_handler = cmox_cmac_construct(&CMAC_Handler, CMOX_CMAC_AESSMALL);

    /* - Init MAC */
    retval = cmox_mac_init(p_mac_handler);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }
    /* - Set Tag length */
    retval = cmox_mac_set_tag_len(p_mac_handler, exp_tag_size);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }
    /* - Set Key  */
    retval = cmox_mac_set_key(p_mac_handler, p_key, key_length);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cmac_append(PLAT_UI8* p_input,
    PLAT_UI16 length)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_append(p_mac_handler, p_input, length);

    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cmac_compute_finish(PLAT_UI8* p_tag, PLAT_UI8* p_tag_len)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_generate_tag(p_mac_handler, p_tag, (size_t *)p_tag_len);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    retval = cmox_mac_cleanup(p_mac_handler);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cmac_verify_finish(PLAT_UI8* p_tag)
{
    cmox_mac_retval_t retval;
    uint32_t cmox_mac_fault_check = 0;

    retval = cmox_mac_verify_tag(
        p_mac_handler,
        p_tag,
        &cmox_mac_fault_check);

    cmox_mac_cleanup(p_mac_handler);

    if ((retval != CMOX_MAC_AUTH_SUCCESS) || (cmox_mac_fault_check != CMOX_MAC_AUTH_SUCCESS))
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cmac_compute(const PLAT_UI8 *p_payload,
    PLAT_UI16 payload_length,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI16 exp_tag_size,
    PLAT_UI8 *p_tag,
    PLAT_UI16 *p_tag_length)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_compute(CMOX_CMAC_AESSMALL_ALGO, /* Use AES CMAC algorithm */
        p_payload,                              /* Message */
        payload_length,                        /* Message length*/
        p_key,                                  /* AES key */
        key_length,                            /* AES key length */
        NULL,                                  /* Custom Data */
        0,                                     /* Custom Data length */
        p_tag,                                  /* Tag */
        exp_tag_size,                          /* Expected Tag size */
        (size_t*)p_tag_length                   /* Generated Tag size */
    );

    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cmac_verify(const PLAT_UI8 *p_payload,
    PLAT_UI16 payload_length,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    const PLAT_UI8 *p_tag,
    PLAT_UI16 tag_length)
{
    cmox_mac_retval_t retval;

    /* - Perform CMAC verification */
    retval = cmox_mac_verify(CMOX_CMAC_AESSMALL_ALGO, /* Use AES CMAC algorithm */
        p_payload,            /* Message length */
        payload_length,      /* Message length */
        p_key,                /* AES key */
        key_length,          /* AES key length */
        NULL,                /* Custom data */
        0,                   /* Custom data length*/
        p_tag,                /* Tag */
        tag_length           /* Tag size */
    );

    if (retval != CMOX_MAC_AUTH_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}
#endif  /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) */

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION)
stse_return_code_t stse_platform_aes_cbc_enc(const PLAT_UI8 *p_plaintext,
    PLAT_UI16 plaintext_length,
    PLAT_UI8 *p_initial_value,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI8 *p_encryptedtext,
    PLAT_UI16 *p_encryptedtext_length)
{
    cmox_cipher_retval_t retval;

    /*- Perform AES ECB Encryption */
    retval = cmox_cipher_encrypt(CMOX_AESSMALL_CBC_ENC_ALGO, /* Use AES CBC algorithm */
        p_plaintext,                                 /* Plain Text */
        plaintext_length,                           /* Plain Text length*/
        p_key,                                       /* AES Key */
        key_length,                                 /* AES Key length*/
        p_initial_value,                             /* Initial Value */
        16,                                         /* Initial Value length */
        p_encryptedtext,                             /* Ciphered Text */
        (size_t*)p_encryptedtext_length              /* Ciphered Text length*/
    );

    /*- Verify AES ECB Encryption status */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_cbc_dec(const PLAT_UI8 *p_encryptedtext,
    PLAT_UI16 encryptedtext_length,
    PLAT_UI8 *p_initial_value,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI8 *p_plaintext,
    PLAT_UI16 *p_plaintext_length)
{
    cmox_cipher_retval_t retval;

    /*- Perform AES ECB decryption */
    retval = cmox_cipher_decrypt(CMOX_AESSMALL_CBC_DEC_ALGO, /* Use AES CBC algorithm */
        p_encryptedtext,                           /* Ciphered Text */
        encryptedtext_length,                     /* Ciphered Text length */
        p_key,                                     /* AES key length */
        key_length,                               /* AES key */
        p_initial_value,                           /* Initial Value */
        16,                                       /* Initial Value length*/
        p_plaintext,                               /* Plain Text */
        (size_t*)p_plaintext_length                /* Plain Text length*/
    );

    /*- Verify AES ECB decrypt return */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_CBC_DECRYPT_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_ecb_enc(const PLAT_UI8 *p_plaintext,
    PLAT_UI16 plaintext_length,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI8 *p_encryptedtext,
    PLAT_UI16 *p_encryptedtext_length)
{
    cmox_cipher_retval_t retval;
    PLAT_UI8 IV[16] = {0};

    /*- Perform AES ECB Encryption */
    retval = cmox_cipher_encrypt(CMOX_AESSMALL_ECB_ENC_ALGO, /* Use AES ECB algorithm */
        p_plaintext,                                 /* Plain Text */
        plaintext_length,                           /* Plain Text length*/
        p_key,                                       /* AES Key */
        key_length,                                 /* AES Key length*/
        IV,                                         /* Initial Value */
        16,                                         /* Initial Value length */
        p_encryptedtext,                             /* Ciphered Text */
        (size_t*)p_encryptedtext_length              /* Ciphered Text length*/
    );

    /*- Verify AES ECB Encryption status */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR;
    }

    return STSE_OK;
}

stse_return_code_t stse_platform_aes_ecb_dec(const PLAT_UI8 *p_encryptedtext,
    PLAT_UI16 encryptedtext_length,
    const PLAT_UI8 *p_key,
    PLAT_UI16 key_length,
    PLAT_UI8 *p_plaintext,
    PLAT_UI16 *p_plaintext_length)
{
    cmox_cipher_retval_t retval;
    PLAT_UI8 IV[16] = {0};

    /*- Perform AES ECB decryption */
    retval = cmox_cipher_decrypt(CMOX_AESSMALL_ECB_DEC_ALGO, /* Use AES ECB algorithm */
        p_encryptedtext,                           /* Ciphered Text */
        encryptedtext_length,                     /* Ciphered Text length */
        p_key,                                     /* AES key length */
        key_length,                               /* AES key */
        IV,                                       /* Initial Value */
        16,                                       /* Initial Value length*/
        p_plaintext,                               /* Plain Text */
        (size_t*)p_plaintext_length                /* Plain Text length*/
    );

    /*- Verify AES ECB decrypt return */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_ECB_DECRYPT_ERROR;
    }

    return STSE_OK;
}
#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) */
```
