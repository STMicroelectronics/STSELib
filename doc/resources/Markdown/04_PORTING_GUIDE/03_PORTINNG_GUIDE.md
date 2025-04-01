# Porting Guidelines

This section of the documentation provides a comprehensive description of the STSELib Platform Abstraction Layer (PAL), along with detailed guidance on the process to be followed to adapt the library to a specific host platform, whether it be a microcontroller or a microprocessor. The goal of these guidelines is to ensure a smooth and efficient porting process, enabling developers to leverage the capabilities of STSELib across various hardware platforms with minimal effort.

# Overview of STSELib Platform Abstraction

The STSELib Platform Abstraction Layer is designed to provide a consistent and high-level interface to the underlying hardware, abstracting away the platform-specific details. This abstraction allows developers to write application code that is portable and reusable across different platforms. The PAL includes a set of APIs that facilitate interaction with common hardware features such as cryptgraphic library functions , GPIO, timers and communication interfaces.

![STSELib_PLAT](../Pictures/STSELib_PAL.png)


The STSELib embeds a platform abstraction header (stse_platform.h) listing all functions to be implemented by the application developer to adapt the library to a specific host microcontroller/microprocessor and toolchain.

This Platform Abstraction Layer decouples the STSELibrary Middleware from the underlying hardware and the toolchain used by the embedded system developers.


![PAL_integration](../Pictures/PAL_integration.png)

# Identifying the platform abstraction functions to be implemented 

To assist developers in identifying the callback functions to be developed and simplify the porting process of the library, it is recommended to use an stse_config.h file that activates only the configuration parameters required by the end application (see Library Configuration documentation section). By doing this, advanced IDEs such as STM32Cube IDE will highlight which platform functions need to be ported. Below is an extract of the platform configuration:


<Add picture>


By enabling only the necessary configuration parameters, developers can focus on implementing the required platform-specific functions, ensuring a streamlined and efficient porting process.


# Platform files description

## stse_platform_generic.h

The stse_platform_generic.h file provides abstraction for both MCU platform/toolchain inclusion and types abstraction. Below is an example for STM32L452RE and GCC platform:

```c

 /******************************************************************************
 * \file	stse_platform_generic.h
 * \brief   STSecureElement library generic platform file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STSE_PLATFORM_GENERIC_H
#define STSE_PLATFORM_GENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define PLAT_UI8 uint8_t
#define PLAT_UI16 uint16_t
#define PLAT_UI32 uint32_t
#define PLAT_UI64 uint64_t
#define PLAT_I8 int8_t
#define PLAT_I16 int16_t
#define PLAT_I32 int32_t
#define PLAT_PACKED_STRUCT __PACKED

#endif /* STSE_PLATFORM_GENERIC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

```

Explanation of stse_platform_generic.h structure :
- Header Guards: Prevents recursive inclusion of the header file.
- C++ Compatibility: Ensures compatibility with C++ compilers by using extern "C".
- Platform-Specific Includes: Includes headers specific to the STM32L452RE and GCC platform.
- Type Definitions: Defines platform-independent types such as PLAT_UI8, PLAT_UI16, PLAT_UI32, PLAT_UI64, PLAT_I8, PLAT_I16, PLAT_I32, and PLAT_PACKED_STRUCT.


## stse_platform_delay.c

stse_platform_crypto.c
The stse_platform_crypto.c file provides the implementation of delay mechanisms that are used by the mibrary for communication inter-frame retry and command processing timmings management. 
These functions abstract the platform-specific details, allowing developers to a dapt/redirect the delay to any specific host platform 

Below is the content of the stse_platform_crypto.c file:

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

/*Includes the configuration header file for STSELib.*/
#include "stse_conf.h"
/*Includes the main STSELib header file*/
#include "stselib.h"
/*Includes the header file for millisecond delay MCU driver*/
#include "Drivers/delay_ms/delay_ms.h"

/*Purpose: Initializes the platform drivers used by the STSELib PAL.*/
stse_ReturnCode_t stse_platform_delay_init(void)
{
    /* initialize the millisecond delay driver*/
    delay_ms_init();

    /*Return Value: Returns STSE_OK to indicate successful initialization*/
    return STSE_OK;
}

/*Purpose: Provides a delay for a specified number of milliseconds.
Parameters provided by the Library : 
-  PLAT_UI32 delay_val : Number of milliseconds to delay.*/
void stse_platform_Delay_ms (PLAT_UI32 delay_val)
{
    /* Calls the delay_ms(delay_val) function to generate the delay.*/
    delay_ms(delay_val);
}

