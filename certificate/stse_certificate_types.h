/**
  ******************************************************************************
  * \file    stse_certificate_types.h
  * \author  STMicroelectronics - CS application team
  * \brief   STSE certificate type
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

#ifndef STSE_CERTIFICATE_TYPES_H
#define STSE_CERTIFICATE_TYPES_H

#include "core/stse_device.h"
#include "core/stse_return_codes.h"
#include <stdint.h>

/** \defgroup 	stse_certificate_core STSE Certificate parser
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate parser
 *  @{
 */

/** \brief Definitions of X509 TAGs */
typedef PLAT_UI8 stse_certificate_tag_t;
#define TAG_BOOLEAN ((stse_certificate_tag_t)0x01U)           /*!< x509 tag for BOOLEAN */
#define TAG_INTEGER ((stse_certificate_tag_t)0x02U)           /*!< x509 tag for INTEGER */
#define TAG_BITSTRING ((stse_certificate_tag_t)0x03U)         /*!< x509 tag for BITSTRING */
#define TAG_OCTETSTRING ((stse_certificate_tag_t)0x04U)       /*!< x509 tag for OCTETSTRING */
#define TAG_NULL ((stse_certificate_tag_t)0x05U)              /*!< x509 tag for NULL */
#define TAG_OBJECT_IDENTIFIER ((stse_certificate_tag_t)0x06U) /*!< x509 tag for OBJECT_IDENTIFIER */
#define TAG_ObjectDescriptor ((stse_certificate_tag_t)0x07U)  /*!< x509 tag for ObjectDescriptor */
#define TAG_ENUMERATED ((stse_certificate_tag_t)0x0AU)        /*!< x509 tag for ENUMERATED */
#define TAG_UTF8String ((stse_certificate_tag_t)0x0CU)        /*!< x509 tag for UTF8String */
#define TAG_PrintableString ((stse_certificate_tag_t)0x13U)   /*!< x509 tag for PrintableString */
#define TAG_IA5String ((stse_certificate_tag_t)0x16U)         /*!< x509 tag for IA5String */
#define TAG_UTCTime ((stse_certificate_tag_t)0x17U)           /*!< x509 tag for UTCTime */
#define TAG_GeneralizedTime ((stse_certificate_tag_t)0x18U)   /*!< x509 tag for GeneralizedTime */
#define TAG_SEQUENCE ((stse_certificate_tag_t)0x30U)          /*!< x509 tag for SEQUENCE */
#define TAG_SET ((stse_certificate_tag_t)0x31U)               /*!< x509 tag for SET */
#define TAG_x509VERSION ((stse_certificate_tag_t)0xA0U)       /*!< x509 tag for x509VERSION */
#define TAG_issuerUniqueID ((stse_certificate_tag_t)0xA1U)    /*!< x509 tag for issuerUniqueID */
#define TAG_subjectUniqueID ((stse_certificate_tag_t)0xA2U)   /*!< x509 tag for subjectUniqueID */
#define TAG_extensions ((stse_certificate_tag_t)0xA3U)        /*!< x509 tag for extensions */

/** \brief Definitions of available signature algorithms */
typedef PLAT_UI8 stse_certificate_signature_algorithms_t;
#define SIG_ECDSA_SHA1 ((stse_certificate_signature_algorithms_t)0U)    /*!< ECDSA with SHA1 algorithm */
#define SIG_ECDSA_SHA224 ((stse_certificate_signature_algorithms_t)1U)  /*!< ECDSA with SHA224 algorithm */
#define SIG_ECDSA_SHA256 ((stse_certificate_signature_algorithms_t)2U)  /*!< ECDSA with SHA256 algorithm */
#define SIG_ECDSA_SHA384 ((stse_certificate_signature_algorithms_t)3U)  /*!< ECDSA with SHA384 algorithm */
#define SIG_ECDSA_SHA512 ((stse_certificate_signature_algorithms_t)4U)  /*!< ECDSA with SHA512 algorithm */
#define SIG_EDDSA_ED25519 ((stse_certificate_signature_algorithms_t)5U) /*!< EDDSA with ED25519 algorithm */

/** \brief Definitions of supported Name Attributes */
typedef PLAT_UI8 stse_certificate_name_attributes_t;
#define ATTR_CN ((stse_certificate_name_attributes_t)3U)   /*!< CommonName */
#define ATTR_SN ((stse_certificate_name_attributes_t)5U)   /*!< SerialNumber */
#define ATTR_C ((stse_certificate_name_attributes_t)6U)    /*!< Country */
#define ATTR_LN ((stse_certificate_name_attributes_t)7U)   /*!< LocalityName */
#define ATTR_SOPN ((stse_certificate_name_attributes_t)8U) /*!< stateOrProvinceName */
#define ATTR_ON ((stse_certificate_name_attributes_t)10U)  /*!< Organization Name */
#define ATTR_OUN ((stse_certificate_name_attributes_t)11U) /*!< Organization Unit Name */
#define ATTR_UID ((stse_certificate_name_attributes_t)45U) /*!< UniqueIdentifier */
#define ATTR_DN ((stse_certificate_name_attributes_t)49U)  /*!< DistinguishedName */

