/*!
 * ******************************************************************************
 * \file    stse_device.h
 * \brief   STSELib api for device (header)
 * \author  STMicroelectronics - CS application team
 *
 ******************************************************************************
 * \attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component. If no LICENSE file comes
 * with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef STSE_DEVICE_H
#define STSE_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "core/stse_generic_typedef.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"
#include "stse_conf.h"

/*! \defgroup stse_device Device management
 *  \ingroup stse_core
 *  @{
 */

typedef struct stse_handle_t stse_handle_t;

/*
 * \brief STSE Session_type type
 */
typedef PLAT_UI8 stse_session_type_t;
#define STSE_HOST_SESSION ((stse_session_type_t)0x00) /*!< Host session */
#define STSE_VOLATILE_KEK_SESSION                                              \
  ((stse_session_type_t)0x01) /*!< Volatile KEK session */

/*
 * \details STMicroelectronics Secure Element device type
 */
typedef PLAT_UI8 stse_device_t;
/* !!!! Shall be incremental */
#ifdef STSE_CONF_STSAFE_A_SUPPORT
#define STSAFEA_PRODUCT_COUNT 3U
#define STSAFE_A100 ((stse_device_t)0) /*!< STSAFE-A100 target device */
#define STSAFE_A110 ((stse_device_t)1) /*!< STSAFE-A110 target device */
#define STSAFE_A120 ((stse_device_t)2) /*!< STSAFE-A120 target device */
#endif                                 /* STSE_CONF_STSAFE_A_SUPPORT */
#ifdef STSE_CONF_STSAFE_L_SUPPORT
#define STSAFEL_PRODUCT_COUNT 1U
#define STSAFE_L010 ((stse_device_t)3) /*!< STSAFE-L010 target device */
#endif                                 /* STSE_CONF_STSAFE_L_SUPPORT */

typedef struct stse_perso_info_t {
  PLAT_UI32 cmd_encryption_status;
  PLAT_UI32 rsp_encryption_status;
  PLAT_UI32 ext_cmd_encryption_status;
  PLAT_UI32 ext_rsp_encryption_status;
  PLAT_UI64 cmd_ac_status;
  PLAT_UI64 ext_cmd_ac_status;
} PLAT_PACKED_STRUCT stse_perso_info_t;

/*
 * \details STSE Bus type
 */
typedef PLAT_UI8 stse_bus_t;
#if defined(STSE_CONF_STSAFE_A_SUPPORT) ||                                     \
    (defined(STSE_CONF_STSAFE_L_SUPPORT) && defined(STSE_CONF_USE_I2C))
#define STSE_BUS_TYPE_I2C ((stse_bus_t)0x00) /*!< I2C bus */
#endif /* STSE_CONF_STSAFE_A_SUPPORT || (STSE_CONF_STSAFE_L_SUPPORT &&         \
          STSE_CONF_USE_I2C) */
#ifdef STSE_CONF_USE_ST1WIRE
#define STSE_BUS_TYPE_ST1WIRE ((stse_bus_t)0x01) /*!< ST1Wire bus */
#endif                                           /* STSE_CONF_USE_ST1WIRE */

/*
 * \struct stse_io_t
 * \brief STSE Input/Output type
 */
