 /******************************************************************************
 * \file	stsafe_timings.c
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

#include "services/stsafea/stsafea_timings.h"

#define STSAFEA_EXEC_TIME_DEFAULT				500U	/*!< STSAFE default command processing time (used when specific time == 0) */
#define STSAFEA_BOOT_TIME_DEFAULT 				10U
#define STSAFEA_WAKEUP_TIME_DEFAULT 			10U

const PLAT_UI16 stsafea_cmd_timings[4][30] = {
	/* STSAFE_A100 */
	{
			STSAFEA_EXEC_TIME_DEFAULT,	/* Echo */
			STSAFEA_EXEC_TIME_DEFAULT,  /* Reset */
			STSAFEA_EXEC_TIME_DEFAULT,   /* Generate Random */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Start Session */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Decrement zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Read zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Update zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Increment zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Derive LORA key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap issuer envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Delete MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Hibernate */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Wrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put attribute */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Configure */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Query */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Get Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Establish Key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Standby */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Password */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Encrypt */
			STSAFEA_EXEC_TIME_DEFAULT	/* Decrypt */
	},
	/* STSAFE_A110 */
	{
			STSAFEA_EXEC_TIME_DEFAULT,	/* Echo */
			STSAFEA_EXEC_TIME_DEFAULT,  /* Reset */
			STSAFEA_EXEC_TIME_DEFAULT,  /* Generate Random */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Start Session */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Decrement zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Read zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Update zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Increment zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Derive LORA key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap issuer envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Delete MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Hibernate */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Wrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put attribute */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Configure */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Query */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Get Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Establish Key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Standby */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Password */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Encrypt */
			STSAFEA_EXEC_TIME_DEFAULT	/* Decrypt */
	},
	/* STSAFE_A120 */
	{
			50,	/* Echo */
			22,  /* Reset */
			26,  /* Generate Random */
			22,	/* Start Session */
			23,	/* Decrement zone counter */
			23,	/* Read zone */
			22,	/* Update zone */
			26,	/* Increment zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Derive LORA key (Not applicable) */
			23,	/* Generate MAC */
			23,	/* Verify MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap issuer envelope (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Delete MAC (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Hibernate, (Not applicable)*/
			25,	/* Wrap Local Envelope */
			24,	/* Unwrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put attribute(Not applicable)  */
			80,	/* Generate key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put key (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Configure */
			10,	/* Query */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Get Signature */
			96,	/* Generate Signature */
			156,	/* Verify Signature */
			34,	/* Establish Key */ 
			STSAFEA_EXEC_TIME_DEFAULT,	/* Standby (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Password (Not applicable)*/
			25,	/* Encrypt */
			26	/* Decrypt */
	},
	/* STSAFE_A200 */
	{
			STSAFEA_EXEC_TIME_DEFAULT,	/* Echo */
			STSAFEA_EXEC_TIME_DEFAULT,  /* Reset */
			STSAFEA_EXEC_TIME_DEFAULT,  /* Generate Random */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Start Session */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Decrement zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Read zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Update zone */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Increment zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Derive LORA key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap issuer envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Delete MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Hibernate */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Wrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put attribute */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Configure */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Query */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Get Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Generate Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Signature */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Establish Key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Standby */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Password */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Encrypt */
			STSAFEA_EXEC_TIME_DEFAULT	/* Decrypt */
	}
};

const PLAT_UI16 stsafea_extended_cmd_timings[4][27] = {
	/* STSAFE_A100 */
	{
		STSAFEA_EXEC_TIME_DEFAULT,		/* start hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* process hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* finish hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* start volatile KEK session*/
		STSAFEA_EXEC_TIME_DEFAULT,		/* establish symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* confirm symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* stop volatile KEK session */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_PLAINTEXT */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_WRAPPED */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_SYMMETRIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_PUBLIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* GENERATE_ECDHE */
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT
	},
	/* STSAFE_A110 */
	{
		STSAFEA_EXEC_TIME_DEFAULT,		/* start hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* process hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* finish hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* start volatile KEK session*/
		STSAFEA_EXEC_TIME_DEFAULT,		/* establish symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* confirm symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* stop volatile KEK session */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_PLAINTEXT */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_WRAPPED */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_SYMMETRIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_PUBLIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* GENERATE_ECDHE */
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT
	},
	/* STSAFE_A120 */
	{
			50,	/* Echo */
			22,  /* Reset */
			26,  /* Generate Random */
			22,	/* Start Session */
			23,	/* Decrement zone counter */
			23,	/* Read zone */
			22,	/* Update zone */
			26,	/* Increment zone counter */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Derive LORA key (Not applicable) */
			23,	/* Generate MAC */
			23,	/* Verify MAC */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Unwrap issuer envelope (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Delete MAC (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Hibernate, (Not applicable)*/
			25,	/* Wrap Local Envelope */
			24,	/* Unwrap Local Envelope */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put attribute(Not applicable)  */
			80,	/* Generate key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Put key (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Configure */
			10,	/* Query */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Get Signature (Not applicable)*/
			96,	/* Generate Signature */
			156,	/* Verify Signature */
			34,	/* Establish Key */
			STSAFEA_EXEC_TIME_DEFAULT,	/* Standby (Not applicable)*/
			STSAFEA_EXEC_TIME_DEFAULT,	/* Verify Password (Not applicable)*/
			25,	/* Encrypt */
			26	/* Decrypt */
	},
	/* STSAFE_A200 */
	{
		STSAFEA_EXEC_TIME_DEFAULT,		/* start hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* process hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* finish hash */
		STSAFEA_EXEC_TIME_DEFAULT,		/* start volatile KEK session*/
		STSAFEA_EXEC_TIME_DEFAULT,		/* establish symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* confirm symmetric keys */
		STSAFEA_EXEC_TIME_DEFAULT,		/* stop volatile KEK session */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_PLAINTEXT */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_HOST_KEY_V2_WRAPPED */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_SYMMETRIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* WRITE_PUBLIC_KEY */
		STSAFEA_EXEC_TIME_DEFAULT,		/* GENERATE_ECDHE */
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT,
		STSAFEA_EXEC_TIME_DEFAULT
	}
};

const PLAT_UI16 stsafea_boot_time[4] = {
		STSAFEA_BOOT_TIME_DEFAULT, /* STSAFE_A100 */
		STSAFEA_BOOT_TIME_DEFAULT, /* STSAFE_A110 */
		STSAFEA_BOOT_TIME_DEFAULT, /* STSAFE_A120 */
		STSAFEA_BOOT_TIME_DEFAULT /* STSAFE_A200 */
};

const PLAT_UI16 stsafea_wakeup_time[4] = {
		STSAFEA_WAKEUP_TIME_DEFAULT, /* STSAFE_A100 */
		STSAFEA_WAKEUP_TIME_DEFAULT, /* STSAFE_A110 */
		STSAFEA_WAKEUP_TIME_DEFAULT, /* STSAFE_A120 */
		STSAFEA_WAKEUP_TIME_DEFAULT /* STSAFE_A200 */
};


