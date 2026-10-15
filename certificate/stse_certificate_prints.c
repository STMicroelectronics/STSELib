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

#include "certificate/stse_certificate_prints.h"
#include "certificate/stse_certificate_subparsing.h"
#include "core/stse_platform.h"
#include <inttypes.h>
#include <string.h>

/** @brief print the extensions_flags field of a certificate */
static void print_extensions(PLAT_UI32 extensions_flags);

void stse_certificate_print_tag(PLAT_I32 tag) {
    switch (tag) {
    case TAG_BOOLEAN:
        stse_platform_printf("Boolean");
        break;
    case TAG_INTEGER:
        stse_platform_printf("Integer");
        break;
    case TAG_BITSTRING:
        stse_platform_printf("Bistring");
        break;
    case TAG_OCTETSTRING:
        stse_platform_printf("Octectstring");
        break;
    case TAG_NULL:
        stse_platform_printf("Null");
        break;
    case TAG_OBJECT_IDENTIFIER:
        stse_platform_printf("Object Identifier");
        break;
    case TAG_ObjectDescriptor:
        stse_platform_printf("Object Descriptor");
        break;
    case TAG_ENUMERATED:
        stse_platform_printf("Enumerated");
        break;
    case TAG_UTF8String:
        stse_platform_printf("UTF8");
        break;
    case TAG_SEQUENCE:
        stse_platform_printf("Sequence");
        break;
    case TAG_SET:
        stse_platform_printf("Set");
        break;
    case TAG_PrintableString:
        stse_platform_printf("PrintableString");
        break;
    case TAG_IA5String:
        stse_platform_printf("IA5String");
        break;
    case TAG_UTCTime:
        stse_platform_printf("UTCTime");
        break;
    case TAG_GeneralizedTime:
        stse_platform_printf("GeneralizedTime");
        break;
    case TAG_x509VERSION:
        stse_platform_printf("x509VERSION");
        break;
    case TAG_subjectUniqueID:
        stse_platform_printf("subjectUniqueID");
        break;
    case TAG_extensions:
        stse_platform_printf("extensions");
        break;
    case TAG_issuerUniqueID:
        stse_platform_printf("issuerUniqueID");
        break;
    }
}

void stse_certificate_print_signature_algorithm(PLAT_I32 type) {
    switch (type) {
    case SIG_EDDSA_ED25519:
        stse_platform_printf("eddsa-with-SHA256");
        break;
    case SIG_ECDSA_SHA1:
        stse_platform_printf("ecdsa-with-SHA1");
        break;
    case SIG_ECDSA_SHA224:
        stse_platform_printf("ecdsa-with-SHA224");
        break;
    case SIG_ECDSA_SHA256:
        stse_platform_printf("ecdsa-with-SHA256");
        break;
    case SIG_ECDSA_SHA384:
        stse_platform_printf("ecdsa-with-SHA384");
        break;
    case SIG_ECDSA_SHA512:
        stse_platform_printf("ecdsa-with-SHA512");
        break;
    default:
        stse_platform_printf("Failed Identification of SignatureAlgorithm");
        break;
    }
}

void stse_certificate_print_attribute(PLAT_I32 type) {
    switch (type) {
    case ATTR_CN:
        stse_platform_printf("CommonName");
        break;
    case ATTR_C:
        stse_platform_printf("CountryName");
        break;
    case ATTR_SN:
        stse_platform_printf("SerialNumber");
        break;
    case ATTR_DN:
        stse_platform_printf("DistinguishedName");
        break;
    case ATTR_ON:
        stse_platform_printf("OrganizationalName");
        break;
    case ATTR_OUN:
        stse_platform_printf("OrganizationalUnitName");
        break;
    case ATTR_SOPN:
        stse_platform_printf("StateOrProvinceName");
        break;
    case ATTR_LN:
        stse_platform_printf("LocalityName");
        break;
    case ATTR_UID:
        stse_platform_printf("UniqueIdentifier");
        break;
    default:
        stse_platform_printf("Failed Identification of Attribute");
        break;
    }
}

void stse_certificate_print_elliptic_curve(PLAT_I32 type) {
    switch (type) {
    case EC_P256:
        stse_platform_printf("P-256");
        break;
    case EC_P384:
        stse_platform_printf("P-384");
        break;
    case EC_P521:
        stse_platform_printf("P-521");
        break;
    case EC_bp256r1:
        stse_platform_printf("brainpoolP256r1");
        break;
    case EC_bp256t1:
        stse_platform_printf("brainpoolP256t1");
        break;
    case EC_bp384r1:
        stse_platform_printf("brainpoolP384r1");
        break;
    case EC_bp384t1:
        stse_platform_printf("brainpoolP384t1");
        break;
    case EC_bp512r1:
        stse_platform_printf("brainpoolP512r1");
        break;
    case EC_bp512t1:
        stse_platform_printf("brainpoolP512t1");
        break;
    case EC_Ed25519:
        stse_platform_printf("ed25519");
        break;

    default:
        stse_platform_printf("Failed Identification of EllipticCurve");
        break;
    }
}

