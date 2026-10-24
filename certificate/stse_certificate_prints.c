/**
  ******************************************************************************
  * @file    stse_certificate_prints.c
  * @author  CS Application Team
  * @brief   Informational functions for the x509 parsing
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

#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"
#include "core/stse_platform.h"
#include <inttypes.h>
#include <string.h>

/** @brief print the extensions_flags field of a certificate */
static void print_extensions(PLAT_UI32 extensions_flags);

void stse_certificate_print_tag(PLAT_I32 tag)
{
  switch (tag)
  {
    /* Print tag name for TAG_BOOLEAN */
    case TAG_BOOLEAN:
      /* Output formatted string to console */
      stse_platform_printf("Boolean");
      break;
    /* Print tag name for TAG_INTEGER */
    case TAG_INTEGER:
      /* Output formatted string to console */
      stse_platform_printf("Integer");
      break;
    /* Print tag name for TAG_BITSTRING */
    case TAG_BITSTRING:
      /* Output formatted string to console */
      stse_platform_printf("Bistring");
      break;
    /* Print tag name for TAG_OCTETSTRING */
    case TAG_OCTETSTRING:
      /* Output formatted string to console */
      stse_platform_printf("Octectstring");
      break;
    /* Print tag name for TAG_NULL */
    case TAG_NULL:
      /* Output formatted string to console */
      stse_platform_printf("Null");
      break;
    /* Print tag name for TAG_OBJECT_IDENTIFIER */
    case TAG_OBJECT_IDENTIFIER:
      /* Output formatted string to console */
      stse_platform_printf("Object Identifier");
      break;
    /* Print tag name for TAG_OBJECT_DESCRIPTOR */
    case TAG_OBJECT_DESCRIPTOR:
      /* Output formatted string to console */
      stse_platform_printf("Object Descriptor");
      break;
    /* Print tag name for TAG_ENUMERATED */
    case TAG_ENUMERATED:
      /* Output formatted string to console */
      stse_platform_printf("Enumerated");
      break;
    /* Print tag name for TAG_UTF8_STRING */
    case TAG_UTF8_STRING:
      /* Output formatted string to console */
      stse_platform_printf("UTF8");
      break;
    /* Print tag name for TAG_SEQUENCE */
    case TAG_SEQUENCE:
      /* Output formatted string to console */
      stse_platform_printf("Sequence");
      break;
    /* Print tag name for TAG_SET */
    case TAG_SET:
      /* Output formatted string to console */
      stse_platform_printf("Set");
      break;
    /* Print tag name for TAG_PRINTABLE_STRING */
    case TAG_PRINTABLE_STRING:
      /* Output formatted string to console */
      stse_platform_printf("PrintableString");
      break;
    /* Print tag name for TAG_IA5_STRING */
    case TAG_IA5_STRING:
      /* Output formatted string to console */
      stse_platform_printf("IA5String");
      break;
    /* Print tag name for TAG_UTC_TIME */
    case TAG_UTC_TIME:
      /* Output formatted string to console */
      stse_platform_printf("UTCTime");
      break;
    /* Print tag name for TAG_GENERALIZED_TIME */
    case TAG_GENERALIZED_TIME:
      /* Output formatted string to console */
      stse_platform_printf("GeneralizedTime");
      break;
    /* Print tag name for TAG_X509_VERSION */
    case TAG_X509_VERSION:
      /* Output formatted string to console */
      stse_platform_printf("x509VERSION");
      break;
    /* Print tag name for TAG_SUBJECT_UNIQUE_ID */
    case TAG_SUBJECT_UNIQUE_ID:
      /* Output formatted string to console */
      stse_platform_printf("subjectUniqueID");
      break;
    /* Print tag name for TAG_EXTENSIONS */
    case TAG_EXTENSIONS:
      /* Output formatted string to console */
      stse_platform_printf("extensions");
      break;
    /* Print tag name for TAG_ISSUER_UNIQUE_ID */
    case TAG_ISSUER_UNIQUE_ID:
      /* Output formatted string to console */
      stse_platform_printf("issuerUniqueID");
      break;
  }
}

