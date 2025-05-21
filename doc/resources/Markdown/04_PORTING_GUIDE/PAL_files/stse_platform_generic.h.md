# stse_platform_generic.h {#stse_platform_generic}

The `stse_platform_generic.h` file provides generic platform definitions and includes for the STSecureElement library, abstracting the platform-specific details of data types and includes.

## Definitions:

### Data Types:

- **PLAT_UI8**: Defines an 8-bit unsigned integer (`uint8_t`).
- **PLAT_UI16**: Defines a 16-bit unsigned integer (`uint16_t`).
- **PLAT_UI32**: Defines a 32-bit unsigned integer (`uint32_t`).
- **PLAT_UI64**: Defines a 64-bit unsigned integer (`uint64_t`).
- **PLAT_I8**: Defines an 8-bit signed integer (`int8_t`).
- **PLAT_I16**: Defines a 16-bit signed integer (`int16_t`).
- **PLAT_I32**: Defines a 32-bit signed integer (`int32_t`).

### Struct Packing:

- **PLAT_PACKED_STRUCT**: Defines a packed structure (`__PACKED`).

## Includes:

- **stm32l4xx.h**: Includes the STM32L4xx header file.
- **stddef.h**: Includes the standard definitions header file.
- **string.h**: Includes the string manipulation functions header file.
- **stdio.h**: Includes the standard input/output functions header file.

## Implementation Example:

Please find below an example of the `stse_platform_generic` header file for the STM32 platform:

```c
/******************************************************************************
 * \file    stse_platform_generic.h
 * \brief   STSecureElement generic platform file
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

#ifdef __cplusplus
}
#endif

#endif /* STSE_PLATFORM_GENERIC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
```

This example demonstrates how to define generic data types and includes for the STM32 platform, ensuring compatibility and ease of use within the STSecureElement library.
