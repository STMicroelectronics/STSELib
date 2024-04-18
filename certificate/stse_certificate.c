/**
  ******************************************************************************
  * @file    stse_certificate.c
  * @author  STMicroelectronics - CS application team
  * @brief   STSAFE-A x509 certificate Parser
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

#include <stdint.h>
#include <string.h>
#include "certificate/stse_certificate.h"
#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"
#include "certificate/stse_certificate_crypto.h"

stse_Handler_t * stsafe_x509_parser_companion_handler = NULL;

/**
 * \brief  Parse the extensions part of a certificate
 * \param[in]  *extensions pointer to the extensions part of a certificate
 * \param[in]  extensionsSize Size of the extensions field
 * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
 */
static void stse_certificate_parse_extensions(const PLAT_UI8 *extensions, PLAT_I32 extensionsSize, stse_certificate_t *stse_certificate)
{
	const PLAT_UI8 *next_value;
	PLAT_I32 size, tag, parsed;
	stse_cert_extension_t ext_st = {.type = -1, .value = NULL, .valueSize = 0};

	if(extensions != NULL && stse_certificate != NULL)
	{
		/* Start parsing the extensions, extensions is a sequence */
		tag = stse_certificate_identify_ASN1_TLV(extensions, &parsed, &size, &next_value);
		if (tag == TAG_SEQUENCE)
		{
			/* Now we loop over sequences */
			while (next_value < extensions + extensionsSize)
			{
				tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
				if (tag == TAG_SEQUENCE)
				{
					stse_certificate_parse_extension(next_value, &stse_certificate->extensionsFlags, &ext_st);
				}
				next_value += size;
			}
		}
	}
}

/**
 * \brief  Parse the tbs part of a certificate
 * \param[in]  *tbs pointer to the TBSCertificate field of a certificate
 * \param[in]  *tbsSize size of the TBSCertificate
 * \param[out] *stse_certificate pointer to the stse_certificate_t that will be filled
 */
static void parsetbsCertificate(const PLAT_UI8 *tbs, PLAT_I32 tbsSize, stse_certificate_t *stse_certificate)
{
  const PLAT_UI8 *next_value;
  PLAT_I32 size, tag, parsed;

  if(tbs != NULL && stse_certificate != NULL)
  {
	  /* Start parsing the TBS, tbs is a sequence */
	  tag = stse_certificate_identify_ASN1_TLV(tbs, &parsed, &size, &next_value);
	  if (tag == TAG_SEQUENCE)
	  {
		/* Now we expect the version */
		tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
		if (tag == TAG_x509VERSION)
		{
		  stse_certificate_parse_X509_version(next_value, stse_certificate, &next_value);
		}
		else /* The Version is optional, if it's not present then 0 (v1) is assumed */
		{
		  stse_certificate->x509Version = 0;
		  next_value -= parsed;
		}
		/* Move on to Serial Number */
		stse_certificate_parse_integer(next_value, &stse_certificate->serialNumber, &stse_certificate->serialNumberSize, &next_value);
		/* Parse tbsSignature(Algorihtm)*/
		stse_certificate_parse_signature_algorithm(next_value, &stse_certificate->signature, &next_value);
		/* Now we face the issuer */
		tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
		if (tag == TAG_SEQUENCE)
		{
		  stse_certificate->issuer = next_value - parsed;
		  stse_certificate->issuerSize = size + parsed;
		}
		next_value += size; /* Issuer is not parsed */
		/* Now we face validity */
		tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
		if (tag == TAG_SEQUENCE)
		{
		  stse_certificate->validity = next_value - parsed;
		  stse_certificate->validitySize = size + parsed;
		}
		next_value += size; /* Validity is not parsed */
		/* Now we face Subject */
		tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
		if (tag == TAG_SEQUENCE)
		{
		  stse_certificate->subject = next_value - parsed;
		  stse_certificate->subjectSize = size + parsed;
		}
		next_value += size; /* Subject is not parsed */
		/* Now we face SubjectPublicKey */
		stse_certificate_parse_ECC_public_key(next_value, stse_certificate, &next_value);
		/* Now we face Optional Extensions, but those are optional */
		if (next_value < tbs + tbsSize)
		{
		  tag = stse_certificate_identify_ASN1_TLV(next_value, &parsed, &size, &next_value);
		  if (tag == TAG_extensions)
		  {
			stse_certificate->extensions = next_value;
			stse_certificate->extensionsSize = size;
			stse_certificate_parse_extensions(stse_certificate->extensions, stse_certificate->extensionsSize, stse_certificate);
		  }
		}
	  }
  }
}

