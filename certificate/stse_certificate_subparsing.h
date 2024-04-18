/**
  ******************************************************************************
  * \file    stse_certificate_subparsing.h
  * \author  STMicroelectronics - CS application team
  * \brief   Helper for the x509 certificate Parser
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

#ifndef STSE_CERTIFICATE_SUBPARSING_H
#define STSE_CERTIFICATE_SUBPARSING_H

#include "certificate/stse_certificate_types.h"

/** \defgroup 	stse_certificate_subparsing STSE Certificate subparsing routines
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate subparsing routines
 *  \{
 */

//void stse_certificate_parse_integerValue(const PLAT_UI8 *value, PLAT_I32 size, const PLAT_UI8 **outp, PLAT_I32 *outSize);

/**
* \brief  Identify the ASN.1 TLV
* \param[in]  *asn1 pointer to the ASN.1 TLV
* \param[out] *parsed will contain the number of processed bytes (rarely used)
* \param[out] *size  will contain the size (in bytes) of the value field of this ASN.1 TLV
* \param[out] **value  will contain a pointer to the value field of this ASN.1 TLV
* \return   The internal TAG identifier of this TLV or -1 if failure
*/
PLAT_I32 stse_certificate_identify_ASN1_TLV(const PLAT_UI8 *asn1, PLAT_I32 *parsed, PLAT_I32 *size, const PLAT_UI8 **value);

/**
* \brief  Identify the Name Attribute from its OID
* \param[in]  *oid The value of the OID of the Attribute (got from \ref stse_certificate_identify_ASN1_TLV )
* \param[in]  size size (in bytes) of the value field of this OID (got from \ref stse_certificate_identify_ASN1_TLV )
* \note  Only a very limited set of Attributes is supported
* \return    The internal type identifing the Attribute or -1 for failure
*/
PLAT_I32 stse_certificate_identify_attribute(const PLAT_UI8 *oid, PLAT_I32 size);

/**
* \brief  Parse an ECDSA signature
* \param[in]  *signature pointer to the expected signature field of the x509 certificate
* \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
* \param[out] **next_thing  will contain a pointer to the next TLV
*/
void stse_certificate_parse_ECDSA_signature(const PLAT_UI8 *signature, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing);

/**
* \brief  Parse the x509 Version of a certificate
* \param[in]  *x509VersionField pointer to the version field of a certificate
* \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
* \param[out] **next_thing output pointer to next TLV
*/
void stse_certificate_parse_X509_version(const PLAT_UI8 *x509VersionField, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing);

/**
* \brief  Parse a SignatureAlgorithm (or signature of a tbsCertificate)
* \param[in]  *SA pointer to the SignatureAlgorithm (or signature of a tbsCertificate)
* \param[out] *singatureAlgorithm integer that will be filled with the internal encoding of signatureAlgorithm
* \param[out] **next_thing output pointer to next TLV
* \note  Only a very limited set of SignatureAlgorithm is supported
*/
void stse_certificate_parse_signature_algorithm(const PLAT_UI8 *SA, PLAT_I32 *singatureAlgorithm, const PLAT_UI8 **next_thing);

/**
* \brief  Parse an ECC public Key from a certificate
* \param[in]  *EccPK pointer to the ECC public Key
* \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
* \param[out] **next_thing output pointer to next TLV
*/
void stse_certificate_parse_ECC_public_key(const PLAT_UI8 *EccPK, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing);

/**
* \brief  Parse an ASN.1 INTEGER
* \param[in]  *integer pointer to the expected INTEGER TLV
* \param[out] *outp will point to the starting byte of the integer
* \param[out] *outSize will contain the size of the integer in bytes
* \param[out] **next_thing  will contain a pointer to the next TLV
* \note This doesn't copy the integer. To keep memory small this just returns a pointer to it.
* \warning only non-negative integers are supported
*/
void stse_certificate_parse_integer(const PLAT_UI8 *integer, const PLAT_UI8 **outp, PLAT_I32 *outSize, const PLAT_UI8 **next_thing);

