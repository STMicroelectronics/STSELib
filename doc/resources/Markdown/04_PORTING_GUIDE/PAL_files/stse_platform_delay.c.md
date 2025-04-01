# stse_platform_delay.c{#stse_platform_delay}

The stse_platform_crypto.c file provides the implementation of delay mechanisms that are used by the mibrary for communication inter-frame retry and command processing timmings management. 
These functions abstract the platform-specific details, allowing developers to a dapt/redirect the delay to any specific host platform 

Below is the content of the stse_platform_crypto.c file:

```c
/******************************************************************************
 * \file    stse_platform_crypto.c
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

/*Includes the configuration header file for STSELib.*/
#include "stse_conf.h"
/*Includes the main STSELib header file*/
#include "stselib.h"
/*Includes the header file for millisecond delay MCU driver*/
#include "Drivers/delay_ms/delay_ms.h"

/*Purpose: Initializes the platform drivers used by the STSELib PAL.*/
stse_ReturnCode_t stse_platform_delay_init(void)
{
    /* initialize the millisecond delay driver*/
    delay_ms_init();

    /*Return Value: Returns STSE_OK to indicate successful initialization*/
    return STSE_OK;
}

/*Purpose: Provides a delay for a specified number of milliseconds.
Parameters provided by the Library : 
-  PLAT_UI32 delay_val : Number of milliseconds to delay.*/
void stse_platform_Delay_ms (PLAT_UI32 delay_val)
{
    /* Calls the delay_ms(delay_val) function to generate the delay.*/
    delay_ms(delay_val);
}

/*Purpose: Starts a timeout for a specified number of milliseconds.
Parameters provided by the Library : 
-  PLAT_UI16 timeout_val : Number of milliseconds for the timeout.*/
void stse_platform_timeout_ms_start(PLAT_UI16 timeout_val)
{
    /* Calls the timeout_ms_start(timeout_val) function to start the timeout.*/
    timeout_ms_start(timeout_val);
}

/* Purpose: Gets the status of the timeout.
Return Value: Returns the status of the timeout as a PLAT_UI8 value.
- 0 : timeout not reached
- 1 : timeout reached */

PLAT_UI8 stse_platform_timeout_ms_get_status(void)
{
    /*Calls the timeout_ms_get_status() function to retrieve the timeout status.*/
    return timeout_ms_get_status();
}

```