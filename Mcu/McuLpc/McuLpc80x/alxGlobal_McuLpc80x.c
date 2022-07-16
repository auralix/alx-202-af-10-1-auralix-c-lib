/**
  ******************************************************************************
  * @file		alxGlobal_McuLpc80x.c
  * @brief		Auralix C Library - ALX Global MCU LPC80X Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal_McuLpc80x.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Functions
//******************************************************************************
uint8_t AlxPROTECTED_IoPin_GetIoconPortPinIndex(uint8_t pin, uint8_t port)
{
	#if defined(ALX_LPC80X)
	if (pin == 17)		return 0;
	if (pin == 13)		return 1;
	if (pin == 12)		return 2;
	if (pin == 5)		return 3;
	if (pin == 4)		return 4;
	if (pin == 3)		return 5;
	if (pin == 2)		return 6;
	if (pin == 11)		return 7;
	if (pin == 10)		return 8;
	if (pin == 16)		return 9;
	if (pin == 15)		return 10;
	if (pin == 1)		return 11;
	if (pin == 21)		return 12;
	if (pin == 9)		return 13;
	if (pin == 8)		return 14;
	if (pin == 7)		return 15;
	if (pin == 29)		return 16;
	if (pin == 0)		return 17;
	if (pin == 14)		return 18;
	if (pin == 28)		return 19;
	if (pin == 27)		return 20;
	if (pin == 26)		return 21;
	if (pin == 20)		return 22;
	if (pin == 30)		return 23;
	if (pin == 19)		return 24;
	if (pin == 25)		return 25;
	if (pin == 24)		return 26;
	if (pin == 23)		return 27;
	if (pin == 22)		return 28;
	if (pin == 18)		return 29;
	#endif

	#if defined(ALX_LPC84X)
	if (port == 0 && pin == 17)		return 0;
	if (port == 0 && pin == 13)		return 1;
	if (port == 0 && pin == 12)		return 2;
	if (port == 0 && pin == 5)		return 3;
	if (port == 0 && pin == 4)		return 4;
	if (port == 0 && pin == 3)		return 5;
	if (port == 0 && pin == 2)		return 6;
	if (port == 0 && pin == 11)		return 7;
	if (port == 0 && pin == 10)		return 8;
	if (port == 0 && pin == 16)		return 9;
	if (port == 0 && pin == 15)		return 10;
	if (port == 0 && pin == 1)		return 11;
	if (port == 0 && pin == 9)		return 13;
	if (port == 0 && pin == 8)		return 14;
	if (port == 0 && pin == 7)		return 15;
	if (port == 0 && pin == 6)		return 16;
	if (port == 0 && pin == 0)		return 17;
	if (port == 0 && pin == 14)		return 18;
	if (port == 0 && pin == 28)		return 20;
	if (port == 0 && pin == 27)		return 21;
	if (port == 0 && pin == 26)		return 22;
	if (port == 0 && pin == 25)		return 23;
	if (port == 0 && pin == 24)		return 24;
	if (port == 0 && pin == 23)		return 25;
	if (port == 0 && pin == 22)		return 26;
	if (port == 0 && pin == 21)		return 27;
	if (port == 0 && pin == 20)		return 28;
	if (port == 0 && pin == 19)		return 29;
	if (port == 0 && pin == 18)		return 30;
	if (port == 1 && pin == 8)		return 31;
	if (port == 1 && pin == 9)		return 32;
	if (port == 1 && pin == 12)		return 33;
	if (port == 1 && pin == 13)		return 34;
	if (port == 0 && pin == 31)		return 35;
	if (port == 1 && pin == 0)		return 36;
	if (port == 1 && pin == 1)		return 37;
	if (port == 1 && pin == 2)		return 38;
	if (port == 1 && pin == 14)		return 39;
	if (port == 1 && pin == 15)		return 40;
	if (port == 1 && pin == 3)		return 41;
	if (port == 1 && pin == 4)		return 42;
	if (port == 1 && pin == 5)		return 43;
	if (port == 1 && pin == 16)		return 44;
	if (port == 1 && pin == 17)		return 45;
	if (port == 1 && pin == 6)		return 46;
	if (port == 1 && pin == 18)		return 47;
	if (port == 1 && pin == 19)		return 48;
	if (port == 1 && pin == 7)		return 49;
	if (port == 0 && pin == 29)		return 50;
	if (port == 0 && pin == 30)		return 51;
	if (port == 1 && pin == 20)		return 52;
	if (port == 1 && pin == 21)		return 53;
	if (port == 1 && pin == 11)		return 54;
	if (port == 1 && pin == 10)		return 55;
	#endif

	// MF: Assert cannot be used
	return 255;
}
void AlxPROTECTED_IoPin_SetIoconMode(uint8_t pin, uint8_t port, uint32_t mode)
{
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(pin, port);

	if (mode == IOCON_MODE_INACT)
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
		return;
	}

	if (mode == IOCON_MODE_PULLDOWN)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
		return;
	}

	if (mode == IOCON_MODE_PULLUP)
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 4U);
		return;
	}

	if (mode == IOCON_MODE_REPEATER)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 4U);
		return;
	}

	// MF: Assert cannot be used
}

#endif // Module Guard
