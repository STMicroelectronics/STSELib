/*!
 ******************************************************************************
 * \file	stsafea_data_partition.h
 * \brief   Data partition services for STSAFE-A
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

#ifndef STSAFEA_DATA_PARTITION_H
#define STSAFEA_DATA_PARTITION_H

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_timings.h"

/*! \defgroup stsafea_data_partition STSAFE-A Data partition management
 *  \ingroup stsafea_services
 *  @{
 */

/* Zone Info Record (ZIR) bitfields definition  */
#define STSAFEA_ZIR_AC_READ_CR_Pos 7
#define STSAFEA_ZIR_AC_READ_Pos 4
#define STSAFEA_ZIR_AC_UPDATE_CR_Pos 3
#define STSAFEA_ZIR_AC_UPDATE_Pos 0
#define STSAFEA_ZIR_AC_READ_CR_Msk 128 /*0b10000000*/
#define STSAFEA_ZIR_AC_READ_Msk 112    /*0b01110000*/
#define STSAFEA_ZIR_AC_UPDATE_CR_Msk 8 /*0b00001000*/
#define STSAFEA_ZIR_AC_UPDATE_Msk 7    /*0b00000111*/
#define STSAFEA_ZIR_AC_READ_CR_GET(x) (((x) & STSAFEA_ZIR_AC_READ_CR_Msk) >> STSAFEA_ZIR_AC_READ_CR_Pos)
#define STSAFEA_ZIR_AC_READ_GET(x) (((x) & STSAFEA_ZIR_AC_READ_Msk) >> STSAFEA_ZIR_AC_READ_Pos)
#define STSAFEA_ZIR_AC_UPDATE_CR_GET(x) (((x) & STSAFEA_ZIR_AC_UPDATE_CR_Msk) >> STSAFEA_ZIR_AC_UPDATE_CR_Pos)
#define STSAFEA_ZIR_AC_UPDATE_GET(x) (((x) & STSAFEA_ZIR_AC_UPDATE_Msk) >> STSAFEA_ZIR_AC_UPDATE_Pos)

/**
 * \struct stsafea_read_option_t
 * \brief STSAFE Read service options
 */
typedef struct stsafea_read_option_t {
    stse_zone_ac_t new_read_ac : STSE_3BIT_LEN;                          /*!< Access condition */
    stse_ac_change_right_t new_read_ac_change_right : STSE_1BIT_LEN;     /*!< Access condition change right */
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN; /*!< Access condition change indicator */
    PLAT_UI8 filler : STSE_3BIT_LEN;                                     /*!< Must be 0b0 */
} stsafea_read_option_t;

/**
 * \struct stsafea_update_option_t
 * \brief STSAFE Update service options
 */
typedef struct stsafea_update_option_t {
    stse_zone_ac_t new_update_ac : STSE_3BIT_LEN;                        /*!< Access condition */
    stse_ac_change_right_t new_update_ac_change_right : STSE_1BIT_LEN;   /*!< Access condition change right */
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN; /*!< Access condition change indicator */
    PLAT_UI8 filler : STSE_2BIT_LEN;                                     /*!< Must be 0b0 */
    stse_zone_update_atomicity_t atomicity : STSE_1BIT_LEN;              /*!< Atomicity */
} PLAT_PACKED_STRUCT stsafea_update_option_t;

/**
 * \struct stsafea_decrement_option_t
 * \brief STSAFE decrement service options
 */
typedef struct stsafea_decrement_option_t {
    stse_zone_ac_t new_decrement_ac : STSE_3BIT_LEN;                      /*!< Access condition */
    stse_ac_change_right_t new_decrement_ac_change_right : STSE_1BIT_LEN; /*!< Access condition change right */
    stse_zone_ac_change_indicator_t change_ac_indicator : STSE_1BIT_LEN;  /*!< Access condition change indicator */
    PLAT_UI8 filler : STSE_3BIT_LEN;                                      /*!< Must be 0b0 */
} PLAT_PACKED_STRUCT stsafea_decrement_option_t;

/**
 * \struct stsafea_data_partition_record_t
 * \brief STSAFE data partition record typedef
 */
typedef struct {
    PLAT_UI8 index;
    PLAT_UI8 zone_type; /* 0x00 = simple region, 0x01 = counter region (write requires Decrement, locked once counter reaches 0) */
    uint16_t data_segment_length;
    stse_ac_change_right_t read_ac_cr;
    stse_zone_ac_t read_ac;
    stse_ac_change_right_t update_ac_cr;
    stse_zone_ac_t update_ac;
    uint32_t counter_value;
} stsafea_data_partition_record_t;

