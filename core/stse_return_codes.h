/*!
 * ******************************************************************************
 * \file	stse_return_codes.h
 * \brief   STSE return code type definition
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

/*! \defgroup stse_return_codes Return Codes definition
 *  \ingroup stse_core
 *  @{
 */


/**
 * \enum stse_ReturnCode_t
 * \brief STSE response codes enumeration
 */
enum stse_ReturnCode_t {
	/*- Target device response code (MSB Mask 0x00xx)*/
	STSE_OK               			  	= 0x00, /*!< STSE Successful processing */
	STSE_COMMUNICATION_ERROR            = 0x01,	/*!< STSE Generic communication error */
	STSE_INCONSISTENT_COMMAND_DATA      = 0x02,	/*!< STSE Wrong command parameters */
	STSE_VALUE_OUT_OF_RANGE  			= 0x03,	/*!< STSE Value out of range */
	STSE_COMMAND_CODE_NOT_SUPPORTED     = 0x04,	/*!< STSE Command code not supported */
	STSE_UNEXPECTED_ERROR               = 0x05,	/*!< STSE Encouter an unexpected error */
	STSE_BUFFER_LENGTH_EXCEEDED         = 0x06,	/*!< STSE Buffer length exceeded */
	STSE_SESSION_ERROR            	    = 0x07,	/*!< STSE No or wrong session */
	STSE_TOO_MANY_MESSAGES_LOST         = 0x08,	/*!< STSE Too many masseage lost during communication */
	STSE_C_MAC_COUNTER_MAXIMUM_REACHED  = 0x09,	/*!< STSE C-MAC counter maximum reached */
	STSE_KEY_NOT_FOUND                  = 0x0A,	/*!< STSE Key not found in the corresponding slot */
	STSE_KEY_BLOCKED                    = 0x0B,	/*!< STSE Key locked or not available for this usage */
	STSE_WRONG_KEY_TYPE                 = 0x0C,	/*!< STSE Key type do not correspong to the command needs */
	STSE_SESSION_COUNTER_OUT_OF_RANGE   = 0x0D,	/*!< STSE Session counter out of range */
	STSE_INVALID_LIFE_CYCLE_STATE       = 0x0F,	/*!< STSE Invalid life cycle state */
	STSE_ENTRY_NOT_FOUND                = 0x10,	/*!< STSE Requested ressource not found */
	STSE_ACCESS_CONDITION_NOT_SATISFIED = 0x11,	/*!< STSE Access condition not satisfied */
	STSE_WRONG_ZONE_TYPE                = 0x12,	/*!< STSE Wrong zone type */
	STSE_COUNTER_LIMIT_EXCEEDED         = 0x13,	/*!< STSE Counter limit exceeded */
	STSE_BOUNDARY_EXCEEDED              = 0x14,	/*!< STSE Boundary exceeded */
	STSE_INVALID_PRIVATE_KEY            = 0x15,	/*!< STSE Invalid private key */
	STSE_INVALID_C_MAC                  = 0x16,	/*!< STSE Invalid C-MAC */
	STSE_CHALLENGE_MISSING              = 0x17,	/*!< STSE Challenge missing */
	STSE_COMMAND_DECRYPTION_ERROR       = 0x18,	/*!< STSE Error during command decryption */
	STSE_INVALID_PUBLIC_KEY             = 0x19,	/*!< STSE Invalid public key */
	STSE_PASSWORD_BLOCKED               = 0x1A,	/*!< STSE Password blocked */
	STSE_WRONG_KEY_USAGE                = 0x1B,	/*!< STSE Wrong key usage */
	STSE_WRONG_MODE_OF_OPERATION        = 0x1C,	/*!< STSE Wrong mode of operation */
	STSE_WRONG_MAC_LENGTH               = 0x1D,	/*!< STSE Wrong MAC length */
	STSE_COMMAND_NOT_AUTHORIZED         = 0x1E,	/*!< STSE Command not authorized */
	STSE_OLD_MESSAGE                    = 0x1F,	/*!< STSE Old message */

