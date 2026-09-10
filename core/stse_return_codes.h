/*!
 * ******************************************************************************
 * \file	stse_return_codes.h
 * \brief   STSELib return code type definition
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_RETURN_CODES_H
#define STSE_RETURN_CODES_H

#include "stse_platform_generic.h"

/*! \defgroup stse_return_codes Return Codes definition
 *  \ingroup stse_core
 *  @{
 */

/**
 * \typedef stse_ReturnCode_t
 * \brief STSE response codes
 */
typedef PLAT_UI16 stse_ReturnCode_t;

/* - STSE response code (MSB Mask 0x00xx)*/
#define STSE_OK ((stse_ReturnCode_t)0x00)                             /*!< STSE Successful processing */
#define STSE_COMMUNICATION_ERROR ((stse_ReturnCode_t)0x01)            /*!< STSE Generic communication error */
#define STSE_INCONSISTENT_COMMAND_DATA ((stse_ReturnCode_t)0x02)      /*!< STSE Wrong command parameters */
#define STSE_VALUE_OUT_OF_RANGE ((stse_ReturnCode_t)0x03)             /*!< STSE Value out of range */
#define STSE_COMMAND_CODE_NOT_SUPPORTED ((stse_ReturnCode_t)0x04)     /*!< STSE Command code not supported */
#define STSE_UNEXPECTED_ERROR ((stse_ReturnCode_t)0x05)               /*!< STSE Encouter an unexpected error */
#define STSE_BUFFER_LENGTH_EXCEEDED ((stse_ReturnCode_t)0x06)         /*!< STSE Buffer length exceeded */
#define STSE_SESSION_ERROR ((stse_ReturnCode_t)0x07)                  /*!< STSE No or wrong session */
#define STSE_TOO_MANY_MESSAGES_LOST ((stse_ReturnCode_t)0x08)         /*!< STSE Too many masseage lost during communication */
#define STSE_C_MAC_COUNTER_MAXIMUM_REACHED ((stse_ReturnCode_t)0x09)  /*!< STSE C-MAC counter maximum reached */
#define STSE_KEY_NOT_FOUND ((stse_ReturnCode_t)0x0A)                  /*!< STSE Key not found in the corresponding slot */
#define STSE_KEY_BLOCKED ((stse_ReturnCode_t)0x0B)                    /*!< STSE Key locked or not available for this usage */
#define STSE_WRONG_KEY_TYPE ((stse_ReturnCode_t)0x0C)                 /*!< STSE Key type do not correspong to the command needs */
#define STSE_SESSION_COUNTER_OUT_OF_RANGE ((stse_ReturnCode_t)0x0D)   /*!< STSE Session counter out of range */
#define STSE_INVALID_LIFE_CYCLE_STATE ((stse_ReturnCode_t)0x0F)       /*!< STSE Invalid life cycle state */
#define STSE_ENTRY_NOT_FOUND ((stse_ReturnCode_t)0x10)                /*!< STSE Requested ressource not found */
#define STSE_ACCESS_CONDITION_NOT_SATISFIED ((stse_ReturnCode_t)0x11) /*!< STSE Access condition not satisfied */
#define STSE_WRONG_ZONE_TYPE ((stse_ReturnCode_t)0x12)                /*!< STSE Wrong zone type */
#define STSE_COUNTER_LIMIT_EXCEEDED ((stse_ReturnCode_t)0x13)         /*!< STSE Counter limit exceeded */
#define STSE_BOUNDARY_EXCEEDED ((stse_ReturnCode_t)0x14)              /*!< STSE Boundary exceeded */
#define STSE_INVALID_PRIVATE_KEY ((stse_ReturnCode_t)0x15)            /*!< STSE Invalid private key */
#define STSE_INVALID_C_MAC ((stse_ReturnCode_t)0x16)                  /*!< STSE Invalid C-MAC */
#define STSE_CHALLENGE_MISSING ((stse_ReturnCode_t)0x17)              /*!< STSE Challenge missing */
#define STSE_COMMAND_DECRYPTION_ERROR ((stse_ReturnCode_t)0x18)       /*!< STSE Error during command decryption */
#define STSE_INVALID_PUBLIC_KEY ((stse_ReturnCode_t)0x19)             /*!< STSE Invalid public key */
#define STSE_PASSWORD_BLOCKED ((stse_ReturnCode_t)0x1A)               /*!< STSE Password blocked */
#define STSE_WRONG_KEY_USAGE ((stse_ReturnCode_t)0x1B)                /*!< STSE Wrong key usage */
#define STSE_WRONG_MODE_OF_OPERATION ((stse_ReturnCode_t)0x1C)        /*!< STSE Wrong mode of operation */
#define STSE_WRONG_MAC_LENGTH ((stse_ReturnCode_t)0x1D)               /*!< STSE Wrong MAC length */
#define STSE_COMMAND_NOT_AUTHORIZED ((stse_ReturnCode_t)0x1E)         /*!< STSE Command not authorized */
#define STSE_OLD_MESSAGE ((stse_ReturnCode_t)0x1F)                    /*!< STSE Old message */

