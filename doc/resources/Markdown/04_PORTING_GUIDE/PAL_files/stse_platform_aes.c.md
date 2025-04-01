# stse_platform_aes.c{#stse_platform_aes}

The stse_platform_aes.c file provides the implementation of cryptographic functions for the STSecureElement library. These functions abstract the platform-specific details of cryptographic operations, allowing developers to use a consistent API across different hardware platforms. The file includes functions for AES CMAC, AES CBC, and AES ECB encryption and decryption, as well as initialization and verification processes.

- AES CMAC functions 

  - stse_platform_aes_cmac_init:

Purpose: Initializes the AES CMAC operation.
Parameters: pKey (key), key_length (length of the key), exp_tag_size (expected tag size).
Implementation: Constructs the CMAC handler, initializes the MAC, sets the tag length, and sets the key. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.

  - stse_platform_aes_cmac_append:

Purpose: Appends data to the AES CMAC operation.
Parameters: pInput (input data), length (length of the input data).
Implementation: Appends the input data to the MAC handler. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.

  - stse_platform_aes_cmac_compute_finish:

Purpose: Completes the AES CMAC computation and generates the tag.
Parameters: pTag (output tag), pTagLen (length of the output tag).
Implementation: Generates the tag and cleans up the MAC handler. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.

  - stse_platform_aes_cmac_verify_finish:

Purpose: Verifies the AES CMAC tag.
Parameters: pTag (tag to be verified).
Implementation: Verifies the tag and cleans up the MAC handler. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.

  - stse_platform_aes_cmac_compute:

Purpose: Computes the AES CMAC tag for a given payload.
Parameters: pPayload (payload), payload_length (length of the payload), pKey (key), key_length (length of the key), exp_tag_size (expected tag size), pTag (output tag), pTag_length (length of the output tag).
Implementation: Computes the CMAC tag using the specified parameters. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.

  - stse_platform_aes_cmac_verify:

Purpose: Verifies the AES CMAC tag for a given payload.
Parameters: pPayload (payload), payload_length (length of the payload), pKey (key), key_length (length of the key), pTag (tag to be verified), tag_length (length of the tag).
Implementation: Verifies the CMAC tag using the specified parameters. Returns STSE_OK on success or STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.


- AES CBC Functions

  - stse_platform_aes_cbc_enc:

Purpose: Encrypts data using AES CBC mode.
Parameters: pPlaintext (plaintext), plaintext_length (length of the plaintext), pInitial_value (initial value), pKey (key), key_length (length of the key), pEncryptedtext (output encrypted text), pEncryptedtext_length (length of the output encrypted text).
Implementation: Encrypts the plaintext using AES CBC mode. Returns STSE_OK on success or STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR on failure.

  - stse_platform_aes_cbc_dec:

Purpose: Decrypts data using AES CBC mode.
Parameters: pEncryptedtext (encrypted text), encryptedtext_length (length of the encrypted text), pInitial_value (initial value), pKey (key), key_length (length of the key), pPlaintext (output plaintext), pPlaintext_length (length of the output plaintext).
Implementation: Decrypts the encrypted text using AES CBC mode. Returns STSE_OK on success or STSE_PLATFORM_AES_CBC_DECRYPT_ERROR on failure.
AES ECB Functions

  - stse_platform_aes_ecb_enc:

Purpose: Encrypts data using AES ECB mode.
Parameters: pPlaintext (plaintext), plaintext_length (length of the plaintext), pKey (key), key_length (length of the key), pEncryptedtext (output encrypted text), pEncryptedtext_length (length of the output encrypted text).
Implementation: Encrypts the plaintext using AES ECB mode. Returns STSE_OK on success or STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR on failure.

  - stse_platform_aes_ecb_dec:

Purpose: Decrypts data using AES ECB mode.
Parameters: pEncryptedtext (encrypted text), encryptedtext_length (length of the encrypted text), pKey (key), key_length (length of the key), pPlaintext (output plaintext), pPlaintext_length (length of the output plaintext).
Implementation: Decrypts the encrypted text using AES ECB mode. Returns STSE_OK on success or STSE_PLATFORM_AES_ECB_DECRYPT_ERROR on failure.


