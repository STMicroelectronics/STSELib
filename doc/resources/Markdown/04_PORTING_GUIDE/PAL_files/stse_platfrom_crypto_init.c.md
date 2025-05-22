# stse_platform_crypto_init.c {#stse_platform_crypto_init}

The `stse_platform_crypto_init.c` file provides cryptographic initialization functions for the STSecureElement library, abstracting the platform-specific details of cryptographic library initialization.

## stse_platform_crypto_init:

- **Purpose**: Initializes the cryptographic platform.
- **Parameters**: None.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_CRYPTO_INIT_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the cryptographic library.

## Implementation Example:

Please find below an example of the `stse_platform_crypto_init` implementation for the STM32 CMOX library:

```c
/******************************************************************************
 * \file    stse_platform_crypto_init.c
 * \brief   STSecureElement cryptographic platform file
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

stse_ReturnCode_t stse_platform_crypto_init(void)
{
    stse_ReturnCode_t ret = STSE_OK;

    /* - Initialize STM32 CMOX library */
    if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    {
        ret = STSE_PLATFORM_CRYPTO_INIT_ERROR;
    }

    return ret;
}
```

This example demonstrates how to initialize the cryptographic library using the STM32 CMOX library, ensuring that the cryptographic platform is ready for use within the STSecureElement library.
