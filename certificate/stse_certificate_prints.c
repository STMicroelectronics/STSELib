/**
  ******************************************************************************
  * @file    stse_certificate_prints.c
  * @author  STMicroelectronics - CS application team
  * @brief   Informational functions for the x509 parsing
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"

/** @brief print the extensionsFlags field of a certificate */
static void printExtensions(PLAT_UI32 extensionsFlags);

void stse_certificate_print_tag(PLAT_I32 tag)
{
  switch (tag)
  {
	  case TAG_BOOLEAN:
		printf("Boolean");
		break;
	  case TAG_INTEGER:
		printf("Integer");
		break;
	  case TAG_BITSTRING:
		printf("Bistring");
		break;
	  case TAG_OCTETSTRING:
		printf("Octectstring");
		break;
	  case TAG_NULL:
		printf("Null");
		break;
	  case TAG_OBJECT_IDENTIFIER:
		printf("Object Identifier");
		break;
	  case TAG_ObjectDescriptor:
		printf("Object Descriptor");
		break;
	  case TAG_ENUMERATED:
		printf("Enumerated");
		break;
	  case TAG_UTF8String:
		printf("UTF8");
		break;
	  case TAG_SEQUENCE:
		printf("Sequence");
		break;
	  case TAG_SET:
		printf("Set");
		break;
	  case TAG_PrintableString:
		printf("PrintableString");
		break;
	  case TAG_IA5String:
		printf("IA5String");
		break;
	  case TAG_UTCTime:
		printf("UTCTime");
		break;
	  case TAG_GeneralizedTime:
		printf("GeneralizedTime");
		break;
	  case TAG_x509VERSION:
		printf("x509VERSION");
		break;
	  case TAG_subjectUniqueID:
		printf("subjectUniqueID");
		break;
	  case TAG_extensions:
		printf("extensions");
		break;
	  case TAG_issuerUniqueID:
		printf("issuerUniqueID");
		break;
  }
}

void stse_certificate_print_signature_algorithm(PLAT_I32 type)
{
  switch (type)
  {
	  case SIG_ECDSA_SHA1:
		printf("ecdsa-with-SHA1");
		break;
	  case SIG_ECDSA_SHA224:
		printf("ecdsa-with-SHA224");
		break;
	  case SIG_ECDSA_SHA256:
		printf("ecdsa-with-SHA256");
		break;
	  case SIG_ECDSA_SHA384:
		printf("ecdsa-with-SHA384");
		break;
	  case SIG_ECDSA_SHA512:
		printf("ecdsa-with-SHA512");
		break;
	  default:
		printf("Failed Identification of SignatureAlgorithm");
		break;
  }
}

void stse_certificate_print_attribute(PLAT_I32 type)
{
  switch (type)
  {
	  case ATTR_CN:
		printf("CommonName");
		break;
	  case ATTR_C:
		printf("CountryName");
		break;
	  case ATTR_SN:
		printf("SerialNumber");
		break;
	  case ATTR_DN:
		printf("DistinguishedName");
		break;
	  case ATTR_ON:
		printf("OrganizationalName");
		break;
	  case ATTR_OUN:
		printf("OrganizationalUnitName");
		break;
	  case ATTR_SOPN:
		printf("StateOrProvinceName");
		break;
	  case ATTR_LN:
		printf("LocalityName");
		break;
	  case ATTR_UID:
		printf("UniqueIdentifier");
		break;
	  default:
		printf("Failed Identification of Attribute");
		break;
  }
}

void stse_certificate_print_elliptic_curve(PLAT_I32 type)
{
  switch (type)
  {
	  case EC_P256:
		printf("P-256");
		break;
	  case EC_P384:
		printf("P-384");
		break;
	  case EC_P521:
		printf("P-521");
		break;
	  case EC_bp256r1:
		printf("brainpoolP256r1");
		break;
	  case EC_bp256t1:
		printf("brainpoolP256t1");
		break;
	  case EC_bp384r1:
		printf("brainpoolP384r1");
		break;
	  case EC_bp384t1:
		printf("brainpoolP384t1");
		break;
	  case EC_bp512r1:
		printf("brainpoolP512r1");
		break;
	  case EC_bp512t1:
		printf("brainpoolP512t1");
		break;
	  default:
		printf("Failed Identification of EllipticCurve");
		break;
  }
}

