/**
  ******************************************************************************
  * @file    stse_certificate_subparsing.c
  * @author  STMicroelectronics - CS application team
  * @brief   Helper for the x509 certificate Parser
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

#include "certificate/stse_certificate_subparsing.h"
#include <string.h>

PLAT_I32 stse_certificate_identify_ASN1_TLV(const PLAT_UI8 *asn1, PLAT_I32 *parsed, PLAT_I32 *size, const PLAT_UI8 **value)
{
  /* TLV format has the TAG in the first byte*/
  PLAT_I32 tag = (PLAT_I32)asn1[0];
  PLAT_UI32 count, i;

  *parsed = 0;
  /* Check if TAG is valid */
  if (IS_VALID_TAG(tag) == 0)
  {
    //printf("Error! Tag %d is not valid\n", tag);
    return -1;
  }
  /*Second byte tells us about the size if it's < 128. Otherwise if bit7 is set, it tell us how many bytes encode the size */
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
  else /* If bit7 is not set, then it is the size */
  {
    count = 0;
    *size = (PLAT_I32)asn1[1];
  }
  /* We have written the output size */
  /* We effectivly write as output both the pointer to value (which follows the size) and the number of bytes we've scanned */
  *parsed = 2 + (PLAT_I32)count;
  *value = asn1 + *parsed;
  /* And we return the tag */
  return(tag);
}

void stse_certificate_parse_integer(const PLAT_UI8 *integer, const PLAT_UI8 **outp, PLAT_I32 *outSize, const PLAT_UI8 **next_thing)
{
  PLAT_I32 size=0, tag, parsed=0;
  const PLAT_UI8 *next;

  tag = stse_certificate_identify_ASN1_TLV(integer, &parsed, &size, &next);
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
    *outp = &next[i];
    *outSize = size - i;
  }
  *next_thing = integer + parsed + size;
}

void stse_certificate_parse_ECDSA_signature(const PLAT_UI8 *signature, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing)
{    
  PLAT_I32 wholesize=0, size=0, tag, parsed;
  const PLAT_UI8 *next;
  tag = stse_certificate_identify_ASN1_TLV(signature, &parsed, &wholesize, &next);
  /* We expect a BITSTRING with the first byte (indicating the number of bits to exclude form the LSB) to be zero */
  /* If it's not, we return */
  if (tag == TAG_BITSTRING && next[0] == 0x00U)
  {
    tag = stse_certificate_identify_ASN1_TLV((next + 1), &parsed, &size, &next);
    if (tag == TAG_SEQUENCE)
    {
      stse_certificate_parse_integer(next, &stse_certificate->Sign.pR, &stse_certificate->Sign.rSize, &next);
      stse_certificate_parse_integer(next, &stse_certificate->Sign.pS, &stse_certificate->Sign.sSize, &next);
    }
  }  
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
    return(-1);
  }

  for (; i < size; i++)
  {
    retval <<= 8;
    retval += value[i];
  }
  if (retval < UINT32_MAX)
  {
    return((PLAT_I32)retval);
  }
  else
  {
    return(-1);
  }
}

void stse_certificate_parse_signature_algorithm(const PLAT_UI8 *SA, PLAT_I32 *singatureAlgorithm, const PLAT_UI8 **next_thing)
{
#define N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS 5
  const struct SignatureAlgorithmOID_st signatureAlgorithms_oids[N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS] = {
    { .len = 8, .type = SIG_ECDSA_SHA256, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02 } },
    { .len = 8, .type = SIG_ECDSA_SHA384, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x03 } },
    { .len = 8, .type = SIG_ECDSA_SHA512, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x04 } },
    { .len = 7, .type = SIG_ECDSA_SHA1  , .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x01, 0x00 } },
    { .len = 8, .type = SIG_ECDSA_SHA224, .oid = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x01 } }
  };
  PLAT_I32 i, total_size=0, size=0, parsed, tag;
  const PLAT_UI8 *next;

  tag = stse_certificate_identify_ASN1_TLV(SA, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SEQUENCE)
  {
    tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
    if (tag == TAG_OBJECT_IDENTIFIER)
    {
      for (i = 0; i < N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS; i++)
      {
        if (size == signatureAlgorithms_oids[i].len)
        {
          if (memcmp(signatureAlgorithms_oids[i].oid, next, (PLAT_UI32)size) == 0)
          {
            *singatureAlgorithm = signatureAlgorithms_oids[i].type;
            break;
          }
        }
      }
    }
  }  
  *next_thing = SA+total_size;
#undef N_OF_IDENTIFIABLE_SIGNATURE_ALGORITHMS
}

