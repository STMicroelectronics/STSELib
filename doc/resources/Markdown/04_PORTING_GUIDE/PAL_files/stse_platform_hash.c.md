# stse_platform_hash.c{#stse_platform_hash}

The `stse_platform_hash.c` file provides hash and HMAC functions for the STSecureElement library, abstracting the platform-specific details of hash and HMAC operations.

## stse_platform_hash_compute:

- **Purpose**: Computes the hash of the given payload.
- **Parameters**:
  - `hash_algo`: Hash algorithm to use.
  - `p_payload`: Pointer to the payload.
  - `payload_length`: Length of the payload.
  - `p_hash`: Pointer to the output hash.
  - `hash_length`: Pointer to the length of the output hash.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_HASH_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that performs the hash computation.

## stse_platform_hmac_sha256_extract:

- **Purpose**: Extracts a pseudorandom key using HMAC-SHA256.
- **Parameters**:
  - `p_salt`: Pointer to the salt.
  - `salt_length`: Length of the salt.
  - `p_input_keying_material`: Pointer to the input keying material.
  - `input_keying_material_length`: Length of the input keying material.
  - `p_pseudorandom_key`: Pointer to the output pseudorandom key.
  - `pseudorandom_key_expected_length`: Expected length of the pseudorandom key.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_HKDF_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that performs the HMAC-SHA256 extraction.

## stse_platform_hmac_sha256_expand:

- **Purpose**: Expands a pseudorandom key using HMAC-SHA256.
- **Parameters**:
  - `p_pseudorandom_key`: Pointer to the pseudorandom key.
  - `pseudorandom_key_length`: Length of the pseudorandom key.
  - `p_info`: Pointer to the info.
  - `info_length`: Length of the info.
  - `p_output_keying_material`: Pointer to the output keying material.
  - `output_keying_material_length`: Length of the output keying material.
- **Return Value**: Returns `STSE_OK` on success, `STSE_PLATFORM_HKDF_ERROR` on failure.

**Implementation directives**: This abstraction function should implement or call a platform function/driver that performs the HMAC-SHA256 expansion.

## implementation example :

Please find below an example of the stse_platform_hash.c implemntation for the STM32 CMOX library :

