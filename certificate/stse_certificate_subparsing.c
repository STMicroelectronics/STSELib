/**
  ******************************************************************************
  * @file    stse_certificate_subparsing.c
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

#include "certificate/stse_certificate_subparsing.h"
#include <string.h>

PLAT_I32 stse_certificate_identify_asn1_tlv(const PLAT_UI8 *asn1, PLAT_I32 *parsed, PLAT_I32 *size,
                                            const PLAT_UI8 **value)
{
  /* TLV format has the TAG in the first byte*/
  PLAT_I32 tag = (PLAT_I32)asn1[0];
  PLAT_UI32 count, i;

  *parsed = 0;
  /* Check if TAG is valid */
  if (IS_VALID_TAG(tag) == 0)
  {
    /* printf("Error! Tag %d is not valid\n", tag); */
    /* Parsing error or invalid tag encountered */
    return -1;
  }
  /* Second byte tells us about the size if it's < 128.
   * Otherwise if bit7 is set, it tell us how many bytes encode the size */
  if (asn1[1] >> 7 == 1U) /* if bit7 is set, then size is expressed over multiple byte*/
  {
    count = (PLAT_UI32)asn1[1] & 127U; /* Read the number of bytes expressing the size */
    /* Read the size */
    *size = 0;
    for (i = 0; i < count; i++)
    {
      *size <<= 8U;
      *size += (PLAT_I32)asn1[2U + i];
    }
  }
  else   /* If bit7 is not set, then it is the size */
  {
    count = 0;
    *size = (PLAT_I32)asn1[1];
  }
  /* We have written the output size */
  /* We effectively write as output both the pointer to value (which follows the size)
   * and the number of bytes we've scanned */
  *parsed = 2 + (PLAT_I32)count;
  *value = asn1 + *parsed;
  /* And we return the tag */
  /* Return recognized ASN.1 tag identifier */
  return (tag);
}

void stse_certificate_parse_integer(const PLAT_UI8 *integer, const PLAT_UI8 **outp, PLAT_I32 *out_size,
                                    const PLAT_UI8 **next_thing)
{
  PLAT_I32 size = 0, tag, parsed = 0;
  const PLAT_UI8 *next;

  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(integer, &parsed, &size, &next);
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_INTEGER)
  {
    PLAT_I32 i = 0;

    /* The first byte tells us if it's positive or negative */
    /* So skip it */
    if (next[0] == 0U)
    {
      i++;
    }

    /* Now we have our integer */
    /* Set output pointer to extracted component */
    *outp = &next[i];
    /* Store length of parsed ASN.1 element */
    *out_size = size - i;
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = integer + parsed + size;
}

void stse_certificate_parse_ecdsa_signature(const PLAT_UI8 *signature, stse_certificate_t *stse_certificate,
                                            const PLAT_UI8 **next_thing)
{
  PLAT_I32 wholesize = 0, size = 0, tag, parsed;
  const PLAT_UI8 *next;
  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(signature, &parsed, &wholesize, &next);
  /* We expect a BITSTRING with the first byte (indicating the number of bits to exclude form the LSB) to be zero */
  /* If it's not, we return */
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_BITSTRING && next[0] == 0x00U)
  {
    next = next + 1; /* Skip "unused bits" octet */

    if (stse_certificate->signature_algorithm == SIG_EDDSA_ED25519)
    {
      stse_certificate->sign.p_r = next;
      stse_certificate->sign.r_size = 32;
      stse_certificate->sign.p_s = next + 32;
      stse_certificate->sign.s_size = 32;
    }
    else
    {
      /* Identify current ASN.1 TLV structure */
      tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
      /* Validate expected ASN.1 tag type */
      if (tag == TAG_SEQUENCE)
      {
        stse_certificate_parse_integer(next, &stse_certificate->sign.p_r, &stse_certificate->sign.r_size, &next);
        stse_certificate_parse_integer(next, &stse_certificate->sign.p_s, &stse_certificate->sign.s_size, &next);
      }
    }
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = signature + wholesize + parsed;
}