void stse_certificate_parse_X509_version(const PLAT_UI8 *x509VersionField, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing)
{
  PLAT_I32 size=0, tag, parsed;
  const PLAT_UI8 *next;

  tag = stse_certificate_identify_ASN1_TLV(x509VersionField, &parsed, &size, &next);
  if (tag == TAG_INTEGER)
  {
    stse_certificate->x509Version = stse_certificate_get_small_integer(next, size);
  }
  *next_thing = x509VersionField + parsed + size;
  
}

void stse_certificate_parse_ECC_public_key(const PLAT_UI8 *EccPK, stse_certificate_t *stse_certificate, const PLAT_UI8 **next_thing)
{
  PLAT_I32 i, total_size=0, size=0, parsed, tag;
  const PLAT_UI8 *next;
#define N_OF_IDENTIFIABLE_ECS 9
  const struct EllipticCurveOID_st ellipticCurves_oids[N_OF_IDENTIFIABLE_ECS] = {
    { .len = 8, .type = EC_P256   , .oid = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0    } },
    { .len = 5, .type = EC_P384   , .oid = { 0x2B, 0x81, 0x04, 0x00, 34  , 0   , 0   , 0   , 0    } },
    { .len = 5, .type = EC_P521   , .oid = { 0x2B, 0x81, 0x04, 0x00, 35  , 0   , 0   , 0   , 0    } },
    { .len = 9, .type = EC_bp256r1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x07 } },
    { .len = 9, .type = EC_bp256t1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x08 } },
    { .len = 9, .type = EC_bp384r1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 11   } },
    { .len = 9, .type = EC_bp384t1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 12   } },
    { .len = 9, .type = EC_bp512r1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 13   } },
    { .len = 9, .type = EC_bp512t1, .oid = { 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 14   } },
  };

  tag = stse_certificate_identify_ASN1_TLV(EccPK, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SEQUENCE)
  {
    tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
    if (tag == TAG_SEQUENCE)
    {
      tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
      if (tag == TAG_OBJECT_IDENTIFIER)
      {
        if ((memcmp(next, "\x2A\x86\x48\xCE\x3D\x02\x01", 7) == 0) && (size == 7))
        {
          next += 7;
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
          if (tag == TAG_OBJECT_IDENTIFIER)
          {
            for (i = 0; i < N_OF_IDENTIFIABLE_ECS; i++)
            {
              if (size == ellipticCurves_oids[i].len)
              {
                if (memcmp(ellipticCurves_oids[i].oid, next, (PLAT_UI32)size) == 0)
                {
                  stse_certificate->EllipticCurve = ellipticCurves_oids[i].type;
                  break;
                }
              }
            }
            next += size;
            tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
            if (tag == TAG_BITSTRING)
            {
            	if (next[0] == 0x00U)
            	{
            		stse_certificate->pPubKey_point_representation_id = &next[1];
            		if(*stse_certificate->pPubKey_point_representation_id == 0x04U)
					{
						stse_certificate->PubKey.fsize = (size - 2) / 2;
						stse_certificate->PubKey.pX = &next[2];
						stse_certificate->PubKey.pY = &next[2 + stse_certificate->PubKey.fsize];
					}
            		else if(*stse_certificate->pPubKey_point_representation_id == 0x02U
            			 || *stse_certificate->pPubKey_point_representation_id == 0x03U)
					{
            			stse_certificate->PubKey.fsize = (size - 2);
						stse_certificate->PubKey.pX = &next[2];
					}
            	}
            }
          }
        }
      }
    }
  }
  *next_thing = EccPK + total_size;  
#undef N_OF_IDENTIFIABLE_ECS
}

PLAT_I32 stse_certificate_identify_attribute(const PLAT_UI8 *oid, PLAT_I32 size)
{
#define N_OF_IDENTIFIABLE_ATTRIBUTES 9
  const struct AttributeOID_st attributes_oids[N_OF_IDENTIFIABLE_ATTRIBUTES] = {
    { .len = 3, .type = ATTR_CN  , .oid = { 0x55, 0x04, ATTR_CN   } },
    { .len = 3, .type = ATTR_C   , .oid = { 0x55, 0x04, ATTR_C    } },
    { .len = 3, .type = ATTR_SN  , .oid = { 0x55, 0x04, ATTR_SN   } },
    { .len = 3, .type = ATTR_DN  , .oid = { 0x55, 0x04, ATTR_DN   } },
    { .len = 3, .type = ATTR_ON  , .oid = { 0x55, 0x04, ATTR_ON   } },
    { .len = 3, .type = ATTR_OUN , .oid = { 0x55, 0x04, ATTR_OUN  } },
    { .len = 3, .type = ATTR_SOPN, .oid = { 0x55, 0x04, ATTR_SOPN } },
    { .len = 3, .type = ATTR_LN  , .oid = { 0x55, 0x04, ATTR_LN   } },
    { .len = 3, .type = ATTR_UID , .oid = { 0x55, 0x04, ATTR_UID  } },
  };
  PLAT_I32 i;
  for (i = 0; i < N_OF_IDENTIFIABLE_ATTRIBUTES; i++)
  {
    if (size == attributes_oids[i].len)
    {
      if (memcmp(attributes_oids[i].oid, oid, (PLAT_UI32)size) == 0)
      {
        return(attributes_oids[i].type);
      }
    }
  }
  return(-1);
#undef N_OF_IDENTIFIABLE_ATTRIBUTES
}

