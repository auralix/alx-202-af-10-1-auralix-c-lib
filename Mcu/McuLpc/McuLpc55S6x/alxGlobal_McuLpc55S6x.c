/**
  ******************************************************************************
  * @file alxGlobal_McuLpc80x.c
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal_McuLpc55S6x.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Functions
//******************************************************************************
void AlxPROTECTED_IoPin_SetIoconMode(uint8_t port, uint8_t pin, uint32_t mode)
{
	if (mode == IOCON_MODE_INACT)
	{
		IOCON->PIO[port][pin] &= ~(0x1 << 4U);
		IOCON->PIO[port][pin] &= ~(0x1 << 5U);
		return;
	}

	if (mode == IOCON_MODE_PULLDOWN)
	{
		IOCON->PIO[port][pin] |=  (0x1 << 4U);
		IOCON->PIO[port][pin] &= ~(0x1 << 5U);
		return;
	}

	if (mode == IOCON_MODE_PULLUP)
	{
		IOCON->PIO[port][pin] &= ~(0x1 << 4U);
		IOCON->PIO[port][pin] |=  (0x1 << 5U);
		return;
	}

	if (mode == IOCON_MODE_REPEATER)
	{
		IOCON->PIO[port][pin] |=  (0x1 << 4U);
		IOCON->PIO[port][pin] |=  (0x1 << 5U);
		return;
	}

	// MF: Assert cannot be used
}

#endif // Module Guard