PLAT_I32 stse_certificate_get_small_integer(const PLAT_UI8 *value, PLAT_I32 size)
{
  PLAT_UI32 retval = 0;
  PLAT_I32 i = 0;
  /*Check that it is small enough and that it is positive*/
  if (value[0] == 0U)
  {
    i++;
  }
  if ((size - i) > 4 || value[0] >> 7 == 1U)
  {
    return (-1);
  }

  for (; i < size; i++)
  {
    retval <<= 8;
    retval += value[i];
  }
  if (retval < UINT32_MAX)
  {
    return ((PLAT_I32)retval);
  }
  else
  {
    return (-1);
  }
}

void stse_certificate_parse_signature_algorithm(const PLAT_UI8 *SA, PLAT_I32 *singature_algorithm,
                                                const PLAT_UI8 **next_thing)
{
#define N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS 6
  const struct signature_algorithm_oid_st signature_algorithms_oids[N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS] =
  {
    {.len = 8, .type = SIG_ECDSA_SHA256, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02}},
    {.len = 8, .type = SIG_ECDSA_SHA384, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x03}},
    {.len = 8, .type = SIG_ECDSA_SHA512, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x04}},
    {.len = 7, .type = SIG_ECDSA_SHA1, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x01, 0x00}},
    {.len = 8, .type = SIG_ECDSA_SHA224, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x01}},
    {.len = 3, .type = SIG_EDDSA_ED25519, .oid = {0x2B, 0x65, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00}},
  };
  PLAT_I32 i, total_size = 0, size = 0, parsed, tag;
  const PLAT_UI8 *next;

  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(SA, &parsed, &total_size, &next);
  total_size += parsed;
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_SEQUENCE)
  {
    /* Identify current ASN.1 TLV structure */
    tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
    /* Validate expected ASN.1 tag type */
    if (tag == TAG_OBJECT_IDENTIFIER)
    {
      for (i = 0; i < N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS; i++)
      {
        if (size == signature_algorithms_oids[i].len)
        {
          /* Compare Object Identifier bytes */
          if (memcmp(signature_algorithms_oids[i].oid, next, (PLAT_UI32)size) == 0)
          {
            *singature_algorithm = signature_algorithms_oids[i].type;
            break;
          }
        }
      }
    }
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = SA + total_size;
#undef N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS
}

void stse_certificate_parse_x509_version(const PLAT_UI8 *x509_version_field, stse_certificate_t *stse_certificate,
                                         const PLAT_UI8 **next_thing)
{
  PLAT_I32 size = 0, tag, parsed;
  const PLAT_UI8 *next;

  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(x509_version_field, &parsed, &size, &next);
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_INTEGER)
  {
    stse_certificate->x509_version = stse_certificate_get_small_integer(next, size);
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = x509_version_field + parsed + size;
}