/**
* \brief  Parse the validity of a certificate
* \param[in]  *p pointer to the Name Attribute SET to be parsed
* \param[out] *notBefore_st pointer to stse_cert_validity_t structure that will be filled with the "not before" date
* \param[out] *notAfter_st  pointer to stse_cert_validity_t structure that will be filled with the "not after" date
* \param[out] **next_thing output pointer to next TLV
* \note In this function next_thing can be NULL
*/
void stse_certificate_parse_validity(const PLAT_UI8 *p, stse_cert_validity_t *notBefore_st, stse_cert_validity_t *notAfter_st, const PLAT_UI8 **next_thing);

/**
* \brief  Parse an RelativeDistinguishedName
* \param[in]  *p pointer to the RelativeDistinguishedName (expeting a SET)
* \param[out] **nextRDN output pointer to next RDN
* \param[out] **attribute output pointer to RDN first attribute
*/
void stse_certificate_parse_relative_distinguished_name(const PLAT_UI8 *p, const PLAT_UI8 **nextRDN, const PLAT_UI8 **attribute);

/**
* \brief  Parse a Name Attribute
* \param[in]  *p pointer to the AttributeTypeAndValue SEQUENCE to be parsed
* \param[out] *attribute_st pointer to stse_cert_attribute_t structure that will be filled
* \param[out] **next_thing output pointer to next TLV
*/
void stse_certificate_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st, const PLAT_UI8 **next_thing);

/**
* \brief  Compare two stse_cert_validity_t structures
* \param[in]  *D1 pointer to the first stse_cert_validity_t
* \param[in]  *D2 pointer to the second stse_cert_validity_t
* \return Result of Comparison
* \retval   -1  D1 < D2
* \retval    0  D1 = D2
* \retval    1  D1 > D2
*/
PLAT_I32 stse_certificate_date_compare(const stse_cert_validity_t *D1, const stse_cert_validity_t *D2);

/**
* \brief  Count the number of Attributes within a NAME
* \param[in]  *p pointer to the RDNSequence to be parsed
* \returun The number of Attributes
*/
PLAT_I32 stse_certificate_count_attributes(const PLAT_UI8 *p);

/**
* \brief  String Comparison (case insensitive and for utf8)
* \param[in]  *p1 first string to compare
* \param[in]  *p2 second string to compare
* \param[in]  size p1/p2 size
* \return  result of comparison
* \retval -1 strings are different
* \retval 0 strings match
*/
PLAT_I32 stse_certificate_case_insensitive_compare(const PLAT_UI8 *p1, const PLAT_UI8 *p2, PLAT_I32 size);

/**
* \brief  Identify the Extension from its OID
* \param[in]  *oid The value of the OID of the Extension (got from \ref stse_certificate_identify_ASN1_TLV )
* \param[in]  size size (in bytes) of the value field of this OID (got from \ref stse_certificate_identify_ASN1_TLV )
* \note  Only a very limited set of Extension is supported
* \return    The internal type identifing the Extension or -1 for failure
*/
PLAT_I32 stse_certificate_case_identify_extension(const PLAT_UI8 *oid, PLAT_I32 size);

/**
* \brief  Parse an x509 Extension
* \param[in]  *ext pointer to the extension
* \param[out]  extFlags pointer to a field extensionsFlags of an stse_certificate_t structure
* \param[out]  *ext_st pointer to an stse_cert_extension_t structure
*/
void stse_certificate_parse_extension(const PLAT_UI8 *ext, PLAT_UI32 *extFlags, stse_cert_extension_t *ext_st);

/**
* \brief  Reads the value of an INTEGER and returns it as a PLAT_I32
* \param[in]  *value pointer to the value field of an INTEGER TLV (got from \ref stse_certificate_identify_ASN1_TLV)
* \param[in]  size size of the integer (got from \ref stse_certificate_identify_ASN1_TLV)
* \return the integer value as an PLAT_I32
* \warning only non-negative integers are supported
*/
PLAT_I32 stse_certificate_get_small_integer(const PLAT_UI8 *value, PLAT_I32 size);

/** \}*/

#endif /* STSE_CERTIFICATE_SUBPARSING_H */
