/**
  ******************************************************************************
  * @file    stsafea_wrap_unwrap.h
  * @author  CS Application Team
  * @brief   STSAFE-A services for wrap & unwrap (header)
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

#ifndef STSAFEA_WRAP_UNWRAP_H
#define STSAFEA_WRAP_UNWRAP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_frame.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "services/stsafea/stsafea_commands.h"
#include "services/stsafea/stsafea_sessions.h"
#include "services/stsafea/stsafea_timings.h"

#define STSAFEA_WRAP_LOCAL_ENVELOPE_MAX_PAYLOAD_SIZE ((PLAT_UI16)480U)
#define STSAFEA_LOCAL_ENVELOPE_HEADER_SIZE ((PLAT_UI16)8U)
#define STSAFEA_UNWRAP_LOCAL_ENVELOPE_MAX_SIZE \
  (STSAFEA_WRAP_LOCAL_ENVELOPE_MAX_PAYLOAD_SIZE + STSAFEA_LOCAL_ENVELOPE_HEADER_SIZE)

/*! \defgroup stsafea_wrap STSAFE-A Wrap/Un-wrap
 *  \ingroup stsafea_services
 *  \{
 */

/**
  * \brief     STSAFEA wrap service
  * \details   This service format and send STSAFEA wrap command/response to target STSE
  * \param[in] p_stse          Pointer to target SE handler
  * \param[in] wrap_key_slot     Wrap key slot
  * \param[in]   p_payload       Pointer to the payload buffer to be wrapped
  * \param[in] payload_size      size of the payload buffer
  * \param[out]  p_wrapped_payload   Pointer to the wrapped payload buffer
  * \param[in] wrapped_payload_size  size of the wrapped payload buffer
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_wrap_payload(
  stse_handle_t *p_stse,
  PLAT_UI8 wrap_key_slot,
  PLAT_UI8 *p_payload,
  PLAT_UI16 payload_size,
  PLAT_UI8 *p_wrapped_payload,
  PLAT_UI16 wrapped_payload_size);

/**
  * \brief     STSAFEA un-wrap service
  * \details   This service format and send STSAFEA un-wrap command/response to target STSE
  * \param[in] p_stse          Pointer to target SE handler
  * \param[in] wrap_key_slot     wrap key slot
  * \param[in] p_wrapped_payload   Pointer to the wrapped payload buffer to be un-wrapped
  * \param[in] wrapped_payload_size  Size of the wrapped payload buffer
  * \param[out]  p_payload       Pointer to the plain text payload buffer
  * \param[in] payload_size      Size of the payload buffer
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stsafea_unwrap_payload(
  stse_handle_t *p_stse,
  PLAT_UI8 wrap_key_slot,
  PLAT_UI8 *p_wrapped_payload,
  PLAT_UI16 wrapped_payload_size,
  PLAT_UI8 *p_payload,
  PLAT_UI16 payload_size);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*STSAFEA_WRAP_UNWRAP_H*/
