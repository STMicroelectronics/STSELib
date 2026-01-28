/*!
 ******************************************************************************
 * \file	stsafea_data_partition.c
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

#include "services/stsafea/stsafea_data_partition.h"
#include "services/stsafea/stsafea_frame_transfer.h"

#ifdef STSE_CONF_STSAFE_A_SUPPORT

#define STSAFEA_ZONE_INDEX_SIZE 1U
#define STSAFEA_ZONE_OFFSET_SIZE 2U
#define STSAFEA_ZONE_ACCESS_OPTION_SIZE 1U
#define STSAFEA_INC_DEC_AMOUT_SIZE 4U
#define STSAFEA_ZONE_ACCESS_LENGTH_SIZE 2U

stse_ReturnCode_t stsafea_switch_data_partition_access_protection(stse_Handler_t *pSTSE, PLAT_UI8 command_code, stse_cmd_protection_t protection) {
    switch (protection) {

    case STSE_HOST_C_MAC_R_MAC:
        stsafea_perso_info_set_cmd_AC(&pSTSE->perso_info, command_code, STSE_CMD_AC_HOST);
    case STSE_NO_PROT:
        break;

    case STSE_HOST_C_WRAP:
    case STSE_HOST_R_WRAP:
    case STSE_HOST_C_WRAP_R_WRAP:
    default:
        return STSE_SERVICE_INVALID_PARAMETER;
        break;
    }
    return STSE_OK;
}

stse_ReturnCode_t stsafea_get_total_partition_count(stse_Handler_t *pSTSE,
                                                    PLAT_UI8 *pTotal_partition_count) {
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
    PLAT_UI8 rsp_header;

    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eTag, 1, &tag);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&RspFrame, eTotal_partition_count, 1, pTotal_partition_count);

    /*- Perform Transfer*/
    return stsafea_frame_raw_transfer(pSTSE,
                                      &CmdFrame,
                                      &RspFrame,
                                      stsafea_cmd_timings[pSTSE->device_type][cmd_header]);
}

stse_ReturnCode_t stsafea_get_data_partitions_configuration(stse_Handler_t *pSTSE,
                                                            PLAT_UI8 total_partitions_count,
                                                            stsafea_data_partition_record_t *pRecord_table,
                                                            PLAT_UI16 record_table_length) {
    stse_ReturnCode_t ret;
    volatile PLAT_UI8 partition_idx;
    PLAT_UI8 cmd_header = STSAFEA_CMD_QUERY;
    PLAT_UI8 tag = STSAFEA_SUBJECT_TAG_DATA_PARTITION_CONFIGURATION;
    PLAT_UI8 rsp_header;
    PLAT_UI8 raw_data[record_table_length];

    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmd_header, 1, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eTag, 1, &tag);

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, 1, &rsp_header);
    stse_frame_element_allocate_push(&RspFrame, eRaw, record_table_length, raw_data);

    /*- Perform Transfer*/
    ret = stsafea_frame_raw_transfer(pSTSE,
                                     &CmdFrame,
                                     &RspFrame,
                                     stsafea_cmd_timings[pSTSE->device_type][cmd_header]);

    /* - Verify transfer result and build Partition record table */
    if (ret == STSE_OK) {
        PLAT_UI16 i = 1;
        for (partition_idx = 0; partition_idx < total_partitions_count; partition_idx++) {
            pRecord_table->index = raw_data[i++];
            pRecord_table->zone_type = raw_data[i++];
            pRecord_table->read_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_READ_CR_GET(raw_data[i]);
            pRecord_table->read_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_READ_GET(raw_data[i]);
            pRecord_table->update_ac_cr = (stse_ac_change_right_t)STSAFEA_ZIR_AC_UPDATE_CR_GET(raw_data[i]);
            pRecord_table->update_ac = (stse_zone_ac_t)STSAFEA_ZIR_AC_UPDATE_GET(raw_data[i++]);
            pRecord_table->data_segment_length = UI16_B1_SET(raw_data[i++]);
            pRecord_table->data_segment_length += UI16_B0_SET(raw_data[i++]);
            if (pRecord_table->zone_type == 1) {
                pRecord_table->counter_value = UI32_B3_SET(raw_data[i++]);
                pRecord_table->counter_value += UI32_B2_SET(raw_data[i++]);
                pRecord_table->counter_value += UI32_B1_SET(raw_data[i++]);
                pRecord_table->counter_value += UI32_B0_SET(raw_data[i++]);
            } else {
                pRecord_table->counter_value = 0;
            }
            pRecord_table++;
        }
    }

    return (ret);
}

