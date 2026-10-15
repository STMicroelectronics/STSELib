/*!
 ******************************************************************************
 * \file    stsafel_low_power.c
 * \brief   STSAFE-L services for low-power modes services (source)
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_echo.h"
#include "services/stsafel/stsafel_frame_transfer.h"
#include "services/stsafel/stsafel_low_power.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT

/**
 * \brief 		Put STSAFE-L device in hibernate mode
 * \details 	This function call hibernate service to put the device in hibernate
 * \param[in] 	p_stse 				Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stsafel_hibernate(stse_handle_t *p_stse) {
    PLAT_UI8 cmd_header = STSAFEL_CMD_HIBERNATE;
    PLAT_UI8 rsp_header;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(cmd_frame);
    stse_frame_element_allocate_push(&cmd_frame, e_cmd_header, STSAFEL_HEADER_SIZE, &cmd_header);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(rsp_frame);
    stse_frame_element_allocate_push(&rsp_frame, e_rsp_header, STSAFEL_HEADER_SIZE, &rsp_header);

    /*- Perform Transfer*/
    return stsafel_frame_transfer(p_stse,
                                  &cmd_frame,
                                  &rsp_frame);
}

stse_return_code_t stsafel_wakeup(stse_handle_t *p_stse) {
    stse_return_code_t ret;

    if (p_stse == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /* When device wakeup callback is available, call it */
    if (p_stse->io.bus_wake != NULL) {
        ret = p_stse->io.bus_wake(
            p_stse->io.bus_id,
            p_stse->io.devaddr,
            p_stse->io.bus_speed);
    }
#ifdef STSE_CONF_USE_I2C
    /* When wakeup callback isn't available but bus type is I²C, send a small echo command */
    else if (p_stse->io.bus_type == STSE_BUS_TYPE_I2C) {
        PLAT_UI8 echo_message[1] = {0x00};
        ret = stsafel_echo(p_stse, echo_message, echo_message, 1);
    }
#endif /* STSE_CONF_USE_I2C */
    /* If wakeup callback isn't available and bus type isn't I²C, return an error */
    else {
        ret = STSE_SERVICE_INVALID_PARAMETER;
    }

    return ret;
}

#endif /* STSE_CONF_STSAFE_L_SUPPORT */