PLAT_I32 stse_certificate_print_bit_string(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_I32 i;
  /* The first byte of Value identifies the number of discarded bits in the LSB.
  This is a feature we don't support
  */
  if (value[0] != 0)
  {
    printf("BITSTRING exclues some LSBits, we don't handle that, error!\n");
    return(-1);
  }
  /* Print the remaining string */
  for (i = 1; i < size; i++)
  {
    printf("%02X", value[i]);
    if (i % 16 == 0 && i != size - 1)
      printf("\n\t\t");
  }
  return(0);
}

PLAT_I32 stse_certificate_print_integer(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_I32 i = 0;
  /* The first byte tells us if it's positive or negative. We don't support negative */
  if (value[0] >> 7 == 1)
  {
    printf("Integer is negative, we don't handle that, error!\n");
    return(-1);
  }
  /* First byte might be zero in case Integer is positive and first byte >127 */
  /* So skip it */
  if (value[0] == 0)
  {
    i++;
  }
  /* And print the rest */
  for (; i < size; i++)
  {
    printf("%02X", value[i]);
    if (i % 16 == 0 && (i != size - 1) && i != 0)
      printf("\n\t\t");
  }
  return(0);
}

/* Print a byte buffer */
static void print_buffer(const char * str, const PLAT_UI8 * buf, PLAT_I32 bufsize)
{
  PLAT_I32 i = 0;
  printf("%s", str);
  for (i = 0; i < bufsize; i++)
  {
    printf("%02X", buf[i]);
  }
}

void stse_certificate_print_parsed_cert(stse_certificate_t *stse_certificate)
{
  //printf("Fields: %08X\n", stse_certificate->fields);
  //printFields(stse_certificate->fields);

  printf("\n\r\t x509 Version: %" PRId32, stse_certificate->x509Version + 1);
  if (stse_certificate->serialNumber != NULL && stse_certificate->serialNumberSize > 0)
    print_buffer("\n\r\tSerialNumber: ", stse_certificate->serialNumber, stse_certificate->serialNumberSize);
  if (stse_certificate->issuer != NULL && stse_certificate->issuerSize > 0)
  {
    printf("\n\r\t Issuer:");
    stse_certificate_print_name(stse_certificate->issuer, stse_certificate->issuerSize);
  }
  if (stse_certificate->subject != NULL && stse_certificate->subjectSize > 0)
  {
    printf("\n\r\t Subject:");
    stse_certificate_print_name(stse_certificate->subject, stse_certificate->subjectSize);
  }
  stse_certificate_print_validity(stse_certificate->validity);
  printf("\n\r\t SignatureAlgorithm: "); stse_certificate_print_signature_algorithm(stse_certificate->SignatureAlgorithm);
  printf("\n\r\t tbsSignature: "); stse_certificate_print_signature_algorithm(stse_certificate->signature);
  printf("\n\r\t EllipticCurve: ");  stse_certificate_print_elliptic_curve(stse_certificate->EllipticCurve);
  printf("\n\r\t Cert PubKey (%s): ", *stse_certificate->pPubKey_point_representation_id == 0x04 ? "Uncompressed" : "Compressed");
  print_buffer("\n\r\t\t X: ", stse_certificate->PubKey.pX, stse_certificate->PubKey.fsize);
  if(*stse_certificate->pPubKey_point_representation_id == 0x04)
  {
    print_buffer("\n\r\t\t Y: ", stse_certificate->PubKey.pY, stse_certificate->PubKey.fsize);
  }
  printf("\n\r\t Cert Signature: ");
  print_buffer("\n\r\t\t r: ", stse_certificate->Sign.pR, stse_certificate->Sign.rSize);
  print_buffer("\n\r\t\t s :", stse_certificate->Sign.pS, stse_certificate->Sign.sSize);
  printExtensions(stse_certificate->extensionsFlags);
}

void stse_certificate_simple_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st, const PLAT_UI8 **next_thing)
{
  PLAT_I32 total_size, size, parsed, tag;
  const PLAT_UI8 *next = p;
  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SET)
  {
    tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
    if (tag == TAG_SEQUENCE)
    {
      tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
      if (tag == TAG_OBJECT_IDENTIFIER)
      {
        attribute_st->type = stse_certificate_identify_attribute(next, size);
        if (attribute_st->type != -1)
        {
          next += size;
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
          attribute_st->strFormat = tag;
          attribute_st->str = next;
          attribute_st->strSize = size;
        }
      }
    }
  }
  *next_thing = p + total_size;
}