stse_ReturnCode_t stsafea_decrement_counter_zone(stse_Handler_t *pSTSE,
                                                 PLAT_UI8 zone_index,
                                                 stsafea_decrement_option_t option,
                                                 PLAT_UI32 amount,
                                                 PLAT_UI16 offset,
                                                 PLAT_UI8 *pData,
                                                 PLAT_UI8 data_length,
                                                 PLAT_UI32 *pNew_counter_value,
                                                 stse_cmd_protection_t protection) {

    volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header = STSAFEA_CMD_DECREMENT;
    PLAT_UI8 rsp_header;

    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if ((pData == NULL) || (pNew_counter_value == NULL) || (amount == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

#ifdef STSE_CONF_USE_HOST_SESSION
    stse_perso_info_t perso_info_backup = pSTSE->perso_info;
    ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
    if (ret != STSE_OK) {
        return ret;
    }
#endif

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
    stse_frame_element_allocate_push(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, &zone_index);
    stse_frame_element_allocate_push(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
    stse_frame_element_allocate_push(&CmdFrame, eAmount, STSAFEA_INC_DEC_AMOUT_SIZE, (PLAT_UI8 *)&amount);
    stse_frame_element_allocate_push(&CmdFrame, eData, data_length, pData);

    if (data_length >= stsafea_maximum_frame_length[pSTSE->device_type]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&RspFrame, eNewCounterVal, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)pNew_counter_value);

    /*- Swap Elements byte order before sending*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eAmount);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(pSTSE,
                                 &CmdFrame,
                                 &RspFrame);

    /*- unSwap Elements bytes from Command frame*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eAmount);

    /*- Swap New Counter value byte order before sending*/
    stse_frame_element_swap_byte_order(&eNewCounterVal);

#ifdef STSE_CONF_USE_HOST_SESSION
    pSTSE->perso_info = perso_info_backup;
#endif

    return ret;
}

stse_ReturnCode_t stsafea_read_counter_zone(stse_Handler_t *pSTSE,
                                            PLAT_UI32 zone_index,
                                            stsafea_read_option_t option,
                                            PLAT_UI16 offset,
                                            PLAT_UI8 *pAssociated_data,
                                            PLAT_UI16 Associated_data_length,
                                            PLAT_UI32 *pCounter_value,
                                            stse_cmd_protection_t protection) {

    volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
    PLAT_UI8 rsp_header;

    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if ((pCounter_value == NULL)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

#ifdef STSE_CONF_USE_HOST_SESSION
    stse_perso_info_t perso_info_backup = pSTSE->perso_info;
    ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
    if (ret != STSE_OK) {
        return ret;
    }
#endif

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
    stse_frame_element_allocate_push(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
    stse_frame_element_allocate_push(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
    stse_frame_element_allocate_push(&CmdFrame, eLength, STSAFEA_ZONE_ACCESS_LENGTH_SIZE, (PLAT_UI8 *)&Associated_data_length);

    if (Associated_data_length >= stsafea_maximum_frame_length[pSTSE->device_type]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    stse_frame_element_allocate_push(&RspFrame, eCounterVal, STSAFEA_COUNTER_VALUE_SIZE, (PLAT_UI8 *)pCounter_value);
    stse_frame_element_allocate_push(&RspFrame, eAssociatedData, Associated_data_length, pAssociated_data);

    /*- Swap Elements bytes from Command frame*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eLength);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(pSTSE,
                                 &CmdFrame,
                                 &RspFrame);

    /*- Swap Counter value*/
    stse_frame_element_swap_byte_order(&eCounterVal);

    /*- Un-Swap Elements bytes from Command frame*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eLength);

#ifdef STSE_CONF_USE_HOST_SESSION
    pSTSE->perso_info = perso_info_backup;
#endif

    return (ret);
}

stse_ReturnCode_t stsafea_read_data_zone(stse_Handler_t *pSTSE,
                                         PLAT_UI32 zone_index,
                                         stsafea_read_option_t option,
                                         PLAT_UI16 offset,
                                         PLAT_UI8 *pReadBuffer,
                                         PLAT_UI16 read_length,
                                         stse_cmd_protection_t protection) {
    volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header = STSAFEA_CMD_READ;
    PLAT_UI8 rsp_header;

    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    // If change access condition indicator is set to STSE_AC_CHANGE then we allow zero length and no need of a read buffer
    if (option.change_ac_indicator != STSE_AC_CHANGE && (pReadBuffer == NULL || read_length == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

#ifdef STSE_CONF_USE_HOST_SESSION
    stse_perso_info_t perso_info_backup = pSTSE->perso_info;
    ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
    if (ret != STSE_OK) {
        return ret;
    }
#endif

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
    stse_frame_element_allocate_push(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
    stse_frame_element_allocate_push(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
    stse_frame_element_allocate_push(&CmdFrame, eLength, STSAFEA_ZONE_ACCESS_LENGTH_SIZE, (PLAT_UI8 *)&read_length);

    if (read_length >= stsafea_maximum_frame_length[pSTSE->device_type]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);
    if (read_length != 0) {
        stse_frame_element_allocate_push(&RspFrame, eData, read_length, (PLAT_UI8 *)pReadBuffer);
    }

    /*- Swap Elements byte order before sending*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eLength);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(pSTSE,
                                 &CmdFrame,
                                 &RspFrame);

    /*- UnSwap Elements bytes from Command frame*/
    stse_frame_element_swap_byte_order(&eOffset);
    stse_frame_element_swap_byte_order(&eLength);

#ifdef STSE_CONF_USE_HOST_SESSION
    pSTSE->perso_info = perso_info_backup;
#endif

    return ret;
}

stse_ReturnCode_t stsafea_update_data_zone(stse_Handler_t *pSTSE,
                                           PLAT_UI32 zone_index,
                                           stsafea_update_option_t option,
                                           PLAT_UI16 offset,
                                           PLAT_UI8 *pData,
                                           PLAT_UI32 data_length,
                                           stse_cmd_protection_t protection) {

    volatile stse_ReturnCode_t ret = STSE_SERVICE_INVALID_PARAMETER;
    PLAT_UI8 cmd_header = STSAFEA_CMD_UPDATE;
    PLAT_UI8 rsp_header;

    /* - Check stsafe handler initialization */
    if (pSTSE == NULL) {
        return STSE_SERVICE_HANDLER_NOT_INITIALISED;
    }

    if ((pData == NULL) || (data_length == 0)) {
        return STSE_SERVICE_INVALID_PARAMETER;
    }

#ifdef STSE_CONF_USE_HOST_SESSION
    stse_perso_info_t perso_info_backup = pSTSE->perso_info;
    ret = stsafea_switch_data_partition_access_protection(pSTSE, cmd_header, protection);
    if (ret != STSE_OK) {
        return ret;
    }
#endif

    /*- Create CMD frame and populate elements */
    stse_frame_allocate(CmdFrame);
    stse_frame_element_allocate_push(&CmdFrame, eCmdHeader, STSAFEA_HEADER_SIZE, &cmd_header);
    stse_frame_element_allocate_push(&CmdFrame, eOption, STSAFEA_ZONE_ACCESS_OPTION_SIZE, (PLAT_UI8 *)&option);
    stse_frame_element_allocate_push(&CmdFrame, eZoneIndex, STSAFEA_ZONE_INDEX_SIZE, (PLAT_UI8 *)&zone_index);
    stse_frame_element_allocate_push(&CmdFrame, eOffset, STSAFEA_ZONE_OFFSET_SIZE, (PLAT_UI8 *)&offset);
    stse_frame_element_allocate_push(&CmdFrame, eData, data_length, pData);

    if (data_length >= stsafea_maximum_frame_length[pSTSE->device_type]) {
        return STSE_SERVICE_FRAME_SIZE_ERROR;
    }

    /*- Create Rsp frame and populate elements*/
    stse_frame_allocate(RspFrame);
    stse_frame_element_allocate_push(&RspFrame, eRsp_header, STSAFEA_HEADER_SIZE, &rsp_header);

    /*- Swap Elements byte order before sending*/
    stse_frame_element_swap_byte_order(&eOffset);

    /*- Perform Transfer*/
    ret = stsafea_frame_transfer(pSTSE,
                                 &CmdFrame,
                                 &RspFrame);

#ifdef STSE_CONF_USE_HOST_SESSION
    pSTSE->perso_info = perso_info_backup;
#endif

    return ret;
}

#endif /* STSE_CONF_STSAFE_A_SUPPORT */