/** \brief Definitions of supported Elliptic Curve Names */
typedef PLAT_UI8 stse_certificate_elliptic_curves_t;
#define EC_P256 ((stse_certificate_elliptic_curves_t)0U)    /*!< NIST P-256 */
#define EC_P384 ((stse_certificate_elliptic_curves_t)1U)    /*!< NIST P-384 */
#define EC_P521 ((stse_certificate_elliptic_curves_t)2U)    /*!< NIST P-521 */
#define EC_bp256r1 ((stse_certificate_elliptic_curves_t)3U) /*!< brainpoolP256r1 */
#define EC_bp256t1 ((stse_certificate_elliptic_curves_t)4U) /*!< brainpoolP256t1 */
#define EC_bp384r1 ((stse_certificate_elliptic_curves_t)5U) /*!< brainpoolP384r1 */
#define EC_bp384t1 ((stse_certificate_elliptic_curves_t)6U) /*!< brainpoolP384t1 */
#define EC_bp512r1 ((stse_certificate_elliptic_curves_t)7U) /*!< brainpoolP512r1 */
#define EC_bp512t1 ((stse_certificate_elliptic_curves_t)8U) /*!< brainpoolP512t1 */
#define EC_Ed25519 ((stse_certificate_elliptic_curves_t)9U) /*!< Ed25519 */

/** \brief Definitions of supported Extensions types */
typedef PLAT_UI8 stse_certificate_extensions_type_t;
#define EXTENSION_KU ((stse_certificate_extensions_type_t)15U)  /*!< KeyUsage */
#define EXTENSION_BC ((stse_certificate_extensions_type_t)19U)  /*!< BasicContrains */
#define EXTENSION_EKU ((stse_certificate_extensions_type_t)37U) /*!< extKeyUsage */

/** \brief Structure used to keep SignatureAlgorithm OIDs */
struct SignatureAlgorithmOID_st {
    PLAT_I32 len;    /*!< size of the encoded OID */
    PLAT_I32 type;   /*!< type of OID */
    PLAT_UI8 oid[8]; /*!< encoded OID */
};

/** \brief Structure used to keep Name Attribute OIDs */
struct AttributeOID_st {
    PLAT_I32 len;    /*!< size of the encoded OID */
    PLAT_I32 type;   /*!< type of OID */
    PLAT_UI8 oid[3]; /*!< encoded OID */
};

/** \brief Structure used to keep named Elliptic Curve OIDs */
struct EllipticCurveOID_st {
    PLAT_I32 len;    /*!< size of the encoded OID */
    PLAT_I32 type;   /*!< type of OID */
    PLAT_UI8 oid[9]; /*!< encoded OID */
};

/** \brief typedef for the attribute */
typedef struct attribute_st {
    PLAT_I32 type;       /*!< type of attribute (corresponding to OID) */
    PLAT_I32 strFormat;  /*!< format of the string */
    const PLAT_UI8 *str; /*!< pointer to the string of the attribute */
    PLAT_I32 strSize;    /*!< size of the string */
} stse_cert_attribute_t;

/** \brief Structure used to keep Name Attribute OIDs */
struct ExtensionOID_st {
    PLAT_I32 len;    /*!< size of the encoded OID */
    PLAT_I32 type;   /*!< type of OID */
    PLAT_UI8 oid[5]; /*!< encoded OID */
};

/** \brief typedef for the extension */
typedef struct extension_st {
    PLAT_I32 type;         /*!< type of extension (corresponding to OID) */
    PLAT_I32 critical;     /*!< critical */
    const PLAT_UI8 *value; /*!< pointer to the value hold by the extension (this is an octet string)  */
    PLAT_I32 valueSize;    /*!< size of the string */
} stse_cert_extension_t;

/** \brief typedef for the structure keeping the validity */
typedef struct validity_st {
    PLAT_UI8 seconds; /*!< seconds (range 0 to 59) */
    PLAT_UI8 minutes; /*!< minutes (range 0 to 59) */
    PLAT_UI8 hours;   /*!< hours   (range 0 to 23) */
    PLAT_UI8 days;    /*!< days    (range 1 to 31) */
    PLAT_UI8 month;   /*!< month   (range 1 to 12) */
    PLAT_UI32 year;   /*!< years 0 to whatever     */
} stse_cert_validity_t;

/** \brief typedef for the structure keeping the Ellipitc Curve Public Key */
typedef struct stse_cert_public_key_t {
    const PLAT_UI8 *pX; /*!<  Pointer to X Coordinate */
    const PLAT_UI8 *pY; /*!<  Pointer to Y Coordiante */
    PLAT_I32 fsize;     /*!< Size of the field in bytes */
} stse_cert_public_key_t;

