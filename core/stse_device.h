/*!
 * ******************************************************************************
 * \file	stse_device.h
 * \brief   STSE device (header)
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

#ifndef STSE_DEVICE_H
#define STSE_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stse_conf.h"
#include "core/stse_util.h"
#include "core/stse_return_codes.h"
#include "core/stse_generic_typedef.h"


/*! \defgroup stse_device Device management
 *  \ingroup stse_core
 *  @{
 */

typedef struct stse_Handler_t stse_Handler_t;

/*
 * \brief STSE Session_type type
 */
typedef enum
{
	STSE_HOST_SESSION,				/*!< Host session */
	STSE_VOLATILE_KEK_SESSION		/*!< Volatile KEK session */
}stse_session_type_t;


/*
 * \details STMicroelectronics Secure Element device type
 */
typedef enum stse_device_t {
	STSAFE_A100 = 0,		/*!< STSAFE-A100 target device */
	STSAFE_A110,			/*!< STSAFE-A110 target device */
	STSAFE_A120,			/*!< STSAFE-A120 target device */
	STSAFE_A200				/*!< STSAFE-A200 target device */
}stse_device_t;


typedef struct stse_perso_info_t {
	PLAT_UI32 cmd_encryption_status;
	PLAT_UI32 rsp_encryption_status;
	PLAT_UI32 ext_cmd_encryption_status;
	PLAT_UI32 ext_rsp_encryption_status;
	PLAT_UI64 cmd_AC_status;
	PLAT_UI32 ext_cmd_AC_status;
} PLAT_PACKED_STRUCT stse_perso_info_t;


/*
 * \details STSE Bus type
 */
typedef enum stse_bus {
	STSE_BUS_TYPE_I2C		/*!< I2C bus */
}stse_bus_t;

/*
 * \struct stse_io_t
 * \brief STSE Input/Output type
 */
typedef struct
	{
		stse_ReturnCode_t (* BusRecvStart) (
				PLAT_UI8,	/* busID */
				PLAT_UI8,	/* devAddr */
				PLAT_UI16,	/* speed */
				PLAT_UI16 *	/* pFrameLength */
		); /*\var stse_io_t::BusRecvStart Bus Receive start function callback */
		stse_ReturnCode_t (* BusRecvContinue) (
				PLAT_UI8,	/*busID*/
				PLAT_UI8,	/*devAddr*/
				PLAT_UI16,	/*speed*/
				PLAT_UI8 *,	/*pElement*/
				PLAT_UI16	/*element_size*/
		); /*<\var stse_io_t::BusRecvContinue Bus Receive continue function callback */
		stse_ReturnCode_t (* BusRecvStop) (
				PLAT_UI8,	/*busID*/
				PLAT_UI8,	/*devAddr*/
				PLAT_UI16,	/*speed*/
				PLAT_UI8 *,	/*pElement*/
				PLAT_UI16	/*element_size*/
		); /*<\var stse_io_t::BusRecvStop Bus Receive stop function callback */
		stse_ReturnCode_t (* BusSendStart)(
				PLAT_UI8,	/* busID */
				PLAT_UI8,	/* devAddr */
				PLAT_UI16,	/* speed */
				PLAT_UI16	/* FrameLength */
		); /*<\var stse_io_t::BusSendStart Bus Send start function callback */
		stse_ReturnCode_t (* BusSendContinue)(
				PLAT_UI8,	/*busID*/
				PLAT_UI8,	/*devAddr*/
				PLAT_UI16,	/*speed*/
				PLAT_UI8 *,	/*pElement*/
				PLAT_UI16	/*element_size*/
		); /*<\var stse_io_t::BusSendContinue Bus Send continue function callback */
		stse_ReturnCode_t (* BusSendStop)(
				PLAT_UI8,	/*busID*/
				PLAT_UI8,	/*devAddr*/
				PLAT_UI16,	/*speed*/
				PLAT_UI8 *,	/*pElement*/
				PLAT_UI16	/*element_size*/
		); /*<\var stse_io_t::BusSendStop Bus Send stop function callback */
		stse_ReturnCode_t (* IOLineGet)(
				PLAT_UI8
		); /*<\var stse_io_t::IOLineGet Get Bus I/O Line state function callback */
		stse_ReturnCode_t (* BusWake)(
				PLAT_UI8,
				PLAT_UI8,
				PLAT_UI16
		); /*<\var stse_io_t::BusWake Bus wake function callback */
		stse_ReturnCode_t (* BusRecovery)(
				PLAT_UI8,
				PLAT_UI8
		); /*<\var stse_io_t::BusRecovery Bus recovery function callback */
		stse_ReturnCode_t (* PowerLineOff)(
				PLAT_UI8,
				PLAT_UI8
		); /*<\var stse_io_t::PowerLineOff Bus power line off function callback */
		stse_ReturnCode_t (* PowerLineOn)(
				PLAT_UI8,
				PLAT_UI8
		); /*<\var stse_io_t::PowerLineOn Bus power line on function callback */
		PLAT_UI8 Busaddr;				/*<\var stse_io_t::Busaddr Bus Address */
		PLAT_UI8 Devaddr;				/*<\var stse_io_t::Devaddr Device address */
		PLAT_UI16 BusSpeed;				/*<\var stse_io_t::BusSpeed Bus speed */
		stse_bus_t BusType;				/*<\var stse_io_t::BusType Bus type */
	} PLAT_PACKED_STRUCT stse_io_t;


	typedef struct stse_session_t stse_session_t;

	/*
	 * \brief STSE Session type
	 */
	struct stse_session_t
	{
		stse_session_type_t type;
		union {
			struct {
				stse_Handler_t* pSTSE;
				PLAT_UI8* pHost_MAC_key;
				PLAT_UI8* pHost_cypher_key;
				stse_aes_key_type_t key_type;
				PLAT_UI32  MAC_counter;
			} host;

			struct {
				PLAT_UI8 base_key[STSE_AES_256_KEY_SIZE];
				PLAT_UI8 working_kek_counter;
			} kek;
		}context;
	} PLAT_PACKED_STRUCT;


/*!
 * \typedef stse_Handler_t
 * \brief STSE Handler
 * 		  This handler stores all the context and working data related to a specific STSE target. \n
 * 		  Pointer to a specific stsafe_Handler is the main parameters of all STSE middleware API functions. \n
 * 		  A specific STSE target Handler must be initialized using the "stsafe_init" API function
 */
struct stse_Handler_t {

	stse_device_t device_type;
	stse_perso_info_t *pPerso_info;
	stse_session_t *pActive_host_session;
	stse_session_t *pActive_other_session;
	stse_io_t io;

}PLAT_PACKED_STRUCT;

/* Exported variables --------------------------------------------------------*/

extern stse_perso_info_t dynamic_product_perso;

/**
 * \brief 		Initialise the STSE handler to default value
 * \details 		This core function initialise the handler to default value
 * \param[in] 		pStseHandler 	Pointer to STSE handler
 * \return \ref stsafe_ReturnCode_t : STSAFE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_set_default_handler_value(stse_Handler_t *pStseHandler);

/*! @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_DEVICE_H */