typedef struct {
  stse_return_code_t (*bus_recv_start)(PLAT_UI8,  /* bus_id */
                                       PLAT_UI8,  /* dev_addr */
                                       PLAT_UI16, /* speed */
                                       PLAT_UI16  /* p_frame_length */
  ); /*\var stse_io_t::bus_recv_start Bus Receive start function callback */
  stse_return_code_t (*bus_recv_continue)(PLAT_UI8,   /*bus_id*/
                                          PLAT_UI8,   /*dev_addr*/
                                          PLAT_UI16,  /*speed*/
                                          PLAT_UI8 *, /*p_element*/
                                          PLAT_UI16   /*element_size*/
  ); /*<\var stse_io_t::bus_recv_continue Bus Receive continue function callback
      */
  stse_return_code_t (*bus_recv_stop)(PLAT_UI8,   /*bus_id*/
                                      PLAT_UI8,   /*dev_addr*/
                                      PLAT_UI16,  /*speed*/
                                      PLAT_UI8 *, /*p_element*/
                                      PLAT_UI16   /*element_size*/
  ); /*<\var stse_io_t::bus_recv_stop Bus Receive stop function callback */
  stse_return_code_t (*bus_send_start)(PLAT_UI8,  /* bus_id */
                                       PLAT_UI8,  /* dev_addr */
                                       PLAT_UI16, /* speed */
                                       PLAT_UI16  /* frame_length */
  ); /*<\var stse_io_t::bus_send_start Bus Send start function callback */
  stse_return_code_t (*bus_send_continue)(PLAT_UI8,   /*bus_id*/
                                          PLAT_UI8,   /*dev_addr*/
                                          PLAT_UI16,  /*speed*/
                                          PLAT_UI8 *, /*p_element*/
                                          PLAT_UI16   /*element_size*/
  ); /*<\var stse_io_t::bus_send_continue Bus Send continue function callback */
  stse_return_code_t (*bus_send_stop)(PLAT_UI8,   /*bus_id*/
                                      PLAT_UI8,   /*dev_addr*/
                                      PLAT_UI16,  /*speed*/
                                      PLAT_UI8 *, /*p_element*/
                                      PLAT_UI16   /*element_size*/
  ); /*<\var stse_io_t::bus_send_stop Bus Send stop function callback */
  stse_return_code_t (*io_line_get)(
      PLAT_UI8); /*<\var stse_io_t::io_line_get Get Bus I/O Line state function
                    callback */
  stse_return_code_t (*bus_wake)(
      PLAT_UI8, PLAT_UI8,
      PLAT_UI16); /*<\var stse_io_t::bus_wake Bus wake function callback */
  stse_return_code_t (*bus_recovery)(
      PLAT_UI8, PLAT_UI8); /*<\var stse_io_t::bus_recovery Bus recovery function
                              callback */
  stse_return_code_t (*power_line_off)(
      PLAT_UI8, PLAT_UI8); /*<\var stse_io_t::power_line_off Bus power line off
                              function callback */
  stse_return_code_t (*power_line_on)(
      PLAT_UI8, PLAT_UI8); /*<\var stse_io_t::power_line_on Bus power line on
                              function callback */
  PLAT_UI8 bus_id;         /*<\var stse_io_t::bus_id Bus ID */
  PLAT_UI8 devaddr;        /*<\var stse_io_t::devaddr Device address */
  PLAT_UI16 bus_speed;     /*<\var stse_io_t::bus_speed Bus speed */
  stse_bus_t bus_type;     /*<\var stse_io_t::bus_type Bus type */
} PLAT_PACKED_STRUCT stse_io_t;

typedef struct stse_session_t stse_session_t;

/*
 * \brief STSE Session type
 */
struct stse_session_t {
  stse_session_type_t type;
  union {
    struct {
      stse_handle_t *p_stse;
      PLAT_UI32 host_mac_key_idx;
      PLAT_UI32 host_cypher_key_idx;
      stse_aes_key_type_t key_type;
      PLAT_UI32 mac_counter;
    } host;

    struct {
      PLAT_UI8 base_key[STSE_AES_256_KEY_SIZE];
      PLAT_UI8 working_kek_counter;
    } kek;
  } context;
} PLAT_PACKED_STRUCT;

/*!
 * \typedef stse_handle_t
 * \brief STSE Handler
 *        This handler stores all the context and working data related to a
 * specific STSE target. \n Pointer to a specific stsafe_handler is the main
 * parameters of all STSE middleware API functions. \n A specific STSE target
 * Handler must be initialized using the "stsafe_init" API function
 */
struct stse_handle_t {
  stse_device_t device_type;
  stse_perso_info_t perso_info;
  stse_session_t *p_active_host_session;
  stse_session_t *p_active_other_session;
  stse_io_t io;
} PLAT_PACKED_STRUCT;

/* Exported variables --------------------------------------------------------*/

/**
 * \brief       Initialise the STSE handler to default value
 * \details     This core function initialise the handler to default value
 * \param[in]   p_stse_handle : Pointer to STSE handler
 * \return \ref stse_return_code_t : STSE_OK on success ; error code otherwise
 */
stse_return_code_t stse_set_default_handler_value(stse_handle_t *p_stse_handle);

/*! @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STSE_DEVICE_H */
