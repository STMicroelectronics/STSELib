/**
  ******************************************************************************
  * @file    stse_device_authentication.h
  * @author  CS Application Team
  * @brief   STSE Middleware Authentication API (header)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef STSE_DEVICE_AUTHENTICATION_H
#define STSE_DEVICE_AUTHENTICATION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stselib.h"

/** \defgroup stse_api_authentication STSE Authentication
  *  \ingroup stse_api
  *  \brief    STSE Middleware Authentication API
  *  \details    The Application Programming Interface (API) layer is the entry point for the upper system
  *              application layer. \n
  *              It provides high level Authentication functions to the application layer.
  *  \{
  */

/* Exported Functions  ------------------------------------------------------------*/

/**
  * \brief Get STSE Device ID
  * \details This function reads the STSE Device ID
  * \param[in] p_stse      the pointer to STSE handler
  * \param[in] certificate_zone  Zone containing STSE Device certificate chain
  * \param[out] p_device_id    the pointer to an array of 11 bytes returning the STSE Device ID
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  * \details   \include{doc} stse_get_device_id.dox
  */
stse_return_code_t stse_get_device_id(stse_handle_t *p_stse,
                                      PLAT_UI8 certificate_zone,
                                      PLAT_UI8 *p_device_id);

/**
  * \brief Get STSE Device Certificate Size
  * \details This function reads the STSE Certificate Size
  * \param[in] p_stse      Pointer to STSE handler
  * \param[in] certificate_zone  Zone containing STSE Device certificate chain
  * \param[out] p_certificate_size Returned size of the STSE certificate
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  * \details \include{doc} stse_get_device_certificate_size.dox
  */
stse_return_code_t stse_get_device_certificate_size(stse_handle_t *p_stse,
                                                    PLAT_UI8 certificate_zone,
                                                    PLAT_UI16 *p_certificate_size);

/**
  * \brief Get STSE Device Certificate
  * \details This function reads the STSE Zone where the
  *          certificate is stored
  * \param[in] p_stse      Pointer to STSE handler
  * \param[in] certificate_zone  Zone containing STSE Device certificate chain
  * \param[in] certificate_size  Indicate the STSE certificate size
  * \param[out] p_certificate  Pointer to an array returning the STSE certificate
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  * \details \include{doc} stse_get_device_certificate.dox
  */
stse_return_code_t stse_get_device_certificate(stse_handle_t *p_stse,
                                               PLAT_UI8 certificate_zone,
                                               PLAT_UI16 certificate_size,
                                               PLAT_UI8 *p_certificate);

/**
  * \brief STSE Device Authenticate
  * \details This function is a 1-step authentication for the STSE device, including parsing and verifying a
  *          certificate chain
  * \param[in] p_stse          Pointer to STSE handler
  * \param[in] p_root_ca_certificate     Root CA certificate used to verify the STSE Device certificate
  * \param[in] certificate_zone      Zone containing STSE Device certificate chain
  * \param[in] priv_key_slot_number    Private key slot of the STSE Device associated to the leaf certificate public key
  * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  * \details   \include{doc} stse_device_authenticate.dox
  */
stse_return_code_t stse_device_authenticate(
  stse_handle_t *p_stse,
  const PLAT_UI8 *p_root_ca_certificate,
  PLAT_UI8 certificate_zone,
  PLAT_UI8 priv_key_slot_number);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_DEVICE_AUTHENTICATION_H */

/** \}*/
