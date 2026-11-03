/**
  ******************************************************************************
  * @file    stse_frame.h
  * @author  CS Application Team
  * @brief   STSELib api for frame layer (header)
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

/*! \defgroup stse_frame Frame Management
 *  \ingroup stse_core
 *  @{
 */

#ifndef STSAFE_FRAME_H
#define STSAFE_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_return_codes.h"
#include "core/stse_util.h"

#define STSE_FRAME_CRC_SIZE 2U
#define STSE_FRAME_LENGTH_SIZE 2U
#define STSE_RSP_FRAME_HEADER_SIZE 1U
#define STSE_STSAFEA_RSP_STATUS_MASK 0x1F
#define STSE_STSAFEL_RSP_STATUS_MASK 0x0F

typedef struct stse_frame_t stse_frame_t;
typedef struct stse_frame_element_t stse_frame_element_t;

struct stse_frame_t
{
  PLAT_UI8 element_count;
  PLAT_UI16 length;
  stse_frame_element_t *first_element;
  stse_frame_element_t *last_element;
};

struct stse_frame_element_t
{
  PLAT_UI16 length;
  PLAT_UI8 *p_data;
  stse_frame_element_t *next;
};

typedef PLAT_UI8 stse_frame_encrypt_flag_t;
#define STSAFE_FRAME_PLAINTEXT ((stse_frame_encrypt_flag_t)0)
#define STSAFE_FRAME_ENCRYPT ((stse_frame_encrypt_flag_t)1)

#define STSE_FRAME_ALLOCATE(frame) stse_frame_t frame = {0};

#define STSE_FRAME_ELEMENT_ALLOCATE(element, length, p_data) \
  stse_frame_element_t element = {length, p_data, NULL};

#define STSE_FRAME_ELEMENT_ALLOCATE_PUSH(p_frame, element, length, p_data) \
  stse_frame_element_t element = {length, p_data, NULL};                \
  stse_frame_push_element(p_frame, &element);

#define STSE_FRAME_STRAP_ALLOCATE(strap) \
  stse_frame_element_t strap = {0, NULL, NULL};

#define STSE_FRAME_STRAP(p_frame, strap, p_base_element, p_strapped_element) \
  STSE_FRAME_STRAP_ALLOCATE(strap);                                   \
  stse_frame_insert_strap(&strap, p_base_element, p_strapped_element);    \
  stse_frame_update(p_frame);

/**
  * \brief       Attach a strap element that reroute a frame element (Element1) to another (Element2)
  * \details     This core function attach a strap element that link Element1 to Element2
  *              until un-strap command is executed
  * \param[in]     p_strap       Pointer to strap element
  * \param[in]     p_element_1     Pointer to frame element 1
  * \param[in]     p_element_2     Pointer to frame element 2
  */
void stse_frame_insert_strap(stse_frame_element_t *p_strap, stse_frame_element_t *p_element_1,
                             stse_frame_element_t *p_element_2);

/**
  * \brief       Frame un-strap
  * \details     This core function remove strap element from a frame
  * \param[in]     p_frame       Pointer to a frame
  */
void stse_frame_unstrap(stse_frame_t *p_frame);

/**
  * \brief       Update frame meta data
  * \details     This core function update frame structure
  * \param[in,out]   p_frame   Pointer to the frame to be updated
  */
void stse_frame_update(stse_frame_t *p_frame);

/**
  * \brief       Compute Frame CRC
  * \details     This core function compute and return the CRC of a Frame
  * \param[in]     p_frame       Pointer to frame
  * \param[in]     p_crc       Pointer to crc (2-byte CRC value)
  * \return      \ref STSE_OK on success ; \ref stse_return_code_t error code otherwise
  */
stse_return_code_t stse_frame_crc16_compute(stse_frame_t *p_frame, PLAT_UI16 *p_crc);

/**
  * \brief       swap the Data byte order pointed by p_data frame element
  * \details     This core function swap the Data byte order pointed/defined by p_data and length value from frame
  * \param[in]     p_element       Pointer to frame element
  */
void stse_frame_element_swap_byte_order(stse_frame_element_t *p_element);

/**
  * \brief       Push frame element into frame
  * \details     This core function set selected element as the frame last one
  * \param[in]     p_frame         Pointer to frame
  * \param[in]     p_element       Pointer to frame element
  */
void stse_frame_push_element(stse_frame_t *p_frame,
                             stse_frame_element_t *p_element);

/**
  * \brief       Pop last element from frame
  * \details     This core function remove the last element from frame
  * \param[in,out]   p_frame         Pointer to frame
  */
void stse_frame_pop_element(stse_frame_t *p_frame);

/**
  * \brief       Frame debug print
  * \details     This core function print the content of a frame
  * \param[in,out]   p_frame         Pointer to the frame to be printed
  */
void stse_frame_debug_print(stse_frame_t *p_frame);

/*! @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFE_FRAME_H */
