/**
  ******************************************************************************
  * @file    stse_certificate.c
  * @author  CS Application Team
  * @brief   STSAFE-A x509 certificate Parser
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

#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_crypto.h"
#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"
#include <stdint.h>
#include <string.h>

stse_handle_t *stsafe_x509_parser_companion_handler = NULL;

/**
  * \brief  Parse the extensions part of a certificate
  * \param[in]  *extensions pointer to the extensions part of a certificate
  * \param[in]  extensions_size Size of the extensions field
  * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
  */
static void stse_certificate_parse_extensions(const PLAT_UI8 *extensions, PLAT_I32 extensions_size,
                                              stse_certificate_t *stse_certificate)
{
  const PLAT_UI8 *next_value;
  PLAT_I32 size, tag, parsed;
  stse_cert_extension_t ext_st = {.type = -1, .value = NULL, .value_size = 0};

  if (extensions != NULL && stse_certificate != NULL)
  {
    /* Start parsing the extensions, extensions is a sequence */
    tag = stse_certificate_identify_asn1_tlv(extensions, &parsed, &size, &next_value);
    if (tag == TAG_SEQUENCE)
    {
      /* Now we loop over sequences */
      while (next_value < extensions + extensions_size)
      {
        tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
        if (tag == TAG_SEQUENCE)
        {
          stse_certificate_parse_extension(next_value, &stse_certificate->extensions_flags, &ext_st);
        }
        next_value += size;
      }
    }
  }
}

/**
  * \brief  Parse the tbs part of a certificate
  * \param[in]  *tbs pointer to the TBSCertificate field of a certificate
  * \param[in]  *tbs_size size of the TBSCertificate
  * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
  */
static void parsetbs_certificate(const PLAT_UI8 *tbs, PLAT_I32 tbs_size, stse_certificate_t *stse_certificate)
{
  const PLAT_UI8 *next_value;
  PLAT_I32 size, tag, parsed;

  if (tbs != NULL && stse_certificate != NULL)
  {
    /* Start parsing the TBS, tbs is a sequence */
    tag = stse_certificate_identify_asn1_tlv(tbs, &parsed, &size, &next_value);
    if (tag == TAG_SEQUENCE)
    {
      /* Now we expect the version */
      tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
      if (tag == TAG_X509_VERSION)
      {
        stse_certificate_parse_x509_version(next_value, stse_certificate, &next_value);
      }
      else   /* The Version is optional, if it's not present then 0 (v1) is assumed */
      {
        stse_certificate->x509_version = 0;
        next_value -= parsed;
      }
      /* Move on to Serial Number */
      stse_certificate_parse_integer(next_value,
                                     &stse_certificate->serial_number,
                                     &stse_certificate->serial_number_size,
                                     &next_value);
      /* Parse tbs_signature(Algorithm)*/
      stse_certificate_parse_signature_algorithm(next_value, &stse_certificate->signature, &next_value);
      /* Now we face the issuer */
      tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
      if (tag == TAG_SEQUENCE)
      {
        stse_certificate->issuer = next_value - parsed;
        stse_certificate->issuer_size = size + parsed;
      }
      next_value += size; /* Issuer is not parsed */
      /* Now we face validity */
      tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
      if (tag == TAG_SEQUENCE)
      {
        stse_certificate->validity = next_value - parsed;
        stse_certificate->validity_size = size + parsed;
      }
      next_value += size; /* Validity is not parsed */
      /* Now we face Subject */
      tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
      if (tag == TAG_SEQUENCE)
      {
        stse_certificate->subject = next_value - parsed;
        stse_certificate->subject_size = size + parsed;
      }
      next_value += size; /* Subject is not parsed */
      /* Now we face SubjectPublicKey */
      stse_certificate_parse_ecc_public_key(next_value, stse_certificate, &next_value);
      /* Now we face Optional Extensions, but those are optional */
      if (next_value < tbs + tbs_size)
      {
        tag = stse_certificate_identify_asn1_tlv(next_value, &parsed, &size, &next_value);
        if (tag == TAG_EXTENSIONS)
        {
          stse_certificate->extensions = next_value;
          stse_certificate->extensions_size = size;
          stse_certificate_parse_extensions(stse_certificate->extensions,
                                            stse_certificate->extensions_size,
                                            stse_certificate);
        }
      }
    }
  }
}

