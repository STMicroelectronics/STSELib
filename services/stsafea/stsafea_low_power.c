/*!
 ******************************************************************************
 * \file	stsafea_low_power.c
 * \brief   STSAFE-A services for low-power modes services (source)
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

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "services/stsafea/stsafea_frame_transfer.h"
#include "services/stsafea/stsafea_low_power.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

/**
 * \brief 		Put STSAFE-A device in hibernate mode
 * \details 	This function call hibernate service to put the device in hibernate
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in]	wake_up_mode 		Event to wake up from (only significant with STSAFE-A devices),
 * 									listed in type \ref stse_hibernate_wake_up_mode_t
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_hibernate(stse_Handle_t *pSTSE,
                                    stse_hibernate_wake_up_mode_t wake_up_mode)

{
    PLAT_UI8 cmd_header = STSAFEA_CMD_HIBERNATE;
    PLAT_UI8 rsp_header;

    if (pSTSE == NULL) {
        return (STSE_SERVICE_HANDLER_NOT_INITIALISED);
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, e_wake_up_mode, 1, (PLAT_UI8 *)&wake_up_mode);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, 1, &rsp_header);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(pSTSE,
                                      &CmdFrame,
                                      &RspFrame,
                                      stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
