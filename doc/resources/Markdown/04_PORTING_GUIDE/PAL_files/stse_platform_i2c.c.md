# stse_platform_i2c.c {#stse_platform_i2c}

The `stse_platform_i2c.c` file provides I2C communication functions for the STSecureElement library.

## stse_platform_i2c_init:

- **Purpose**: Initializes the I2C bus.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that initializes the I2C bus.

## stse_platform_i2c_wake:

- **Purpose**: Wakes up the I2C device.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that wakes up the I2C device.

## stse_platform_i2c_send_start:

- **Purpose**: Starts an I2C send operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `frame_length`: Length of the I2C frame.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_BUFFER_ERR` on buffer overflow.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that starts an I2C send operation.

## stse_platform_i2c_send_continue:

- **Purpose**: Continues an I2C send operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `p_data`: Pointer to the data to send.
  - `data_size`: Size of the data to send.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that continues an I2C send operation.

## stse_platform_i2c_send_stop:

- **Purpose**: Stops an I2C send operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `p_data`: Pointer to the data to send.
  - `data_size`: Size of the data to send.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_BUS_ACK_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that stops an I2C send operation.

## stse_platform_i2c_receive_start:

- **Purpose**: Starts an I2C receive operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `frame_length`: Length of the I2C frame.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_BUS_ACK_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that starts an I2C receive operation.

## stse_platform_i2c_receive_continue:

- **Purpose**: Continues an I2C receive operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `p_data`: Pointer to the data buffer.
  - `data_size`: Size of the data to receive.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_BUFFER_ERR` on buffer overflow.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that continues an I2C receive operation.

## stse_platform_i2c_receive_stop:

- **Purpose**: Stops an I2C receive operation.
- **Parameters**:
  - `bus_id`: Identifier for the I2C bus.
  - `dev_addr`: I2C device address.
  - `speed`: I2C bus speed.
  - `p_data`: Pointer to the data buffer.
  - `data_size`: Size of the data to receive.
- **Return Value**: Returns `STSE_OK` on success.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that stops an I2C receive operation.

## Implementation example:

Please find below an example of the `stse_platform_i2c.c` implementation for the STM32 platform:

