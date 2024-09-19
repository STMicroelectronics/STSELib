 /******************************************************************************
 * \file	stsafel_timings.c
 * \brief   STSAFE-L Timings definitions
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "services/stsafel/stsafel_timings.h"

#define STSAFEL_EXEC_TIME_DEFAULT       500U /*!< STSAFE-L default command processing time (used when specific time == 0) */
#define STSAFEL_BOOT_TIME_DEFAULT       10U
#define STSAFEL_WAKEUP_TIME_DEFAULT     10U

const PLAT_UI16 stsafel_cmd_timings[STSAFEL_PRODUCT_COUNT][STSAFEL_MAX_CMD_COUNT] = {
    /* STSAFE_L010 */
    {
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L general purpose "Echo" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L general purpose "Reset" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L general purpose "Reset" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L general purpose "Get data" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L general purpose "Get Public key certificate" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L data partition "Decrement" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L data partition "Read" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L data partition "Update" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L "Establish PAIRING Key [resume]" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L "Generate Signature" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L "Put Data" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L "Regenerate" command code */
        STSAFEL_EXEC_TIME_DEFAULT, /*!< STSAFE-L "Repeat" command code */
    }
};

const PLAT_UI16 stsafel_boot_time[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_BOOT_TIME_DEFAULT, /* STSAFE_L010 */
};

const PLAT_UI16 stsafel_wakeup_time[STSAFEL_PRODUCT_COUNT] = {
    STSAFEL_WAKEUP_TIME_DEFAULT, /* STSAFE_L010 */
};