void stse_certificate_parse_relative_distinguished_name (const PLAT_UI8 *p, const PLAT_UI8 **nextRDN, const PLAT_UI8 **attribute)
{
  PLAT_I32 size=0, parsed;
  (void)stse_certificate_identify_ASN1_TLV(p, &parsed, &size, attribute);
  *nextRDN = *attribute + size;
}

PLAT_I32 stse_certificate_case_identify_extension(const PLAT_UI8 *oid, PLAT_I32 size)
{
#define N_OF_IDENTIFIABLE_EXTENSIONS 3
  const struct ExtensionOID_st extensions_oids[N_OF_IDENTIFIABLE_EXTENSIONS] = {
    { .len = 3, .type = EXTENSION_BC , .oid = { 0x55, 0x1D, 0x13, 0, 0 } },
    { .len = 3, .type = EXTENSION_KU , .oid = { 0x55, 0x1D, 0x0F, 0, 0 } },
    { .len = 3, .type = EXTENSION_EKU, .oid = { 0x55, 0x1D, 0x25, 0, 0 } },
  };
  PLAT_I32 i;
  for (i = 0; i < N_OF_IDENTIFIABLE_EXTENSIONS; i++)
  {
    if (size == extensions_oids[i].len)
    {
      if (memcmp(extensions_oids[i].oid, oid, (PLAT_UI32)size) == 0)
      {
        return(extensions_oids[i].type);
      }
    }
  }
  return(-1);
#undef N_OF_IDENTIFIABLE_EXTENSIONS
}

PLAT_I32 stse_certificate_case_insensitive_compare(const PLAT_UI8 *p1, const PLAT_UI8 *p2, PLAT_I32 size)
{
  PLAT_I32 i; /* For loop*/
  PLAT_UI8 d; /* byte difference */

  /* Try a raw comparison, if it's ok then we are ok */
  if (memcmp(p1, p2, (PLAT_UI32)size) == 0)
  {
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
        return -1;
      }
    }      
  }
  return(0);
}

void stse_certificate_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st, const PLAT_UI8 **next_thing)
{
  PLAT_I32 total_size=0, size=0, parsed, tag;
  const PLAT_UI8 *next = p;
  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &total_size, &next);
  total_size += parsed;
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
  *next_thing = p + total_size;
}