/*Purpose: Starts a timeout for a specified number of milliseconds.
Parameters provided by the Library : 
-  PLAT_UI16 timeout_val : Number of milliseconds for the timeout.*/
void stse_platform_timeout_ms_start(PLAT_UI16 timeout_val)
{
    /* Calls the timeout_ms_start(timeout_val) function to start the timeout.*/
    timeout_ms_start(timeout_val);
}

/* Purpose: Gets the status of the timeout.
Return Value: Returns the status of the timeout as a PLAT_UI8 value.
- 0 : timeout not reached
- 1 : timeout reached */

PLAT_UI8 stse_platform_timeout_ms_get_status(void)
{
    /*Calls the timeout_ms_get_status() function to retrieve the timeout status.*/
    return timeout_ms_get_status();
}

```

## stse_platfrom_crypto_init.c

The stse_platform_crypto_init.c file provides the implementation of the initialization function for cryptographic operations in the STSecureElement library. This function ensures that the cryptographic library is properly initialized, allowing secure operations to be performed on the target platform.

Below is an example of stse_platform_crypto_init.c file for STM32 CMOX library usage:

```c
/******************************************************************************
 * \file    stse_platform_crypto_init.c
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

/* Includes the configuration header file for STSELib.*/
#include "stse_conf.h"
/* Includes the main STSELib header file*/
#include "stselib.h"
/* Includes the header file for the STM32 cryptographic library (CMOX)*/
#include "Middleware/STM32_Cryptographic/include/cmox_crypto.h"


/* Purpose: Initializes the cryptographic library required for secure operations.
Return Value: Returns a status code of type stse_ReturnCode_t.
STSE_OK: Indicates successful initialization.
STSE_PLATFORM_CRYPTO_INIT_ERROR: Indicates an error occurred during initialization.*/
stse_ReturnCode_t stse_platform_crypto_init(void)
{
    
    stse_ReturnCode_t ret = STSE_OK;

    /*- Calls the cmox_initialize function to initialize the STM32 CMOX cryptographic library.*/
    if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    {
        ret = STSE_PLATFORM_CRYPTO_INIT_ERROR;
    }

    return ret;
}

```

## stse_platform_aes.c

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

## stse_platform_crc.c

The stse_platform_crc.c file provides essential CRC16 functions for the STSecureElement library.

- stse_platform_crc16_init:

  - Purpose: Initializes the CRC16 calculation module.
  - Return Value: Returns STSE_OK to indicate successful initialization.
  - Implementation: Perform the host platform crc16 initialise process .

- stse_platform_Crc16_Calculate:
  - Purpose: Calculates the CRC16 checksum for the given buffer.
  - Parameters:
    - pbuffer: Pointer to the buffer.
    - length: Length of the buffer.
  - Return Value: Returns the calculated CRC16 checksum.
  - Implementation: Perform the crc16 Calculate process at platform level to calculate the CRC16 checksum.

- stse_platform_Crc16_Accumulate:
  - Purpose: Accumulates the CRC16 checksum for the given buffer.
  - Parameters:
    - pbuffer: Pointer to the buffer.
    - length: Length of the buffer.
  - Return Value: Returns the accumulated CRC16 checksum.
  - Implementation: Perform the crc16 accumulate process at platform level to calculate the CRC16 checksum.

Below is an example of stse_platform_crc.c file content for adapting the STSELib to a CRC16 driver :

```c
/******************************************************************************
 * \file    stse_platform_crc.c
 * \brief   STSecureElement CRC16 platform file
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
#include "Drivers/crc16/crc16.h"

/**
 * \brief   Initializes the CRC16 calculation module.
 * \return  STSE_OK on success.
 */
stse_ReturnCode_t stse_platform_crc16_init(void)
{
    crc16_Init();

    return STSE_OK;
}

/**
 * \brief   Calculates the CRC16 checksum for the given buffer.
 * \param   pbuffer Pointer to the buffer.
 * \param   length  Length of the buffer.
 * \return  The calculated CRC16 checksum.
 */
PLAT_UI16 stse_platform_Crc16_Calculate(PLAT_UI8 *pbuffer, PLAT_UI16 length)
{
    return crc16_Calculate(pbuffer, length);
}

/**
 * \brief   Accumulates the CRC16 checksum for the given buffer.
 * \param   pbuffer Pointer to the buffer.
 * \param   length  Length of the buffer.
 * \return  The accumulated CRC16 checksum.
 */
PLAT_UI16 stse_platform_Crc16_Accumulate(PLAT_UI8 *pbuffer, PLAT_UI16 length)
{
    return crc16_Accumulate(pbuffer, length);
}
```

## stse_platform_ecc.c



## stse_platform_hash.c
## stse_platform_i2c.c


 
