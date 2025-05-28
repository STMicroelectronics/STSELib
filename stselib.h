 /******************************************************************************
 * \file	stselib.h
 * \brief   STSecureElement Library header file
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


 /*!
 *  \defgroup 	stse_certificate STSE Certificate parser
 *  \brief		SE device certificate parser
 *  \details  	The STSE Certificate parser is the entry point for the upper system application layer.
 *  			It provides high level certificate parsing functions to the application layer.
 *
 *  \defgroup 	stse_api STSE API layer
 *  \brief		Common API set for STMicroelectronics brand protection secure elements
 *  \details  	The STSE API layer is the entry point for the upper system application layer.
 *  			It provides high level functions allowing seamless implementation of advanced authentication
 *  			, SE cryptographic processing  and device life cycle monitoring at applicative level  .
 *
 *  \defgroup 	stse_services STSE Services layer
 *  \brief		Secure Element specific services Layer
 *  \details  	The Services layer provide set of functions that format all commands supported \n
 *  			by a specific secure element from STMicroelectronics Brand Protection portfolio
 *
 *  \defgroup 	stse_core STSE Core layer
 *  \brief		Frame protocol , hardware abstraction and cryptographic abstraction layer
 *  \details  	The Core layer provide SE frame management services and abstraction to specific HW
 *              and cryptographic platforms required by the STSE library to operate on specific HOST
 */
#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stse_conf.h"
#include "stse_platform_generic.h"
#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_aes.h"
#include "services/stsafea/stsafea_asymmetric_key_slots.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_data_partition.h"
#include "services/stsafea/stsafea_ecc.h"
#include "services/stsafea/stsafea_echo.h"
#include "services/stsafea/stsafea_entity_auth.h"
#include "services/stsafea/stsafea_hash.h"
#include "services/stsafea/stsafea_host_key_slot.h"
#include "services/stsafea/stsafea_low_power.h"
#include "services/stsafea/stsafea_mac.h"
#include "services/stsafea/stsafea_password.h"
#include "services/stsafea/stsafea_patch.h"
#include "services/stsafea/stsafea_public_key_slots.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_random.h"
#include "services/stsafea/stsafea_reset.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_symmetric_key_slots.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_wrap_unwrap.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_data_partition.h"
#include "services/stsafel/stsafel_device_info.h"
#include "services/stsafel/stsafel_ecc.h"
#include "services/stsafel/stsafel_echo.h"
#include "services/stsafel/stsafel_low_power.h"
#include "services/stsafel/stsafel_reset.h"
#include "services/stsafel/stsafel_timings.h"
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