/* - Host Platform response code (MSB Mask 0x01xx)*/
#define STSE_PLATFORM_SERVICES_INIT_ERROR ((stse_ReturnCode_t)0x0101) /*!< STSE Host platform service initialization error */
#define STSE_PLATFORM_CRYPTO_INIT_ERROR ((stse_ReturnCode_t)0x0102)   /*!< STSE Host cryptographic service initialization error */
#define STSE_PLATFORM_INVALID_PARAMETER ((stse_ReturnCode_t)0x0103)   /*!< Error detected on the STSE HW Bus */
#define STSE_PLATFORM_BUS_ERR ((stse_ReturnCode_t)0x0104)
#define STSE_PLATFORM_BUFFER_ERR ((stse_ReturnCode_t)0x0105)           /*!< STSE Host platform Communication bus buffer overflow */
#define STSE_PLATFORM_BUS_ARBITRATION_LOST ((stse_ReturnCode_t)0x0106) /*!< STSE Host platform Communication bus Arbitration Lost */
#define STSE_PLATFORM_BUS_RECEIVE_TIMEOUT ((stse_ReturnCode_t)0x0107)  /*!< STSE Host platform Communication bus Receive Timeout */
#define STSE_PLATFORM_BUS_ACK_ERROR ((stse_ReturnCode_t)0x0108)        /*!< STSE Host platform Communication bus Ack Timeout */
#define STSE_PLATFORM_FLASH_WRITE_ERROR ((stse_ReturnCode_t)0x0109)    /*!< STSE Host platform Flash write issue */
#define STSE_PLATFORM_POWER_ERROR ((stse_ReturnCode_t)0x010A)          /*!< STSE Host platform power init/on/off issue */
#define STSE_PLATFORM_ECC_GENERATE_KEY_PAIR_ERROR ((stse_ReturnCode_t)0x010B)
#define STSE_PLATFORM_ECC_SIGN_ERROR ((stse_ReturnCode_t)0x010C)
#define STSE_PLATFORM_ECC_VERIFY_ERROR ((stse_ReturnCode_t)0x010D)
#define STSE_PLATFORM_ECC_ECDH_ERROR ((stse_ReturnCode_t)0x010E)
#define STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR ((stse_ReturnCode_t)0x010F)
#define STSE_PLATFORM_AES_CMAC_VERIFY_ERROR ((stse_ReturnCode_t)0x0110)
#define STSE_PLATFORM_AES_CCM_ENCRYPT_ERROR ((stse_ReturnCode_t)0x0111)
#define STSE_PLATFORM_AES_CCM_DECRYPT_ERROR ((stse_ReturnCode_t)0x0112)
#define STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR ((stse_ReturnCode_t)0x0113)
#define STSE_PLATFORM_AES_CBC_DECRYPT_ERROR ((stse_ReturnCode_t)0x0114)
#define STSE_PLATFORM_AES_ECB_ENCRYPT_ERROR ((stse_ReturnCode_t)0x0115)
#define STSE_PLATFORM_AES_ECB_DECRYPT_ERROR ((stse_ReturnCode_t)0x0116)
#define STSE_PLATFORM_HASH_ERROR ((stse_ReturnCode_t)0x0117)
#define STSE_PLATFORM_KEYWRAP_ERROR ((stse_ReturnCode_t)0x0118)
#define STSE_PLATFORM_HKDF_ERROR ((stse_ReturnCode_t)0x0119)
#define STSE_PLATFORM_UNEXPECTED_ERROR ((stse_ReturnCode_t)0x011A)

