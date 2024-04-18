/*!
 ******************************************************************************
 * \file	stse_util.h
 * \brief   Util Macros for STSAFE Middleware
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

#ifndef STSAFE_UTIL_H
#define STSAFE_UTIL_H

#define UI16_B0(ui16)  ((ui16) & 0x00FF)
#define UI16_B1(ui16) (((ui16) & 0xFF00) >> 8)
#define UI16_B0_SET(ui8) (ui8)
#define UI16_B1_SET(ui8) ((ui8) << 8)
#define UI16_SWAP(ui16) ((UI16_B0(ui16)<<8) + UI16_B1(ui16))


#define UI32_B0(ui32)  ((ui32) & 0x000000FF)
#define UI32_B1(ui32) (((ui32) & 0x0000FF00) >> 8)
#define UI32_B2(ui32) (((ui32) & 0x00FF0000) >> 16)
#define UI32_B3(ui32) (((ui32) & 0xFF000000) >> 24)

#define ARRAY_2B_SWAP_TO_UI16(buf2) (PLAT_UI16)( ( buf2[0]<<8 ) + buf2[1] )
#define ARRAY_3B_SWAP_TO_UI32(buf3) (PLAT_UI32)( ( buf3[0]<<16 ) + ( buf3[1]<<8 ) + buf3[2] )
#define ARRAY_4B_SWAP_TO_UI32(buf4) (PLAT_UI32)( ( buf4[0]<<24 ) + ( buf4[1]<<16 ) + ( buf4[2]<<8 ) + buf4[3] )

#define UI32_B0_SET(ui8) (ui8)
#define UI32_B1_SET(ui8) ((ui8) << 8)
#define UI32_B2_SET(ui8) ((ui8) << 16)
#define UI32_B3_SET(ui8) ((ui8) << 24)

#endif /*STSAFE_UTIL_H*/
