# stse_platform_delay.c {#stse_platform_delay}

The `stse_platform_delay.c` file provides delay functions for the STSecureElement library, abstracting the platform-specific details of delay and timeout processes.

## stse_platform_delay_init:

- **Purpose**: Initializes the delay platform.
- **Parameters**: None.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the delay mechanisms.

## stse_platform_Delay_ms:

- **Purpose**: Delays execution for a specified number of milliseconds.
- **Parameters**:
  - `delay_val`: Number of milliseconds to delay.
- **Return Value**: None.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that performs a delay in milliseconds.

## stse_platform_timeout_ms_start:

- **Purpose**: Starts a timeout with a specified duration in milliseconds.
- **Parameters**:
  - `timeout_val`: Duration of the timeout in milliseconds.
- **Return Value**: None.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that starts a timeout.

## stse_platform_timeout_ms_get_status:

- **Purpose**: Gets the status of the timeout.
- **Parameters**: None.
- **Return Value**: Returns the status of the timeout as a `PLAT_UI8`.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that gets the status of the timeout.

## Implementation Example:

Please find below an example of the `stse_platform_delay` implementation for the STM32 platform:

```c
/******************************************************************************
 * \file    stse_platform_delay.c
 * \brief   STSecureElement delay platform file
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
#include "Drivers/delay_ms/delay_ms.h"
#include "Drivers/delay_us/delay_us.h"

stse_ReturnCode_t stse_platform_delay_init(void)
{
    /* Initialize platform Drivers used by PAL */
    delay_ms_init();
    //delay_us_init();

    return STSE_OK;
}

void stse_platform_Delay_ms(PLAT_UI32 delay_val)
{
    delay_ms(delay_val);
}

void stse_platform_timeout_ms_start(PLAT_UI16 timeout_val)
{
    timeout_ms_start(timeout_val);
}

PLAT_UI8 stse_platform_timeout_ms_get_status(void)
{
    return timeout_ms_get_status();
}
```

This example demonstrates how to initialize the delay mechanisms, perform a delay in milliseconds, start a timeout, and get the status of the timeout using platform-specific drivers.