void stse_certificate_init(stse_certificate_t *cert)
{
  if (cert != NULL)
  {
    cert->tbs = NULL;
    cert->tbs_size = -1;
    cert->x509_version = -1;
    cert->serial_number = NULL;
    cert->serial_number_size = -1;
    cert->signature = -1;
    cert->issuer = NULL;
    cert->issuer_size = -1;
    cert->validity = NULL;
    cert->validity_size = -1;
    cert->subject = NULL;
    cert->subject_size = -1;
    cert->elliptic_curve = -1;
    cert->pub_key.fsize = 0;
    cert->pub_key.p_x = NULL;
    cert->pub_key.p_y = NULL;
    cert->extensions = NULL;
    cert->extensions_size = -1;
    cert->signature_algorithm = -1;
    cert->sign.p_r = NULL;
    cert->sign.p_s = NULL;
    cert->sign.r_size = 0;
    cert->sign.s_size = 0;
    cert->extensions_flags = 0;
  }
}

void stse_certificate_copy(stse_certificate_t *copied_cert, const stse_certificate_t *original_cert)
{
  if (copied_cert != NULL && original_cert != NULL)
  {
    *copied_cert = *original_cert;
  }
}

stse_return_code_t stse_certificate_parse(const PLAT_UI8 *cert, stse_certificate_t *stse_certificate,
                                          const PLAT_UI8 **next)
{
  PLAT_I32 total_size, tag, size, parsed;
  const PLAT_UI8 *next_thing;

  stse_certificate_init(stse_certificate);
  /* First we start by looking at the first item, this will be used to set the size of the whole certificate */
  /* Then the parsing of the other subfields will start */
  tag = stse_certificate_identify_asn1_tlv(cert, &parsed, &total_size, &next_thing);
  if (tag != TAG_SEQUENCE)
  {
    return (STSE_CERT_UNEXPECTED_SEQUENCE);
  }

  /* Get tbs */
  tag = stse_certificate_identify_asn1_tlv(next_thing, &parsed, &size, &next_thing);
  if (tag != TAG_SEQUENCE)
  {
    return (STSE_CERT_UNEXPECTED_SEQUENCE);
  }
  stse_certificate->tbs = next_thing - parsed;
  stse_certificate->tbs_size = size + parsed;

  /* Get signature_algorithm */
  next_thing += size;
  stse_certificate_parse_signature_algorithm(next_thing, &stse_certificate->signature_algorithm, &next_thing);

  /* Now we should have the Signature. If it's ECDSA parse it! */
  if (stse_certificate->signature_algorithm != -1)
  {
    stse_certificate_parse_ecdsa_signature(next_thing, stse_certificate, &next_thing);
  }

  /* We have completed the parsing of the TOP level */
  /* Let's move on to parse the tbs */
  parsetbs_certificate(stse_certificate->tbs, stse_certificate->tbs_size, stse_certificate);

  if (next != NULL)
  {
    *next = next_thing;
  }

  return (STSE_OK);
}

stse_return_code_t stse_certificate_is_valid(const stse_certificate_t *stse_certificate,
                                             const stse_cert_validity_t *current_time)
{
  if (
    (stse_certificate->issuer == NULL || stse_certificate->issuer_size == -1) ||
    (stse_certificate->subject == NULL || stse_certificate->subject_size == -1) ||
    (stse_certificate->serial_number == NULL || stse_certificate->serial_number_size == -1) ||
    (stse_certificate->signature == -1 || stse_certificate->signature_algorithm != stse_certificate->signature) ||
    (stse_certificate->validity == NULL || stse_certificate->validity_size == -1) ||
    (stse_certificate->elliptic_curve == -1 || stse_certificate->x509_version == -1) ||
    (stse_certificate->pub_key.fsize <= 0 || stse_certificate->pub_key.p_x == NULL) ||
    (*stse_certificate->p_pub_key_point_representation_id == 0x04 && stse_certificate->pub_key.p_y == NULL) ||
    (*stse_certificate->p_pub_key_point_representation_id != 0x04 && stse_certificate->pub_key.p_y != NULL) ||
    (stse_certificate->sign.p_r == NULL || stse_certificate->sign.r_size == -1) ||
    (stse_certificate->sign.p_s == NULL || stse_certificate->sign.s_size == -1))
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  if (current_time != NULL)
  {
    stse_cert_validity_t not_before_st, not_after_st;
    stse_certificate_parse_validity(stse_certificate->validity, &not_before_st, &not_after_st, NULL);

    if (stse_certificate_date_compare(current_time, &not_before_st) < 0)
    {
      return (STSE_CERT_INVALID_CERTIFICATE);
    }

    if (stse_certificate_date_compare(current_time, &not_after_st) > 0)
    {
      return (STSE_CERT_INVALID_CERTIFICATE);
    }
  }

  return (STSE_OK);
}