void stse_certificate_init(stse_certificate_t *cert)
{
	if(cert != NULL)
	{
		cert->tbs = NULL;
		cert->tbsSize = -1;
		cert->x509Version = -1;
		cert->serialNumber = NULL;
		cert->serialNumberSize = -1;
		cert->signature = -1;
		cert->issuer = NULL;
		cert->issuerSize = -1;
		cert->validity = NULL;
		cert->validitySize = -1;
		cert->subject = NULL;
		cert->subjectSize = -1;
		cert->EllipticCurve = -1;
		cert->PubKey.fsize = 0;
		cert->PubKey.pX = NULL;
		cert->PubKey.pY = NULL;
		cert->extensions = NULL;
		cert->extensionsSize = -1;
		cert->SignatureAlgorithm = -1;
		cert->Sign.pR = NULL;
		cert->Sign.pS = NULL;
		cert->Sign.rSize = 0;
		cert->Sign.sSize = 0;
		cert->extensionsFlags = 0;
	}
}

void stse_certificate_copy(stse_certificate_t *copiedCert, const stse_certificate_t *originalCert)
{
	if(copiedCert != NULL && originalCert != NULL)
	{
		*copiedCert = *originalCert;
	}
}

stse_ReturnCode_t stse_certificate_parse(const PLAT_UI8 *cert, stse_certificate_t *stse_certificate, const PLAT_UI8 **next)
{
  PLAT_I32 total_size, tag, size, parsed;
  const PLAT_UI8 *next_thing;

  stse_certificate_init(stse_certificate);
  /* First we start by looking at the first item, this will be used to set the size of the whole certificate */
  /* Then the parsing of the other subfields will start */
  tag = stse_certificate_identify_ASN1_TLV(cert, &parsed, &total_size, &next_thing);
  if (tag != TAG_SEQUENCE)
  {     
    return(STSE_CERT_UNEXPECTED_SEQUENCE);
  }

  /* Get tbs */
  tag = stse_certificate_identify_ASN1_TLV(next_thing, &parsed, &size, &next_thing);
  if (tag != TAG_SEQUENCE)
  {
    return(STSE_CERT_UNEXPECTED_SEQUENCE);
  }
  stse_certificate->tbs = next_thing-parsed;
  stse_certificate->tbsSize = size+parsed;
  
  /* Get SignatureAlgorithm */
  next_thing += size;
  stse_certificate_parse_signature_algorithm(next_thing, &stse_certificate->SignatureAlgorithm, &next_thing);

  /* Now we should have the Signature. If it's ECDSA parse it! */
  if (stse_certificate->SignatureAlgorithm != -1)
  {
    stse_certificate_parse_ECDSA_signature(next_thing, stse_certificate, &next_thing);
  }

  /* We have completed the parsing of the TOP level */
  /* Let's move on to parse the tbs */
  parsetbsCertificate(stse_certificate->tbs, stse_certificate->tbsSize, stse_certificate);

  if (next != NULL)
  {
    *next = next_thing;
  }  

  return(STSE_OK);
}