void stse_certificate_parse_ecc_public_key(const PLAT_UI8 *ecc_pk, stse_certificate_t *stse_certificate,
                                           const PLAT_UI8 **next_thing)
{
  PLAT_I32 i, total_size = 0, size = 0, parsed, tag;
  const PLAT_UI8 *next;
#define N_OF_IDENTIFIABLE_ECS 10
  const struct elliptic_curve_oid_st elliptic_curves_oids[N_OF_IDENTIFIABLE_ECS] =
  {
    {.len = 8, .type = EC_P256, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0}},
    {.len = 5, .type = EC_P384, .oid = {0x2B, 0x81, 0x04, 0x00, 34, 0, 0, 0, 0}},
    {.len = 5, .type = EC_P521, .oid = {0x2B, 0x81, 0x04, 0x00, 35, 0, 0, 0, 0}},
    {.len = 9, .type = EC_BP_256_R1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x07}},
    {.len = 9, .type = EC_BP_256_T1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x08}},
    {.len = 9, .type = EC_BP_384_R1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 11}},
    {.len = 9, .type = EC_BP_384_T1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 12}},
    {.len = 9, .type = EC_BP_512_R1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 13}},
    {.len = 9, .type = EC_BP_512_T1, .oid = {0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 14}},
    {.len = 3, .type = EC_ED_25519, .oid = {0x2B, 0x65, 0x70, 0, 0, 0, 0, 0, 0}},
  };

  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(ecc_pk, &parsed, &total_size, &next);
  total_size += parsed;
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_SEQUENCE)
  {
    /* Identify current ASN.1 TLV structure */
    tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
    /* Validate expected ASN.1 tag type */
    if (tag == TAG_SEQUENCE)
    {
      /* Identify current ASN.1 TLV structure */
      tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
      /* Validate expected ASN.1 tag type */
      if (tag == TAG_OBJECT_IDENTIFIER)
      {
        const PLAT_UI8 ec_pub_key_oid[7] = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01};
        if ((memcmp(next, ec_pub_key_oid, sizeof(ec_pub_key_oid)) == 0) && (size == sizeof(ec_pub_key_oid)))
        {
          next += 7;
          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        }
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_OBJECT_IDENTIFIER)
        {
          for (i = 0; i < N_OF_IDENTIFIABLE_ECS; i++)
          {
            if (size == elliptic_curves_oids[i].len)
            {
              /* Compare Object Identifier bytes */
              if (memcmp(elliptic_curves_oids[i].oid, next, (PLAT_UI32)size) == 0)
              {
                stse_certificate->elliptic_curve = elliptic_curves_oids[i].type;
                break;
              }
            }
          }
          next += size;
          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
          /* Validate expected ASN.1 tag type */
          if (tag == TAG_BITSTRING)
          {
            if ((next[0] == 0x00U) && (stse_certificate->elliptic_curve != EC_ED_25519))
            {

              switch (next[1])
              {
                case 0x04U:
                  stse_certificate->pub_key.fsize = (size - 2) / 2;
                  stse_certificate->pub_key.p_x = &next[2];
                  stse_certificate->pub_key.p_y = &next[2 + stse_certificate->pub_key.fsize];
                  stse_certificate->p_pub_key_point_representation_id = &next[1];
                  break;
                case 0x02U:
                case 0x03U:
                  stse_certificate->pub_key.fsize = (size - 2);
                  stse_certificate->pub_key.p_x = &next[2];
                  stse_certificate->p_pub_key_point_representation_id = &next[1];
                  break;
                default:
                  stse_certificate->pub_key.fsize = (size - 1);
                  stse_certificate->pub_key.p_x = &next[1];
                  stse_certificate->p_pub_key_point_representation_id = 0;
                  break;
              }
            }
            else
            {
              stse_certificate->pub_key.fsize = (size - 1);
              stse_certificate->pub_key.p_x = &next[1];
              stse_certificate->p_pub_key_point_representation_id = 0;
            }
          }
        }
      }
    }
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = ecc_pk + total_size;
#undef N_OF_IDENTIFIABLE_ECS
}

PLAT_I32 stse_certificate_identify_attribute(const PLAT_UI8 *oid, PLAT_I32 size)
{
#define N_OF_IDENTIFIABLE_ATTRIBUTES 9
  const struct attribute_oid_st attributes_oids[N_OF_IDENTIFIABLE_ATTRIBUTES] =
  {
    {.len = 3, .type = ATTR_CN, .oid = {0x55, 0x04, ATTR_CN}},
    {.len = 3, .type = ATTR_C, .oid = {0x55, 0x04, ATTR_C}},
    {.len = 3, .type = ATTR_SN, .oid = {0x55, 0x04, ATTR_SN}},
    {.len = 3, .type = ATTR_DN, .oid = {0x55, 0x04, ATTR_DN}},
    {.len = 3, .type = ATTR_ON, .oid = {0x55, 0x04, ATTR_ON}},
    {.len = 3, .type = ATTR_OUN, .oid = {0x55, 0x04, ATTR_OUN}},
    {.len = 3, .type = ATTR_SOPN, .oid = {0x55, 0x04, ATTR_SOPN}},
    {.len = 3, .type = ATTR_LN, .oid = {0x55, 0x04, ATTR_LN}},
    {.len = 3, .type = ATTR_UID, .oid = {0x55, 0x04, ATTR_UID}},
  };
  PLAT_I32 i;
  for (i = 0; i < N_OF_IDENTIFIABLE_ATTRIBUTES; i++)
  {
    if (size == attributes_oids[i].len)
    {
      /* Compare Object Identifier bytes */
      if (memcmp(attributes_oids[i].oid, oid, (PLAT_UI32)size) == 0)
      {
        return (attributes_oids[i].type);
      }
    }
  }
  return (-1);
#undef N_OF_IDENTIFIABLE_ATTRIBUTES
}