stse_return_code_t stse_certificate_is_ca(const stse_certificate_t *cert)
{
  if (((cert->extensions_flags >> 0) & 1U) == 1U)
  {
    /* BasicContrain is present */
    if (((cert->extensions_flags >> 2) & 1U) == 1U)
    {
      /* This is a CA */
      return (STSE_OK);
    }
  }
  return (STSE_CERT_INVALID_CERTIFICATE);
}

stse_return_code_t stse_certificate_is_parent(const stse_certificate_t *parent, const stse_certificate_t *child,
                                              const stse_cert_validity_t *current_time)
{
  /* Get the two name structure to be compared */
  const PLAT_UI8 *name1 = child->issuer;
  PLAT_I32 name1_size = child->issuer_size;
  const PLAT_UI8 *name2 = parent->subject;
  PLAT_I32 name2_size = parent->subject_size;
  PLAT_I32 found, parsed;
  const PLAT_UI8 *attr2_loop;
  const PLAT_UI8 *attr1, *attr2;
  stse_cert_attribute_t attr1_st, attr2_st;

  /* Do not compare two invalid certificates */
  if (stse_certificate_is_valid(parent, current_time) != STSE_OK)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  if (stse_certificate_is_valid(child, current_time) != STSE_OK)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  if (stse_certificate_is_ca(parent) != STSE_OK)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  /* Check that they have the same number of attributes */
  if (stse_certificate_count_attributes(name1) != stse_certificate_count_attributes(name2))
  {
    return (STSE_CERT_CA_NOT_MATCHING);
  }

  /* Now we need to scan name1 and look for corresponding attribute in name2 */
  /* Start entering in the root sequence */
  if (stse_certificate_identify_asn1_tlv(name1, &parsed, &found, &name1) != TAG_SEQUENCE)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  if (stse_certificate_identify_asn1_tlv(name2, &parsed, &found, &name2) != TAG_SEQUENCE)
  {
    return (STSE_CERT_INVALID_CERTIFICATE);
  }

  /* Loop on all RDN */
  while ((name1 < child->issuer + name1_size) && (name2 < parent->subject + name2_size))
  {
    /* Parse RDN1 and RDN2. The SEQUENCE is order so this should match */
    stse_certificate_parse_relative_distinguished_name(name1, &name1, &attr1);
    stse_certificate_parse_relative_distinguished_name(name2, &name2, &attr2);
    /* Now loop on the attributes inside the RDN, which are not ordered */
    while (attr1 < name1) /*Loop until the next RDN1 */
    {
      /* A single DN is a SET which might (but usually doesn't) contain more Attributes */
      /* Since the SET is not ordered we need to select the attribute from the name1 and scan
      all the SET of name2 for a matching attribute */

      stse_certificate_parse_attribute(attr1, &attr1_st, &attr1); /*Read the attribute of Name1 */

      attr2_loop = attr2; /* copy attr2 as attr2loop will loop for each attribute of RDN1 */
      found = 0;          /* Flag to tell us if we found or not the attribute in Name2 */
      do                  /* Scans the whole DN SET in Name2 */
      {
        /* Read Attribute */
        stse_certificate_parse_attribute(attr2_loop, &attr2_st, &attr2_loop);
        /* Check if it match with the attribute of name 1 */
        if (attr1_st.type == attr2_st.type)
        {
          found = 1; /* Yes, we found the matching attribute */

          /* Now we need to compare */
          /* In this if we need to either break the do/while or return -1 */
          /* We don't support IA5String so we support only BITSTRING, UTF8 and PrintableString */

          /* Now consider the case where attr1 is a string (either Printable or UTF8) */
          if (attr1_st.str_format == TAG_PRINTABLE_STRING || attr1_st.str_format == TAG_UTF8_STRING)
          {
            /* Then the attr2 should be of string type with the same size */
            if ((attr2_st.str_format != TAG_PRINTABLE_STRING && attr2_st.str_format != TAG_UTF8_STRING) ||
                attr1_st.str_size != attr2_st.str_size)
            {
              return (STSE_CERT_CA_NOT_MATCHING);
            }
            /* Now they are both string type and of same size, they should match */
            if (stse_certificate_case_insensitive_compare(attr1_st.str, attr2_st.str, attr1_st.str_size) != 0)
            {
              return (STSE_CERT_CA_NOT_MATCHING);
            }
            /* Here they are both string type, of same size, and match. So break the loop */
            break;
          }
          /* If we are here we didn't return -1 and we didn't break */
          /* Therefore the type of attr1 was not among the allowed ones (e.g. it might be IA5String) */
          /* Just try a very raw comparison.. it's the only thing we can do */
          /* Note: this perfectly cover the BITSTRING case */
          /* attributes should be of same type and size */
          if (attr1_st.str_format == attr2_st.str_format && attr1_st.str_size == attr2_st.str_size)
          {
            /* And they should match*/
            if (memcmp(attr1_st.str, attr2_st.str, (PLAT_UI32)attr1_st.str_size) != 0)
            {
              return (STSE_CERT_CA_NOT_MATCHING);
            }
            /* Here they match, so we break the loop */
            break;
          }
          return (STSE_CERT_CA_NOT_MATCHING);
        }
      } while (attr2_loop < name2);
      /* Did we found the attribute ? If not return -1 */
      if (found != 1)
      {
        return (STSE_CERT_CA_NOT_MATCHING);
      }
    }
  }

  /* If we are here then certificates are ok. */
  /* But is the signature valid ? */
  return (stse_certificate_verify_cert_signature(parent, child));
}

