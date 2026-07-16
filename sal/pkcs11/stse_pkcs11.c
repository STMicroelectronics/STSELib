/*!
 ******************************************************************************
 * \file    stse_pkcs11.c
 * \brief   STSE PKCS#11 provider — internal implementation
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "sal/pkcs11/stse_pkcs11.h"

/* ---------------------------------------------------------------------------
 * Default configuration
 *   Users may call stse_pkcs11_set_config() before C_Initialize() to override.
 * --------------------------------------------------------------------------*/

static stse_pkcs11_config_t g_stse_pkcs11_config = {
    .i2c_addr        = 0x20U,         /* STSAFE-A default 7-bit address */
    .bus_id          = 0U,
    .bus_speed       = 400U,           /* 400 kHz */
    .device_type     = STSAFE_A120,
    .cert_zone_count = 0U,
    .cert_zone_indices = { 0U }
};

/* ---------------------------------------------------------------------------
 * Global library singleton
 * --------------------------------------------------------------------------*/

stse_pkcs11_lib_t g_stse_pkcs11_lib = { .initialized = CK_FALSE };

/* ---------------------------------------------------------------------------
 * Configuration helpers
 * --------------------------------------------------------------------------*/

void stse_pkcs11_set_config(const stse_pkcs11_config_t *pConfig)
{
    if (pConfig != NULL) {
        g_stse_pkcs11_config = *pConfig;
    }
}

const stse_pkcs11_config_t *stse_pkcs11_get_config(void)
{
    return &g_stse_pkcs11_config;
}

/* ---------------------------------------------------------------------------
 * Handler initialisation
 * --------------------------------------------------------------------------*/

stse_ReturnCode_t stse_pkcs11_init_handler(stse_pkcs11_lib_t *pLib)
{
    stse_ReturnCode_t ret;

    if (pLib == NULL) {
        return STSE_CORE_INVALID_PARAMETER;
    }

    /* Copy current configuration */
    pLib->config = g_stse_pkcs11_config;

    /* Initialise the STSELib handler to default platform callbacks */
    ret = stse_set_default_handler_value(&pLib->stse_handler);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Apply I2C parameters from configuration */
    pLib->stse_handler.device_type   = pLib->config.device_type;
    pLib->stse_handler.io.Devaddr    = pLib->config.i2c_addr;
    pLib->stse_handler.io.busID      = pLib->config.bus_id;
    pLib->stse_handler.io.BusSpeed   = pLib->config.bus_speed;

    /* Open communication with the secure element */
    ret = stse_init(&pLib->stse_handler);
    if (ret != STSE_OK) {
        return ret;
    }

    /* Discover the private-key slot table */
    pLib->key_slot_count = 0U;
    ret = stse_get_ecc_key_slots_count(&pLib->stse_handler, &pLib->key_slot_count);
    if (ret != STSE_OK) {
        /* Non-fatal: proceed with zero key slots */
        pLib->key_slot_count = 0U;
        return STSE_OK;
    }

    if (pLib->key_slot_count > STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS) {
        pLib->key_slot_count = STSE_PKCS11_MAX_PRIVATE_KEY_SLOTS;
    }

    if (pLib->key_slot_count > 0U) {
        PLAT_UI16 global_usage_limit = 0U;
        ret = stse_get_ecc_key_table_info(
            &pLib->stse_handler,
            pLib->key_slot_count,
            &global_usage_limit,
            pLib->key_slot_info);
        if (ret != STSE_OK) {
            /* Non-fatal: key slots won't be advertised */
            pLib->key_slot_count = 0U;
        }
    }

    return STSE_OK;
}

/* ---------------------------------------------------------------------------
 * Object-template matching
 * --------------------------------------------------------------------------*/