void stse_certificate_parse_relative_distinguished_name(const PLAT_UI8 *p, const PLAT_UI8 **next_rdn,
                                                        const PLAT_UI8 **attribute)
{
  PLAT_I32 size = 0, parsed;
  (void)stse_certificate_identify_asn1_tlv(p, &parsed, &size, attribute);
  *next_rdn = *attribute + size;
}

PLAT_I32 stse_certificate_case_identify_extension(const PLAT_UI8 *oid, PLAT_I32 size)
{
#define N_OF_IDENTIFIABLE_EXTENSIONS 3
  const struct extension_oid_st extensions_oids[N_OF_IDENTIFIABLE_EXTENSIONS] =
  {
    {.len = 3, .type = EXTENSION_BC, .oid = {0x55, 0x1D, 0x13, 0, 0}},
    {.len = 3, .type = EXTENSION_KU, .oid = {0x55, 0x1D, 0x0F, 0, 0}},
    {.len = 3, .type = EXTENSION_EKU, .oid = {0x55, 0x1D, 0x25, 0, 0}},
  };
  PLAT_I32 i;
  for (i = 0; i < N_OF_IDENTIFIABLE_EXTENSIONS; i++)
  {
    if (size == extensions_oids[i].len)
    {
      /* Compare Object Identifier bytes */
      if (memcmp(extensions_oids[i].oid, oid, (PLAT_UI32)size) == 0)
      {
        return (extensions_oids[i].type);
      }
    }
  }
  return (-1);
#undef N_OF_IDENTIFIABLE_EXTENSIONS
}

PLAT_I32 stse_certificate_case_insensitive_compare(const PLAT_UI8 *p1, const PLAT_UI8 *p2, PLAT_I32 size)
{
  PLAT_I32 i; /* For loop*/
  PLAT_UI8 d; /* byte difference */

  /* Try a raw comparison, if it's ok then we are ok */
  /* Compare Object Identifier bytes */
  if (memcmp(p1, p2, (PLAT_UI32)size) == 0)
  {
    /* Success completion status */
    return 0;
  }
  /* Otherwise try to be case insensitive */
  for (i = 0; i < size; i++)
  {
    d = p1[i] ^ p2[i];
    if (d != 0U)
    {
      if (d == 32U || ((p1[i] >= (PLAT_UI8)'a' && p1[i] <= (PLAT_UI8)'z') ||
                       (p1[i] >= (PLAT_UI8)'A' && p1[i] <= (PLAT_UI8)'Z')))
      {
        continue;
      }
      else
      {
        /* Parsing error or invalid tag encountered */
        return -1;
      }
    }
  }
  return (0);
}

void stse_certificate_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st,
                                      const PLAT_UI8 **next_thing)
{
  PLAT_I32 total_size = 0, size = 0, parsed, tag;
  const PLAT_UI8 *next = p;
  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(next, &parsed, &total_size, &next);
  total_size += parsed;
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_SEQUENCE)
  {
    /* Identify current ASN.1 TLV structure */
    tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
    /* Validate expected ASN.1 tag type */
    if (tag == TAG_OBJECT_IDENTIFIER)
    {
      attribute_st->type = stse_certificate_identify_attribute(next, size);
      if (attribute_st->type != -1)
      {
        next += size;
        /* Identify current ASN.1 TLV structure */
        tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        attribute_st->str_format = tag;
        attribute_st->str = next;
        attribute_st->str_size = size;
      }
    }
  }
  /* Update reference to next element in ASN.1 stream */
  *next_thing = p + total_size;
}

PLAT_I32 stse_certificate_count_attributes(const PLAT_UI8 *p)
{
  PLAT_I32 size = 0, parsed, tag, total_size = 0, count = 0;
  const PLAT_UI8 *next = p;
  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(next, &parsed, &total_size, &next);
  total_size += parsed;
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_SEQUENCE)
  {
    /* Scans all RDNs */
    while (next < p + total_size)
    {
      const PLAT_UI8 *end_of_set = next;
      /* Identify current ASN.1 TLV structure */
      tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
      end_of_set += parsed + size;
      /* Validate expected ASN.1 tag type */
      if (tag == TAG_SET)
      {
        while (next < end_of_set)
        {
          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
          /* Validate expected ASN.1 tag type */
          if (tag == TAG_SEQUENCE)
          {
            next += size;
            count++;
          }
        }
      }
    }
  }
  return (count);
}

