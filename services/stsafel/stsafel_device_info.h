/*!
 ******************************************************************************
 * \file	stsafel_device_info.h
 * \brief   Device info services for STSAFE-L
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

#ifndef STSAFEL_DEVICE_INFO_H
#define STSAFEL_DEVICE_INFO_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"

/*! \defgroup stsafel_device_info STSAFE-L Device info
 *  \ingroup stsafel_services
 *  @{
 */

#define STSAFEL_UID_LENGTH 8U
#define STSAFEL_CPSN_LENGTH 7U
#define STSAFEL_HARDWARE_ID_LENGTH 2U
#define STSAFEL_BOOT_ROM_ID_LENGTH 2U
#define STSAFEL_SOC_ID_LENGTH 3U
#define STSAFEL_TRACEABILITY_RESERVED_LENGTH 8U
#define STSAFEL_DATA_CONFIGURATION_IDENTIFIER_LENGTH 8U

typedef enum stsafel_device_info_tag_t{
	STSAFEL_INFO_TRACEABILITY_DATA = 0x00,
	STSAFEL_INFO_UNIQUE_IDENTIFIER,
	STSAFEL_INFO_LIFE_CYCLE_STATE,
	STSAFEL_INFO_KILL_COMMAND_CONFIGURATION,
	STSAFEL_INFO_SWITCH_PROTOCOL_FLAG,
	STSAFEL_INFO_ZONE_META_DATA_V1,
	STSAFEL_INFO_ZONE_META_DATA_V2,
	STSAFEL_INFO_STAGE_1_DATA_CONFIGURATION_IDENTIFIER = 0x0C,
	STSAFEL_INFO_STAGE_2_DATA_CONFIGURATION_IDENTIFIER,
	STSAFEL_INFO_STAGE_3_DATA_CONFIGURATION_IDENTIFIER
} stsafel_device_info_tag_t;

typedef struct /*!< Device traceability info */
{
	struct {
		PLAT_UI8 hardware_id[STSAFEL_HARDWARE_ID_LENGTH];
		PLAT_UI8 boot_rom_id[STSAFEL_BOOT_ROM_ID_LENGTH];
		PLAT_UI8 soc_id[STSAFEL_SOC_ID_LENGTH];
		PLAT_UI8 data_config_id;
	} product_type;
	PLAT_UI8 cpsn[STSAFEL_CPSN_LENGTH]; /*!< Chip Serial Number */
	PLAT_UI8 reserved[STSAFEL_TRACEABILITY_RESERVED_LENGTH];
} stsafel_device_traceability_t;

/**
 * \brief 		Get target device unique identifier
 * \details 	This service format and send the get UID command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[out]	pDevice_UID			Device unique identifier
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_get_device_UID(
		stse_Handler_t * pSTSE,
		PLAT_UI8 * pDevice_UID);

/**
 * \brief 		Get target device traceability info
 * \details 	This service format and send the get traceability info command
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[out]	pDevice_traceability	Device traceability info
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_get_device_traceability(
		stse_Handler_t * pSTSE,
		stsafel_device_traceability_t * pDevice_traceability);

/**
 * \brief 		Get target device info
 * \details 	This service format and send the get device info command
 * \param[in] 	pSTSE                       Pointer to STSE Handler
 * \param[in]	tag                         Tag of the data to get
 * \param[in]	additional_data_length      Additional data length
 * \param[in]	pAdditional_data            Additional data relative to requested info
 * \param[in]	device_info_expected_length	Expected device info length
 * \param[out]	pDevice_info			    Device info output buffer
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_get_data(
		stse_Handler_t * pSTSE,
		stsafel_device_info_tag_t tag,
		PLAT_UI16 additional_data_length,
		PLAT_UI8 * pAdditional_data,
		PLAT_UI16 device_info_expected_length,
		PLAT_UI8 * pDevice_info);

/**
 * \brief 		Put target device info
 * \details 	This service format and send the put device info command
 * \param[in] 	pSTSE                       Pointer to STSE Handler
 * \param[in]	tag                         Tag of the data to put
 * \param[in]	device_info_length	        Device info length
 * \param[in]	pDevice_info			    Device info buffer
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_put_data(
		stse_Handler_t * pSTSE,
		stsafel_device_info_tag_t tag,
		PLAT_UI16 device_info_length,
		PLAT_UI8 * pDevice_info);

/** \}*/

#endif /* STSAFEL_DEVICE_INFO_H */