PLAT_I32 stse_certificate_print_bit_string(const PLAT_UI8 *value, PLAT_I32 size) {
    PLAT_I32 i;
    /* The first byte of Value identifies the number of discarded bits in the LSB.
  This is a feature we don't support
  */
    if (value[0] != 0) {
        stse_platform_printf("BITSTRING exclues some LSBits, we don't handle that, error!\n");
        return (-1);
    }
    /* Print the remaining string */
    for (i = 1; i < size; i++) {
        stse_platform_printf("%02X", value[i]);
        if (i % 16 == 0 && i != size - 1)
            stse_platform_printf("\n\t\t");
    }
    return (0);
}

PLAT_I32 stse_certificate_print_integer(const PLAT_UI8 *value, PLAT_I32 size) {
    PLAT_I32 i = 0;
    /* The first byte tells us if it's positive or negative. We don't support negative */
    if (value[0] >> 7 == 1) {
        stse_platform_printf("Integer is negative, we don't handle that, error!\n");
        return (-1);
    }
    /* First byte might be zero in case Integer is positive and first byte >127 */
    /* So skip it */
    if (value[0] == 0) {
        i++;
    }
    /* And print the rest */
    for (; i < size; i++) {
        stse_platform_printf("%02X", value[i]);
        if (i % 16 == 0 && (i != size - 1) && i != 0)
            stse_platform_printf("\n\t\t");
    }
    return (0);
}

/* Print a byte buffer */
static void print_buffer(const char *str, const PLAT_UI8 *buf, PLAT_I32 bufsize) {
    PLAT_I32 i = 0;
    stse_platform_printf("%s", str);
    for (i = 0; i < bufsize; i++) {
        stse_platform_printf("%02X", buf[i]);
    }
}

void stse_certificate_print_parsed_cert(stse_certificate_t *stse_certificate) {
    //stse_platform_printf("Fields: %08X\n", stse_certificate->fields);
    //stse_platform_printfields(stse_certificate->fields);

    stse_platform_printf("\n\r\t x509 Version: %" PRIi32, stse_certificate->x509_version + 1);
    if (stse_certificate->serial_number != NULL && stse_certificate->serial_number_size > 0)
        print_buffer("\n\r\t_serial_number: ", stse_certificate->serial_number, stse_certificate->serial_number_size);
    if (stse_certificate->issuer != NULL && stse_certificate->issuer_size > 0) {
        stse_platform_printf("\n\r\t Issuer:");
        stse_certificate_print_name(stse_certificate->issuer, stse_certificate->issuer_size);
    }
    if (stse_certificate->subject != NULL && stse_certificate->subject_size > 0) {
        stse_platform_printf("\n\r\t Subject:");
        stse_certificate_print_name(stse_certificate->subject, stse_certificate->subject_size);
    }
    stse_certificate_print_validity(stse_certificate->validity);
    stse_platform_printf("\n\r\t SignatureAlgorithm: ");
    stse_certificate_print_signature_algorithm(stse_certificate->signature_algorithm);
    stse_platform_printf("\n\r\t tbs_signature: ");
    stse_certificate_print_signature_algorithm(stse_certificate->signature);
    stse_platform_printf("\n\r\t EllipticCurve: ");
    stse_certificate_print_elliptic_curve(stse_certificate->elliptic_curve);
    stse_platform_printf("\n\r\t Cert PubKey (%s): ", *stse_certificate->p_pub_key_point_representation_id == 0x04 ? "Uncompressed" : "Compressed");
    print_buffer("\n\r\t\t X: ", stse_certificate->pub_key.p_x, stse_certificate->pub_key.fsize);
    if (*stse_certificate->p_pub_key_point_representation_id == 0x04) {
        print_buffer("\n\r\t\t Y: ", stse_certificate->pub_key.p_y, stse_certificate->pub_key.fsize);
    }
    stse_platform_printf("\n\r\t Cert Signature: ");
    print_buffer("\n\r\t\t r: ", stse_certificate->sign.p_r, stse_certificate->sign.r_size);
    print_buffer("\n\r\t\t s :", stse_certificate->sign.p_s, stse_certificate->sign.s_size);
    print_extensions(stse_certificate->extensions_flags);
}