void stse_certificate_parse_validity(const PLAT_UI8 *p, stse_cert_validity_t *not_before_st,
                                     stse_cert_validity_t *not_after_st, const PLAT_UI8 **next_thing)
{
  PLAT_I32 i, total_size = 0, size = 0, parsed, tag;
  const PLAT_UI8 *next = p;
  stse_cert_validity_t *p_validity_st;

  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(next, &parsed, &total_size, &next);
  total_size += parsed;
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_SEQUENCE)
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        p_validity_st = not_before_st;
      }
      else
      {
        p_validity_st = not_after_st;
      }
      /* Identify current ASN.1 TLV structure */
      tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
      /* Validate expected ASN.1 tag type */
      if (tag == TAG_UTC_TIME || tag == TAG_GENERALIZED_TIME)
      {
        PLAT_UI32 timevalue;
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_UTC_TIME)
        {
          timevalue = ((PLAT_UI32)next[0] - 0x30U) * 10U + ((PLAT_UI32)next[1] - 0x30U);
          next += 2;
          size -= 2;
          if (timevalue > 50U)
          {
            p_validity_st->year = timevalue + 1900U;
          }
          else
          {
            p_validity_st->year = timevalue + 2000U;
          }
        }
        else
        {
          p_validity_st->year = ((PLAT_UI32)next[0] - 0x30U) * 1000U + ((PLAT_UI32)next[1] - 0x30U) * 100U +
                                ((PLAT_UI32)next[2] - 0x30U) * 10U + ((PLAT_UI32)next[3] - 0x30U);
          next += 4;
          size -= 4;
        }
        p_validity_st->month = ((PLAT_UI8)next[0] - 0x30U) * 10U + (PLAT_UI8)next[1] - 0x30U;
        p_validity_st->days = ((PLAT_UI8)next[2] - 0x30U) * 10U + (PLAT_UI8)next[3] - 0x30U;
        p_validity_st->hours = ((PLAT_UI8)next[4] - 0x30U) * 10U + (PLAT_UI8)next[5] - 0x30U;
        p_validity_st->minutes = ((PLAT_UI8)next[6] - 0x30U) * 10U + (PLAT_UI8)next[7] - 0x30U;
        p_validity_st->seconds = ((PLAT_UI8)next[8] - 0x30U) * 10U + (PLAT_UI8)next[9] - 0x30U;
        next += size;
      }
    }
  }
  if (next_thing != NULL)
  {
    /* Update reference to next element in ASN.1 stream */
    *next_thing = p + total_size;
  }
}

PLAT_I32 stse_certificate_date_compare(const stse_cert_validity_t *D1, const stse_cert_validity_t *D2)
{
  PLAT_I32 test = 0;
  /* First check if year is the same */
  if (D1->year != D2->year)
  {
    /* If it's not then we have our answer as the difference of the two */
    test = (PLAT_I32)D1->year - (PLAT_I32)D2->year;
  }
  /* If years are equal check the months field */
  if ((test == 0) && (D1->month != D2->month))
  {
    test = (PLAT_I32)D1->month - (PLAT_I32)D2->month;
  }
  /* And so on... */
  if ((test == 0) && (D1->days != D2->days))
  {
    test = (PLAT_I32)D1->days - (PLAT_I32)D2->days;
  }
  if ((test == 0) && (D1->hours != D2->hours))
  {
    test = (PLAT_I32)D1->hours - (PLAT_I32)D2->hours;
  }
  if ((test == 0) && (D1->minutes != D2->minutes))
  {
    test = (PLAT_I32)D1->minutes - (PLAT_I32)D2->minutes;
  }
  if ((test == 0) && (D1->minutes != D2->minutes))
  {
    test = (PLAT_I32)D1->minutes - (PLAT_I32)D2->minutes;
  }
  if ((test == 0) && (D1->seconds != D2->seconds))
  {
    test = (PLAT_I32)D1->seconds - (PLAT_I32)D2->seconds;
  }
  /* Now we have three cases */
  /* test < 0 if D1 < D2 */
  /* test = 0 if D1 = D2 */
  /* test > 0 if D1 > D2 */
  if (test < 0)
  {
    return (-1);
  }
  else
  {
    if (test > 0)
    {
      return (1);
    }
  }
  return (0);
}