stse_ReturnCode_t stse_certificate_is_valid(const stse_certificate_t *stse_certificate, const stse_cert_validity_t *currentTime)
{
  if (
    (stse_certificate->issuer == NULL || stse_certificate->issuerSize == -1) ||
    (stse_certificate->subject == NULL || stse_certificate->subjectSize == -1) ||
    (stse_certificate->serialNumber == NULL || stse_certificate->serialNumberSize == -1) ||
    (stse_certificate->signature == -1 || stse_certificate->SignatureAlgorithm != stse_certificate->signature) ||
    (stse_certificate->validity == NULL || stse_certificate->validitySize == -1) ||
    (stse_certificate->EllipticCurve == -1 || stse_certificate->x509Version == -1)||
    (stse_certificate->PubKey.fsize <= 0 || stse_certificate->PubKey.pX == NULL) || 
    (*stse_certificate->pPubKey_point_representation_id == 0x04 && stse_certificate->PubKey.pY == NULL) ||
    (*stse_certificate->pPubKey_point_representation_id != 0x04 && stse_certificate->PubKey.pY != NULL) ||
    (stse_certificate->Sign.pR == NULL || stse_certificate->Sign.rSize == -1) ||
    (stse_certificate->Sign.pS == NULL || stse_certificate->Sign.sSize == -1)
    )
  {
	  return(STSE_CERT_INVALID_CERTIFICATE);
  }

  if (currentTime != NULL)
  {
    stse_cert_validity_t notBefore_st, notAfter_st;    
    stse_certificate_parse_validity(stse_certificate->validity, &notBefore_st, &notAfter_st, NULL);

    if (stse_certificate_date_compare(currentTime, &notBefore_st) < 0)
    {
	  return(STSE_CERT_INVALID_CERTIFICATE);
    }

    if (stse_certificate_date_compare(currentTime, &notAfter_st) > 0)
    {
	  return(STSE_CERT_INVALID_CERTIFICATE);
    }
  }

  return(STSE_OK);
}

stse_ReturnCode_t stse_certificate_is_CA(const stse_certificate_t *cert)
{
  if (((cert->extensionsFlags >> 0) & 1U) == 1U)
  {
    /* BasicContrain is present */
    if (((cert->extensionsFlags >> 2) & 1U) == 1U)
    {
      /* This is a CA */
	  return(STSE_OK);
    }
  }
  return(STSE_CERT_INVALID_CERTIFICATE);
}

