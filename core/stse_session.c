#include "core/stse_return_codes.h"
#include "core/stse_device.h"
#include <core/stse_platform.h>

/* Private variables ---------------------------------------------------------*/

//static uint8_t  evaluation_host_mac_key[ ]    = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; /*!< STSAFE-A's Host cipher key */
//static uint8_t  evaluation_host_cipher_key[ ] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }; /*!< STSAFE-A's Host Mac key */

/* Public functions ----------------------------------------------------------*/

void stse_session_erase_context(stse_session_t *pSession)
{
	/* - Check stsafe handler initialization */
	if (pSession==NULL)
	{
		return;
	}

	/*Todo validate correct erase */
	memset(pSession, 0x00, sizeof(stse_session_t));
}


stse_ReturnCode_t stse_set_active_session( stse_Handler_t *pSTSE, stse_session_t *pSession )
{
	if(pSTSE == NULL)
	{
		return STSE_CORE_HANDLER_NOT_INITIALISED;
	}

	if(pSession == NULL)
	{
		return STSE_CORE_SESSION_ERROR;
	}

	pSTSE->pActive_host_session = pSession;

	return( STSE_OK );
}



