# stse_platform_random.c {#stse_platform_random}

The `stse_platform_random.c` file provides random number generation functions for the STSecureElement library, abstracting the platform-specific details of random number generation.

## stse_platform_generate_random_init:

- **Purpose**: Initializes the random number generator.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the random number generator.

## stse_platform_generate_random:

- **Purpose**: Generates a random number.
- **Return Value**: Returns a random number.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that generates a random number.

## Implementation example:

Please find below an example of the `stse_platform_random.c` implementation for the STM32 platform:

```c
/******************************************************************************
 * \file    stse_platform_random.c
 * \brief   STSecureElement random number generator platform file
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
#include "Drivers/rng/rng.h"

/**
 * \brief   Initializes the random number generator.
 * \return  STSE_OK on success.
 */
stse_ReturnCode_t stse_platform_generate_random_init(void)
{
    rng_start();

    return (STSE_OK);
}

/**
 * \brief   Generates a random number.
 * \return  A random number.
 */
PLAT_UI32 stse_platform_generate_random(void)
{
    return rng_generate_random_number();
}
```
