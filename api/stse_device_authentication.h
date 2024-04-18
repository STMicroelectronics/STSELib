/*!
 *******************************************************************************
 * \file	stse_device_authentication.h
 * \brief   STSE Middleware Authentication API (header)
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
#ifndef STSE_DEVICE_AUTHENTICATION_H
#define STSE_DEVICE_AUTHENTICATION_H

/* Includes ------------------------------------------------------------------*/
#include "stselib.h"

/** \defgroup stse_api_authentication STSE Authentication
 *  \ingroup stse_api
 *  \brief		STSE Middleware Authentication API
 *  \details  	The Application Programming Interface (API) layer is the entry point for the upper system application layer. \n
 *  			It provides high level Authentication functions to the application layer.
 *  \{
 */


/* Exported Functions  ------------------------------------------------------------*/

/**
 * \brief Get STSE Device ID
 * \details This function reads the STSE Device ID
 * \param[in] pSTSE the pointer to STSE handler
 * \param[out] device_id the pointer to an array of 11 bytes returning the STSE Device ID
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_get_device_id.dox
 */
stse_ReturnCode_t stse_get_device_id(stse_Handler_t * pSTSE, PLAT_UI8* device_id);

/**
 * \brief Get STSE Device Certificate Size
 * \details This function reads the STSE Certificate Size
 * \param[in] pSTSE 			Pointer to STSE handler
 * \param[out] certificate_size Returned size of the STSE certificate
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details \include{doc} stse_get_device_certificate_size.dox
 */
stse_ReturnCode_t stse_get_device_certificate_size(stse_Handler_t * pSTSE, PLAT_UI16 * certificate_size);

/**
 * \brief Get STSE Device Certificate
 * \details This function reads the STSE Zone where the
 *          certificate is stored
 * \param[in] pSTSE 			Pointer to STSE handler
 * \param[in] certificate_size 	Indicate the STSE certificate size
 * \param[out] STSE_certificate Pointer to an array returning the STSE certificate
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details \include{doc} stse_get_device_certificate.dox
 */
stse_ReturnCode_t stse_get_device_certificate(stse_Handler_t * pSTSE, PLAT_UI16 certificate_size, PLAT_UI8* STSE_certificate);

/**
 * \brief STSE Device Authenticate
 * \details This function is a 1-step authentication for the STSE device, including parsing and verifying a certificate chain
 * \param[in] pSTSE 					Pointer to STSE handler
 * \param[in] pRoot_CA_certificate 		Root CA certificate used to verify the STSE Device certificate
 * \param[in] pCertificate_chain 		Certificate chain containing the STSE Device certificate
 * \param[in] certificate_chain_size	Certificate chain size
 * \param[in] priv_key_slot_number 		Private key slot of the STSE Device associated to the leaf certificate public key
 * \return \ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 * \details 	\include{doc} stse_device_authenticate.dox
 */
stse_ReturnCode_t stse_device_authenticate(
		stse_Handler_t * pSTSE,
		const PLAT_UI8 *pRoot_CA_certificate,
		const PLAT_UI8 *pCertificate_chain,
		PLAT_UI16 certificate_chain_size,
		PLAT_UI8 priv_key_slot_number);

#endif /* STSE_DEVICE_AUTHENTICATION_H */

/** \}*/
