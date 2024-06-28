/*!
 * ******************************************************************************
 * \file	stse_frame.h
 * \brief   STSAFE Frame layer (header)
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

/*! \defgroup stse_frame Frame Management
 *  \ingroup stse_core
 *  @{
 */

#ifndef STSAFE_FRAME_H
#define STSAFE_FRAME_H

#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include "core/stse_platform.h"
#include "core/stse_util.h"

#define STSE_FRAME_CRC_SIZE 					2U
#define STSE_FRAME_LENGTH_SIZE 					2U
#define STSE_RSP_FRAME_HEADER_SIZE				1U

typedef struct stse_frame_t stse_frame_t;
typedef struct stse_frame_element_t stse_frame_element_t;

struct stse_frame_t{
    PLAT_UI8 element_count;
    PLAT_UI16 length;
    stse_frame_element_t * first_element;
    stse_frame_element_t * last_element;
};

struct stse_frame_element_t{
    PLAT_UI16 length;
    PLAT_UI8 * pData;
    stse_frame_element_t * next;
};

typedef enum {
	STSAFE_FRAME_PLAINTEXT=0,
	STSAFE_FRAME_ENCRYPT
}stse_frame_encrypt_flag_t;


#define stse_frame_allocate(frame) stse_frame_t frame = {0};

#define stse_frame_element_allocate(element,length,pData) \
		stse_frame_element_t element = {length,pData,NULL};

#define stse_frame_element_allocate_push(pFrame,element,length,pData) \
		stse_frame_element_t element = {length,pData,NULL}; \
		stse_frame_push_element(pFrame,&element);

#define stse_frame_strap_allocate(strap) \
		stse_frame_element_t strap = {0,NULL,NULL};

#define stse_frame_strap(pFrame,strap,pBaseElement,pStrappedElement) \
		stse_frame_strap_allocate(strap); \
		stse_frame_insert_strap(&strap,pBaseElement,pStrappedElement); \
		stse_frame_update(pFrame);


/**
 * \brief 			Attach a strap element that reroute a frame element (Element1) to another (Element2)
 * \details 		This core function attach a strap element that link Element1 to Element2 until un-strap command is executed
 * \param[in] 		pStrap 			Pointer to strap element
 * \param[in] 		pElement_1 		Pointer to frame element 1
 * \param[in] 		pElement_2 		Pointer to frame element 2
 */
void stse_frame_insert_strap(stse_frame_element_t *pStrap, stse_frame_element_t* pElement_1,
		stse_frame_element_t* pElement_2);

/**
 * \brief 			Frame un-strap
 * \details 		This core function remove strap element from a frame
 * \param[in] 		pFrame 			Pointer to a frame
 */
void stse_frame_unstrap(stse_frame_t* pFrame);

/**
 * \brief 			Update frame meta data
 * \details 		This core function update frame structure
 * \param[in,out] 	pFrame 	Pointer to the frame to be updated
 */
void stse_frame_update(stse_frame_t* pFrame);

/**
 * \brief 			Compute Frame CRC
 * \details 		This core function compute and return the CRC of a Frame
 * \param[in] 		pFrame 			Pointer to frame
 * \return 			2-byte CRC value
 */
PLAT_UI16 stse_frame_crc16_compute (stse_frame_t* pFrame);

/**
 * \brief 			swap the Data byte order pointed by pData frame element
 * \details 		This core function swap the Data byte order pointed/defined by pData and length value from frame
 * \param[in] 		pElement 			Pointer to frame element
 */
void stse_frame_element_swap_byte_order(stse_frame_element_t* pElement);

/**
 * \brief 			Push frame element into frame
 * \details 		This core function set selected element as the frame last one
 * \param[in] 		pFrame 				Pointer to frame
 * \param[in] 		pElement 			Pointer to frame element
 */
void stse_frame_push_element( stse_frame_t* pFrame,
		stse_frame_element_t* pElement);

/**
 * \brief 			Pop last element from frame
 * \details 		This core function remove the last element from frame
 * \param[in,out] 	pFrame 				Pointer to frame
 */
void stse_frame_pop_element( stse_frame_t* pFrame);


/**
 * \brief 			Frame debug print
 * \details 		This core function print the content of a frame
 * \param[in,out] 	pFrame 				Pointer to the frame to be printed
 */
void stse_frame_debug_print(stse_frame_t* pFrame);

/**
 * \brief 			Transmit frame from target STSAFE-Axxx
 * \details 		This core function prepare frame CRC and send frame to target STSAFE-Axxx device
 * \param[in] 		pSTSE 			Pointer to STSAFE Handler
 * \param[in,out] 	pFrame 				Pointer to the frame to be transmitted
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_frame_transmit(stse_Handler_t* pSTSE,
		stse_frame_t* pFrame);

/**
 * \brief 			Receive frame from target STSAFE-Axxx
 * \details 		This core function receive frame from target STSAFE-Axxx device and verify its CRC
 * \param[in] 		pSTSE 			Pointer to STSAFE Handler
 * \param[in,out] 	pFrame 				Pointer to the frame to be received
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_frame_receive(stse_Handler_t* pSTSE,
		stse_frame_t* pFrame);


/**
 * \brief 			Transfer Frames to/from target STSAFE-Axx
 * \details 		This core function send and receive frame to/from target STSAFE-Axxx device
 * \param[in] 		pSTSE 			Pointer to STSAFE Handler
 * \param[in] 		pCmdFrame 			Pointer to the command frame
 * \param[in,out] 	pRspFrame 			Pointer to the response frame
 * \param[in] 		inter_frame_delay 	Delay between command and response frame (in ms)
 * \return 			\ref stse_ReturnCode_t : STSE_OK on success ; error code otherwise
 */
stse_ReturnCode_t stse_frame_transfer (stse_Handler_t* pSTSE,
		stse_frame_t* pCmdFrame,
		stse_frame_t* pRspFrame,
		PLAT_UI16 inter_frame_delay);

/*! @}*/

#endif /* STSAFE_FRAME_H */
