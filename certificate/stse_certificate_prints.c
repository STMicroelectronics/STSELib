/**
  ******************************************************************************
  * @file    stse_certificate_prints.c
  * @author  CS Application Team
  * @brief   Informational functions for the x509 parsing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/** @brief print the extensionsFlags field of a certificate */
/* This helper mirrors the packing layout defined by certificate subparsing. */
static void printExtensions(PLAT_UI32 extensionsFlags);

void stse_certificate_print_tag(PLAT_I32 tag)
{
  /* Map ASN.1 tag constants to human-readable labels for debug dumps. */

  /* These labels are intended for diagnostics only and are not protocol values. */
  switch (tag)
  {
    /* Primitive scalar fields encoded on one octet. */
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
    case TAG_OBJECT_DESCRIPTOR:
      printf("Object Descriptor");
      break;
    case TAG_ENUMERATED:
      printf("Enumerated");
      break;

    /* Character-oriented string payloads used in RDN attributes. */
    case TAG_UTF8_STRING:
      printf("UTF8");
      break;
    case TAG_SEQUENCE:
      printf("Sequence");
      break;
    case TAG_SET:
      printf("Set");
      break;
    case TAG_PRINTABLE_STRING:
      printf("PrintableString");
      break;
    case TAG_IA5_STRING:
      printf("IA5String");
      break;

    /* Time types used by certificate validity windows. */
    case TAG_UTC_TIME:
      printf("UTCTime");
      break;
    case TAG_GENERALIZED_TIME:
      printf("GeneralizedTime");
      break;

    /* Context-specific wrappers frequently found in TBSCertificate. */
    case TAG_X509_VERSION:
      printf("x509VERSION");
      break;
    case TAG_SUBJECT_UNIQUE_ID:
      printf("subjectUniqueID");
      break;
    case TAG_EXTENSIONS:
      printf("extensions");
      break;
    case TAG_ISSUER_UNIQUE_ID:
      printf("issuerUniqueID");
      break;

      /* Silent default preserves existing behavior for unknown tags. */
  }
}

void stse_certificate_print_signature_algorithm(PLAT_I32 type)
{
  /* Convert parsed signature algorithm enum to textual identifier. */

  /* Signature algorithm names are displayed in canonical "algo-with-hash" form. */
  switch (type)
  {
    /* Legacy/interop algorithm identifier supported by parser tables. */
    case SIG_EDDSA_ED25519:
      printf("eddsa-with-SHA256");
      break;

    /* ECDSA families are distinguished by hash size in the OID. */
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
      /* Keep explicit error text for unsupported/unknown algorithms. */
      printf("Failed Identification of SignatureAlgorithm");
      break;
  }
}

void stse_certificate_print_attribute(PLAT_I32 type)
{
  /* Convert RDN attribute enum to RFC-style attribute name. */

  /* Mapping is intentionally concise to keep issuer/subject dumps readable. */
  switch (type)
  {
    /* Core identity attributes from issuer/subject distinguished names. */
    case ATTR_CN:
      printf("CommonName");
      break;
    case ATTR_C:
      printf("CountryName");
      break;
    case ATTR_SN:
      printf("SerialNumber");
      break;

    /* Organization hierarchy and locality naming components. */
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
      /* Unknown OID mapping in helper parser returns this fallback. */
      printf("Failed Identification of Attribute");
      break;
  }
}

void stse_certificate_print_elliptic_curve(PLAT_I32 type)
{
  /* Convert internal elliptic-curve enum to display name. */

  /* This conversion helps correlate parsed key material with trust-policy settings. */
  switch (type)
  {
    /* NIST prime curves. */
    case EC_P256:
      printf("P-256");
      break;
    case EC_P384:
      printf("P-384");
      break;
    case EC_P521:
      printf("P-521");
      break;

    /* Brainpool curves. */
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

    /* Edwards curve family. */
    case EC_Ed25519:
      printf("ed25519");
      break;

    default:
      printf("Failed Identification of EllipticCurve");
      break;
  }
}

PLAT_I32 stse_certificate_print_bit_string(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_I32 i;

  /* The parser currently supports only BIT STRINGs with zero unused LSB count,
   * which is the common encoding form for certificate fields handled here. */
  /* The first byte of Value identifies the number of discarded bits in the LSB.
  This is a feature we don't support
  */
  if (value[0] != 0)
  {
    printf("BITSTRING excludes some LSBits, we don't handle that, error!\n");
    return (-1);
  }
  /* Print the remaining string */
  for (i = 1; i < size; i++)
  {
    /* Hex output is line-wrapped for readability on long bit strings. */
    printf("%02X", value[i]);
    if (i % 16 == 0 && i != size - 1)
    {
      printf("\n\t\t");
    }
  }
  return (0);
}

