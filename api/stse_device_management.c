/*!
 ******************************************************************************
 * \file    stse_device_management.c
 * \brief   STSE device management API set (sources)
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
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

#include "api/stse_device_management.h"

/* Exported variables --------------------------------------------------------*/
#define I2C_ADDR_MAX 0x7F
#define IDLE_BUS_DELAY_MAX 0x1F

/* Exported functions --------------------------------------------------------*/
stse_ReturnCode_t stse_init_platform(void *pArg) {
    stse_ReturnCode_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

    ret = stse_platform_generate_random_init(pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    ret = stse_platform_delay_init(pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    ret = stse_platform_power_init(pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    ret = stse_platform_crc16_init(pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    ret = stse_platform_crypto_init(pArg);

    return ret;
}

stse_ReturnCode_t stse_init_bus(stse_io_t *io, void *pArg) {
    stse_ReturnCode_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

    switch (io->BusType) {
#if defined(STSE_CONF_STSAFE_A_SUPPORT) || \
    (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
    case STSE_BUS_TYPE_I2C:
        ret = stse_platform_i2c_init(io->busID, pArg);
        if (ret != STSE_OK) {
            return ret;
        }
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT && defined(STSE_CONF_USE_I2C) */
#ifdef STSE_CONF_USE_ST1WIRE
    case STSE_BUS_TYPE_ST1WIRE:
        ret = stse_platform_st1wire_init(io->busID, pArg);
        if (ret != STSE_OK) {
            return ret;
        }

        io->BusSendStart = stse_platform_st1wire_send_start;
        io->BusSendContinue = stse_platform_st1wire_send_continue;
        io->BusSendStop = stse_platform_st1wire_send_stop;
        io->BusRecvStart = stse_platform_st1wire_receive_start;
        io->BusRecvContinue = stse_platform_st1wire_receive_continue;
        io->BusRecvStop = stse_platform_st1wire_receive_stop;
        break;
#endif /* STSE_CONF_USE_ST1WIRE */

    default:
        return (STSE_PLATFORM_INVALID_PARAMETER);
    }

    return ret;
}

stse_ReturnCode_t stse_init_device(stse_Handle_t *pSTSE, void *pArg) {
    stse_ReturnCode_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    PLAT_UI8 mask_id[STSAFEA_MASK_ID_SIZE];
    PLAT_UI16 mask_number;
    PLAT_UI8 st_number[STSAFEA_ST_NUMBER_SIZE];
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

    ret = stse_device_power_on(pSTSE);
    if (ret != STSE_OK) {
        return ret;
    }

#ifdef STSE_CONF_STSAFE_A_SUPPORT
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    if (pSTSE->device_type != STSAFE_L010) {
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#ifndef STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS
        ret = stsafea_query_mask_id_st_number(pSTSE, mask_id, st_number);
        if (ret != STSE_OK) {
            return ret;
        }

        mask_number = (mask_id[STSAFEA_MASK_ID_SIZE - 2] << 8) + mask_id[STSAFEA_MASK_ID_SIZE - 1];
        if (mask_number >= 0x6000) {
            pSTSE->device_type = STSAFE_A120;
        } else if (mask_number >= 0x4600) {
            pSTSE->device_type = STSAFE_A110;
        } else if (mask_number >= 0x4000) {
            pSTSE->device_type = STSAFE_A100;
        } else {
            return (STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE);
        }

        ret = stsafea_perso_info_update(pSTSE);
#endif /* STSE_CONF_USE_STATIC_PERSONALIZATION_INFORMATIONS */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
#endif /* STSE_CONF_STSAFE_A_SUPPORT */

    return ret;
}

stse_ReturnCode_t stse_init(stse_Handle_t *pSTSE, void *pArg) {
    stse_ReturnCode_t ret = STSE_PLATFORM_UNEXPECTED_ERROR;

    /* - Check STSAFE handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    /* - Initialize communication bus */
    ret = stse_init_bus(&pSTSE->io, pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Initialize Host platform */
    ret = stse_init_platform(pArg);
    if (ret != STSE_OK) {
        return ret;
    }

    /* - Initialize STSE device */
    ret = stse_init_device(pSTSE, pArg);

    return ret;
}

stse_ReturnCode_t stse_device_power_on(stse_Handle_t *pSTSE) {
    /* - Check STSE handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    /* - Check STSE PowerLineOn callback initialization */
    if (pSTSE->io.PowerLineOn == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    /* - Power-on the device */
    pSTSE->io.PowerLineOn(pSTSE->io.busID, pSTSE->io.Devaddr);

    /* - Wait for device to boot (tboot) */
    switch (pSTSE->device_type) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    case STSAFE_A100:
    case STSAFE_A110:
    case STSAFE_A120:
        stse_platform_Delay_ms(stsafea_boot_time[pSTSE->device_type - STSAFE_A100]);
        break;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
    case STSAFE_L010:
        stse_platform_Delay_ms(stsafel_boot_time[pSTSE->device_type - STSAFE_L010]);
        break;
#endif /* STSE_CONF_STSAFE_L_SUPPORT */
    default:
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
    return (STSE_OK);
}

stse_ReturnCode_t stse_device_power_off(stse_Handle_t *pSTSE) {
    /* - Check STSE handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

    /* - Check STSE PowerLineOff callback initialization */
    if (pSTSE->io.PowerLineOff == NULL) {
        return (STSE_API_INVALID_PARAMETER);
    }

    /* - Power-Off the device */
    pSTSE->io.PowerLineOff(pSTSE->io.busID, pSTSE->io.Devaddr);
    return (STSE_OK);
}

stse_ReturnCode_t stse_device_lock(stse_Handle_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    stse_ReturnCode_t ret;
    PLAT_UI8 password_verification_status = 0;

    /* - Check STSAFE handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    if (pSTSE->device_type == STSAFE_L010) {
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

    if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH) {
        return STSE_API_INVALID_PARAMETER;
    }

    /*- Password submission */
    ret = stsafea_verify_password(pSTSE,
                                  pPassword,
                                  password_length,
                                  &password_verification_status,
                                  NULL);

    if (ret != STSE_OK) {
        return ret;
    }

    if (password_verification_status == 0) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* - Switch device Life-cycle to Lock */
    ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL_AND_LOCKED);

    return ret;
#else
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}

stse_ReturnCode_t stse_device_unlock(stse_Handle_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length) {
#ifdef STSE_CONF_STSAFE_A_SUPPORT
    stse_ReturnCode_t ret;
    PLAT_UI8 password_verification_status = 0;
    PLAT_UI8 remaining_tries;

    /* - Check STSAFE handler initialization */
    if (pSTSE == NULL) {
        return (STSE_API_HANDLER_NOT_INITIALISED);
    }

#ifdef STSE_CONF_STSAFE_L_SUPPORT
    if (pSTSE->device_type == STSAFE_L010) {
        return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
    }
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

    if (pPassword == NULL || password_length != STSAFEA_PASSWORD_LENGTH) {
        return STSE_API_INVALID_PARAMETER;
    }

    /*- Password submission */
    ret = stsafea_verify_password(pSTSE,
                                  pPassword,
                                  password_length,
                                  &password_verification_status,
                                  &remaining_tries);

    if (ret != STSE_OK) {
        return ret;
    }

    if (password_verification_status == 0) {
        return STSE_API_INVALID_PARAMETER;
    }

    /* - Switch device Life-cycle to operational */
    ret = stsafea_put_life_cyle_state(pSTSE, STSAFEA_LCS_OPERATIONAL);

    return ret;
#else
    return STSE_API_INCOMPATIBLE_DEVICE_TYPE;
#endif /* STSE_CONF_STSAFE_A_SUPPORT */
}
