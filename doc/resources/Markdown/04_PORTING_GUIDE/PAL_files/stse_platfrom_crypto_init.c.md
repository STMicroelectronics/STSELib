
# stse_platfrom_crypto_init.c{#stse_platfrom_crypto_init}

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