void stse_certificate_print_signature_algorithm(PLAT_I32 type)
{
  switch (type)
  {
    /* Print tag name for SIG_EDDSA_ED25519 */
    case SIG_EDDSA_ED25519:
      /* Output formatted string to console */
      stse_platform_printf("eddsa-with-SHA256");
      break;
    /* Print tag name for SIG_ECDSA_SHA1 */
    case SIG_ECDSA_SHA1:
      /* Output formatted string to console */
      stse_platform_printf("ecdsa-with-SHA1");
      break;
    /* Print tag name for SIG_ECDSA_SHA224 */
    case SIG_ECDSA_SHA224:
      /* Output formatted string to console */
      stse_platform_printf("ecdsa-with-SHA224");
      break;
    /* Print tag name for SIG_ECDSA_SHA256 */
    case SIG_ECDSA_SHA256:
      /* Output formatted string to console */
      stse_platform_printf("ecdsa-with-SHA256");
      break;
    /* Print tag name for SIG_ECDSA_SHA384 */
    case SIG_ECDSA_SHA384:
      /* Output formatted string to console */
      stse_platform_printf("ecdsa-with-SHA384");
      break;
    /* Print tag name for SIG_ECDSA_SHA512 */
    case SIG_ECDSA_SHA512:
      /* Output formatted string to console */
      stse_platform_printf("ecdsa-with-SHA512");
      break;
    default:
      /* Output formatted string to console */
      stse_platform_printf("Failed Identification of SignatureAlgorithm");
      break;
  }
}

void stse_certificate_print_attribute(PLAT_I32 type)
{
  switch (type)
  {
    /* Print tag name for ATTR_CN */
    case ATTR_CN:
      /* Output formatted string to console */
      stse_platform_printf("CommonName");
      break;
    /* Print tag name for ATTR_C */
    case ATTR_C:
      /* Output formatted string to console */
      stse_platform_printf("CountryName");
      break;
    /* Print tag name for ATTR_SN */
    case ATTR_SN:
      /* Output formatted string to console */
      stse_platform_printf("SerialNumber");
      break;
    /* Print tag name for ATTR_DN */
    case ATTR_DN:
      /* Output formatted string to console */
      stse_platform_printf("DistinguishedName");
      break;
    /* Print tag name for ATTR_ON */
    case ATTR_ON:
      /* Output formatted string to console */
      stse_platform_printf("OrganizationalName");
      break;
    /* Print tag name for ATTR_OUN */
    case ATTR_OUN:
      /* Output formatted string to console */
      stse_platform_printf("OrganizationalUnitName");
      break;
    /* Print tag name for ATTR_SOPN */
    case ATTR_SOPN:
      /* Output formatted string to console */
      stse_platform_printf("StateOrProvinceName");
      break;
    /* Print tag name for ATTR_LN */
    case ATTR_LN:
      /* Output formatted string to console */
      stse_platform_printf("LocalityName");
      break;
    /* Print tag name for ATTR_UID */
    case ATTR_UID:
      /* Output formatted string to console */
      stse_platform_printf("UniqueIdentifier");
      break;
    default:
      /* Output formatted string to console */
      stse_platform_printf("Failed Identification of Attribute");
      break;
  }
}

void stse_certificate_print_elliptic_curve(PLAT_I32 type)
{
  switch (type)
  {
    /* Print tag name for EC_P256 */
    case EC_P256:
      /* Output formatted string to console */
      stse_platform_printf("P-256");
      break;
    /* Print tag name for EC_P384 */
    case EC_P384:
      /* Output formatted string to console */
      stse_platform_printf("P-384");
      break;
    /* Print tag name for EC_P521 */
    case EC_P521:
      /* Output formatted string to console */
      stse_platform_printf("P-521");
      break;
    /* Print tag name for EC_BP_256_R1 */
    case EC_BP_256_R1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP256r1");
      break;
    /* Print tag name for EC_BP_256_T1 */
    case EC_BP_256_T1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP256t1");
      break;
    /* Print tag name for EC_BP_384_R1 */
    case EC_BP_384_R1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP384r1");
      break;
    /* Print tag name for EC_BP_384_T1 */
    case EC_BP_384_T1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP384t1");
      break;
    /* Print tag name for EC_BP_512_R1 */
    case EC_BP_512_R1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP512r1");
      break;
    /* Print tag name for EC_BP_512_T1 */
    case EC_BP_512_T1:
      /* Output formatted string to console */
      stse_platform_printf("brainpoolP512t1");
      break;
    /* Print tag name for EC_ED_25519 */
    case EC_ED_25519:
      /* Output formatted string to console */
      stse_platform_printf("ed25519");
      break;

    default:
      /* Output formatted string to console */
      stse_platform_printf("Failed Identification of EllipticCurve");
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
    /* Output formatted string to console */
    stse_platform_printf("BITSTRING excludes some LSBits, we don't handle that, error!\n");
    return (-1);
  }
  /* Print the remaining string */
  for (i = 1; i < size; i++)
  {
    /* Output formatted string to console */
    stse_platform_printf("%02X", value[i]);
    if (i % 16 == 0 && i != size - 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("\n\t\t");
    }
  }
  return (0);
}

