
# stse_platform_crc.c{#stse_platform_crc}

The stse_platform_crc.c file provides CRC16 functions for the STSecureElement library, abstracting the platform-specific details of CRC16 calculations. By using these functions, developers can ensure consistent and reliable CRC16 operations across different hardware platforms.


## stse_platform_crc16_init:

- Purpose: Initializes the CRC16 calculation module.
- Return Value: Returns STSE_OK to indicate successful initialization.

<b> Implementation directives :</b> This abstaction function should implement or call a platform function/driver that perform CRC16 calculation process.

## stse_platform_Crc16_Calculate:

- Purpose: Calculates the CRC16 checksum for the given buffer.
- Parameters:
  - pbuffer: Pointer to the buffer.
  - length: Length of the buffer.
- Return Value: Returns the calculated CRC16 checksum.

<b> Implementation directives :</b> This abstaction function should implement or call a platform function/driver that perform CRC16 calculation process.

## stse_platform_Crc16_Accumulate:

- Purpose: Accumulates the CRC16 checksum for the given buffer.
- Parameters:
  - pbuffer: Pointer to the buffer.
  - length: Length of the buffer.
- Return Value: Returns the accumulated CRC16 checksum.

<b> Implementation directives :</b> This abstaction function should implement or call a platform function/driver that perform CRC16 acumulate process.

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
