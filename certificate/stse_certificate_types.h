/**
  ******************************************************************************
  * \file    x509.h
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

#ifndef STSE_CERTIFICATE_TYPES_H
#define STSE_CERTIFICATE_TYPES_H

#include <stdint.h>
#include "core/stse_device.h"
#include "core/stse_return_codes.h"

/** \defgroup 	stse_certificate_core STSE Certificate parser
 *  \ingroup 	stse_certificate
 *  \brief		STSE Certificate parser
 *  @{
 */

/** \brief Definitions of X509 TAGs */
typedef enum {
    TAG_BOOLEAN				= 0x01,	/*!< x509 tag for BOOLEAN */
    TAG_INTEGER,					/*!< x509 tag for INTEGER */
    TAG_BITSTRING,					/*!< x509 tag for BITSTRING */
    TAG_OCTETSTRING,				/*!< x509 tag for OCTETSTRING */
    TAG_NULL,						/*!< x509 tag for NULL */
    TAG_OBJECT_IDENTIFIER,			/*!< x509 tag for OBJECT_IDENTIFIER */
    TAG_ObjectDescriptor,			/*!< x509 tag for ObjectDescriptor */
    TAG_ENUMERATED          = 0x0A,	/*!< x509 tag for ENUMERATED */
    TAG_UTF8String          = 0x0C,	/*!< x509 tag for UTF8String */
    TAG_PrintableString     = 0x13,	/*!< x509 tag for PrintableString */
    TAG_IA5String           = 0x16,	/*!< x509 tag for IA5String */
    TAG_UTCTime,					/*!< x509 tag for UTCTime */
    TAG_GeneralizedTime,			/*!< x509 tag for GeneralizedTime */
    TAG_SEQUENCE            = 0x30,	/*!< x509 tag for SEQUENCE */
    TAG_SET,						/*!< x509 tag for SET */
    TAG_x509VERSION         = 0xA0,	/*!< x509 tag for x509VERSION */
    TAG_issuerUniqueID,				/*!< x509 tag for issuerUniqueID */
    TAG_subjectUniqueID,			/*!< x509 tag for subjectUniqueID */
    TAG_extensions,					/*!< x509 tag for extensions */
}stse_certificate_tag_t;

/** \brief Definitions of available signature algorithms */
typedef enum {
	SIG_ECDSA_SHA1 = 0,	/*!< ECDSA with SHA1 algorithm */
	SIG_ECDSA_SHA224,	/*!< ECDSA with SHA224 algorithm */
	SIG_ECDSA_SHA256,	/*!< ECDSA with SHA256 algorithm */
	SIG_ECDSA_SHA384,	/*!< ECDSA with SHA384 algorithm */
	SIG_ECDSA_SHA512,	/*!< ECDSA with SHA512 algorithm */
}stse_certificate_signature_algorithms_t;

/** \brief Definitions of supported Name Attributes */
typedef enum {
	ATTR_CN 	= 3,	/*!< CommonName */
	ATTR_SN 	= 5, 	/*!< SerialNumber */
	ATTR_C 		= 6,  	/*!< Country */
	ATTR_LN 	= 7, 	/*!< LocalityName */
	ATTR_SOPN   = 8, 	/*!< stateOrProvinceName */
	ATTR_ON 	= 10,	/*!< Organization Name */
	ATTR_OUN 	= 11, 	/*!< Organization Unit Name */
	ATTR_UID 	= 45, 	/*!< UniqueIdentifier */
	ATTR_DN 	= 49, 	/*!< DistinguishedName */
}stse_certificate_name_attributes_t;

/** \brief Definitions of supported Elliptic Curve Names */
typedef enum {
	EC_P256 = 0,		/*!< NIST P-256 */
	EC_P384, 			/*!< NIST P-384 */
	EC_P521, 			/*!< NIST P-521 */
	EC_bp256r1,  		/*!< brainpoolP256r1 */
	EC_bp256t1,  		/*!< brainpoolP256t1 */
	EC_bp384r1,  		/*!< brainpoolP384r1 */
	EC_bp384t1,  		/*!< brainpoolP384t1 */
	EC_bp512r1,  		/*!< brainpoolP512r1 */
	EC_bp512t1,  		/*!< brainpoolP512t1 */
}stse_certificate_elliptic_curves_t;

/** \brief Definitions of supported Extensions types */
typedef enum {
	EXTENSION_KU  =	15, /*!< KeyUsage */
	EXTENSION_BC  =	19, /*!< BasicContrains */
	EXTENSION_EKU = 37, /*!< extKeyUsage */
}stse_certificate_extensions_type_t;

/** \brief Structure used to keep SignatureAlgorithm OIDs */
struct SignatureAlgorithmOID_st
{
  PLAT_I32 len;         /*!< size of the encoded OID */
  PLAT_I32 type;        /*!< type of OID */
  PLAT_UI8 oid[8];      /*!< encoded OID */
};

/** \brief Structure used to keep Name Attribute OIDs */
struct AttributeOID_st
{
  PLAT_I32 len;         /*!< size of the encoded OID */
  PLAT_I32 type;        /*!< type of OID */
  PLAT_UI8 oid[3];      /*!< encoded OID */
};

/** \brief Structure used to keep named Elliptic Curve OIDs */
struct EllipticCurveOID_st
{
  PLAT_I32 len;         /*!< size of the encoded OID */
  PLAT_I32 type;        /*!< type of OID */
  PLAT_UI8 oid[9];      /*!< encoded OID */
};

