/**
  ******************************************************************************
  * @file    stse_certificate_subparsing.h
  * @author  CS Application Team
  * @brief   Helper for the x509 certificate Parser
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

#ifndef STSE_CERTIFICATE_SUBPARSING_H
#define STSE_CERTIFICATE_SUBPARSING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "certificate/stse_certificate_types.h"

/** \defgroup   stse_certificate_subparsing STSE Certificate subparsing routines
  *  \ingroup  stse_certificate
  *  \brief    STSE Certificate subparsing routines
  *  \{
  */

/* void stse_certificate_parse_integer_value(const PLAT_UI8 *value, PLAT_I32 size, */
/*                                           const PLAT_UI8 **outp, PLAT_I32 *out_size); */

/**
  * \brief  Identify the ASN.1 TLV
  * \param[in]  *asn1 pointer to the ASN.1 TLV
  * \param[out] *parsed will contain the number of processed bytes (rarely used)
  * \param[out] *size  will contain the size (in bytes) of the value field of this ASN.1 TLV
  * \param[out] **value  will contain a pointer to the value field of this ASN.1 TLV
  * \return   The internal TAG identifier of this TLV or -1 if failure
  */
PLAT_I32 stse_certificate_identify_asn1_tlv(const PLAT_UI8 *asn1, PLAT_I32 *parsed, PLAT_I32 *size,
                                            const PLAT_UI8 **value);

/**
  * \brief  Identify the Name Attribute from its OID
  * \param[in]  *oid The value of the OID of the Attribute (got from \ref stse_certificate_identify_asn1_tlv )
  * \param[in]  size size (in bytes) of the value field of this OID (got from \ref stse_certificate_identify_asn1_tlv )
  * \note  Only a very limited set of Attributes is supported
  * \return    The internal type identifying the Attribute or -1 for failure
  */
PLAT_I32 stse_certificate_identify_attribute(const PLAT_UI8 *oid, PLAT_I32 size);

/**
  * \brief  Parse an ECDSA signature
  * \param[in]  *signature pointer to the expected signature field of the x509 certificate
  * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
  * \param[out] **next_thing  will contain a pointer to the next TLV
  */
void stse_certificate_parse_ecdsa_signature(const PLAT_UI8 *signature, stse_certificate_t *stse_certificate,
                                            const PLAT_UI8 **next_thing);

/**
  * \brief  Parse the x509 Version of a certificate
  * \param[in]  *x509_version_field pointer to the version field of a certificate
  * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
  * \param[out] **next_thing output pointer to next TLV
  */
void stse_certificate_parse_x509_version(const PLAT_UI8 *x509_version_field, stse_certificate_t *stse_certificate,
                                         const PLAT_UI8 **next_thing);

/**
  * \brief  Parse a signature_algorithm (or signature of a tbs_certificate)
  * \param[in]  *SA pointer to the signature_algorithm (or signature of a tbs_certificate)
  * \param[out] *singature_algorithm integer that will be filled with the internal encoding of signature_algorithm
  * \param[out] **next_thing output pointer to next TLV
  * \note  Only a very limited set of signature_algorithm is supported
  */
void stse_certificate_parse_signature_algorithm(const PLAT_UI8 *SA, PLAT_I32 *singature_algorithm,
                                                const PLAT_UI8 **next_thing);

/**
  * \brief  Parse an ECC public Key from a certificate
  * \param[in]  *ecc_pk pointer to the ECC public Key
  * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
  * \param[out] **next_thing output pointer to next TLV
  */
void stse_certificate_parse_ecc_public_key(const PLAT_UI8 *ecc_pk, stse_certificate_t *stse_certificate,
                                           const PLAT_UI8 **next_thing);

/**
  * \brief  Parse an ASN.1 INTEGER
  * \param[in]  *integer pointer to the expected INTEGER TLV
  * \param[out] *outp will point to the starting byte of the integer
  * \param[out] *out_size will contain the size of the integer in bytes
  * \param[out] **next_thing  will contain a pointer to the next TLV
  * \note This doesn't copy the integer. To keep memory small this just returns a pointer to it.
  * \warning only non-negative integers are supported
  */
void stse_certificate_parse_integer(const PLAT_UI8 *integer, const PLAT_UI8 **outp, PLAT_I32 *out_size,
                                    const PLAT_UI8 **next_thing);

/**
  * \brief  Parse the validity of a certificate
  * \param[in]  *p pointer to the Name Attribute SET to be parsed
  * \param[out] *not_before_st pointer to stse_cert_validity_t structure that will be filled with the "not before" date
  * \param[out] *not_after_st  pointer to stse_cert_validity_t structure that will be filled with the "not after" date
  * \param[out] **next_thing output pointer to next TLV
  * \note In this function next_thing can be NULL
  */
void stse_certificate_parse_validity(const PLAT_UI8 *p, stse_cert_validity_t *not_before_st,
                                     stse_cert_validity_t *not_after_st, const PLAT_UI8 **next_thing);

/**
  * \brief  Parse an RelativeDistinguishedName
  * \param[in]  *p pointer to the RelativeDistinguishedName (expecting a SET)
  * \param[out] **next_rdn output pointer to next RDN
  * \param[out] **attribute output pointer to RDN first attribute
  */
void stse_certificate_parse_relative_distinguished_name(const PLAT_UI8 *p, const PLAT_UI8 **next_rdn,
                                                        const PLAT_UI8 **attribute);

/**
  * \brief  Parse a Name Attribute
  * \param[in]  *p pointer to the AttributeTypeAndValue SEQUENCE to be parsed
  * \param[out] *attribute_st pointer to stse_cert_attribute_t structure that will be filled
  * \param[out] **next_thing output pointer to next TLV
  */
void stse_certificate_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st,
                                      const PLAT_UI8 **next_thing);

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
  * \return The number of Attributes
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
  * \param[in]  *oid The value of the OID of the Extension (got from \ref stse_certificate_identify_asn1_tlv )
  * \param[in]  size size (in bytes) of the value field of this OID (got from \ref stse_certificate_identify_asn1_tlv )
  * \note  Only a very limited set of Extension is supported
  * \return    The internal type identifying the Extension or -1 for failure
  */
PLAT_I32 stse_certificate_case_identify_extension(const PLAT_UI8 *oid, PLAT_I32 size);

/**
  * \brief  Parse an x509 Extension
  * \param[in]  *ext pointer to the extension
  * \param[out]  ext_flags pointer to a field extensions_flags of an stse_certificate_t structure
  * \param[out]  *ext_st pointer to an stse_cert_extension_t structure
  */
void stse_certificate_parse_extension(const PLAT_UI8 *ext, PLAT_UI32 *ext_flags, stse_cert_extension_t *ext_st);

/**
  * \brief  Reads the value of an INTEGER and returns it as a PLAT_I32
  * \param[in]  *value pointer to the value field of an INTEGER TLV (got from \ref stse_certificate_identify_asn1_tlv)
  * \param[in]  size size of the integer (got from \ref stse_certificate_identify_asn1_tlv)
  * \return the integer value as an PLAT_I32
  * \warning only non-negative integers are supported
  */
PLAT_I32 stse_certificate_get_small_integer(const PLAT_UI8 *value, PLAT_I32 size);

/** \}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_CERTIFICATE_SUBPARSING_H */
