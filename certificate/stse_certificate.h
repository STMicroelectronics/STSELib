/**
  ******************************************************************************
  * \file    stse_certificate.h
  * \author  STMicroelectronics - CS application team
  * \brief   STSAFE-A x509 certificate Parser
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

#ifndef STSE_CERTIFICATE_H
#define STSE_CERTIFICATE_H

#include "certificate/stse_certificate_crypto.h"
#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"
#include "certificate/stse_certificate_types.h"
#include <stdint.h>

/** \defgroup 	stse_certificate_core STSE Certificate parser
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate parser
 *  @{
 */

/* Exported Functions */

/**
 * \brief  Initialize a certificate (\ref stse_certificate_t) to an empty one
 * \param[in,out]  cert 				pointer to the stse_certificate_t to be initialized
 * \note  This functions should be called before parsing a certificate
 */
void stse_certificate_init(stse_certificate_t *cert);

/**
 * \brief  Parse an x509 certificate
 * \param[in]  cert 				pointer to the x509 certificate to be parsed
 * \param[out] stse_certificate 	pointer to the stse_certificate_t that will be filled
 * \param[out] next					pointer to cert array after the parsed certificate (it can be NULL)
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 * \warning The parsed certificated could be empty, it is necessary to call a function
 *       	which verifies the certificate to be valid before using it.
 */
stse_return_code_t stse_certificate_parse(const PLAT_UI8 *cert, stse_certificate_t *stse_certificate, const PLAT_UI8 **next);

/**
 * \brief  Check that an imported x509 certificate is valid
 * \param[in]  stse_certificate 	pointer to the parsed x509 certificate to be validated
 * \param[in]  current_time 			pointer to a stse_cert_validity_t with the current DateTime. If NULL no date check is done
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 * \warning If current_time==NULL the check on the validity dates of the certificate will be bypassed.
 */
stse_return_code_t stse_certificate_is_valid(const stse_certificate_t *stse_certificate, const stse_cert_validity_t *current_time);

/**
 * \brief  Check whether a certificate is marked as belonging to a CA
 * \param[in]  *cert 				pointer to the parsed x509 certificate to be checked
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 */
stse_return_code_t stse_certificate_is_ca(const stse_certificate_t *cert);

/**
 * \brief  Check whether "parent" issued "child"
 * \param[in]  parent 			pointer to the parsed x509 certificate of the supposed issuer of child
 * \param[in]  child  			pointer to the parsed x509 certificate of the certificated supposedly issued by parent
 * \param[in]  current_time 		pointer to a stse_cert_validity_t with the current DateTime. If NULL no date check is done.
 * \return \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
 * \note RFC 5280 mandates an extremely expensive comparison, which we don't perform.
 * 		 As result of this simplification some legitimate parentship relation could be not acknowledged (the opposite should not occur).
 * \warning This check only a single level parent relashion (i.e. whether parent issued child)
 */
stse_return_code_t stse_certificate_is_parent(const stse_certificate_t *parent, const stse_certificate_t *child, const stse_cert_validity_t *current_time);

/**
 * \brief  Copy a certificate (\ref stse_certificate_t) into another structure
 * \param[out]  copied_cert 		pointer to the stse_certificate_t that will be written
 * \param[in]   original_cert 	pointer to the stse_certificate_t that will be copied
 */
void stse_certificate_copy(stse_certificate_t *copied_cert, const stse_certificate_t *original_cert);

/**
 * \brief Iterate over the certificate chain in argument and check each child/parent pairs
 * \param[in] 	root_ca 			Pointer to the root CA certificate buffer
 * \param[in] 	cert_chain 		Pointer to the certificate chain buffer
 * \param[in] 	cert_chain_size 	Size of the certificate chain buffer
 * \param[out] 	leaf_cert 		Pointer to the leaf certificate structure
 * \return PLAT_I32 : 0 on success ; error code otherwise
 */
stse_return_code_t stse_certificate_parse_chain(
    PLAT_UI8 *root_ca,
    PLAT_UI8 *cert_chain,
    PLAT_UI16 cert_chain_size,
    stse_certificate_t *leaf_cert);

/**
 * \brief Assign the certificate parser STSAFE-A companion
 * \param[in] 	p_stse 	Pointer to STSAFE handler
 */
void stse_certificate_set_stse_companion(stse_handle_t *p_stse);

/**
 * \brief Reset the certificate parser STSAFE-A companion
 */
void stse_certificate_reset_stse_companion();

/** @}*/

#endif /* STSE_CERTIFICATE_H */
