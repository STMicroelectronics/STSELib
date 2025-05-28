/*!
 ******************************************************************************
 * \file    stsafel_data_partition.h
 * \brief   Data partition services for STSAFE-L
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

#ifndef STSAFEL_DATA_PARTITION_H
#define STSAFEL_DATA_PARTITION_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "stse_platform_generic.h"

/*! \defgroup stsafel_data_partition STSAFE-L Data partition
 *  \ingroup stsafel_services
 *  @{
 */

/*!
 * \brief STSAFEL read service option structure
 */
typedef struct stsafel_read_option_t {
    stse_zone_ac_t new_read_ac : STSE_4BIT_LEN;                          /*!< \brief New read access condition */
    stse_ac_change_right_t new_read_ac_change_right : STSE_1BIT_LEN;     /*!< \brief New read access condition change right */
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN; /*!< \brief Change access condition indicator */
    PLAT_UI8 filler : STSE_2BIT_LEN;                                     /*!< \brief Option filler ; Must be 0b00 */
} stsafel_read_option_t;

/*!
 * \brief STSAFEL update service option structure
 */
typedef struct stsafel_update_option_t {
    stse_zone_ac_t new_update_ac : STSE_4BIT_LEN;
    stse_ac_change_right_t new_update_ac_change_right : STSE_1BIT_LEN;
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN;
    PLAT_UI8 filler : STSE_1BIT_LEN;
    stse_zone_update_atomicity_t atomicity : STSE_1BIT_LEN;
} stsafel_update_option_t;

/*!
 * \brief STSAFEL decrement service option structure
 */
typedef struct stsafel_decrement_option_t {
    stse_zone_ac_t new_decrement_ac : STSE_4BIT_LEN;
    stse_ac_change_right_t new_decrement_ac_change_right : STSE_1BIT_LEN;
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN;
    PLAT_UI8 filler : STSE_2BIT_LEN;
} stsafel_decrement_option_t;

/**
 * \brief       Send a read data zone command to target device
 * \details     This service format and send the read data zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The data zone index to read in target STSAFEL device
 * \param[in]   option               \ref stsafel_read_option_t Read option structure
 * \param[in]   offset               Offset of data to be read
 * \param[in]   pData                Pointer to the read data buffer
 * \param[in]   data_length          Data buffer length in byte
 * \param[in]   protection           \ref stse_cmd_protection_t Command protection flag
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_read_data_zone(stse_Handler_t *pSTSE,
                                         PLAT_UI8 zone_index,
                                         stsafel_read_option_t option,
                                         PLAT_UI16 offset,
                                         PLAT_UI8 *pData,
                                         PLAT_UI16 data_length,
                                         stse_cmd_protection_t protection);

/**
 * \brief       Send an update data zone command to target device
 * \details     This service format and send the update data zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The data zone index to update in target STSAFEL device
 * \param[in]   option               \ref stsafel_update_option_t Update option structure
 * \param[in]   offset               Offset used for new data update
 * \param[in]   pData                Pointer to the new data buffer (data to be written in the zone)
 * \param[in]   data_length          New data buffer length in byte
 * \param[in]   protection           \ref stse_cmd_protection_t Command protection flag
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_update_data_zone(stse_Handler_t *pSTSE,
                                           PLAT_UI8 zone_index,
                                           stsafel_update_option_t option,
                                           PLAT_UI16 offset,
                                           PLAT_UI8 *pData,
                                           PLAT_UI16 data_length,
                                           stse_cmd_protection_t protection);

/**
 * \brief       Send a read counter zone command to target device
 * \details     This service format and send the read counter zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The counter zone index to read in target STSAFEL device
 * \param[in]   option               \ref stsafel_read_option_t Read option structure
 * \param[in]   offset               Offset used in case of associated data read
 * \param[in]   pData                Pointer to the read data buffer
 * \param[in]   data_length          Associated data buffer length in byte
 * \param[out]  pCounter_value       Pointer to counter read buffer
 * \param[in]   protection           \ref stse_cmd_protection_t Command protection flag
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_read_counter_zone(stse_Handler_t *pSTSE,
                                            PLAT_UI8 zone_index,
                                            stsafel_read_option_t option,
                                            PLAT_UI16 offset,
                                            PLAT_UI8 *pData,
                                            PLAT_UI16 data_length,
                                            PLAT_UI32 *pCounter_value,
                                            stse_cmd_protection_t protection);

/**
 * \brief       Send a decrement counter zone command to target device
 * \details     This service format and send the decrement counter zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The counter zone index to decrement in target STSAFEL device
 * \param[in]   option               \ref stsafel_decrement_option_t Decrement option structure
 * \param[in]   amount               Amount to be decremented from zone counter
 * \param[in]   offset               Offset used in case of associated data update
 * \param[in]   pData                Pointer to the new associated data buffer (data to be written along with counter decrement in the zone)
 * \param[in]   data_length          New associated data buffer length in byte
 * \param[out]  pNew_counter_value   Pointer to counter read buffer
 * \param[in]   protection           \ref stse_cmd_protection_t Command protection flag
 * \return \ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafel_decrement_counter_zone(stse_Handler_t *pSTSE,
                                                 PLAT_UI8 zone_index,
                                                 stsafel_decrement_option_t option,
                                                 PLAT_UI32 amount,
                                                 PLAT_UI16 offset,
                                                 PLAT_UI8 *pData,
                                                 PLAT_UI16 data_length,
                                                 PLAT_UI32 *pNew_counter_value,
                                                 stse_cmd_protection_t protection);

/** \}*/

#endif /* STSAFEL_DATA_PARTITION_H */
