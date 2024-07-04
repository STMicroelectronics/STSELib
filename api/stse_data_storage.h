/*!
 *******************************************************************************
 * \file	stse_data_storage.h
 * \brief   STSE secure data storage API set (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_DATA_STORAGE_H
#define STSE_DATA_STORAGE_H


/* Includes ------------------------------------------------------------------*/
#include "services/stsafea/stsafea_put_query.h"
#include "services/stsafea/stsafea_data_partition.h"


/** \defgroup 	stse_data_storage 	STSE Data storage
 *  \ingroup 	stse_api
 *  \brief		STSE data storage API set
 *  \details  	The data storage API set provides high level functions to manage Access to the Secure User NVM of STSAFE devices.
 *  @{
 */


/**
 * \brief Get the total partition count from the target STSAFE device
 * \details This API functions use the STSAFE get service to report the total partition count from the target STSAFE device
 * \param[in] 	pSTSE 					Pointer to target STSAFE handler
 * \param[out] 	pTotal_partition_count 	Pointer to total partition count applicative variable
 * \return STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_data_storage_get_total_partition_count.dox
 */
stse_ReturnCode_t stse_data_storage_get_total_partition_count(
		stse_Handler_t* pSTSE,
		PLAT_UI8* pTotal_partition_count
);

/**
 * \brief Get the partition table from the target STSAFE device
 * \details This API functions use the STSAFE get service to report the total partition count from the target STSAFE device
 * \param[in]	pSTSE 					Pointer to target STSAFE handler
 * \param[in]	total_partition_count	Total partition count
 * \param[out]	pPartitioning_table 	Pointer to the partition table buffer
 * \param[in]	Partitioning_table_length Length of the partition table to be received
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_data_storage_get_partitioning_table.dox
 */
stse_ReturnCode_t stse_data_storage_get_partitioning_table(
		stse_Handler_t* pSTSE,
		PLAT_UI8 total_partition_count,
		stsafea_data_partition_record_t*  pPartitioning_table,
		PLAT_UI16 Partitioning_table_length
);

/*!
 * \brief 		Read one memory zone of the STSAFE device
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]  	offset 			Read offset
 * \param[out] 	pBuffer 		Pointer to applicative read buffer
 * \param[in]  	length 			Read length in byte
 * \param[in] 	chunk_size 		Read chunk size in byte
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_read_zone.dox
 */
stse_ReturnCode_t stse_data_storage_read_data_zone(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI16 chunk_size,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		Update one memory zone of the STSAFE device
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]  	offset 			Update offset
 * \param[in] 	pBuffer 		Pointer to applicative read buffer
 * \param[in]  	length 			Update length in byte
 * \param[in] 	atomicity 		\ref stsafea_atomicity_t atomicity of the update access
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_update_zone.dox
 */
stse_ReturnCode_t stse_data_storage_update_data_zone(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		stsafea_atomicity_t atomicity,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		Decrement one counter zone of the STSAFE device
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]  	amount 			Decrement amount
 * \param[in]	offset			Associated data update offset (optional : set to 0 if not used)
 * \param[in] 	pBuffer 		Associated data update buffer (optional : set to NULL if not used)
 * \param[in]  	length 			Associated data update length in byte (optional : set to 0 if not used)
 * \param[out] 	 new_counter_value 		Pointer to applicative counter value buffer
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_decrement_counter.dox
 */
stse_ReturnCode_t stse_data_storage_decrement_counter_zone(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		PLAT_UI32 amount,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI32 *new_counter_value,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		read one counter zone of the STSAFE device
 * \param[in]   pSTSE 			Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]	offset			Associated data read offset (optional : set to 0 if not used)
 * \param[in] 	pBuffer 		Associated data read buffer (optional : set to NULL if not used)
 * \param[in]  	length 			Associated data read length in byte  (optional : set to 0 if not used)
 * \param[in] 	chunk_size 		Associated data read chunk size in byte  (optional : set to 0 if not used)
 * \param[out] 	counter_value 	Pointer to applicative counter value buffer
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_read_counter.dox
 */
stse_ReturnCode_t stse_data_storage_read_counter_zone(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI16 chunk_size,
		PLAT_UI32 *counter_value,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		Change the Read access condition of one target STSAFE device zone
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]	ac				\ref stsafea_ac_t new access condition
 * \param[in]	ac_change_right	\ref stsafea_ac_change_right_t new access change right
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_change_read_access_condition.dox
 */
stse_ReturnCode_t stse_data_storage_change_read_access_condition(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		stsafea_ac_t ac,
		stsafea_ac_change_right_t ac_change_right,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		Change the Read access condition of one target STSAFE device zone
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]	ac				\ref stsafea_ac_t new access condition
 * \param[in]	ac_change_right	\ref stsafea_ac_change_right_t new access change right
 * \param[in]  	offset 			Update offset
 * \param[in] 	pBuffer 		Pointer to applicative read buffer
 * \param[in]  	length 			Update length in byte
 * \param[in] 	atomicity 		\ref stsafea_atomicity_t atomicity of the update access
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_change_update_access_condition.dox
 */
stse_ReturnCode_t stse_data_storage_change_update_access_condition(stse_Handler_t * pSTSE,
		PLAT_UI32 zone,
		stsafea_ac_t ac,
		stsafea_ac_change_right_t ac_change_right,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		stsafea_atomicity_t atomicity,
		stsafea_cmd_protection_t protection
);

/*!
 * \brief 		Change the Decrement access condition of one target STSAFE device counter zone
 * \param[in]   pSTSE 		Pointer to target STSAFE handler
 * \param[in]  	zone 			Target STSAFE zone index
 * \param[in]	ac				\ref stsafea_ac_t new access condition
 * \param[in]	ac_change_right	\ref stsafea_ac_change_right_t new access change right
 * \param[in]  	amount 			Decrement amount
 * \param[in]	offset			Associated data update offset (optional : set to 0 if not used)
 * \param[in] 	pBuffer 		Associated data update buffer (optional : set to NULL if not used)
 * \param[in]  	length 			Associated data update length in byte (optional : set to 0 if not used)
 * \param[out] 	new_counter_value 		Pointer to applicative counter value buffer
 * \param[in]  	protection 		\ref stsafea_cmd_protection_t command response protection indicator
 * \result 		\ref STSE_OK on success ; \ref stse_ReturnCode_t error code otherwise
 * \note 		- A target STSAFE handler must be initialized using the \ref stse_init routine prior to execute this API function
 * \note 		- If command response protection is required an active session between Host/Companion and STSAFE must be open
 * \details 	\include{doc} stse_data_storage_change_decrement_access_condition.dox
 */
stse_ReturnCode_t stse_data_storage_change_decrement_access_condition(
		stse_Handler_t *pSTSE,
		PLAT_UI32 zone,
		stsafea_ac_t ac,
		stsafea_ac_change_right_t ac_change_right,
		PLAT_UI32 amount,
		PLAT_UI16 offset,
		PLAT_UI8 *pBuffer,
		PLAT_UI16 length,
		PLAT_UI32 *new_counter_value,
		stsafea_cmd_protection_t protection
);

/** \}*/

#endif /* STSAFE_DATA_STORAGE_H */