PLAT_I32 stse_certificate_print_integer(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_I32 i = 0;

  /* INTEGER fields are rendered as positive hexadecimal values only. */
  /* The first byte tells us if it's positive or negative. We don't support negative */
  if (value[0] >> 7 == 1)
  {
    printf("Integer is negative, we don't handle that, error!\n");
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
    printf("%02X", value[i]);
    if (i % 16 == 0 && (i != size - 1) && i != 0)
    {
      printf("\n\t\t");
    }
  }
  return (0);
}

/* Print a byte buffer */
static void print_buffer(const char *str, const PLAT_UI8 *buf, PLAT_I32 bufsize)
{
  PLAT_I32 i = 0;

  /* Generic byte-buffer printer used for serial numbers, keys, and signatures. */
  /* Prefix string allows callers to preserve indentation and field labels. */
  printf("%s", str);
  for (i = 0; i < bufsize; i++)
  {
    printf("%02X", buf[i]);
  }
}

void stse_certificate_print_parsed_cert(stse_certificate_t *stse_certificate)
{
  /* Pretty-printer for the high-level parsed certificate structure. */

  /* Version in DER is zero-based, display convention is one-based. */
  printf("\n\r\t x509 Version: %" PRIi32, stse_certificate->x509Version + 1);

  /* Serial number is optional in parsed structure when source parsing failed. */
  if (stse_certificate->serialNumber != NULL && stse_certificate->serialNumberSize > 0)
  {
    print_buffer("\n\r\tSerialNumber: ", stse_certificate->serialNumber, stse_certificate->serialNumberSize);
  }

  /* Issuer and subject are full Name TLVs rendered through attribute walker. */
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

  /* Validity is decoded from UTCTime/GeneralizedTime in subparser helper. */
  stse_certificate_print_validity(stse_certificate->validity);

  /* First algorithm belongs to outer certificate signatureAlgorithm field. */
  printf("\n\r\t SignatureAlgorithm: ");
  stse_certificate_print_signature_algorithm(stse_certificate->SignatureAlgorithm);

  /* Second algorithm comes from TBSCertificate signature field. */
  printf("\n\r\t tbsSignature: ");
  stse_certificate_print_signature_algorithm(stse_certificate->signature);

  /* Curve and point encoding summarize SubjectPublicKeyInfo parameters. */
  printf("\n\r\t EllipticCurve: ");
  stse_certificate_print_elliptic_curve(stse_certificate->EllipticCurve);
  printf("\n\r\t Cert PubKey (%s): ",
         *stse_certificate->pPubKey_point_representation_id == 0x04 ? "Uncompressed" : "Compressed");
  /* Public key coordinates are printed from parser-owned pointers into source DER. */
  print_buffer("\n\r\t\t X: ", stse_certificate->PubKey.pX, stse_certificate->PubKey.fsize);
  if (*stse_certificate->pPubKey_point_representation_id == 0x04)
  {
    /* Uncompressed points include both affine coordinates. */
    print_buffer("\n\r\t\t Y: ", stse_certificate->PubKey.pY, stse_certificate->PubKey.fsize);
  }

  /* Signature integer pair follows ASN.1 ECDSA-Sig-Value order r then s. */
  printf("\n\r\t Cert Signature: ");
  print_buffer("\n\r\t\t r: ", stse_certificate->Sign.pR, stse_certificate->Sign.rSize);
  print_buffer("\n\r\t\t s :", stse_certificate->Sign.pS, stse_certificate->Sign.sSize);

  /* Extensions summary is decoded from compact flags built during subparsing. */
  printExtensions(stse_certificate->extensionsFlags);
}