/* - STSE Core layer response code (MSB Mask 0x02xx)*/
#define STSE_CORE_INVALID_PARAMETER ((stse_ReturnCode_t)0x0201)
#define STSE_CORE_HANDLER_NOT_INITIALISED ((stse_ReturnCode_t)0x0202)
#define STSE_CORE_FRAME_RMAC_ERROR ((stse_ReturnCode_t)0x0203) /*!< STSE communication authenticity/integrity error */
#define STSE_CORE_SESSION_ERROR ((stse_ReturnCode_t)0x0204)
#define STSE_CORE_INCONSISTENT_FRAME ((stse_ReturnCode_t)0x0205)

/* - STSE Service layer response code (MSB Mask 0x03xx)*/
#define STSE_SERVICE_INVALID_PARAMETER ((stse_ReturnCode_t)0x0301)       /*!< STSE Wrong function parameters */
#define STSE_SERVICE_HANDLER_NOT_INITIALISED ((stse_ReturnCode_t)0x0301) /*!< STSE is not initialized please run stsafe_init function first */
#define STSE_SERVICE_FRAME_CRC_ERROR ((stse_ReturnCode_t)0x0302)         /*!< STSE communication integrity error */
#define STSE_SERVICE_SESSION_ERROR ((stse_ReturnCode_t)0x0303)
#define STSE_SERVICE_FRAME_SIZE_ERROR ((stse_ReturnCode_t)0x0304)
#define STSE_SERVICE_INVALID_FRAME ((stse_ReturnCode_t)0x0305)
#define STSE_SERVICE_INCOMPATIBLE_DEVICE_TYPE ((stse_ReturnCode_t)0x0306)

/* - STSE API layer response code (MSB Mask 0x04xx)*/
#define STSE_API_INVALID_PARAMETER ((stse_ReturnCode_t)0x0401)
#define STSE_API_HANDLER_NOT_INITIALISED ((stse_ReturnCode_t)0x0402) /*!< STSE is not initialized please run stsafe_init function first */
#define STSE_API_SESSION_ERROR ((stse_ReturnCode_t)0x0403)
#define STSE_API_KEY_NOT_FOUND ((stse_ReturnCode_t)0x0404)
#define STSE_API_INVALID_SIGNATURE ((stse_ReturnCode_t)0x0405)
#define STSE_API_INCOMPATIBLE_DEVICE_TYPE ((stse_ReturnCode_t)0x0406)

/* - STSE Certificate layer response code (MSB Mask 0x05xx)*/
#define STSE_CERT_INVALID_PARAMETER ((stse_ReturnCode_t)0x0501)       /*!< STSE Wrong function parameters */
#define STSE_CERT_HANDLER_NOT_INITIALISED ((stse_ReturnCode_t)0x0502) /*!< STSE is not initialized please run stsafe_init function first */
#define STSE_CERT_UNEXPECTED_SEQUENCE ((stse_ReturnCode_t)0x0503)
#define STSE_CERT_UNEXPECTED_BITSTRING ((stse_ReturnCode_t)0x0504)
#define STSE_CERT_INVALID_SIGNATURE ((stse_ReturnCode_t)0x0505)
#define STSE_CERT_INVALID_CERTIFICATE ((stse_ReturnCode_t)0x0506)
#define STSE_CERT_CA_NOT_MATCHING ((stse_ReturnCode_t)0x0507)
#define STSE_CERT_UNSUPPORTED_FEATURE ((stse_ReturnCode_t)0x0508)

/** @}*/

#endif /* STSE_RETURN_CODES_H */
