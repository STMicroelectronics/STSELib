
# stse_platform_crc.c{#stse_platform_crc}

The stse_platform_crc.c file provides CRC16 functions for the STSecureElement library, abstracting the platform-specific details of CRC16 calculations. By using these functions, developers can ensure consistent and reliable CRC16 operations across different hardware platforms.


## stse_platform_crc16_init:

- Purpose: Initializes the CRC16 calculation module.
- Return Value: Returns STSE_OK to indicate successful initialization.

<b> Implementation directives :</b> This abstaction function should implement or call a platform function/driver that perform CRC16 calculation process.

## stse_platform_Crc16_ContextInit:

- Purpose: Initializes a CRC16 context.
- Parameters:
  - crc16_context: Pointer to the CRC16 context to initialize.
- Return Value: Returns `STSE_OK` to indicate successful context initialization.

<b> Implementation directives :</b> This abstraction function should implement or call a platform function/driver that perform CRC16 accumulate process.

## stse_platform_Crc16_Accumulate:

- Purpose: Accumulates the CRC16 checksum for the given buffer.
- Parameters:
  - pbuffer: Pointer to the buffer.
  - length: Length of the buffer.
  - crc16_context: Pointer to the CRC context holder previously initialized successfully via `stse_platform_Crc16_ContextInit()`.
    This context is used internally to maintain the state required for incremental CRC computation across multiple calls to `stse_platform_Crc16_Accumulate()`.
- Return Value: Returns the accumulated CRC16 checksum.

<b> Implementation directives :</b> This abstaction function should implement or call a platform function/driver that perform CRC16 acumulate process.

## stse_platform_Crc16_ContextRelease:

- Purpose: Releases resources associated with a CRC16 context.
- Parameters:
  - crc16_context: Pointer to a CRC context previously initialized with `stse_platform_Crc16_ContextInit()`.

<b> Implementation directives :</b> This abstraction function should implement or call a platform function/driver that perform CRC16 accumulate process.

Please find below an extract

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
stse_ReturnCode_t stse_platform_crc16_init(void *pArg)
{
    crc16_Init();

    return STSE_OK;
}

/**
 * \brief Initializes a CRC16 context.
 *
 * Initializes the specified CRC16 context and prepares it for use with `stse_platform_Crc16_Accumulate()`.
 * The same context instance shall be passed unchanged to all subsequent accumulation calls.
 *
 * \param  crc16_context  Pointer to the CRC16 context to initialize.
 * \return \ref STSE_OK on success; \ref stse_ReturnCode_t error code otherwise.
 */
stse_ReturnCode_t stse_platform_Crc16_ContextInit(stse_crc16_context_t *crc16_context)
{
    // If hardware CRC is used...
    // mutex_lock() if the same CRC hardware module is used by multiple thread
    __HAL_CRC_DR_RESET(&stm32_crc_hal);

    // or just initialize the context if CRC computation is done in software
    crc16_context->value = (PLAT_UI16)0xFFFF;
}

/**
 * \brief   Accumulates the CRC16 checksum for the given buffer.
 * \param   pbuffer       Pointer to the buffer.
 * \param   length        Length of the buffer.
 * \param   crc16_context Pointer to the CRC context holder previously initialized successfully
 * via `stse_platform_Crc16_ContextInit()`. This context is used internally to maintain the state
 * required for incremental CRC computation across multiple calls to `stse_platform_Crc16_Accumulate()`.
 * \return  The accumulated CRC16 checksum.
 */
PLAT_UI16 stse_platform_Crc16_Accumulate(PLAT_UI8 *pbuffer, PLAT_UI16 length, 
                                         stse_crc16_context_t *crc16_context);
{
    // If hardware CRC computation is used...
    return ~HAL_CRC_Accumulate(&stm32_crc_hal, (uint32_t *)pbuffer, length);

    // If software CRC computation is used...
    return crc16_Accumulate(pbuffer, length, crc16_context);
}

/**
 * \brief Releases resources associated with a CRC16 context.
 * 
 * Releases any resources allocated or acquired by `stse_platform_Crc16_ContextInit()`. Once 
 * released, the context shall no longer be used unless it is reinitialized.
 * 
 * \param crc16_context Pointer to a CRC context previously initialized with `stse_platform_Crc16_ContextInit()`.
 */
void stse_platform_Crc16_ContextRelease(stse_crc16_context_t *crc16_context)
{
    // mutex_unlock() if the same CRC hardware module is used by multiple thread
}
```