/** \brief typedef for the structure keeping the ECDSA Signature */
typedef struct stse_cert_signature_t {
    const PLAT_UI8 *pR; /*!< Pointer to r */
    PLAT_I32 rSize;     /*!< size to r */
    const PLAT_UI8 *pS; /*!<  Pointer to s*/
    PLAT_I32 sSize;     /*!< size to s */
} stse_cert_signature_t;

/** \brief internal structure used to keep the values parsed from the x509 */
struct stse_internal_certificate {
    const PLAT_UI8 *pPubKey_point_representation_id; /*!< 0x04: Uncompressed key, 0x02 or 0x03: Compressed key */
    const PLAT_UI8 *tbs;                             /*!<  Pointer to tcs field */
    PLAT_I32 tbsSize;                                /*!<  Size of tcs */
    PLAT_I32 x509Version;                            /*!<  x509 Version */
    const PLAT_UI8 *serialNumber;                    /*!<  Pointer to SerialNumber*/
    PLAT_I32 serialNumberSize;                       /*!<  Size of SerialNumber */
    PLAT_I32 signature;                              /*!<  Signature (algorithm) but from tcs */
    const PLAT_UI8 *issuer;                          /*!<  Pointer to Issuer */
    PLAT_I32 issuerSize;                             /*!<  Size of Issuer */
    const PLAT_UI8 *validity;                        /*!<  Pointer to Validity */
    PLAT_I32 validitySize;                           /*!<  Size of Validity */
    const PLAT_UI8 *subject;                         /*!<  Pointer to subject */
    PLAT_I32 subjectSize;                            /*!<  Size of Subject */
    PLAT_I32 EllipticCurve;                          /*!<  Identifies the Elliptic Curve */
    stse_cert_public_key_t PubKey;                   /*!<  Contain the struct \ref stse_cert_public_key_t */
    const PLAT_UI8 *extensions;                      /*!<  Pointer to Extensions */
    PLAT_I32 extensionsSize;                         /*!<  Size of Extensions */
    PLAT_UI32 extensionsFlags;                       /*!<  Integer encoding the fields present in the tcs:			\n
									 *	bit 0:   BasicConstraints is present						\n
									 *  bit 1:   BasicConstraints is critical						\n
									 *  bit 2:   BasicConstraints indicates this is a CA			\n
									 *  bit 3: 	 BasicConstraints has a pathLenConstraint			\n
									 *  bit 4-7: PathLen											\n
									 *  bit 8: 	 keyUsage is present								\n
									 *  bit 9: 	 keyUsage is critical								\n
									 *  bit 15:  keyUsage field decipherOnly						\n
									 *  bit 16:  keyUsage field digitalSignature					\n
									 *  bit 17:  keyUsage field contentCommitment/nonRepudiation	\n
									 *  bit 18:  keyUsage field keyEncipherment						\n
									 *  bit 19:  keyUsage field dataEncipherment					\n
									 *  bit 20:  keyUsage field keyAgreement						\n
									 *  bit 21:  keyUsage field keyCertSign							\n
									 *  bit 22:  keyUsage field cRLSign								\n
									 *  bit 23:  keyUsage field encipherOnly						\n
									 *  bit 24:  ExtendedKeyUsage is present						\n
									 *  bit 25:  ExtendedKeyUsage is critical						\n
									 *  bit 27-29: Unused
                              	  	 */
    PLAT_I32 SignatureAlgorithm;                     /*!<  SignatureAlgorithm */
    stse_cert_signature_t Sign;                      /*!<  Contain the struct \ref stse_cert_signature_t */
};

/** \brief typedef for the stse_certificate_t */
typedef struct stse_internal_certificate stse_certificate_t;

/* Exported Macros */

/** \brief Macro which checks if a TAG is valid (known) */
#define IS_VALID_TAG(tag) ((                                                               \
                               (TAG_BOOLEAN <= (tag) && (tag) <= TAG_ObjectDescriptor) ||  \
                               (tag) == TAG_ENUMERATED ||                                  \
                               (tag) == TAG_UTF8String ||                                  \
                               (tag) == TAG_PrintableString ||                             \
                               (TAG_IA5String <= (tag) && (tag) <= TAG_GeneralizedTime) || \
                               (tag) == TAG_SEQUENCE ||                                    \
                               (tag) == TAG_SET ||                                         \
                               (TAG_x509VERSION <= (tag) && (tag) <= TAG_extensions))      \
                               ? 1                                                         \
                               : 0)

/* Exported Variables */

extern stse_Handle_t *stsafe_x509_parser_companion_handler;

/** @}*/

#endif /* STSE_CERTIFICATE_TYPES_H */