void stse_certificate_print_name(const PLAT_UI8 *name, PLAT_I32 nameSize)
{
  PLAT_I32 wholesize, tag, parsed;
  const PLAT_UI8 *next;
  stse_cert_attribute_t attribute_st;
  tag = stse_certificate_identify_ASN1_TLV(name, &parsed, &wholesize, &next);
  if (tag == TAG_SEQUENCE)
  {
    /* Here we are inside the name, now loop */
    while (next < name + nameSize)
    {
      attribute_st.type = -1;
      stse_certificate_simple_parse_attribute(next, &attribute_st, &next);
      printf("\n\r\t\t "); stse_certificate_print_attribute(attribute_st.type); printf(": ");
      if (attribute_st.type != -1 &&
        (attribute_st.strFormat == TAG_PrintableString ||
        attribute_st.strFormat == TAG_UTF8String)
        )
      {
        PLAT_I32 i;
        for (i = 0; i < attribute_st.strSize; i++)
        {
          printf("%c", attribute_st.str[i]);
        }
      }
      else if (attribute_st.type != -1 && attribute_st.strFormat == TAG_BITSTRING)
      {
        stse_certificate_print_bit_string(attribute_st.str, attribute_st.strSize);
      }
    }      
  }
}

void stse_certificate_print_validity(const PLAT_UI8 *validity)
{
  stse_cert_validity_t notBefore_st, notAfter_st;
  const PLAT_UI8 *next;
  stse_certificate_parse_validity(validity, &notBefore_st, &notAfter_st, &next);
  printf("\n\r\t Validity:");
  printf("\n\r\t\t Not Before: %04" PRId32 "-%02d-%02d %02d:%02d:%02d", notBefore_st.year, notBefore_st.month, notBefore_st.days, notBefore_st.hours, notBefore_st.minutes, notBefore_st.seconds);
  printf("\n\r\t\t Not After:  %04" PRId32 "-%02d-%02d %02d:%02d:%02d", notAfter_st.year, notAfter_st.month, notAfter_st.days, notAfter_st.hours, notAfter_st.minutes, notAfter_st.seconds);
}

static void printExtensions(PLAT_UI32 extensionsFlags)
{
  printf("\n\r\t List of Extensions:");
  if ((extensionsFlags & 1) == 1)
  {
    printf("\n\r\t\t BasicConstraints: ");
    if (((extensionsFlags >> 1) & 1) == 1)
    {
      printf("Critical. ");
    }
    if (((extensionsFlags >> 2) & 1) == 1)
    {
      printf("CA certificate. ");
    }
    else
    {
      printf("Not a CA certificate. ");
    }
    if (((extensionsFlags >> 3) & 1) == 1)
    {
      printf("PathSize: %" PRId32, (extensionsFlags >> 4) & 15);
    }
  }
  if (((extensionsFlags >> 8) & 1) == 1)
  {
    printf("\n\r\t\t KeyUsage: ");
    if (((extensionsFlags >> 9) & 1) == 1)
    {
      printf("Critical. ");
    }
    if (((extensionsFlags >> (16+0)) & 1) == 1)
    {
      printf("digitalSignature ");
    }
    if (((extensionsFlags >> (16 + 1)) & 1) == 1)
    {
      printf("contentCommitment ");
    }
    if (((extensionsFlags >> (16 + 2)) & 1) == 1)
    {
      printf("keyEncipherment ");
    }
    if (((extensionsFlags >> (16 + 3)) & 1) == 1)
    {
      printf("dataEncipherment ");
    }
    if (((extensionsFlags >> (16 + 4)) & 1) == 1)
    {
      printf("keyAgreement ");
    }
    if (((extensionsFlags >> (16 + 5)) & 1) == 1)
    {
      printf("keyCertSign ");
    }
    if (((extensionsFlags >> (16 + 6)) & 1) == 1)
    {
      printf("cRLSign ");
    }
    if (((extensionsFlags >> (16 + 7)) & 1) == 1)
    {
      printf("encipherOnly ");
    }
    if (((extensionsFlags >> (15)) & 1) == 1)
    {
      printf("decipherOnly ");
    }
  }

  if (((extensionsFlags >> 24) & 1) == 1)
  {
    printf("\n\r\t\t ExtendedKeyUsage: ");
    if (((extensionsFlags >> 25) & 1) == 1)
    {
      printf("Critical. ");
    }
  }

}