stse_ReturnCode_t stse_certificate_is_parent(const stse_certificate_t *parent, const stse_certificate_t *child, const stse_cert_validity_t *currentTime)
{
  /* Get the two name structure to be compared */
  const PLAT_UI8 *name1 = child->issuer;
  PLAT_I32 name1Size = child->issuerSize;
  const PLAT_UI8 *name2 = parent->subject;
  PLAT_I32 name2Size = parent->subjectSize;
  PLAT_I32 found, parsed;
  const PLAT_UI8 *attr2_loop;
  const PLAT_UI8 *attr1, *attr2;
  stse_cert_attribute_t attr1_st, attr2_st;
  
  /* Do not compare two invalid certificates */
  if (stse_certificate_is_valid(parent, currentTime) != STSE_OK)
  {
    return(STSE_CERT_INVALID_CERTIFICATE);
  }

  if (stse_certificate_is_valid(child, currentTime) != STSE_OK)
  {
    return(STSE_CERT_INVALID_CERTIFICATE);
  }

  if ( stse_certificate_is_CA(parent) != STSE_OK)
  {
    return(STSE_CERT_INVALID_CERTIFICATE);
  }


  /* Check that they have the same number of attributes */
  if (stse_certificate_count_attributes(name1) != stse_certificate_count_attributes(name2))
  {
    return (STSE_CERT_CA_NOT_MATCHING);
  }  

  /* Now we need to scan name1 and look for corresponding attribute in name2 */
  /* Start entering in the root sequence */
  if (stse_certificate_identify_ASN1_TLV(name1, &parsed, &found, &name1) != TAG_SEQUENCE)
  {
	return(STSE_CERT_INVALID_CERTIFICATE);
  }

  if (stse_certificate_identify_ASN1_TLV(name2, &parsed, &found, &name2) != TAG_SEQUENCE)
  {
	return(STSE_CERT_INVALID_CERTIFICATE);
  }

  /* Loop on all RDN */
  while ( (name1 < child->issuer + name1Size) && (name2<parent->subject+name2Size) )
  {
    /* Parse RDN1 and RDN2. The SEQUENCE is order so this should match */
    stse_certificate_parse_relative_distinguished_name(name1, &name1, &attr1);
    stse_certificate_parse_relative_distinguished_name(name2, &name2, &attr2);
    /* Now loop on the attributes inside the RDN, which are not ordered */
    while (attr1 < name1) /*Loop untill the next RDN1 */
    {
      /* A single DN is a SET which might (but usually doesn't) contain more Attributes */
      /* Since the SET is not ordered we need to select the attribute from the name1 and scan
         all the SET of name2 for a matching attribute */
      
      stse_certificate_parse_attribute(attr1, &attr1_st, &attr1); /*Read the attribute of Name1 */

      attr2_loop = attr2; /* copy attr2 as attr2loop will loop for each attribute of RDN1 */      
      found = 0; /* Flag to tell us if we found or not the attribute in Name2 */
      do /* Scans the whole DN SET in Name2 */
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
          if (attr1_st.strFormat == TAG_PrintableString || attr1_st.strFormat == TAG_UTF8String)
          {
            /* Then the attr2 should be of string type with the same size */
            if ((attr2_st.strFormat != TAG_PrintableString && attr2_st.strFormat != TAG_UTF8String) ||
              attr1_st.strSize != attr2_st.strSize)
            {
                return (STSE_CERT_CA_NOT_MATCHING);
            }
            /* Now they are both string type and of same size, they should match */
            if (stse_certificate_case_insensitive_compare(attr1_st.str, attr2_st.str, attr1_st.strSize) != 0)
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
          if (attr1_st.strFormat == attr2_st.strFormat && attr1_st.strSize == attr2_st.strSize)
          {           
            /* And they should match*/
            if (memcmp(attr1_st.str, attr2_st.str, (PLAT_UI32)attr1_st.strSize) != 0)
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
  return(stse_certificate_verify_cert_signature(parent, child));
}

stse_ReturnCode_t stse_certificate_parse_chain(
		PLAT_UI8 *rootCA,
		PLAT_UI8 *certChain,
		PLAT_UI16 certChainSize,
		stse_certificate_t *leafCert)
{
	stse_ReturnCode_t ret;
	PLAT_UI8 *next;
	PLAT_UI8 *eofCertChain = certChain + certChainSize;
	stse_certificate_t parsed_parent_cert;
	stse_certificate_t parsed_child_cert;
	/* Parse first parent certificate */
	ret = stse_certificate_parse(certChain, &parsed_child_cert, (const PLAT_UI8 **)&next);
	if (ret != STSE_OK)
	{
		return ret;
	}
	/* If a root CA is passed in arguments, process it */
	if(rootCA != NULL)
	{
		/* Parse root CA certificate */
		ret = stse_certificate_parse(rootCA, &parsed_parent_cert, NULL);
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
	while(next < eofCertChain)
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
	/* If leafCert != NULL, it need to be returned */
	if(leafCert != NULL)
	{
		/* Last child certificate is returned as leaf certificate */
		stse_certificate_copy(leafCert, &parsed_child_cert);
	}
	return STSE_OK;
}

void stse_certificate_set_stse_companion(stse_Handler_t *pSTSE)
{
	if(pSTSE != NULL)
	{
		stsafe_x509_parser_companion_handler = pSTSE;
	}
}

void stse_certificate_reset_stse_companion()
{
	stsafe_x509_parser_companion_handler = NULL;
}