void stse_certificate_simple_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st,
                                             const PLAT_UI8 **next_thing)
{
  /* Lightweight AVA parser used by name printer, assumes SET->SEQUENCE shape. */
  PLAT_I32 total_size, size, parsed, tag;
  const PLAT_UI8 *next = p;
  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SET)
  {
    /* RelativeDistinguishedName set must contain one AttributeTypeAndValue. */
    tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
    if (tag == TAG_SEQUENCE)
    {
      /* Sequence starts with OID identifying the attribute semantic. */
      tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
      if (tag == TAG_OBJECT_IDENTIFIER)
      {
        /* Decode attribute OID first; value decoding depends on recognized type. */
        attribute_st->type = stse_certificate_identify_attribute(next, size);
        if (attribute_st->type != -1)
        {
          next += size;
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
          /* Keep raw pointer/size to avoid copying string payload bytes. */
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
  /* Walk and print all RelativeDistinguishedName entries from Name sequence. */

  /* The function intentionally does not fail hard on unknown attributes. */
  /* This keeps debug output available even with partially recognized names. */
  PLAT_I32 wholesize, tag, parsed;
  const PLAT_UI8 *next;
  stse_cert_attribute_t attribute_st;
  tag = stse_certificate_identify_ASN1_TLV(name, &parsed, &wholesize, &next);
  if (tag == TAG_SEQUENCE)
  {
    /* Here we are inside the name, now loop */
    while (next < name + nameSize)
    {
      /* Parse one SET/AVA and print as "Attribute: Value" line. */
      attribute_st.type = -1;
      stse_certificate_simple_parse_attribute(next, &attribute_st, &next);
      printf("\n\r\t\t ");
      stse_certificate_print_attribute(attribute_st.type);
      printf(": ");

      /* Most subject/issuer values are textual and emitted byte-for-byte. */
      if (attribute_st.type != -1
          && (attribute_st.strFormat == TAG_PRINTABLE_STRING
              || attribute_st.strFormat == TAG_UTF8_STRING))
      {
        /* PrintableString and UTF8String are rendered as plain characters. */
        PLAT_I32 i;
        for (i = 0; i < attribute_st.strSize; i++)
        {
          printf("%c", attribute_st.str[i]);
        }
      }
      else if (attribute_st.type != -1 && attribute_st.strFormat == TAG_BITSTRING)
      {
        /* Non-text attributes can still be surfaced in hexadecimal form. */
        stse_certificate_print_bit_string(attribute_st.str, attribute_st.strSize);
      }
    }
  }
}

void stse_certificate_print_validity(const PLAT_UI8 *validity)
{
  /* Decode and print NotBefore/NotAfter dates in ISO-like format. */

  /* Time normalization has already been performed by parse helper. */
  stse_cert_validity_t notBefore_st, notAfter_st;
  const PLAT_UI8 *next;

  /* Parsing helper advances next pointer for callers that chain TLV parsing. */
  stse_certificate_parse_validity(validity, &notBefore_st, &notAfter_st, &next);
  printf("\n\r\t Validity:");
  printf("\n\r\t\t Not Before: %04" PRIu32 "-%02d-%02d %02d:%02d:%02d", notBefore_st.year, notBefore_st.month,
         notBefore_st.days, notBefore_st.hours, notBefore_st.minutes, notBefore_st.seconds);
  printf("\n\r\t\t Not After:  %04" PRIu32 "-%02d-%02d %02d:%02d:%02d", notAfter_st.year, notAfter_st.month,
         notAfter_st.days, notAfter_st.hours, notAfter_st.minutes, notAfter_st.seconds);
}

static void printExtensions(PLAT_UI32 extensionsFlags)
{
  /* Decode compact extension bitfield generated by subparser extension logic. */

  /* Absent extension bits simply omit the corresponding output lines. */
  /* Output format remains deterministic to simplify log comparison. */
  printf("\n\r\t List of Extensions:");
  if ((extensionsFlags & 1) == 1)
  {
    /* BasicConstraints fields occupy low-order bits in extensionsFlags. */

    /* Presence of BasicConstraints controls whether CA/pathLen are interpreted. */
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
      /* pathLenConstraint is stored on four bits [7:4]. */
      printf("PathSize: %" PRIu32, (extensionsFlags >> 4) & 15);
    }
  }
  if (((extensionsFlags >> 8) & 1) == 1)
  {
    /* KeyUsage presence/critical bits and usage mask occupy upper bit range. */

    /* Multiple key-usage bits can be active and are printed in sequence. */
    printf("\n\r\t\t KeyUsage: ");
    if (((extensionsFlags >> 9) & 1) == 1)
    {
      printf("Critical. ");
    }
    if (((extensionsFlags >> (16 + 0)) & 1) == 1)
    {
      /* digitalSignature */
      printf("digitalSignature ");
    }
    if (((extensionsFlags >> (16 + 1)) & 1) == 1)
    {
      /* contentCommitment / nonRepudiation */
      printf("contentCommitment ");
    }
    if (((extensionsFlags >> (16 + 2)) & 1) == 1)
    {
      /* keyEncipherment */
      printf("keyEncipherment ");
    }
    if (((extensionsFlags >> (16 + 3)) & 1) == 1)
    {
      /* dataEncipherment */
      printf("dataEncipherment ");
    }
    if (((extensionsFlags >> (16 + 4)) & 1) == 1)
    {
      /* keyAgreement */
      printf("keyAgreement ");
    }
    if (((extensionsFlags >> (16 + 5)) & 1) == 1)
    {
      /* keyCertSign */
      printf("keyCertSign ");
    }
    if (((extensionsFlags >> (16 + 6)) & 1) == 1)
    {
      /* cRLSign */
      printf("cRLSign ");
    }
    if (((extensionsFlags >> (16 + 7)) & 1) == 1)
    {
      /* encipherOnly */
      printf("encipherOnly ");
    }
    if (((extensionsFlags >> (15)) & 1) == 1)
    {
      /* decipherOnly (stored separately for 9th KU bit handling) */
      printf("decipherOnly ");
    }
  }

  if (((extensionsFlags >> 24) & 1) == 1)
  {
    /* ExtendedKeyUsage presence/critical bits are stored at [24] and [25]. */

    /* Individual EKU OIDs are not currently decoded in this compact printer. */
    /* Presence still confirms EKU extension was parsed and recognized. */
    printf("\n\r\t\t ExtendedKeyUsage: ");
    if (((extensionsFlags >> 25) & 1) == 1)
    {
      printf("Critical. ");
    }
  }
}