PLAT_I32 stse_certificate_count_attributes(const PLAT_UI8 *p)
{
  PLAT_I32 size=0, parsed, tag,total_size=0, count=0;
  const PLAT_UI8 *next=p;
  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SEQUENCE)
  {
    /* Scans all RDNs */
    while (next < p + total_size)
    {
      const PLAT_UI8 *EoSet = next;
      tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
      EoSet += parsed + size;
      if (tag == TAG_SET)
      {
        while (next < EoSet)
        {
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
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

void stse_certificate_parse_validity(const PLAT_UI8 *p, stse_cert_validity_t *notBefore_st, stse_cert_validity_t *notAfter_st, const PLAT_UI8 **next_thing)
{
  PLAT_I32 i, total_size=0, size=0, parsed, tag;
  const PLAT_UI8 *next = p;
  stse_cert_validity_t *pValidity_st;

  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &total_size, &next);
  total_size += parsed;
  if (tag == TAG_SEQUENCE)
  {
    for (i = 0; i < 2; i++)
    {    
      if (i == 0)
      {
        pValidity_st = notBefore_st;
      }
      else
      {
        pValidity_st = notAfter_st;
      }
      tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
      if (tag == TAG_UTCTime || tag == TAG_GeneralizedTime)
      {
        PLAT_UI32 timevalue;
        if (tag == TAG_UTCTime)
        {
          timevalue = ((PLAT_UI32)next[0] - 0x30U) * 10U + ((PLAT_UI32)next[1] - 0x30U);
          next += 2; 
          size -= 2;
          if (timevalue > 50U)
          {
            pValidity_st->year = timevalue + 1900U;
          }
          else
          {
            pValidity_st->year = timevalue + 2000U;
          }
        }
        else
        {
          pValidity_st->year = ((PLAT_UI32)next[0] - 0x30U) * 1000U + ((PLAT_UI32)next[1] - 0x30U) * 100U +
                               ((PLAT_UI32)next[2] - 0x30U) * 10U + ((PLAT_UI32)next[3] - 0x30U);
          next += 4;
          size -= 4;
        }
        pValidity_st->month = ((PLAT_UI8)next[0] - 0x30U) * 10U + (PLAT_UI8)next[1] - 0x30U;
        pValidity_st->days = ((PLAT_UI8)next[2] - 0x30U) * 10U + (PLAT_UI8)next[3] - 0x30U;
        pValidity_st->hours = ((PLAT_UI8)next[4] - 0x30U) * 10U + (PLAT_UI8)next[5] - 0x30U;
        pValidity_st->minutes = ((PLAT_UI8)next[6] - 0x30U) * 10U + (PLAT_UI8)next[7] - 0x30U;
        pValidity_st->seconds = ((PLAT_UI8)next[8] - 0x30U) * 10U + (PLAT_UI8)next[9] - 0x30U;
        next += size;
      }
    }
  }
  if (next_thing != NULL)
  {
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

void stse_certificate_parse_extension(const PLAT_UI8 *ext, PLAT_UI32 *extFlags, stse_cert_extension_t *ext_st)
{
  PLAT_I32 i, size = 0, parsed, tag, seqSize;
  const PLAT_UI8 *next = ext;

  /* we expect the OID */
  tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
  if (tag == TAG_OBJECT_IDENTIFIER)
  {
    ext_st->type = stse_certificate_case_identify_extension(next, size);
    next += size;

    switch (ext_st->type)
    {
      case EXTENSION_BC:
        ext_st->critical = 0;
        *extFlags |= 1U; /* BC is present */
        tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
        /* Either we get the boolean or the octect string */
        if (tag == TAG_BOOLEAN)
        {
          if (size == 1 && next[0] != 0U)
          {
            ext_st->critical = 1;
            *extFlags |= 1U << 1; /* BC is critical */
          }
          next += size;
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
        }
        if (tag == TAG_OCTETSTRING)
        {
          ext_st->value = next;
          ext_st->valueSize = size;

          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &seqSize, &next);
          /* It should start with a sequence */
          if (tag == TAG_SEQUENCE)
          {
            /* Not CA and No Path */
            if (seqSize > 0) /* There is a CA or PATH */
            {
              tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
              /* Boolean and Optional pathLen (integer) */
              if (tag == TAG_BOOLEAN && next[0] != 0U)
              {
                *extFlags |= 1U << 2; /* BC says it's CA */
              }
              /* Is there more ? */
              if (next < ext_st->value + ext_st->valueSize)
              { /* Yes, then it's the path */
                tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
                if (tag == TAG_INTEGER)
                {
                  *extFlags |= 1U << 3; /* Path is present */
                  *extFlags |= ((PLAT_UI32)stse_certificate_get_small_integer(next, size) & 15U) << 4; /* 4-bit encoding for pathLen */
                }
              }
            }
          }
        }
        break;
      case EXTENSION_KU:
        ext_st->critical = 0;
        *extFlags |= 1U << 8; /* KU is present */
        tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
        /* Either we get the boolean or the octect string */
        if (tag == TAG_BOOLEAN)
        {
          if (size == 1 && next[0] != 0U)
          {
            ext_st->critical = 1;
            *extFlags |= 1U << 9; /* KU is critical */
          }
          next += size;
          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &size, &next);
        }
        if (tag == TAG_OCTETSTRING)
        {
          ext_st->value = next;
          ext_st->valueSize = size;

          tag = stse_certificate_identify_ASN1_TLV(next, &parsed, &seqSize, &next);
          /* It should start with a sequence */
          if (tag == TAG_BITSTRING)
          {
#define MAX(a,b) (((a)>(b))?(a):(b))
            /* This reads the 8 bits (or less) and put it in the right place */
            for (i = 0; i < MAX((PLAT_I32)next[0],8); i++)
            {
              /* Not very readable */
              *extFlags |= (((PLAT_UI32)next[1] >> (PLAT_UI8)(7U - (PLAT_UI8)i)) & 1U) << (PLAT_UI8)(16U + (PLAT_UI8)i);
            }
            if (next[0] == 9U && seqSize == 2)
            {
              *extFlags |= (((PLAT_UI32)next[2] >> 7) & 1U) << (15);
            }
#undef MAX
          }
        }
        break;
      default:
        break;
    }
  }  
}

