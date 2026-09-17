
/*!
 *******************************************************************************
 * \file	stse_device_management.h
 * \brief   STSE SE Management API (header)
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
#ifndef STSE_MANAGEMENT_H
#define STSE_MANAGEMENT_H

/* Includes ------------------------------------------------------------------*/

#include "core/stse_device.h"
#include "core/stse_return_codes.h"

#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_low_power.h"
#include "services/stsafea/stsafea_password.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_reset.h"

#include "services/stsafel/stsafel_echo.h"
#include "services/stsafel/stsafel_low_power.h"
#include "services/stsafel/stsafel_reset.h"

/** \defgroup 	stse_device_management 	STSE Device Management
 *  \ingroup 	stse_api
 *  \brief		STSecureElement middleware device Management API
 *  \details  	The Application Programming Interface (API) layer is the entry point for the upper system application layer. \n
 *  			It provides high level functions to the application layer for secure element management.
 *  @{
 */

/*!
 * \typedef stse_low_power_mode_t
 * STSAFEA Low power mode type
 */
typedef PLAT_UI8 stse_low_power_mode_t;
#define STSE_LPM_NONE ((stse_zone_ac_change_indicator_t)0)    /*!< No low power mode */
#define STSE_LPM_STANDBY ((stse_zone_ac_change_indicator_t)2) /*!< Standby power mode */

/* Exported Functions  ------------------------------------------------------------*/

/**
 * \brief 		Initialize & setup communication bus
 * \details 	This function setup & initialize the communication bus
 * \param[in] 	io			Pointer to STSE Input/Output type
 * \param[in]   pArg		Argument passed as-is to the `stse_platform_xxx_init()` functions through their `pArg` parameter
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_init_bus(stse_io_t *io, void *pArg);

/**
 * \brief 		Initialize platform
 * \details 	This function initialize platform layer (RNG, delay, power, crc16, crypto)
 * \param[in]   pArg		Argument passed as-is to the `stse_platform_xxx_init()` functions through their `pArg` parameter
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_init.dox
 */
stse_ReturnCode_t stse_init_platform(void *pArg);

/**
 * \brief 		Start target device initialization
 * \details 	This function power on the STSE, setup the STSE device type (stse_device_t) & setup perso information (stse_perso_info_t)
 * \param[in] 	pSTSE 		Pointer to STSE Handler
 * \param[in]   pArg 		Argument passed as-is to the `stse_platform_xxx_init()` functions through their `pArg` parameter
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_init.dox
 */
stse_ReturnCode_t stse_init_device(stse_Handle_t *pSTSE, void *pArg);

/**
 * \brief 		Initialize target device
 * \details 	This function call the handler initialization function from core layer
 *          	to initialize STSE handler in argument
 * \param[in] 	pSTSE 		Pointer to STSE Handler
 * \param[in]   pArg 		Argument passed as-is to the `stse_platform_xxx_init()` functions through their `pArg` parameter
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_init.dox
 */
stse_ReturnCode_t stse_init(stse_Handle_t *pSTSE, void *pArg);

/**
 * \brief 		Power-on target device
 * \details 	This function power-on the target device
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stse_device_power_on(stse_Handle_t *pSTSE);

/**
 * \brief 		Power-off target device
 * \details 	This function power-off the target device
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	Please refer to stse_device_power_on()
 */
stse_ReturnCode_t stse_device_power_off(stse_Handle_t *pSTSE);

/**
 * \brief 		Lock target device
 * \details 	This function lock the target device using the password in argument
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \param[in] 	pPassword 			Pointer to the password buffer
 * \param[in]	password_length 	Length of the password buffer in bytes
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_device_lock.dox
 */
stse_ReturnCode_t stse_device_lock(stse_Handle_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length);

/**
 * \brief 		Unlock target device
 * \details 	This function unlock the target device using the password in argument
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	pPassword 			Pointer to the password buffer
 * \param[in]	password_length 	Length of the password buffer in bytes
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \details 	\include{doc} stse_device_unlock.dox
 */
stse_ReturnCode_t stse_device_unlock(stse_Handle_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length);

/** \}*/

#endif /* STSE_MANAGEMENT_H */