/**
 * \brief 			STSAFEA get total partition count service
 * \details 		This service format and send/receive STSAFE-Axxx start hash command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \param[out]		total_partition_count	total partition count in bytes
 * \return 			\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_get_total_partition_count(stse_Handler_t *pSTSE,
                                                    PLAT_UI8 *total_partition_count);

/**
 * \brief 			STSAFEA get data partition configuration service
 * \details 		This service format and send/receive STSAFE-Axxx start hash command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \param[in]		total_partitions_count	Total partition count in bytes
 * \param[in,out]	pRecord_table			Pointer to applicative partition record table
 * \param[in]		record_table_length		Applicative record table length
 * \return 			\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_get_data_partitions_configuration(stse_Handler_t *pSTSE,
                                                            PLAT_UI8 total_partitions_count,
                                                            stsafea_data_partition_record_t *pRecord_table,
                                                            PLAT_UI16 record_table_length);

/**
 * \brief 		Decrement counter zone
 * \details 	This service formats and sends the decrement counter zone command
 * \param[in] 	pSTSE 					Pointer to STSE Handler
 * \param[in] 	zone_index				Zone index to decrement
 * \param[in] 	option					Decrement option
 * \param[in] 	amount					Amount to decrement
 * \param[in] 	offset					Associated data offset
 * \param[in] 	data					Pointer to associated data
 * \param[in] 	data_length				Length of associated data
 * \param[out] 	new_counter_value		Pointer to store new counter value
 * \param[in] 	protection				Command protection type
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_decrement_counter_zone(stse_Handler_t *pSTSE,
                                                 PLAT_UI8 zone_index,
                                                 stsafea_decrement_option_t option,
                                                 PLAT_UI32 amount,
                                                 PLAT_UI16 offset,
                                                 PLAT_UI8 *data,
                                                 PLAT_UI8 data_length,
                                                 PLAT_UI32 *new_counter_value,
                                                 stse_cmd_protection_t protection);

/**
 * \brief 		Read counter zone
 * \details 	This service formats and sends the read counter zone command
 * \param[in] 	pSTSE 				Pointer to STSE Handler
 * \param[in] 	zone_index			Zone index to read
 * \param[in] 	read_ac				Read access condition
 * \param[in] 	read_offset			Read offset
 * \param[out] 	pReadBuffer			Pointer to read buffer
 * \param[in] 	read_length			Read length
 * \param[out] 	pCounter_value		Pointer to store counter value
 * \param[in] 	protection			Command protection type
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_read_counter_zone(stse_Handler_t *pSTSE,
                                            PLAT_UI32 zone_index,
                                            stsafea_read_option_t read_ac,
                                            PLAT_UI16 read_offset,
                                            PLAT_UI8 *pReadBuffer,
                                            PLAT_UI16 read_length,
                                            PLAT_UI32 *pCounter_value,
                                            stse_cmd_protection_t protection);

/**
 * \brief 		Read data zone
 * \details 	This service formats and sends the read data zone command
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \param[in] 	zone_index		Zone index to read
 * \param[in] 	read_ac			Read access condition
 * \param[in] 	read_offset		Read offset
 * \param[out] 	pReadBuffer		Pointer to read buffer
 * \param[in] 	read_length		Read length
 * \param[in] 	protection		Command protection type
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_read_data_zone(stse_Handler_t *pSTSE,
                                         PLAT_UI32 zone_index,
                                         stsafea_read_option_t read_ac,
                                         PLAT_UI16 read_offset,
                                         PLAT_UI8 *pReadBuffer,
                                         PLAT_UI16 read_length,
                                         stse_cmd_protection_t protection);

/**
 * \brief 		Update data zone
 * \details 	This service formats and sends the update data zone command
 * \param[in] 	pSTSE 			Pointer to STSE Handler
 * \param[in] 	zone_index		Zone index to update
 * \param[in] 	option			Update option
 * \param[in] 	offset			Update offset
 * \param[in] 	data			Pointer to data to write
 * \param[in] 	data_length		Length of data to write
 * \param[in] 	protection		Command protection type
 * \return 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 */
stse_ReturnCode_t stsafea_update_data_zone(stse_Handler_t *pSTSE,
                                           PLAT_UI32 zone_index,
                                           stsafea_update_option_t option,
                                           PLAT_UI16 offset,
                                           PLAT_UI8 *data,
                                           PLAT_UI32 data_length,
                                           stse_cmd_protection_t protection);

/** \}*/

#endif /*STSAFE_DATA_PARTITION_H*/