	/* - Host Platform response code (MSB Mask 0x01xx)*/
	STSE_PLATFORM_SERVICES_INIT_ERROR = 0x0101,		/*!< STSE Host platform service initialization error */
	STSE_PLATFORM_CRYPTO_INIT_ERROR,				/*!< STSE Host criptographic service initialization error */
	STSE_PLATFORM_INVALID_PARAMETER,				/*!< Error detected on the STSE HW Bus */
	STSE_PLATFORM_BUS_ERR,
	STSE_PLATFORM_BUFFER_ERR,						/*!< STSE Host platform Communication bus buffer overflow */
	STSE_PLATFORM_BUS_ARBITRATION_LOST,				/*!< STSE Host platform Communication bus Arbitration Lost */
	STSE_PLATFORM_BUS_RECEIVE_TIMEOUT,				/*!< STSE Host platform Communication bus Receive Timeout */
	STSE_PLATFORM_BUS_ACK_ERROR,					/*!< STSE Host platform Communication bus Ack Timout */
	STSE_PLATFORM_FLASH_WRITE_ERROR,				/*!< STSE Host platform Flash write issue */
	STSE_PLATFORM_POWER_ERROR,						/*!< STSE Host platform power init/on/off issue */
	STSE_PLATFORM_ECC_GENERATE_KEY_PAIR_ERROR,
	STSE_PLATFORM_ECC_SIGN_ERROR,
	STSE_PLATFORM_ECC_VERIFY_ERROR,
	STSE_PLATFORM_ECC_ECDH_ERROR,
	STSE_PLATFORM_AES_CMAC_COMPUTE_ERROR,
	STSE_PLATFORM_AES_CMAC_VERIFY_ERROR,
	STSE_PLATFORM_AES_CCM_ENCRYPT_ERROR,
	STSE_PLATFORM_AES_CCM_DECRYPT_ERROR,
	STSE_PLATFORM_AES_CBC_ENCRYPT_ERROR,
	STSE_PLATFORM_AES_CBC_DECRYPT_ERROR,
	STSE_PLATFORM_HASH_ERROR,
	STSE_PLATFORM_KEYWRAP_ERROR,

	/* - STSE Core layer response code (MSB Mask 0x02xx)*/
	STSE_CORE_INVALID_PARAMETER			= 0x0201,
	STSE_CORE_HANDLER_NOT_INITIALISED,
	STSE_CORE_FRAME_CRC_ERROR,            		 	/*!< STSE communication integrity error */
	STSE_CORE_FRAME_RMAC_ERROR,            		 	/*!< STSE communication authenticity/integrity error */
	STSE_CORE_SESSION_ERROR,

	/* - STSE Service layer response code (MSB Mask 0x03xx)*/
	STSE_SERVICE_INVALID_PARAMETER	  	= 0x0301,	/*!< STSE Wrong function parameters */
	STSE_SERVICE_HANDLER_NOT_INITIALISED,   		/*!< STSE is not initialized please run stsafe_init function first */
	STSE_SERVICE_SESSION_ERROR,
	STSE_SERVICE_BUFFER_OVERFLOW,

	/* - STSE API layer response code (MSB Mask 0x04xx)*/
	STSE_API_INVALID_PARAMETER			= 0x0401,
	STSE_API_HANDLER_NOT_INITIALISED,   			/*!< STSE is not initialized please run stsafe_init function first */
	STSE_API_SESSION_ERROR,
	STSE_API_KEY_NOT_FOUND,
	STSE_API_INVALID_SIGNATURE,

	/* - STSE Certificate layer response code (MSB Mask 0x05xx)*/
	STSE_CERT_INVALID_PARAMETER	  		= 0x0501,	/*!< STSE Wrong function parameters */
	STSE_CERT_HANDLER_NOT_INITIALISED,   			/*!< STSE is not initialized please run stsafe_init function first */
	STSE_CERT_UNEXPECTED_SEQUENCE,
	STSE_CERT_UNEXPECTED_BITSTRING,
	STSE_CERT_INVALID_SIGNATURE,
	STSE_CERT_INVALID_CERTIFICATE,
	STSE_CERT_CA_NOT_MATCHING,
	STSE_CERT_UNSUPPORTED_FEATURE,
};

typedef enum stse_ReturnCode_t stse_ReturnCode_t;

/** @}*/


#endif /* STSE_RETURN_CODES_H */