```c
/******************************************************************************
 * \file    stse_platform_hash.c
 * \brief   STSecureElement HASH platform file
 * \author  STMicroelectronics - CS application team
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */

#include "stse_conf.h"
#include "stselib.h"
#include "Middleware/STM32_Cryptographic/include/cmox_crypto.h"

static cmox_hash_algo_t stse_platform_get_cmox_hash_algo(stse_hash_algorithm_t hash_algo)
{
    switch(hash_algo)
    {
#ifdef STSE_CONF_HASH_SHA_1
        case STSE_SHA_1:
            return CMOX_SHA1_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_224
        case STSE_SHA_224:
            return CMOX_SHA224_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_256
        case STSE_SHA_256:
            return CMOX_SHA256_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_384
        case STSE_SHA_384:
            return CMOX_SHA384_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_512
        case STSE_SHA_512:
            return CMOX_SHA512_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_3_256
        case STSE_SHA3_256:
            return CMOX_SHA3_256_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_3_384
        case STSE_SHA3_384:
            return CMOX_SHA3_384_ALGO;
#endif
#ifdef STSE_CONF_HASH_SHA_3_512
        case STSE_SHA3_512:
            return CMOX_SHA3_512_ALGO;
#endif
        default:
            return NULL;
    }
}

/**
 * \brief   Computes the hash of the given payload.
 * \param   hash_algo    Hash algorithm to use.
 * \param   p_payload     Pointer to the payload.
 * \param   payload_length Length of the payload.
 * \param   p_hash        Pointer to the output hash.
 * \param   hash_length  Pointer to the length of the output hash.
 * \return  STSE_OK on success, STSE_PLATFORM_HASH_ERROR on failure.
 */
stse_return_code_t stse_platform_hash_compute(stse_hash_algorithm_t hash_algo,
                                      PLAT_UI8 *p_payload, PLAT_UI32 payload_length,
                                      PLAT_UI8 *p_hash, PLAT_UI32 *hash_length)
{
    cmox_hash_retval_t retval;

    retval = cmox_hash_compute(
            stse_platform_get_cmox_hash_algo(hash_algo),
            p_payload,
            payload_length,
            p_hash,
            *hash_length,
            (size_t*)hash_length);

    /*- Verify Hash compute return */
    if (retval != CMOX_HASH_SUCCESS)
    {
        return STSE_PLATFORM_HASH_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Extracts a pseudorandom key using HMAC-SHA256.
 * \param   p_salt        Pointer to the salt.
 * \param   salt_length  Length of the salt.
 * \param   p_input_keying_material Pointer to the input keying material.
 * \param   input_keying_material_length Length of the input keying material.
 * \param   p_pseudorandom_key Pointer to the output pseudorandom key.
 * \param   pseudorandom_key_expected_length Expected length of the pseudorandom key.
 * \return  STSE_OK on success, STSE_PLATFORM_HKDF_ERROR on failure.
 */
stse_return_code_t stse_platform_hmac_sha256_extract(PLAT_UI8 *p_salt, PLAT_UI16 salt_length,
                                             PLAT_UI8 *p_input_keying_material, PLAT_UI16 input_keying_material_length,
                                             PLAT_UI8 *p_pseudorandom_key, PLAT_UI16 pseudorandom_key_expected_length)
{
    cmox_mac_retval_t retval;

    PLAT_UI16 pseudorandom_key_length = pseudorandom_key_expected_length;

    retval = cmox_mac_compute(CMOX_HMAC_SHA256_ALGO,
                           p_input_keying_material,
                           input_keying_material_length,
                           p_salt,
                           salt_length,
                           NULL,
                           0,
                           p_pseudorandom_key,
                           pseudorandom_key_expected_length,
                           (size_t*)&pseudorandom_key_length);

    /*- Verify MAC compute return */
    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_HKDF_ERROR;
    }

    return STSE_OK;
}

/**
 * \brief   Expands a pseudorandom key using HMAC-SHA256.
 * \param   p_pseudorandom_key Pointer to the pseudorandom key.
 * \param   pseudorandom_key_length Length of the pseudorandom key.
 * \param   p_info        Pointer to the info.
 * \param   info_length  Length of the info.
 * \param   p_output_keying_material Pointer to the output keying material.
 * \param   output_keying_material_length Length of the output keying material.
 * \return  STSE_OK on success, STSE_PLATFORM_HKDF_ERROR on failure.
 */
stse_return_code_t stse_platform_hmac_sha256_expand(PLAT_UI8  *p_pseudorandom_key, PLAT_UI16 pseudorandom_key_length,
                                            PLAT_UI8  *p_info, PLAT_UI16 info_length,
                                            PLAT_UI8  *p_output_keying_material, PLAT_UI16 output_keying_material_length)
{
    cmox_mac_retval_t retval;

    PLAT_UI8    tmp[CMOX_SHA256_SIZE];
    PLAT_UI16   tmp_length = 0;
    PLAT_UI16   out_index = 0;
    PLAT_UI8    n = 0x1;

    cmox_mac_handle_t *p_mac_handle;
    cmox_hmac_handle_t hmac_handle;

    /*  RFC 5869 : output keying material must be
     *      - L <= 255*HashLen
     *      - N = ceil(L/HashLen) */
    if ( p_output_keying_material == NULL
      || ((output_keying_material_length/CMOX_SHA256_SIZE) + ((output_keying_material_length % CMOX_SHA256_SIZE) != 0)) > 255)
    {
        return STSE_PLATFORM_HKDF_ERROR;
    }

    p_mac_handle = cmox_hmac_construct(&hmac_handle, CMOX_HMAC_SHA256);
    retval = cmox_mac_init(p_mac_handle);

    if (retval != CMOX_MAC_SUCCESS)
    {
        return STSE_PLATFORM_HKDF_ERROR;
    }

    while (out_index < output_keying_material_length)
    {
        PLAT_UI16 left = output_keying_material_length - out_index;

        retval = cmox_mac_set_key(p_mac_handle, p_pseudorandom_key, pseudorandom_key_length);
        if (retval != CMOX_MAC_SUCCESS)
            break;
        retval = cmox_mac_append(p_mac_handle, tmp, tmp_length);
        if (retval != CMOX_MAC_SUCCESS)
            break;
        retval = cmox_mac_append(p_mac_handle, p_info, info_length);
        if (retval != CMOX_MAC_SUCCESS)
            break;
        retval = cmox_mac_append(p_mac_handle, &n, 1);
        if (retval != CMOX_MAC_SUCCESS)
            break;
        retval = cmox_mac_generate_tag(p_mac_handle, tmp, NULL);
        if (retval != CMOX_MAC_SUCCESS)
            break;

        left = left < CMOX_SHA256_SIZE ? left : CMOX_SHA256_SIZE;
        memcpy(p_output_keying_material+out_index, tmp, left);

        tmp_length = CMOX_SHA256_SIZE;
        out_index += CMOX_SHA256_SIZE;
        n++;
    }

    cmox_mac_cleanup(p_mac_handle);

    /*- Verify MAC compute return */
    if (retval != CMOX_MAC_SUCCESS)
    {
        memset(tmp, 0, CMOX_SHA256_SIZE);
        memset(p_output_keying_material, 0, output_keying_material_length);
        return STSE_PLATFORM_HKDF_ERROR;
    }

    return STSE_OK;
}
```