stse_return_code_t stse_certificate_parse_chain(
  PLAT_UI8 *root_ca,
  PLAT_UI8 *cert_chain,
  PLAT_UI16 cert_chain_size,
  stse_certificate_t *leaf_cert)
{
  stse_return_code_t ret;
  PLAT_UI8 *next;
  PLAT_UI8 *eof_cert_chain = cert_chain + cert_chain_size;
  stse_certificate_t parsed_parent_cert;
  stse_certificate_t parsed_child_cert;
  /* Parse first parent certificate */
  ret = stse_certificate_parse(cert_chain, &parsed_child_cert, (const PLAT_UI8 **)&next);
  if (ret != STSE_OK)
  {
    return ret;
  }
  /* If a root CA is passed in arguments, process it */
  if (root_ca != NULL)
  {
    /* Parse root CA certificate */
    ret = stse_certificate_parse(root_ca, &parsed_parent_cert, NULL);
    if (ret != STSE_OK)
    {
      return ret;
    }
    /* Verify first intermediate CA against root CA */
    ret = stse_certificate_is_parent(&parsed_parent_cert, &parsed_child_cert, NULL);
    if (ret != STSE_OK)
    {
      return ret;
    }
  }
  /* Loop over each certificate in the chain */
  while (next < eof_cert_chain)
  {
    /* Child becomes parent of the next certificate */
    stse_certificate_copy(&parsed_parent_cert, &parsed_child_cert);
    /* Parse next child certificate */
    ret = stse_certificate_parse(next, &parsed_child_cert, (const PLAT_UI8 **)&next);
    if (ret != STSE_OK)
    {
      return ret;
    }
    /* Verify child cert signature against parent public key */
    ret = stse_certificate_is_parent(&parsed_parent_cert, &parsed_child_cert, NULL);
    if (ret != STSE_OK)
    {
      return ret;
    }
  }
  /* If leaf_cert != NULL, it need to be returned */
  if (leaf_cert != NULL)
  {
    /* Last child certificate is returned as leaf certificate */
    stse_certificate_copy(leaf_cert, &parsed_child_cert);
  }
  return ret;
}

void stse_certificate_set_stse_companion(stse_handle_t *p_stse)
{
  if (p_stse != NULL)
  {
    stsafe_x509_parser_companion_handler = p_stse;
  }
}

void stse_certificate_reset_stse_companion()
{
  stsafe_x509_parser_companion_handler = NULL;
}
