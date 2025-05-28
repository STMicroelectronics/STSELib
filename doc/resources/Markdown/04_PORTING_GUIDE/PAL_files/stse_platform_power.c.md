# stse_platform_power.c{#stse_platform_power}

The `stse_platform_power.c` file provides power control functions for the STSecureElement library.

## stse_platform_power_init:

- **Purpose**: Initializes the Secure Elements power control lines.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the power control lines.

## stse_platform_power_on:

- **Purpose**: Powers on the secure element device.
- **Parameters**:
  - `bus`: Identifier for the bus.
  - `devAddr`: Device address.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that powers on the device.

## stse_platform_power_off:

- **Purpose**: Powers off the device.
- **Parameters**:
  - `bus`: Identifier for the bus.
  - `devAddr`: Device address.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that powers off the device.

## Implementation example:

Please find below an example of the `stse_platform_power.c` implementation for the STM32 platform:

```c
/******************************************************************************
 * \file    stse_platform_power.c
 * \brief   STSecureElement power platform file
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
#include "stm32l4xx.h"

/**
 * \brief   Initializes the power control lines.
 * \return  STSE_OK on success.
 */
stse_ReturnCode_t stse_platform_power_init (void)
{
    /* -Initialize power line control (PC0  - open-drain) */
    GPIOC->MODER    &=  ~(GPIO_MODER_MODE0_Msk);
    GPIOC->MODER    |=  (1 << GPIO_MODER_MODE0_Pos);
    GPIOC->ODR      &=  ~(1 << GPIO_ODR_OD0_Pos);

    /* -Initialize power line control (PC1  - open-drain) */
    GPIOC->MODER    &=  ~(GPIO_MODER_MODE1_Msk);
    GPIOC->MODER    |=  (1 << GPIO_MODER_MODE1_Pos);
    GPIOC->ODR      &=  ~(1 << GPIO_ODR_OD1_Pos);

    /* -Initialize power line control (PB0  - open-drain) */
    GPIOB->MODER    &=  ~(GPIO_MODER_MODE0_Msk);
    GPIOB->MODER    |=  (1 << GPIO_MODER_MODE0_Pos);
    GPIOB->ODR      &=  ~(1 << GPIO_ODR_OD0_Pos);

    return STSE_OK;
}

/**
 * \brief   Powers on the device.
 * \param   bus Identifier for the bus.
 * \param   devAddr Device address.
 * \return  STSE_OK on success.
 */
stse_ReturnCode_t stse_platform_power_on (PLAT_UI8 bus , PLAT_UI8 devAddr)
{
    (void)bus;
    (void)devAddr;

    /* - Power on all the STSAFE SLOTS */
    GPIOB->ODR &= ~(1 << GPIO_ODR_OD0_Pos);
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD0_Pos);
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);

    return ( STSE_OK );
}

/**
 * \brief   Powers off the device.
 * \param   bus Identifier for the bus.
 * \param   devAddr Device address.
 * \return  STSE_OK on success.
 */
stse_ReturnCode_t stse_platform_power_off (PLAT_UI8 bus , PLAT_UI8 devAddr)
{
    (void)bus;
    (void)devAddr;

    /* - Power-off all the STSAFE SLOTS */
    GPIOB->ODR |= (1 << GPIO_ODR_OD0_Pos);
    GPIOC->ODR |= (1 << GPIO_ODR_OD0_Pos);
    GPIOC->ODR |= (1 << GPIO_ODR_OD1_Pos);

    return(STSE_OK);
}
```