Below is an example of stse_platform_aes.c file content for adapting the STSELib to the STM32 CMOX library :

```c
/******************************************************************************
 * \file    stse_platform_crypto.c
 * \brief   STSecureElement cryptographic platform file
 * \author  STMicroelectronics - CS application team
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */

#include "stse_conf.h"
#include "stselib.h"
#include "Middleware/STM32_Cryptographic/include/cmox_crypto.h"

cmox_mac_handle_t* pMAC_Handler;
cmox_cmac_handle_t CMAC_Handler;

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION)

/**
 * \brief   Initializes the AES CMAC operation.
 * \param   pKey        Pointer to the key.
 * \param   key_length  Length of the key.
 * \param   exp_tag_size Expected tag size.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_init(const PLAT_UI8 *pKey,
                                              PLAT_UI16 key_length,
                                              PLAT_UI16 exp_tag_size)
{
    cmox_mac_retval_t retval;

    /* - Call CMAC constructor */
    pMAC_Handler = cmox_cmac_construct(&CMAC_Handler, CMOX_CMAC_AESSMALL);

    /* - Init MAC */
    retval = cmox_mac_init(pMAC_Handler);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }
    /* - Set Tag length */
    retval = cmox_mac_setTagLen(pMAC_Handler, exp_tag_size);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }
    /* - Set Key  */
    retval = cmox_mac_setKey(pMAC_Handler, pKey, key_length);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Appends data to the AES CMAC operation.
 * \param   pInput  Pointer to the input data.
 * \param   length  Length of the input data.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_append(PLAT_UI8 *pInput,
                                                PLAT_UI16 length)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_append(pMAC_Handler, pInput, length);

    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Completes the AES CMAC computation and generates the tag.
 * \param   pTag    Pointer to the output tag.
 * \param   pTagLen Pointer to the length of the output tag.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_compute_finish(PLAT_UI8 *pTag, PLAT_UI8 *pTagLen)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_generateTag(pMAC_Handler, pTag, (size_t *)pTagLen);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    retval = cmox_mac_cleanup(pMAC_Handler);
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Verifies the AES CMAC tag.
 * \param   pTag    Pointer to the tag to be verified.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_verify_finish(PLAT_UI8 *pTag)
{
    cmox_mac_retval_t retval;
    uint32_t cmox_mac_fault_check = 0;

    retval = cmox_mac_verifyTag(pMAC_Handler, pTag, &cmox_mac_fault_check);

    cmox_mac_cleanup(pMAC_Handler);

    if ((retval != CMOX_MAC_AUTH_SUCCESS) || (cmox_mac_fault_check != CMOX_MAC_AUTH_SUCCESS))
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Computes the AES CMAC tag for a given payload.
 * \param   pPayload        Pointer to the payload.
 * \param   payload_length  Length of the payload.
 * \param   pKey            Pointer to the key.
 * \param   key_length      Length of the key.
 * \param   exp_tag_size    Expected tag size.
 * \param   pTag            Pointer to the output tag.
 * \param   pTag_length     Pointer to the length of the output tag.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_compute(const PLAT_UI8 *pPayload,
                                                 PLAT_UI16 payload_length,
                                                 const PLAT_UI8 *pKey,
                                                 PLAT_UI16 key_length,
                                                 PLAT_UI16 exp_tag_size,
                                                 PLAT_UI8 *pTag,
                                                 PLAT_UI16 *pTag_length)
{
    cmox_mac_retval_t retval;

    retval = cmox_mac_compute(CMOX_CMAC_AESSMALL_ALGO, /* Use AES CMAC algorithm */
                              pPayload,                /* Message */
                              payload_length,          /* Message length */
                              pKey,                    /* AES key */
                              key_length,              /* AES key length */
                              NULL,                    /* Custom Data */
                              0,                       /* Custom Data length */
                              pTag,                    /* Tag */
                              exp_tag_size,            /* Expected Tag size */
                              (size_t *)pTag_length    /* Generated Tag size */
    );

    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Verifies the AES CMAC tag for a given payload.
 * \param   pPayload        Pointer to the payload.
 * \param   payload_length  Length of the payload.
 * \param   pKey            Pointer to the key.
 * \param   key_length      Length of the key.
 * \param   pTag            Pointer to the tag to be verified.
 * \param   tag_length      Length of the tag.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CMAC_VERIFY_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cmac_verify(const PLAT_UI8 *pPayload,
                                                PLAT_UI16 payload_length,
                                                const PLAT_UI8 *pKey,
                                                PLAT_UI16 key_length,
                                                const PLAT_UI8 *pTag,
                                                PLAT_UI16 tag_length)
{
    cmox_mac_retval_t retval;

    /* - Perform CMAC verification */
    retval = cmox_mac_verify(CMOX_CMAC_AESSMALL_ALGO, /* Use AES CMAC algorithm */
                             pPayload,                /* Message length */
                             payload_length,          /* Message length */
                             pKey,                    /* AES key */
                             key_length,              /* AES key length */
                             NULL,                    /* Custom data */
                             0,                       /* Custom data length */
                             pTag,                    /* Tag */
                             tag_length               /* Tag size */
    );

    if (retval != CMOX_MAC_AUTH_SUCCESS)
    {
        return STSE_PLATFORM_AES_CMAC_VERIFY_ERROR;
    }

    return STSE_OK;
}
#endif /* defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_SYMMETRIC_KEY_ESTABLISHMENT) */

#if defined(STSE_CONF_USE_HOST_KEY_ESTABLISHMENT) || defined(STSE_CONF_USE_HOST_SESSION)

/**
 * \brief   Encrypts data using AES CBC mode.
 * \param   pPlaintext              Pointer to the plaintext.
 * \param   plaintext_length        Length of the plaintext.
 * \param   pInitial_value          Pointer to the initial value.
 * \param   pKey                    Pointer to the key.
 * \param   key_length              Length of the key.
 * \param   pEncryptedtext          Pointer to the output encrypted text.
 * \param   pEncryptedtext_length   Pointer to the length of the output encrypted text.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cbc_enc(const PLAT_UI8 *pPlaintext,
                                            PLAT_UI16 plaintext_length,
                                            PLAT_UI8 *pInitial_value,
                                            const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length,
                                            PLAT_UI8 *pEncryptedtext,
                                            PLAT_UI16 *pEncryptedtext_length)
{
    cmox_cipher_retval_t retval;

    /*- Perform AES CBC Encryption */
    retval = cmox_cipher_encrypt(CMOX_AESSMALL_CBC_ENC_ALGO, /* Use AES CBC algorithm */
                                 pPlaintext,                 /* Plain Text */
                                 plaintext_length,           /* Plain Text length */
                                 pKey,                       /* AES Key */
                                 key_length,                 /* AES Key length */
                                 pInitial_value,             /* Initial Value */
                                 16,                         /* Initial Value length */
                                 pEncryptedtext,             /* Ciphered Text */
                                 (size_t *)pEncryptedtext_length /* Ciphered Text length */
    );

    /*- Verify AES CBC Encryption status */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Decrypts data using AES CBC mode.
 * \param   pEncryptedtext          Pointer to the encrypted text.
 * \param   encryptedtext_length    Length of the encrypted text.
 * \param   pInitial_value          Pointer to the initial value.
 * \param   pKey                    Pointer to the key.
 * \param   key_length              Length of the key.
 * \param   pPlaintext              Pointer to the output plaintext.
 * \param   pPlaintext_length       Pointer to the length of the output plaintext.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_CBC_DECRYPT_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_cbc_dec(const PLAT_UI8 *pEncryptedtext,
                                            PLAT_UI16 encryptedtext_length,
                                            PLAT_UI8 *pInitial_value,
                                            const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length,
                                            PLAT_UI8 *pPlaintext,
                                            PLAT_UI16 *pPlaintext_length)
{
    cmox_cipher_retval_t retval;

    /*- Perform AES CBC decryption */
    retval = cmox_cipher_decrypt(CMOX_AESSMALL_CBC_DEC_ALGO, /* Use AES CBC algorithm */
                                 pEncryptedtext,             /* Ciphered Text */
                                 encryptedtext_length,       /* Ciphered Text length */
                                 pKey,                       /* AES key */
                                 key_length,                 /* AES key length */
                                 pInitial_value,             /* Initial Value */
                                 16,                         /* Initial Value length */
                                 pPlaintext,                 /* Plain Text */
                                 (size_t *)pPlaintext_length /* Plain Text length */
    );

    /*- Verify AES CBC decrypt return */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_CBC_DECRYPT_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Encrypts data using AES ECB mode.
 * \param   pPlaintext              Pointer to the plaintext.
 * \param   plaintext_length        Length of the plaintext.
 * \param   pKey                    Pointer to the key.
 * \param   key_length              Length of the key.
 * \param   pEncryptedtext          Pointer to the output encrypted text.
 * \param   pEncryptedtext_length   Pointer to the length of the output encrypted text.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_ecb_enc(const PLAT_UI8 *pPlaintext,
                                            PLAT_UI16 plaintext_length,
                                            const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length,
                                            PLAT_UI8 *pEncryptedtext,
                                            PLAT_UI16 *pEncryptedtext_length)
{
    cmox_cipher_retval_t retval;
    PLAT_UI8 IV[16] = {0};

    /*- Perform AES ECB Encryption */
    retval = cmox_cipher_encrypt(CMOX_AESSMALL_ECB_ENC_ALGO, /* Use AES ECB algorithm */
                                 pPlaintext,                 /* Plain Text */
                                 plaintext_length,           /* Plain Text length */
                                 pKey,                       /* AES Key */
                                 key_length,                 /* AES Key length */
                                 IV,                         /* Initial Value */
                                 16,                         /* Initial Value length */
                                 pEncryptedtext,             /* Ciphered Text */
                                 (size_t *)pEncryptedtext_length /* Ciphered Text length */
    );

    /*- Verify AES ECB Encryption status */
    if (retval != CMOX_CIPHER_SUCCESS)
    {
        return STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Decrypts data using AES ECB mode.
 * \param   pEncryptedtext          Pointer to the encrypted text.
 * \param   encryptedtext_length    Length of the encrypted text.
 * \param   pKey                    Pointer to the key.
 * \param   key_length              Length of the key.
 * \param   pPlaintext              Pointer to the output plaintext.
 * \param   pPlaintext_length       Pointer to the length of the output plaintext.
 * \return  STSE_OK on success, STSE_PLATFORM_AES_ECB_DECRYPT_ERROR on failure.
 */
stse_ReturnCode_t stse_platform_aes_ecb_dec(const PLAT_UI8 *pEncryptedtext,
                                            PLAT_UI16 encryptedtext_length,
                                            const PLAT_UI8 *pKey,
                                            PLAT_UI16 key_length,
                                            PLAT_UI8 *pPlaintext,
                                            PLAT_UI16 *pPlaintext_length)
{
    cmox_cipher_retval_t retval;
    PLAT_UI8 IV[16] = {0};

    /*- Perform AES ECB decryption */
    retval = cmox_cipher_decrypt(CMOX_AESSMALL_ECB_DEC_ALGO, /* Use AES ECB algorithm */
                                 pEncryptedtext,             /* Ciphered Text */
                                 encryptedtext_length,       /* Ciphered Text length */
                                 pKey,                       /* AES key */
                                 key_length,                 /* AES key length */
                                 IV,                         /* Initial Value */
                                 16,                         /* Initial Value length */
                                 pPlaintext,                 /* Plain Text */
                                 (size_t *)pPlaintext_length /* Plain Text length */
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