/** \brief typedef for the attribute */
typedef struct attribute_st
{
  PLAT_I32 type;        /*!< type of attribute (corresponding to OID) */
  PLAT_I32 strFormat;   /*!< format of the string */
  const PLAT_UI8 *str;  /*!< pointer to the string of the attribute */
  PLAT_I32 strSize;     /*!< size of the string */
} stse_cert_attribute_t;

/** \brief Structure used to keep Name Attribute OIDs */
struct ExtensionOID_st
{
  PLAT_I32 len;    		/*!< size of the encoded OID */
  PLAT_I32 type;   		/*!< type of OID */
  PLAT_UI8 oid[5]; 		/*!< encoded OID */
};

/** \brief typedef for the extension */
typedef struct extension_st
{
  PLAT_I32 type;       	/*!< type of extension (corresponding to OID) */
  PLAT_I32 critical;   	/*!< critical */
  const PLAT_UI8 *value;/*!< pointer to the value hold by the extension (this is an octet string)  */
  PLAT_I32 valueSize;   /*!< size of the string */
} stse_cert_extension_t;

/** \brief typedef for the structure keeping the validity */
typedef struct validity_st {
   PLAT_UI8 seconds;    /*!< seconds (range 0 to 59) */
   PLAT_UI8 minutes;    /*!< minutes (range 0 to 59) */
   PLAT_UI8 hours;      /*!< hours   (range 0 to 23) */
   PLAT_UI8 days;       /*!< days    (range 1 to 31) */
   PLAT_UI8 month;      /*!< month   (range 1 to 12) */
   PLAT_UI32 year;      /*!< years 0 to whatever     */
} stse_cert_validity_t;

/** \brief typedef for the structure keeping the Ellipitc Curve Public Key */
typedef struct stse_cert_public_key_t
{
  const PLAT_UI8 *pX; 	/*!<  Pointer to X Coordinate */
  const PLAT_UI8 *pY; 	/*!<  Pointer to Y Coordiante */
  PLAT_I32 fsize; 		/*!< Size of the field in bytes */
} stse_cert_public_key_t;

/** \brief typedef for the structure keeping the ECDSA Signature */
typedef struct ECDSAsign_st
{
  const PLAT_UI8 *pR; 	/*!< Pointer to r */
  PLAT_I32  rSize;   	/*!< size to r */
  const PLAT_UI8 *pS; 	/*!<  Pointer to s*/
  PLAT_I32  sSize;    	/*!< size to s */
} stse_cert_signature_t; 

/** \brief internal structure used to keep the values parsed from the x509 */
struct stse_internal_certificate {
  const PLAT_UI8 *pPubKey_point_representation_id; /*!< 0x04: Uncompressed key, 0x02 or 0x03: Compressed key */
  const PLAT_UI8 *tbs;              /*!<  Pointer to tcs field */
  PLAT_I32 tbsSize;            		/*!<  Size of tcs */
  PLAT_I32 x509Version;        		/*!<  x509 Version */
  const PLAT_UI8 *serialNumber;     /*!<  Pointer to SerialNumber*/
  PLAT_I32 serialNumberSize;   		/*!<  Size of SerialNumber */
  PLAT_I32 signature;          		/*!<  Signature (algorithm) but from tcs */
  const PLAT_UI8 *issuer;           /*!<  Pointer to Issuer */
  PLAT_I32 issuerSize;         		/*!<  Size of Issuer */
  const PLAT_UI8 *validity;         /*!<  Pointer to Validity */
  PLAT_I32 validitySize;       		/*!<  Size of Validity */
  const PLAT_UI8 *subject;          /*!<  Pointer to subject */
  PLAT_I32 subjectSize;        		/*!<  Size of Subject */
  PLAT_I32 EllipticCurve;      		/*!<  Identifies the Elliptic Curve */
  stse_cert_public_key_t PubKey;    /*!<  Contain the struct \ref stse_cert_public_key_t */
  const PLAT_UI8 *extensions;  		/*!<  Pointer to Extensions */
  PLAT_I32 extensionsSize;			/*!<  Size of Extensions */
  PLAT_UI32 extensionsFlags;   		/*!<  Integer encoding the fields present in the tcs:			\n
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
  PLAT_I32 SignatureAlgorithm; 		/*!<  SignatureAlgorithm */
  stse_cert_signature_t Sign;       /*!<  Contain the struct \ref stse_cert_signature_t */
};

/** \brief typedef for the stse_certificate_t */
typedef struct stse_internal_certificate stse_certificate_t;

/* Exported Macros */

/** \brief Macro which checks if a TAG is valid (known) */
#define IS_VALID_TAG(tag) (( \
		(TAG_BOOLEAN 	<= (tag) && (tag) <= TAG_ObjectDescriptor) 	|| \
		(tag) == TAG_ENUMERATED  									|| \
		(tag) == TAG_UTF8String 									|| \
		(tag) == TAG_PrintableString 								|| \
		(TAG_IA5String 	<= (tag) && (tag) <= TAG_GeneralizedTime)	|| \
		(tag) == TAG_SEQUENCE 										|| \
		(tag) == TAG_SET 											|| \
		(TAG_x509VERSION <= (tag) && (tag) <= TAG_extensions) 	) ? 1 : 0)

/* Exported Variables */

extern stse_Handler_t * stsafe_x509_parser_companion_handler;

/** @}*/

#endif /* STSE_CERTIFICATE_TYPES_H */