```c
/******************************************************************************
 * \file    stse_platform_i2c.c
 * \brief   STSecureElement Services platform (source)
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

#include "core/stse_platform.h"

#ifdef STSAFE_I2C_STATIC

#include <stdlib.h>
#include "drivers/i2c/I2C.h"

static PLAT_UI8 I2c_buffer[755U];        // Set to A120 max input buffer size + 2 bytes needed for response length + 1 byte for command or response header. Shall be adapted to applicative use case!
static PLAT_UI16 i2c_frame_size;
static volatile PLAT_UI16 i2c_frame_offset;

/**
 * \brief   Initializes the I2C bus.
 * \param   bus_id Identifier for the I2C bus.
 * \return  STSE_OK on success.
 */
stse_return_code_t stse_platform_i2c_init (PLAT_UI8 bus_id)
{
    (void) bus_id;
    return (stse_return_code_t)i2c_init(I2C1);
}

/**
 * \brief   Wakes up the I2C device.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \return  STSE_OK on success.
 */
stse_return_code_t stse_platform_i2c_wake (PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed)
{
    (void) bus_id;
    (void) speed;

    i2c_wake(I2C1, dev_addr);

    return (STSE_OK);
}

/**
 * \brief   Starts an I2C send operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   frame_length Length of the I2C frame.
 * \return  STSE_OK on success, STSE_PLATFORM_BUFFER_ERR on buffer overflow.
 */
stse_return_code_t stse_platform_i2c_send_start (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI16 frame_length)
{
    (void)bus_id;
    (void)dev_addr;
    (void)speed;

    /* - Check buffer overflow */
    if (frame_length > sizeof(I2c_buffer)/sizeof(I2c_buffer[0]))
    {
        return STSE_PLATFORM_BUFFER_ERR;
    }

    i2c_frame_size = frame_length;
    i2c_frame_offset = 0;

    return STSE_OK;
}

/**
 * \brief   Continues an I2C send operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   p_data Pointer to the data to send.
 * \param   data_size Size of the data to send.
 * \return  STSE_OK on success.
 */
stse_return_code_t stse_platform_i2c_send_continue (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI8* p_data,
        PLAT_UI16 data_size)
{
    (void)bus_id;
    (void)dev_addr;
    (void)speed;

    if(data_size != 0)
    {
        if(p_data == NULL)
        {
            memset((I2c_buffer + i2c_frame_offset),0x00,data_size);
        }
        else
        {
            memcpy((I2c_buffer + i2c_frame_offset),p_data,data_size);
        }
        i2c_frame_offset += data_size;
    }

    return STSE_OK;
}

/**
 * \brief   Stops an I2C send operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   p_data Pointer to the data to send.
 * \param   data_size Size of the data to send.
 * \return  STSE_OK on success, STSE_PLATFORM_BUS_ACK_ERROR on failure.
 */
stse_return_code_t stse_platform_i2c_send_stop (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI8* p_data,
        PLAT_UI16 data_size)
{
    stse_return_code_t ret;

    ret =  stse_platform_i2c_send_continue (
            bus_id,
            dev_addr,
            speed,
            p_data,
            data_size);

    /* - Send I2C frame buffer */
    if (ret == STSE_OK)
    {
        ret = (stse_return_code_t)i2c_write(I2C1, dev_addr,speed,I2c_buffer,i2c_frame_size);
    }

    if (ret != STSE_OK)
    {
        ret = STSE_PLATFORM_BUS_ACK_ERROR;
    }

    return ret;
}

/**
 * \brief   Starts an I2C receive operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   frame_length Length of the I2C frame.
 * \return  STSE_OK on success, STSE_PLATFORM_BUS_ACK_ERROR on failure.
 */
stse_return_code_t stse_platform_i2c_receive_start (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI16 frame_length)
{
    (void)bus_id;
    PLAT_I8 ret = 0;

    /* - Store response Length */
    i2c_frame_size = frame_length;

    /* - Read full Frame */
    ret = i2c_read(I2C1, dev_addr,speed,I2c_buffer,i2c_frame_size);
    if(ret != 0)
    {
        return STSE_PLATFORM_BUS_ACK_ERROR;
    }

    /* - Reset read offset */
    i2c_frame_offset = 0;

    return STSE_OK;
}

/**
 * \brief   Continues an I2C receive operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   p_data Pointer to the data buffer.
 * \param   data_size Size of the data to receive.
 * \return  STSE_OK on success, STSE_PLATFORM_BUFFER_ERR on buffer overflow.
 */
stse_return_code_t stse_platform_i2c_receive_continue (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI8* p_data,
        PLAT_UI16 data_size)
{
    (void)bus_id;
    (void)dev_addr;
    (void)speed;

    if (p_data != NULL)
    {

        /* Check read overflow */
        if((i2c_frame_size - i2c_frame_offset) < data_size)
        {
            return STSE_PLATFORM_BUFFER_ERR;
        }

        /* Copy buffer content */
        memcpy(p_data,(I2c_buffer + i2c_frame_offset),data_size);
    }

    i2c_frame_offset += data_size;

    return STSE_OK;
}

/**
 * \brief   Stops an I2C receive operation.
 * \param   bus_id Identifier for the I2C bus.
 * \param   dev_addr I2C device address.
 * \param   speed I2C bus speed.
 * \param   p_data Pointer to the data buffer.
 * \param   data_size Size of the data to receive.
 * \return  STSE_OK on success.
 */
stse_return_code_t stse_platform_i2c_receive_stop (
        PLAT_UI8 bus_id,
        PLAT_UI8 dev_addr,
        PLAT_UI16 speed,
        PLAT_UI8* p_data,
        PLAT_UI16 data_size)
{
    /*- Copy last element*/
    stse_platform_i2c_receive_continue(bus_id, dev_addr, speed, p_data, data_size);

    i2c_frame_offset = 0;

    return (STSE_OK);
}

#endif
```
