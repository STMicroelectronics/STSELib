
/*!
 *******************************************************************************
 * \file	stse_device_management.h
 * \brief   STSAFE-Axxx Middleware SE Management API (header)
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
#ifndef STSAFE_SE_MANAGEMENT_H
#define STSAFE_SE_MANAGEMENT_H

/* Includes ------------------------------------------------------------------*/

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_password.h"
#include "services/stsafea/stsafea_low_power.h"
#include "services/stsafea/stsafea_reset.h"
#include "services/stsafea/stsafea_commands.h"

/** \defgroup 	stse_device_management 	STSE Device Management
 *  \ingroup 	stse_api
 *  \brief		STSecureElement middleware device Management API
 *  \details  	The Application Programming Interface (API) layer is the entry point for the upper system application layer. \n
 *  			It provides high level functions to the application layer for secure element management.
 *  @{
 */

/* Exported Functions  ------------------------------------------------------------*/

/**
 * \brief 		Initialize target device
 * \details 	This function call the handler initialization function from core layer
 *          	to initialize STSAFE handler in argument
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_init.dox
 */
stse_ReturnCode_t stse_init(stse_Handler_t *pSTSE);

/**
 * \brief 		Reset target device
 * \details 	This function call reset service to reset the device
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_reset.dox
 */
stse_ReturnCode_t stse_device_reset(stse_Handler_t *pSTSE);

/**
 * \brief 		Put target device in hibernate mode
 * \details 	This function call hibernate service to put the device in hibernate
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in]	wake_up_mode 		Event to wake up from,
 * 									listed in enum \ref stsafea_hibernate_wake_up_mode_t
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_hibernate.dox
 */
stse_ReturnCode_t stse_device_enter_hibernate(stse_Handler_t *pSTSE, stsafea_hibernate_wake_up_mode_t wake_up_mode);

/**
 * \brief 		Power-on target device
 * \details 	This function power-on the target device
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_power_control.dox
 */
stse_ReturnCode_t stse_device_power_on(stse_Handler_t *pSTSE);

/**
 * \brief 		Power-off target device
 * \details 	This function power-off the target device
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	Please refer to stse_device_power_on()
 */
stse_ReturnCode_t stse_device_power_off(stse_Handler_t *pSTSE);

/**
 * \brief 		Send echo command
 * \details 	This function call send service to send an echo command
 * \param[in]  	pSTSE 			Pointer to STSAFE Handler
 * \param[in]  	pIn 				Pointer to data buffer to be sent
 * \param[out] 	pOut 				Pointer to received data buffer
 * \param[in]  	size 				Size in bytes of pIn buffer
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_echo.dox
 */
stse_ReturnCode_t stse_device_echo(stse_Handler_t *pSTSE, PLAT_UI8 *pIn, PLAT_UI8 *pOut, PLAT_UI16 size);

/**
 * \brief 		Lock target device
 * \details 	This function lock the target device using the password in argument
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	pPassword 			Pointer to the password buffer
 * \param[in]	password_length 	Length of the password buffer in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_lock.dox
 */
stse_ReturnCode_t stse_device_lock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length);

/**
 * \brief 		Unlock target device
 * \details 	This function unlock the target device using the password in argument
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	pPassword 			Pointer to the password buffer
 * \param[in]	password_length 	Length of the password buffer in bytes
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_unlock.dox
 */
stse_ReturnCode_t stse_device_unlock(stse_Handler_t *pSTSE, PLAT_UI8 *pPassword, PLAT_UI8 password_length);

/**
 * \brief 		Return the record count of command access conditions
 * \details 	This function query the access conditions
 * 				of the target device command set and return the number of records
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[out] 	record_count 		Command authorization records count
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	Please refer to stse_device_get_command_AC()
 */
stse_ReturnCode_t stse_device_get_command_count(stse_Handler_t *pSTSE, PLAT_UI8 * record_count);


/**
 * \brief 		Return the command access conditions and change right
 * \details 	This function query the access conditions
 * 				of the target device command set
 * \param[in] 	pSTSE 			Pointer to STSAFE Handler
 * \param[in] 	record_count 		Command authorization records count
 * \param[out] 	pChange_rights 		Pointer to change rights structure of the commands AC and host encrytpion flag
 * \param[out] 	pRecord_table 		Command authorization records table
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_get_command_AC.dox
 */
stse_ReturnCode_t stse_device_get_command_AC_records(stse_Handler_t *pSTSE,
										 PLAT_UI8 record_count,
										 stse_cmd_authorization_CR_t *pChange_rights,
										 stse_cmd_authorization_record_t *pRecord_table);

/**
 * \brief 		Return the command access conditions and change right
 * \details 	This function query the access conditions of the target device command set
 * \param[in] 	pSTSE 					Pointer to STSAFE Handler
 * \param[out] 	pLife_cycle_state 		Pointer to \ref stsafea_life_cycle_state_t variable to be updated
 * \return 		\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_get_life_cycle_state.dox
 */
stse_ReturnCode_t stse_device_get_life_cycle_state(stse_Handler_t *pSTSE,
		stsafea_life_cycle_state_t *pLife_cycle_state);

/** \}*/


#endif /* STSAFE_SE_MANAGEMENT_H */