void stse_certificate_parse_extension(const PLAT_UI8 *ext, PLAT_UI32 *ext_flags, stse_cert_extension_t *ext_st)
{
  PLAT_I32 i, size = 0, parsed, tag, seq_size;
  const PLAT_UI8 *next = ext;

  /* we expect the OID */
  /* Identify current ASN.1 TLV structure */
  tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
  /* Validate expected ASN.1 tag type */
  if (tag == TAG_OBJECT_IDENTIFIER)
  {
    ext_st->type = stse_certificate_case_identify_extension(next, size);
    next += size;

    switch (ext_st->type)
    {
      case EXTENSION_BC:
        ext_st->critical = 0;
        *ext_flags |= 1U; /* BC is present */
        /* Identify current ASN.1 TLV structure */
        tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        /* Either we get the boolean or the octet string */
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_BOOLEAN)
        {
          if (size == 1 && next[0] != 0U)
          {
            ext_st->critical = 1;
            *ext_flags |= 1U << 1; /* BC is critical */
          }
          next += size;
          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        }
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_OCTETSTRING)
        {
          ext_st->value = next;
          ext_st->value_size = size;

          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &seq_size, &next);
          /* It should start with a sequence */
          /* Validate expected ASN.1 tag type */
          if (tag == TAG_SEQUENCE)
          {
            /* Not CA and No Path */
            if (seq_size > 0) /* There is a CA or PATH */
            {
              /* Identify current ASN.1 TLV structure */
              tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
              /* Boolean and Optional path_len (integer) */
              /* Validate expected ASN.1 tag type */
              if (tag == TAG_BOOLEAN && next[0] != 0U)
              {
                *ext_flags |= 1U << 2; /* BC says it's CA */
              }
              /* Is there more ? */
              if (next < ext_st->value + ext_st->value_size)   /* Yes, then it's the path */
              {
                /* Identify current ASN.1 TLV structure */
                tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
                /* Validate expected ASN.1 tag type */
                if (tag == TAG_INTEGER)
                {
                  *ext_flags |= 1U << 3; /* Path is present */
                  /* 4-bit encoding for path_len */
                  *ext_flags |= ((PLAT_UI32)stse_certificate_get_small_integer(next, size) & 15U) << 4;
                }
              }
            }
          }
        }
        break;
      case EXTENSION_KU:
        ext_st->critical = 0;
        *ext_flags |= 1U << 8; /* KU is present */
        /* Identify current ASN.1 TLV structure */
        tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        /* Either we get the boolean or the octet string */
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_BOOLEAN)
        {
          if (size == 1 && next[0] != 0U)
          {
            ext_st->critical = 1;
            *ext_flags |= 1U << 9; /* KU is critical */
          }
          next += size;
          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        }
        /* Validate expected ASN.1 tag type */
        if (tag == TAG_OCTETSTRING)
        {
          ext_st->value = next;
          ext_st->value_size = size;

          /* Identify current ASN.1 TLV structure */
          tag = stse_certificate_identify_asn1_tlv(next, &parsed, &seq_size, &next);
          /* It should start with a sequence */
          /* Validate expected ASN.1 tag type */
          if (tag == TAG_BITSTRING)
          {
            /* This reads the 8 bits (or less) and put it in the right place */
            for (i = 0; i < (((PLAT_I32)next[0] > 8) ? (PLAT_I32)next[0] : 8); i++)
            {
              /* Not very readable */
              *ext_flags |= (((PLAT_UI32)next[1] >> (PLAT_UI8)(7U - (PLAT_UI8)i)) & 1U)
                            << (PLAT_UI8)(16U + (PLAT_UI8)i);
            }
            if (next[0] == 9U && seq_size == 2)
            {
              *ext_flags |= (((PLAT_UI32)next[2] >> 7) & 1U) << (15);
            }
          }
        }
        break;
      default:
        break;
    }
  }
}
