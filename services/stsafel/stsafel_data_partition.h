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

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafel/stsafel_commands.h"
#include "services/stsafel/stsafel_timings.h"

/*! \defgroup stsafel_data_partition Data partition
 *  \ingroup stsafel_services
 *  @{
 */

#define STSAFEL_DATA_ZONE_COUNTER_LENGTH 3U

/*!
 * \brief STSAFEL decrement service option structure
 */
typedef struct stsafel_decrement_option_t{
    PLAT_UI8 new_decrement_ac:4;                /*!< 0b0000: Access always authorized
                                                  \n 0b0001: Access authorized under pairing session only
                                                  \n 0b0010: Access authorized under companion session only
                                                  \n Other: Access never */
    PLAT_UI8 new_decrement_ac_change_right:1;   /*!< 0b0: Access condition change authorized
                                                  \n 0b1: Access condition change forbidden*/
    PLAT_UI8 change_ac_indicator:1;             /*!< 0b0: request access condition change
                                                  \n 0b1: ignore access condition change */
    PLAT_UI8 filler:2;                          /*!< Must be 0b00 */
}stsafel_decrement_option_t;

/*!
 * \brief STSAFEL read service option structure
 */
typedef struct stsafel_read_option_t{
    PLAT_UI8 new_read_ac:4;                 /*!< \brief New read access condition \n - 0000b : ALWAYS \n - 0001b : pairing session \n - 0010b : companion session \n - Other : NEVER */
    PLAT_UI8 new_read_ac_change_right:1;    /*!< \brief New read access condition change right \n - 0b : NEVER \n - 1b : ALWAYS */
    PLAT_UI8 change_ac_indicator:1;         /*!< \brief Change access condition indicator \n - 0b : Do not change access condition \n - 1b : Change access condition */
    PLAT_UI8 filler:2;                      /*!< \brief Option filler ; Must be 0b00 */
}stsafel_read_option_t;

/*!
 * \brief STSAFEL update service option structure
 */
typedef struct stsafel_update_option_t{
    PLAT_UI8 new_update_ac:4;               /*!< 0b0000: Access always authorized
                                              \n 0b0001: Access authorized under pairing session only
                                              \n 0b0010: Access authorized under companion session only
                                              \n Other: Access never */
    PLAT_UI8 new_update_ac_change_right:1;  /*!< 0b0: Access condition change authorized
                                              \n 0b1: Access condition change forbidden*/
    PLAT_UI8 change_ac_indicator:1;         /*!< 0b0: request access condition change
                                              \n 0b1: ignore access condition change */
    PLAT_UI8 filler:1;                      /*!< Must be 0b0 */
    PLAT_UI8 atomicity:1;                   /*!< 0b0: non-atomic update
                                              \n 0b1: atomic update */
}stsafel_update_option_t;

/**
 * \brief       Send a read data zone command to target device
 * \details     This service format and send the read data zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The data zone index to read in target STSAFEL device
 * \param[in]   option               \ref stsafel_read_option_t Read option structure
 * \param[in]   offset               Offset of data to be read
 * \param[in]   pData                Pointer to the read data buffer
 * \param[in]   data_length          Data buffer length in byte
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafel_read_data_zone(stse_Handler_t * pSTSE,
        PLAT_UI8 zone_index,
        stsafel_read_option_t option,
        PLAT_UI16 offset,
        PLAT_UI8 *pData,
        PLAT_UI16 data_length);

/**
 * \brief       Send an update data zone command to target device
 * \details     This service format and send the update data zone command
 * \param[in]   pSTSE                Pointer to STSE Handler
 * \param[in]   zone_index           The data zone index to update in target STSAFEL device
 * \param[in]   option               \ref stsafel_update_option_t Update option structure
 * \param[in]   offset               Offset used for new data update
 * \param[in]   pData                Pointer to the new data buffer (data to be written in the zone)
 * \param[in]   data_length          New data buffer length in byte
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafel_update_data_zone(stse_Handler_t * pSTSE,
        PLAT_UI8 zone_index,
        stsafel_update_option_t option,
        PLAT_UI16 offset,
        PLAT_UI8 *pData ,
        PLAT_UI16 data_length);

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
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafel_read_counter_zone(stse_Handler_t * pSTSE,
        PLAT_UI8 zone_index,
        stsafel_read_option_t option,
        PLAT_UI16 offset,
        PLAT_UI8 *pData,
        PLAT_UI16 data_length,
        PLAT_UI32 *pCounter_value);

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
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafel_decrement_counter_zone(stse_Handler_t * pSTSE,
        PLAT_UI8 zone_index,
        stsafel_decrement_option_t option,
        PLAT_UI32 amount,
        PLAT_UI16 offset,
        PLAT_UI8 *pData,
		PLAT_UI16  data_length,
        PLAT_UI32 *pNew_counter_value);

/** \}*/

#endif /* STSAFEL_DATA_PARTITION_H */
