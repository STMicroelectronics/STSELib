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

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"
#include "core/stse_frame.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_timings.h"
#include "services/stsafea/stsafea_put_query.h"

/*! \defgroup stsafe_data_partition Data partition management
 *  \ingroup stsafea_services
 *  @{
 */

/* Zone Info Record (ZIR) bitfields definition  */
#define STSAFEA_ZIR_AC_READ_CR_Pos  		7
#define STSAFEA_ZIR_AC_READ_Pos  	 		4
#define STSAFEA_ZIR_AC_UPDATE_CR_Pos  		3
#define STSAFEA_ZIR_AC_UPDATE_Pos	  		0
#define STSAFEA_ZIR_AC_READ_CR_Msk 			128 /*0b10000000*/
#define STSAFEA_ZIR_AC_READ_Msk 			112 /*0b01110000*/
#define STSAFEA_ZIR_AC_UPDATE_CR_Msk 		8   /*0b00001000*/
#define STSAFEA_ZIR_AC_UPDATE_Msk 			7   /*0b00000111*/
#define STSAFEA_ZIR_AC_READ_CR_GET(x)		( ( (x) & STSAFEA_ZIR_AC_READ_CR_Msk) >> STSAFEA_ZIR_AC_READ_CR_Pos )
#define STSAFEA_ZIR_AC_READ_GET(x)			( ( (x) & STSAFEA_ZIR_AC_READ_Msk) >> STSAFEA_ZIR_AC_READ_Pos )
#define STSAFEA_ZIR_AC_UPDATE_CR_GET(x)		( ( (x) & STSAFEA_ZIR_AC_UPDATE_CR_Msk) >> STSAFEA_ZIR_AC_UPDATE_CR_Pos )
#define STSAFEA_ZIR_AC_UPDATE_GET(x)		( ( (x) & STSAFEA_ZIR_AC_UPDATE_Msk) >> STSAFEA_ZIR_AC_UPDATE_Pos )


/**
 * \enum stsafea_atomicity_t
 * \brief STSAFE Update service atomicity enumeration
 */
typedef enum stsafea_atomicity_t {
	STSAFEA_NON_ATOMIC_ACCESS = 0,		/*!< Non Atomic Access*/
	STSAFEA_ATOMIC_ACCESS				/*!< Atomic Access*/
}stsafea_atomicity_t;

/**
 * \enum stsafea_ac_t
 * \brief STSAFE data storage access condition enumeration
 */
typedef enum stsafea_ac_t{
	STSAFEA_AC_ALWAYS= 0,		/*!< Zone/counter access always granted */
	STSAFEA_AC_HOST,			/*!< Zone/counter access granted on Host C-MAC validation */
	STSAFEA_AC_AUTH,			/*!< Zone/counter access granted on Auth C-MAC validation */
	STSAFEA_AC_NEVER = 7		/*!< Zone/counter access never granted */
}stsafea_ac_t;

/**
 * \enum stsafea_zone_ac_change_indicator_t
 * \brief STSAFE data storage access condition change indicator
 */
typedef enum stsafea_zone_ac_change_indicator_t {
	STSAFEA_AC_CHANGE = 0,		/*!< request access condition change */
	STSAFEA_AC_IGNORE			/*!< ignore access condition change */
}stsafea_zone_ac_change_indicator_t;

/**
 * \enum stsafea_ac_change_right_t
 * \brief STSAFE data storage access condition change right enumeration
 */
typedef enum stsafea_ac_change_right_t {
		STSAFE_ACCR_DISABLED = 0,		/*!< accessible under pairing session*/
		STSAFE_ACCR_ENABLE				/*!<  Always accessible*/
}stsafea_ac_change_right_t;

/**
 * \struct stsafea_read_option_t
 * \brief STSAFE Read service options
 */
typedef struct stsafea_read_option_t {
	stsafea_ac_t new_read_ac:STSE_3BIT_LEN;									/*!< Access condition */
	stsafea_ac_change_right_t new_read_ac_change_right:STSE_1BIT_LEN;		/*!< Access condition change right */
	stsafea_zone_ac_change_indicator_t change_ac_indicator:STSE_1BIT_LEN;	/*!< Access condition change indicator */
	PLAT_UI8 filler:STSE_3BIT_LEN;											/*!< Must be 0b0 */
}stsafea_read_option_t;

/**
 * \struct stsafea_update_option_t
 * \brief STSAFE Update service options
 */
