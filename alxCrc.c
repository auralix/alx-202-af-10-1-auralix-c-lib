/**
  ******************************************************************************
  * @file		alxCrc.c
  * @brief		Auralix C Library - ALX CRC Module
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
#include "alxCrc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxCrc_Reflect(uint32_t data, uint8_t nBits);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCrc_Ctor
(
	AlxCrc* me,
	AlxCrc_Config config
)
{
	// Parameters
	me->config = config;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxCrc_Calc(AlxCrc* me, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRC_ASSERT(me->wasCtorCalled == true);

	// #2 Handle CRC calculation
	switch (me->config)
	{
		case AlxCrc_Config_Ccitt:
		{
			// #1 Prepare variables
			const uint8_t width = (8 * sizeof(uint16_t));
			const uint16_t polynomial = 0x1021;
			const uint16_t initialRemainder = 0xFFFF;
			const uint16_t finalXorValue = 0x0000;
			uint16_t remainder = initialRemainder;
			uint32_t byte = 0;
			uint8_t  bit = 0;

			// #2 Perform modulo-2 division, a byte at a time.
			for (byte = 0; byte < len; ++byte)
			{
				// #2.1 Bring the next byte into the remainder.
				remainder ^= (data[byte] << (width - 8));

				// #2.2 Perform modulo-2 division, a bit at a time.
				for (bit = 8; bit > 0; --bit)
				{

					// #2.3 Try to divide the current data bit.
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}

			// #3 The final remainder is the CRC result.
			return (remainder ^ finalXorValue);
			break;
		}
		case AlxCrc_Config_Crc16:
		{
			// #1 Prepare variables
			const uint8_t width = (8 * sizeof(uint16_t));
			const uint16_t polynomial = 0x8005;
			const uint16_t initialRemainder = 0x0000;
			const uint16_t finalXorValue = 0x0000;
			uint16_t remainder = initialRemainder;
			uint32_t byte = 0;
			uint8_t  bit = 0;

			// #2 Perform modulo-2 division, a byte at a time.
			for (byte = 0; byte < len; ++byte)
			{
				// #2.1 Bring the next byte into the remainder.
				remainder ^= (((uint8_t)AlxCrc_Reflect((data[byte]), 8)) << (width - 8));

				// #2.2 Perform modulo-2 division, a bit at a time.
				for (bit = 8; bit > 0; --bit)
				{
					// #2.3 Try to divide the current data bit.
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}

			// #3 The final remainder is the CRC result.
			return (((uint16_t)AlxCrc_Reflect((remainder), width)) ^ finalXorValue);
			break;
		}
		case AlxCrc_Config_Crc32:
		{
			// #1 Prepare variables
			const uint8_t width = (8 * sizeof(uint32_t));
			const uint32_t polynomial = 0x04C11DB7;
			const uint32_t initialRemainder = 0xFFFFFFFF;
			const uint32_t finalXorValue = 0xFFFFFFFF;
			uint32_t remainder = initialRemainder;
			uint32_t byte = 0;
			uint8_t  bit = 0;

			// #2 Perform modulo-2 division, a byte at a time.
			for (byte = 0; byte < len; ++byte)
			{
				// #2.1 Bring the next byte into the remainder.
				remainder ^= (((uint8_t)AlxCrc_Reflect((data[byte]), 8)) << (width - 8));

				// #2.2 Perform modulo-2 division, a bit at a time.
				for (bit = 8; bit > 0; --bit)
				{
					// #2.3 Try to divide the current data bit.
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}

			// #3 The final remainder is the CRC result.
			return (((uint32_t)AlxCrc_Reflect((remainder), width)) ^ finalXorValue);
			break;
		}
		default:
		{
			ALX_CRC_ASSERT(false);	// We should never get here
			break;
		}
	}
}
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc)
{
	// #1 Assert
	ALX_CRC_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare variables
	union
	{
		uint32_t crcToCheck;
		uint8_t raw[4];
	} crc = {};

	// #3 Get CRC lenght based on configuration
	uint32_t crcLen = AlxCrc_GetLen(me);

	// #4 Copy CRC from dataWithCrc to local union
	memcpy(crc.raw, &dataWithCrc[lenWithCrc - crcLen], crcLen);

	// #5 Calculate new CRC with data from dataWithCrc
	uint32_t crcCalc = AlxCrc_Calc(me, dataWithCrc, lenWithCrc - crcLen);

	// #6 Check if CRCs match
	if (crc.crcToCheck == crcCalc)
	{
		*validatedCrc = crcCalc;
		return true;
	}
	else
	{
		return false;
	}
}
uint32_t AlxCrc_GetLen(AlxCrc* me)
{
	// #1 Assert
	ALX_CRC_ASSERT(me->wasCtorCalled == true);

	// #2 Get & return lenght
	switch (me->config)
	{
		case AlxCrc_Config_Ccitt:
		{
			return 2;
			break;
		}
		case AlxCrc_Config_Crc16:
		{
			return 2;
			break;
		}
		case AlxCrc_Config_Crc32:
		{
			return 4;
			break;
		}
		default:
		{
			ALX_CRC_ASSERT(false);	// We should never get here
			break;
		}
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxCrc_Reflect(uint32_t data, uint8_t nBits)
{
	// #1 Prepare variables
	uint32_t reflection = 0x00000000;

	// #2 Reflect the data about the center bit
	for (uint8_t bit = 0; bit < nBits; ++bit)
	{
		// #2.1 If the LSB bit is set, set the reflection of it
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}
		data = (data >> 1);
	}

	// #3 Return
	return reflection;
}


#endif // #if defined(ALX_C_LIB)
