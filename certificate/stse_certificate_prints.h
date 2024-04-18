/**
  ******************************************************************************
  * \file    stse_certificate_prints.h
  * \author  STMicroelectronics - CS application team
  * \brief   Informational functions for the x509 parsing
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

#ifndef STSE_CERTIFICATE_PRINTS_H
#define STSE_CERTIFICATE_PRINTS_H

#include "certificate/stse_certificate_types.h"

/** \defgroup 	stse_certificate_prints STSE Certificate prints functions
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate prints functions
 *  \{
 */

/**
 * \brief  		Print the Attribute
 * \param[in]	type 	internal type of Attribute
 */
void stse_certificate_print_attribute(PLAT_I32 type);

/**
 * \brief  		Print the Signature Algorithm
 * \param[in] 	type internal type of SignatureAlgorithm
 */
void stse_certificate_print_signature_algorithm(PLAT_I32 type);

/**
 * \brief  Print the TAG
 * \param[in]	tag tag of ASN.1 TLV  (got from \ref stse_certificate_identify_ASN1_TLV )
 */
void stse_certificate_print_tag(PLAT_I32 tag);



/**
 * \brief  		Print the Elliptic Curve Name
 * \param[in]  	type internal type of EllipticCurve
 */
void stse_certificate_print_elliptic_curve(PLAT_I32 type);

/**
 * \brief  Print an INTEGER
 * \param[in]  *value pointer to the value of a INTEGER (got from \ref stse_certificate_identify_ASN1_TLV )
 * \param[in]  size size (in bytes) of the value field of this INTEGER (got from \ref stse_certificate_identify_ASN1_TLV )
 * \retval    0 Success
 * \retval    -1 Failure. The INTEGER is negative, and \
 *                        this implementation doesn't support it
 */
PLAT_I32 stse_certificate_print_integer(const PLAT_UI8 *value, PLAT_I32 size);

/**
 * \brief  Print a BITSTRING
 * \param[in]  *value pointer to the value of a BITSTRING (got from \ref stse_certificate_identify_ASN1_TLV )
 * \param[in]  size size (in bytes) of the value field of this BITSTRING (got from \ref stse_certificate_identify_ASN1_TLV )
 * \retval    0 Success
 * \retval    -1 Failure. The bitstring excludes some of the LSBits of the LSB, which \
 *                        this implementation doesn't support
 */
PLAT_I32 stse_certificate_print_bit_string(const PLAT_UI8 *value, PLAT_I32 size);

/** \brief print informations about a stse_certificate_t certificate */
void stse_certificate_print_parsed_cert(stse_certificate_t *stse_certificate);

/** \brief print the attribute of a name field */
void stse_certificate_print_name(const PLAT_UI8 *name, PLAT_I32 size);

/** \brief print the validity of a certificate */
void stse_certificate_print_validity(const PLAT_UI8 *validity);

/**
 * \brief  Parse a Name Attribute
 * \param[in]  *p pointer to the Name Attribute SET to be parsed
 * \param[out] *attribute_st pointer to stse_cert_attribute_t structure that will be filled
 * \param[out] **next_thing output pointer to next TLV
 * \note partial parse function (for printing only)
 */
void stse_certificate_simple_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st, const PLAT_UI8 **next_thing);

/** \}*/

#endif /* STSE_CERTIFICATE_PRINTS_H */