PLAT_I32 stse_certificate_print_integer(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_I32 i = 0;
  /* The first byte tells us if it's positive or negative. We don't support negative */
  if (value[0] >> 7 == 1)
  {
    /* Output formatted string to console */
    stse_platform_printf("Integer is negative, we don't handle that, error!\n");
    return (-1);
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
    /* Output formatted string to console */
    stse_platform_printf("%02X", value[i]);
    if (i % 16 == 0 && (i != size - 1) && i != 0)
    {
      /* Output formatted string to console */
      stse_platform_printf("\n\t\t");
    }
  }
  return (0);
}

/* Print a byte buffer */
static void print_buffer(const char *str, const PLAT_UI8 *buf, PLAT_I32 bufsize)
{
  PLAT_I32 i = 0;
  /* Output formatted string to console */
  stse_platform_printf("%s", str);
  for (i = 0; i < bufsize; i++)
  {
    /* Output formatted string to console */
    stse_platform_printf("%02X", buf[i]);
  }
}

void stse_certificate_print_parsed_cert(stse_certificate_t *stse_certificate)
{
  /* Output formatted string to console */
  /* stse_platform_printf("Fields: %08X\n", stse_certificate->fields); */
  /* stse_platform_printfields(stse_certificate->fields); */

  /* Output formatted string to console */
  stse_platform_printf("\n\r\t x509 Version: %" PRIi32, stse_certificate->x509_version + 1);
  if (stse_certificate->serial_number != NULL && stse_certificate->serial_number_size > 0)
  {
    print_buffer("\n\r\t_serial_number: ", stse_certificate->serial_number, stse_certificate->serial_number_size);
  }
  if (stse_certificate->issuer != NULL && stse_certificate->issuer_size > 0)
  {
    /* Output formatted string to console */
    stse_platform_printf("\n\r\t Issuer:");
    stse_certificate_print_name(stse_certificate->issuer, stse_certificate->issuer_size);
  }
  if (stse_certificate->subject != NULL && stse_certificate->subject_size > 0)
  {
    /* Output formatted string to console */
    stse_platform_printf("\n\r\t Subject:");
    stse_certificate_print_name(stse_certificate->subject, stse_certificate->subject_size);
  }
  stse_certificate_print_validity(stse_certificate->validity);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t SignatureAlgorithm: ");
  stse_certificate_print_signature_algorithm(stse_certificate->signature_algorithm);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t tbs_signature: ");
  stse_certificate_print_signature_algorithm(stse_certificate->signature);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t EllipticCurve: ");
  stse_certificate_print_elliptic_curve(stse_certificate->elliptic_curve);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t Cert PubKey (%s): ",
                       *stse_certificate->p_pub_key_point_representation_id == 0x04 ? "Uncompressed" : "Compressed");
  print_buffer("\n\r\t\t X: ", stse_certificate->pub_key.p_x, stse_certificate->pub_key.fsize);
  if (*stse_certificate->p_pub_key_point_representation_id == 0x04)
  {
    print_buffer("\n\r\t\t Y: ", stse_certificate->pub_key.p_y, stse_certificate->pub_key.fsize);
  }
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t Cert Signature: ");
  print_buffer("\n\r\t\t r: ", stse_certificate->sign.p_r, stse_certificate->sign.r_size);
  print_buffer("\n\r\t\t s :", stse_certificate->sign.p_s, stse_certificate->sign.s_size);
  print_extensions(stse_certificate->extensions_flags);
}

void stse_certificate_simple_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st,
                                             const PLAT_UI8 **next_thing)
{
  PLAT_I32 total_size, size, parsed, tag;
  const PLAT_UI8 *next = p;
  tag = stse_certificate_identify_asn1_tlv(next, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SET)
  {
    tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
    if (tag == TAG_SEQUENCE)
    {
      tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
      if (tag == TAG_OBJECT_IDENTIFIER)
      {
        attribute_st->type = stse_certificate_identify_attribute(next, size);
        if (attribute_st->type != -1)
        {
          next += size;
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
          attribute_st->str_format = tag;
          attribute_st->str = next;
          attribute_st->str_size = size;
        }
      }
    }
  }
  *next_thing = p + total_size;
}