void stse_certificate_simple_parse_attribute(const PLAT_UI8 *p, stse_cert_attribute_t *attribute_st, const PLAT_UI8 **next_thing) {
    PLAT_I32 total_size, size, parsed, tag;
    const PLAT_UI8 *next = p;
    tag = stse_certificate_identify_asn1_tlv(next, &parsed, &total_size, &next);
    total_size += parsed;
    if (tag == TAG_SET) {
        tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
        if (tag == TAG_SEQUENCE) {
            tag = stse_certificate_identify_asn1_tlv(next, &parsed, &size, &next);
            if (tag == TAG_OBJECT_IDENTIFIER) {
                attribute_st->type = stse_certificate_identify_attribute(next, size);
                if (attribute_st->type != -1) {
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

void stse_certificate_print_name(const PLAT_UI8 *name, PLAT_I32 name_size) {
    PLAT_I32 wholesize, tag, parsed;
    const PLAT_UI8 *next;
    stse_cert_attribute_t attribute_st;
    tag = stse_certificate_identify_asn1_tlv(name, &parsed, &wholesize, &next);
    if (tag == TAG_SEQUENCE) {
        /* Here we are inside the name, now loop */
        while (next < name + name_size) {
            attribute_st.type = -1;
            stse_certificate_simple_parse_attribute(next, &attribute_st, &next);
            stse_platform_printf("\n\r\t\t ");
            stse_certificate_print_attribute(attribute_st.type);
            stse_platform_printf(": ");
            if (attribute_st.type != -1 &&
                (attribute_st.str_format == TAG_PrintableString ||
                 attribute_st.str_format == TAG_UTF8String)) {
                PLAT_I32 i;
                for (i = 0; i < attribute_st.str_size; i++) {
                    stse_platform_printf("%c", attribute_st.str[i]);
                }
            } else if (attribute_st.type != -1 && attribute_st.str_format == TAG_BITSTRING) {
                stse_certificate_print_bit_string(attribute_st.str, attribute_st.str_size);
            }
        }
    }
}

void stse_certificate_print_validity(const PLAT_UI8 *validity) {
    stse_cert_validity_t not_before_st, not_after_st;
    const PLAT_UI8 *next;
    stse_certificate_parse_validity(validity, &not_before_st, &not_after_st, &next);
    stse_platform_printf("\n\r\t Validity:");
    stse_platform_printf("\n\r\t\t Not Before: %04" PRIu32 "-%02d-%02d %02d:%02d:%02d", not_before_st.year, not_before_st.month, not_before_st.days, not_before_st.hours, not_before_st.minutes, not_before_st.seconds);
    stse_platform_printf("\n\r\t\t Not After:  %04" PRIu32 "-%02d-%02d %02d:%02d:%02d", not_after_st.year, not_after_st.month, not_after_st.days, not_after_st.hours, not_after_st.minutes, not_after_st.seconds);
}

static void print_extensions(PLAT_UI32 extensions_flags) {
    stse_platform_printf("\n\r\t List of Extensions:");
    if ((extensions_flags & 1) == 1) {
        stse_platform_printf("\n\r\t\t BasicConstraints: ");
        if (((extensions_flags >> 1) & 1) == 1) {
            stse_platform_printf("Critical. ");
        }
        if (((extensions_flags >> 2) & 1) == 1) {
            stse_platform_printf("CA certificate. ");
        } else {
            stse_platform_printf("Not a CA certificate. ");
        }
        if (((extensions_flags >> 3) & 1) == 1) {
            stse_platform_printf("PathSize: %" PRIu32, (extensions_flags >> 4) & 15);
        }
    }
    if (((extensions_flags >> 8) & 1) == 1) {
        stse_platform_printf("\n\r\t\t KeyUsage: ");
        if (((extensions_flags >> 9) & 1) == 1) {
            stse_platform_printf("Critical. ");
        }
        if (((extensions_flags >> (16 + 0)) & 1) == 1) {
            stse_platform_printf("digital_signature ");
        }
        if (((extensions_flags >> (16 + 1)) & 1) == 1) {
            stse_platform_printf("content_commitment ");
        }
        if (((extensions_flags >> (16 + 2)) & 1) == 1) {
            stse_platform_printf("key_encipherment ");
        }
        if (((extensions_flags >> (16 + 3)) & 1) == 1) {
            stse_platform_printf("data_encipherment ");
        }
        if (((extensions_flags >> (16 + 4)) & 1) == 1) {
            stse_platform_printf("key_agreement ");
        }
        if (((extensions_flags >> (16 + 5)) & 1) == 1) {
            stse_platform_printf("key_cert_sign ");
        }
        if (((extensions_flags >> (16 + 6)) & 1) == 1) {
            stse_platform_printf("c_rl_sign ");
        }
        if (((extensions_flags >> (16 + 7)) & 1) == 1) {
            stse_platform_printf("encipher_only ");
        }
        if (((extensions_flags >> (15)) & 1) == 1) {
            stse_platform_printf("decipher_only ");
        }
    }

    if (((extensions_flags >> 24) & 1) == 1) {
        stse_platform_printf("\n\r\t\t ExtendedKeyUsage: ");
        if (((extensions_flags >> 25) & 1) == 1) {
            stse_platform_printf("Critical. ");
        }
    }
}