typedef struct stsafea_update_option_t {
	stsafea_ac_t new_update_ac:STSE_3BIT_LEN;								/*!< Access condition */
	stsafea_ac_change_right_t new_update_ac_change_right:STSE_1BIT_LEN;		/*!< Access condition change right */
	stsafea_zone_ac_change_indicator_t change_ac_indicator:STSE_1BIT_LEN;	/*!< Access condition change indicator */
	PLAT_UI8 filler:STSE_2BIT_LEN;											/*!< Must be 0b0 */
	stsafea_atomicity_t atomicity:STSE_1BIT_LEN;							/*!< Atomicity */
} PLAT_PACKED_STRUCT stsafea_update_option_t;

/**
 * \struct stsafea_decrement_option_t
 * \brief STSAFE decrement service options
 */
typedef struct stsafea_decrement_option_t {
	stsafea_ac_t new_decrement_ac:STSE_3BIT_LEN;							/*!< Access condition */
	stsafea_ac_change_right_t new_decrement_ac_change_right:STSE_1BIT_LEN;	/*!< Access condition change right */
	stsafea_zone_ac_change_indicator_t change_ac_indicator:STSE_1BIT_LEN;	/*!< Access condition change indicator */
	PLAT_UI8 filler:STSE_3BIT_LEN;											/*!< Must be 0b0 */
} PLAT_PACKED_STRUCT stsafea_decrement_option_t;

/**
 * \struct stsafea_data_partition_record_t
 * \brief STSAFE data partition record typedef
 */
typedef struct {
	PLAT_UI8 index;
	PLAT_UI8 zone_type;
	uint16_t data_segment_length;
	stsafea_ac_change_right_t read_ac_cr;
	stsafea_ac_t read_ac;
	stsafea_ac_change_right_t update_ac_cr;
	stsafea_ac_t update_ac;
	uint32_t counter_value;
}stsafea_data_partition_record_t;

/**
 * \brief 			STSAFEA get total partition count service
 * \details 		This service format and send/receive STSAFE-Axxx start hash command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \param[out]		total_partition_count	total partition count in bytes
 * \return 			\ref stse_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_get_total_partition_count( stse_Handler_t* pSTSE,
		PLAT_UI8* total_partition_count);

/**
 * \brief 			STSAFEA get data partition configuration service
 * \details 		This service format and send/receive STSAFE-Axxx start hash command/response
 * \param[in]		pSTSE					Pointer to target SE handler
 * \param[in]		total_partitions_count	Total partition count in bytes
 * \param[in,out]	pRecord_table			Pointer to applicative partition record table
 * \param[in]		record_table_length		Applicative record table length
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stsafea_get_data_partitions_configuration( stse_Handler_t* pSTSE,
	PLAT_UI8 							total_partitions_count,
	stsafea_data_partition_record_t*  	pRecord_table,
	PLAT_UI16 							record_table_length);

stse_ReturnCode_t stsafea_decrement_counter_zone(stse_Handler_t * pSTSE,
		PLAT_UI8 zone_index,
		stsafea_decrement_option_t option,
		PLAT_UI32 amount,
		PLAT_UI16 offset,
		PLAT_UI8 *data,
		PLAT_UI8  data_length,
		PLAT_UI32 * new_counter_value,
		stsafea_cmd_protection_t protection);

stse_ReturnCode_t stsafea_read_counter_zone(stse_Handler_t * pSTSE,
		PLAT_UI32 zone_index,
		stsafea_read_option_t read_ac,
		PLAT_UI16 read_offset,
		PLAT_UI8 *pReadBuffer,
		PLAT_UI16 read_length,
		PLAT_UI32 *pCounter_value,
		stsafea_cmd_protection_t protection);

stse_ReturnCode_t stsafea_read_data_zone(stse_Handler_t * pSTSE,
		PLAT_UI32 zone_index,
		stsafea_read_option_t read_ac,
		PLAT_UI16 read_offset,
		PLAT_UI8 *pReadBuffer,
		PLAT_UI16 read_length,
		stsafea_cmd_protection_t protection);

stse_ReturnCode_t stsafea_update_data_zone(stse_Handler_t * pSTSE,
		PLAT_UI32 zone_index,
		stsafea_update_option_t option,
		PLAT_UI16 offset,
		PLAT_UI8 *data ,
		PLAT_UI32 data_length,
		stsafea_cmd_protection_t protection);

/** \}*/

#endif /*STSAFE_DATA_PARTITION_H*/