void stse_certificate_print_name(const PLAT_UI8 *name, PLAT_I32 name_size)
{
  PLAT_I32 wholesize, tag, parsed;
  const PLAT_UI8 *next;
  stse_cert_attribute_t attribute_st;
  tag = stse_certificate_identify_asn1_tlv(name, &parsed, &wholesize, &next);
  if (tag == TAG_SEQUENCE)
  {
    /* Here we are inside the name, now loop */
    while (next < name + name_size)
    {
      attribute_st.type = -1;
      stse_certificate_simple_parse_attribute(next, &attribute_st, &next);
      /* Output formatted string to console */
      stse_platform_printf("\n\r\t\t ");
      stse_certificate_print_attribute(attribute_st.type);
      /* Output formatted string to console */
      stse_platform_printf(": ");
      if (attribute_st.type != -1 &&
          (attribute_st.str_format == TAG_PRINTABLE_STRING ||
           attribute_st.str_format == TAG_UTF8_STRING))
      {
        PLAT_I32 i;
        for (i = 0; i < attribute_st.str_size; i++)
        {
          /* Output formatted string to console */
          stse_platform_printf("%c", attribute_st.str[i]);
        }
      }
      else if (attribute_st.type != -1 && attribute_st.str_format == TAG_BITSTRING)
      {
        stse_certificate_print_bit_string(attribute_st.str, attribute_st.str_size);
      }
    }
  }
}

void stse_certificate_print_validity(const PLAT_UI8 *validity)
{
  stse_cert_validity_t not_before_st, not_after_st;
  const PLAT_UI8 *next;
  stse_certificate_parse_validity(validity, &not_before_st, &not_after_st, &next);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t Validity:");
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t\t Not Before: %04" PRIu32 "-%02d-%02d %02d:%02d:%02d",
                       not_before_st.year, not_before_st.month, not_before_st.days,
                       not_before_st.hours, not_before_st.minutes, not_before_st.seconds);
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t\t Not After:  %04" PRIu32 "-%02d-%02d %02d:%02d:%02d",
                       not_after_st.year, not_after_st.month, not_after_st.days,
                       not_after_st.hours, not_after_st.minutes, not_after_st.seconds);
}

static void print_extensions(PLAT_UI32 extensions_flags)
{
  /* Output formatted string to console */
  stse_platform_printf("\n\r\t List of Extensions:");
  if ((extensions_flags & 1) == 1)
  {
    /* Output formatted string to console */
    stse_platform_printf("\n\r\t\t BasicConstraints: ");
    if (((extensions_flags >> 1) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("Critical. ");
    }
    if (((extensions_flags >> 2) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("CA certificate. ");
    }
    else
    {
      /* Output formatted string to console */
      stse_platform_printf("Not a CA certificate. ");
    }
    if (((extensions_flags >> 3) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("PathSize: %" PRIu32, (extensions_flags >> 4) & 15);
    }
  }
  if (((extensions_flags >> 8) & 1) == 1)
  {
    /* Output formatted string to console */
    stse_platform_printf("\n\r\t\t KeyUsage: ");
    if (((extensions_flags >> 9) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("Critical. ");
    }
    if (((extensions_flags >> (16 + 0)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("digital_signature ");
    }
    if (((extensions_flags >> (16 + 1)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("content_commitment ");
    }
    if (((extensions_flags >> (16 + 2)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("key_encipherment ");
    }
    if (((extensions_flags >> (16 + 3)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("data_encipherment ");
    }
    if (((extensions_flags >> (16 + 4)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("key_agreement ");
    }
    if (((extensions_flags >> (16 + 5)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("key_cert_sign ");
    }
    if (((extensions_flags >> (16 + 6)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("c_rl_sign ");
    }
    if (((extensions_flags >> (16 + 7)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("encipher_only ");
    }
    if (((extensions_flags >> (15)) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("decipher_only ");
    }
  }

  if (((extensions_flags >> 24) & 1) == 1)
  {
    /* Output formatted string to console */
    stse_platform_printf("\n\r\t\t ExtendedKeyUsage: ");
    if (((extensions_flags >> 25) & 1) == 1)
    {
      /* Output formatted string to console */
      stse_platform_printf("Critical. ");
    }
  }
}
