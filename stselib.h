 /******************************************************************************
 * \file	stsafe_A1xx.h
 * \brief   Util Macros for STSAFE Middleware
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * "h2""center"&copy; COPYRIGHT 2022 STMicroelectronics"/center""/h2"
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************/

#ifndef STSELIB_H
#define STSELIB_H


 /*! \defgroup 	STSELib STSecureElement Library
 *  \brief 		STMicroelectronics Secure Element host library
 *  \details	\include{doc} STSELib.dox
 *
 *  \defgroup 	stse_certificate STSE Certificate
 *  \ingroup 	STSELib
 *  \brief		STSELib Certificate parser
 *  \details  	The STSE Certificate parser is the entry point for the upper system application layer. \n
 *  			It provides high level certificate parsing functions to the application layer.
 *
 *  \defgroup 	stse_api API layer
 *  \ingroup 	STSELib
 *  \brief		STSELib API Layer
 *  \details  	STSELib API Layer
 *
 *  \defgroup 	stse_services Services layer
 *  \ingroup 	STSELib
 *  \brief		STSELib service Layer
 *  \details  	The Services layer provide set of functions that format all commands supported \n
 *  			by the targeted secure element and reports response to higher layers API/Application
 *
 *  \defgroup 	stse_core Core layer
 *  \ingroup 	STSELib
 *  \brief		STSELib core Layer
 *  \details  	STSELib core Layer
 */
#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stse_conf.h"
#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_data_partition.h"
#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_low_power.h"
#include "services/stsafea/stsafea_password.h"
#include "services/stsafea/stsafea_patch.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_random.h"
#include "services/stsafea/stsafea_reset.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_mac.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_wrap_unwrap.h"
#include "services/stsafea/stsafea_sessions.h"
#include "api/stse_data_storage.h"
#include "api/stse_device_management.h"
#include "api/stse_device_authentication.h"
#include "api/stse_symmetric_keys_management.h"
#include "api/stse_aes.h"
#include "api/stse_asymmetric_keys_management.h"
#include "api/stse_ecc.h"
#include "api/stse_hash.h"
#include "api/stse_mac.h"
#include "api/stse_random.h"
#include "certificate/stse_certificate.h"

#endif /* STSELIB_H */
