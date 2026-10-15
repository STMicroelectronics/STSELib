/**
  ******************************************************************************
  * @file    stsafel_frame_transfer.h
  * @author  CS Application Team
  * @brief   STSAFE-L services for frame transfer (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*! \defgroup stsafel_frame Frame Management
 *  \ingroup stsafel_services
 *  @{
 */

#ifndef STSAFEL_FRAME_TRANSFER_H
#define STSAFEL_FRAME_TRANSFER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"

#ifdef STSE_CONF_STSAFE_L_SUPPORT
extern const PLAT_UI16 stsafel_maximum_frame_length[STSAFEL_PRODUCT_COUNT];
#endif /* STSE_CONF_STSAFE_L_SUPPORT */

/**
  * \brief       Transmit frame from target STSAFE-Lxxx
  * \details     This core function prepare frame CRC and send frame to target STSAFE-Axxx device
  * \param[in]     p_stse      Pointer to STSE Handler
  * \param[in,out]   p_frame         Pointer to the frame to be transmitted
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_frame_transmit(stse_handle_t *p_stse,
                                          stse_frame_t *p_frame);

/**
  * \brief       Receive frame from target STSAFE-Lxxx
  * \details     This core function receive frame from target STSAFE-Axxx device and verify its CRC
  * \param[in]     p_stse      Pointer to STSE Handler
  * \param[in,out]   p_frame         Pointer to the frame to be received
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_frame_receive(stse_handle_t *p_stse,
                                         stse_frame_t *p_frame);

/**
  * \brief       Transfer Raw Frames to/from target STSAFE-Lxx
  * \details     This core function send and receive frame to/from target STSAFE-Axxx device
  * \param[in]     p_stse      Pointer to STSE Handler
  * \param[in]     p_cmd_frame       Pointer to the command frame
  * \param[in,out]   p_rsp_frame       Pointer to the response frame
  * \param[in]     inter_frame_delay   Delay between command and response frame (in ms)
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_frame_raw_transfer(stse_handle_t *p_stse,
                                              stse_frame_t *p_cmd_frame,
                                              stse_frame_t *p_rsp_frame,
                                              PLAT_UI16 inter_frame_delay);

/**
  * \brief       Transfer Frames to/from target STSAFE-Lxx
  * \details     This core function send and receive frame to/from target STSAFE-Axxx device
  * \param[in]     p_stse      Pointer to STSE Handler
  * \param[in]     p_cmd_frame       Pointer to the command frame
  * \param[in,out]   p_rsp_frame       Pointer to the response frame
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafel_frame_transfer(stse_handle_t *p_stse,
                                          stse_frame_t *p_cmd_frame,
                                          stse_frame_t *p_rsp_frame);

/*! @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEL_FRAME_TRANSFER_H */
