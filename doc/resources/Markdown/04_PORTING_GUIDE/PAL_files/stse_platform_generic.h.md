
# stse_platform_generic.h{#stse_platform_generic}

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