CK_BBOOL stse_pkcs11_object_matches_template(
    const stse_pkcs11_object_t *pObj,
    const CK_ATTRIBUTE_PTR      pTemplate,
    CK_ULONG                    ulCount)
{
    CK_ULONG i;

    if (pObj == NULL) {
        return CK_FALSE;
    }

    /* An empty template matches every object */
    if (pTemplate == NULL || ulCount == 0U) {
        return CK_TRUE;
    }

    for (i = 0U; i < ulCount; i++) {
        const CK_ATTRIBUTE *pAttr = &pTemplate[i];

        switch (pAttr->type) {
            case CKA_CLASS: {
                if (pAttr->ulValueLen != sizeof(CK_OBJECT_CLASS)) {
                    return CK_FALSE;
                }
                CK_OBJECT_CLASS req_class;
                memcpy(&req_class, pAttr->pValue, sizeof(CK_OBJECT_CLASS));
                if (req_class != pObj->obj_class) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_KEY_TYPE: {
                if (pAttr->ulValueLen != sizeof(CK_KEY_TYPE)) {
                    return CK_FALSE;
                }
                CK_KEY_TYPE req_ktype;
                memcpy(&req_ktype, pAttr->pValue, sizeof(CK_KEY_TYPE));
                if (req_ktype != CKK_EC) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_TOKEN: {
                if (pAttr->ulValueLen != sizeof(CK_BBOOL)) {
                    return CK_FALSE;
                }
                CK_BBOOL req_token;
                memcpy(&req_token, pAttr->pValue, sizeof(CK_BBOOL));
                /* All our objects are token objects */
                if (req_token != CK_TRUE) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_SIGN: {
                if (pAttr->ulValueLen != sizeof(CK_BBOOL)) {
                    return CK_FALSE;
                }
                CK_BBOOL req_sign;
                memcpy(&req_sign, pAttr->pValue, sizeof(CK_BBOOL));
                /* Only private keys support signing */
                if (req_sign == CK_TRUE && pObj->obj_class != CKO_PRIVATE_KEY) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_VERIFY: {
                if (pAttr->ulValueLen != sizeof(CK_BBOOL)) {
                    return CK_FALSE;
                }
                CK_BBOOL req_verify;
                memcpy(&req_verify, pAttr->pValue, sizeof(CK_BBOOL));
                /* Only public keys support verification */
                if (req_verify == CK_TRUE && pObj->obj_class != CKO_PUBLIC_KEY) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_CERTIFICATE_TYPE: {
                if (pAttr->ulValueLen != sizeof(CK_CERTIFICATE_TYPE)) {
                    return CK_FALSE;
                }
                CK_CERTIFICATE_TYPE req_cert_type;
                memcpy(&req_cert_type, pAttr->pValue, sizeof(CK_CERTIFICATE_TYPE));
                if (pObj->obj_class != CKO_CERTIFICATE || req_cert_type != CKC_X_509) {
                    return CK_FALSE;
                }
                break;
            }

            case CKA_ID:
            case CKA_LABEL:
                /* Match by ID: we use the slot/zone number as a single-byte ID */
                if (pAttr->ulValueLen == 1U && pAttr->pValue != NULL) {
                    uint8_t req_id;
                    memcpy(&req_id, pAttr->pValue, 1U);
                    if (req_id != pObj->slot_or_zone) {
                        return CK_FALSE;
                    }
                }
                break;

            default:
                /* Unknown attributes in the template are ignored */
                break;
        }
    }

    return CK_TRUE;
}

/* ---------------------------------------------------------------------------
 * Object enumeration
 * --------------------------------------------------------------------------*/

CK_RV stse_pkcs11_enumerate_objects(
    stse_pkcs11_lib_t      *pLib,
    stse_pkcs11_find_ctx_t *pFind,
    CK_ATTRIBUTE_PTR        pTemplate,
    CK_ULONG                ulCount)
{
    uint8_t i;

    if (pLib == NULL || pFind == NULL) {
        return CKR_ARGUMENTS_BAD;
    }

    pFind->found_count = 0U;
    pFind->find_pos    = 0U;

    /* --- Private-key and public-key objects -------------------------------- */
    for (i = 0U; i < pLib->key_slot_count; i++) {
        const stsafea_private_key_slot_information_t *pSlot = &pLib->key_slot_info[i];
        stse_ecc_key_type_t kt = STSE_ECC_KT_INVALID;

        /* Resolve the curve from the curve_id stored in the slot info */
        if (pSlot->presence_flag == 1U) {
            (void)stse_get_ecc_key_type_from_curve_id(
                pSlot->curve_id.length[1],
                pSlot->curve_id.value,
                &kt);
        }

        /* Private-key object */
        {
            stse_pkcs11_object_t obj = {
                .obj_class    = CKO_PRIVATE_KEY,
                .handle       = STSE_PKCS11_PRIVKEY_HANDLE(i),
                .slot_or_zone = (uint8_t)i,
                .key_type     = kt,
                .present      = (pSlot->presence_flag == 1U) ? CK_TRUE : CK_FALSE
            };
            if (stse_pkcs11_object_matches_template(&obj, pTemplate, ulCount) == CK_TRUE) {
                if (pFind->found_count < STSE_PKCS11_MAX_OBJECTS) {
                    pFind->found[pFind->found_count++] = obj.handle;
                }
            }
        }

        /* Public-key object (always present — the public key is always accessible) */
        {
            stse_pkcs11_object_t obj = {
                .obj_class    = CKO_PUBLIC_KEY,
                .handle       = STSE_PKCS11_PUBKEY_HANDLE(i),
                .slot_or_zone = (uint8_t)i,
                .key_type     = kt,
                .present      = (pSlot->presence_flag == 1U) ? CK_TRUE : CK_FALSE
            };
            if (stse_pkcs11_object_matches_template(&obj, pTemplate, ulCount) == CK_TRUE) {
                if (pFind->found_count < STSE_PKCS11_MAX_OBJECTS) {
                    pFind->found[pFind->found_count++] = obj.handle;
                }
            }
        }
    }

    /* --- Certificate objects ----------------------------------------------- */
    for (i = 0U; i < pLib->config.cert_zone_count; i++) {
        stse_pkcs11_object_t obj = {
            .obj_class    = CKO_CERTIFICATE,
            .handle       = STSE_PKCS11_CERT_HANDLE(i),
            .slot_or_zone = pLib->config.cert_zone_indices[i],
            .key_type     = STSE_ECC_KT_INVALID,
            .present      = CK_TRUE
        };
        if (stse_pkcs11_object_matches_template(&obj, pTemplate, ulCount) == CK_TRUE) {
            if (pFind->found_count < STSE_PKCS11_MAX_OBJECTS) {
                pFind->found[pFind->found_count++] = obj.handle;
            }
        }
    }

    return CKR_OK;
}

/* ---------------------------------------------------------------------------
 * Format-conversion helpers
 * --------------------------------------------------------------------------*/

CK_ULONG stse_pkcs11_ec_point_size(uint16_t coord_size)
{
    /* DER OCTET STRING: 0x04 (tag) + 1 byte length + 0x04 (point prefix) + X + Y */
    return (CK_ULONG)(3U + 2U * (CK_ULONG)coord_size);
}

CK_ULONG stse_pkcs11_pubkey_to_ec_point(
    const uint8_t *pRaw_xy,
    uint16_t       coord_size,
    uint8_t       *pEc_point)
{
    CK_ULONG inner_len = 1U + 2U * (CK_ULONG)coord_size; /* 0x04 prefix + X + Y */
    CK_ULONG pos = 0U;

    if (pRaw_xy == NULL || pEc_point == NULL || coord_size == 0U) {
        return 0U;
    }

    /* DER OCTET STRING tag */
    pEc_point[pos++] = 0x04U;
    /* DER OCTET STRING length (single-byte: max inner = 1 + 2*66 = 133 which is >= 128 for P-521,
     * so we use definite long form only when needed) */
    if (inner_len < 0x80U) {
        pEc_point[pos++] = (uint8_t)inner_len;
    } else {
        pEc_point[pos++] = 0x81U;
        pEc_point[pos++] = (uint8_t)inner_len;
    }
    /* X9.62 uncompressed point prefix */
    pEc_point[pos++] = 0x04U;
    /* X coordinate */
    memcpy(&pEc_point[pos], pRaw_xy, coord_size);
    pos += coord_size;
    /* Y coordinate */
    memcpy(&pEc_point[pos], pRaw_xy + coord_size, coord_size);
    pos += coord_size;

    return pos;
}

CK_BBOOL stse_pkcs11_ec_point_to_xy(
    const uint8_t *pEc_point,
    CK_ULONG       point_len,
    uint16_t       coord_size,
    uint8_t       *pRaw_xy)
{
    const uint8_t *pInner = pEc_point;
    CK_ULONG       inner_offset = 0U;

    if (pEc_point == NULL || pRaw_xy == NULL || coord_size == 0U) {
        return CK_FALSE;
    }

    /* Strip outer DER OCTET STRING wrapper if present.
     * Recognise: 0x04 <length> 0x04 <X> <Y> */
    if (point_len >= 3U && pEc_point[0] == 0x04U) {
        CK_ULONG octet_len_field;

        if ((pEc_point[1] & 0x80U) != 0U) {
            /* Long form: one extra byte for the length */
            if (point_len < 4U) {
                return CK_FALSE;
            }
            octet_len_field = (CK_ULONG)pEc_point[2];
            inner_offset = 3U;
        } else {
            octet_len_field = (CK_ULONG)pEc_point[1];
            inner_offset = 2U;
        }

        /* If the inner content starts with 0x04 it is the X9.62 point prefix */
        if (inner_offset < point_len && pEc_point[inner_offset] == 0x04U) {
            /* Validate inner length */
            if (octet_len_field != 1U + 2U * (CK_ULONG)coord_size) {
                return CK_FALSE;
            }
            pInner = pEc_point + inner_offset + 1U; /* skip 0x04 point prefix */
        } else {
            /* No DER wrapper — first byte IS the 0x04 point prefix */
            pInner = pEc_point + 1U;
        }
    } else if (point_len >= 1U + 2U * (CK_ULONG)coord_size && pEc_point[0] == 0x04U) {
        /* Raw X9.62 uncompressed: 0x04 <X> <Y> */
        pInner = pEc_point + 1U;
    } else {
        return CK_FALSE;
    }

    /* Check we have enough bytes */
    if ((CK_ULONG)(pInner - pEc_point) + 2U * (CK_ULONG)coord_size > point_len) {
        return CK_FALSE;
    }

    memcpy(pRaw_xy, pInner, 2U * coord_size);
    return CK_TRUE;
}

CK_ULONG stse_pkcs11_get_ec_params(
    stse_ecc_key_type_t key_type,
    uint8_t            *pBuf,
    CK_ULONG            buf_size)
{
    const stse_ecc_info_t *pInfo;
    uint8_t                oid_len;
    CK_ULONG               total;

    if (pBuf == NULL || key_type >= STSE_ECC_KT_INVALID) {
        return 0U;
    }

    pInfo   = &stse_ecc_info_table[key_type];
    /* The curve_id.length field is a big-endian 2-byte value; the OID value
     * fits in ≤ 9 bytes, so the MSB is always 0x00 and the LSB is the length. */
    oid_len = pInfo->curve_id.length[1];
    total   = (CK_ULONG)(2U + oid_len); /* 0x06 tag + 1-byte length + OID bytes */

    if (total > buf_size) {
        return 0U;
    }

    pBuf[0] = 0x06U;                                /* DER OID tag */
    pBuf[1] = oid_len;                              /* OID length */
    memcpy(&pBuf[2], pInfo->curve_id.value, oid_len); /* OID value bytes */

    return total;
}

stse_ecc_key_type_t stse_pkcs11_get_key_type_for_slot(
    const stse_pkcs11_lib_t *pLib,
    uint8_t                  slot_number)
{
    uint8_t i;
    stse_ecc_key_type_t kt = STSE_ECC_KT_INVALID;

    if (pLib == NULL || slot_number >= pLib->key_slot_count) {
        return STSE_ECC_KT_INVALID;
    }

    for (i = 0U; i < pLib->key_slot_count; i++) {
        if (pLib->key_slot_info[i].slot_number == slot_number &&
            pLib->key_slot_info[i].presence_flag == 1U) {
            (void)stse_get_ecc_key_type_from_curve_id(
                pLib->key_slot_info[i].curve_id.length[1],
                pLib->key_slot_info[i].curve_id.value,
                &kt);
            return kt;
        }
    }

    return STSE_ECC_KT_INVALID;
}
