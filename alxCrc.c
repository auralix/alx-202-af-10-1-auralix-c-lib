/**
  ******************************************************************************
  * @file alxCrc.c
  * @brief Auralix C Library - ALX CRC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCrc.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCrc_Ctor
(
	AlxCrc* me,
	AlxCrc_Config config)
{
	// Parameters
	me->config = config;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Private Functions Declarations
//******************************************************************************
uint32_t Reflect(uint32_t data, const uint8_t nBits);


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxCrc_Calc(AlxCrc* me, uint8_t* data, uint32_t len)
{
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
			uint32_t byte;
			uint8_t  bit;

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
			uint32_t byte;
			uint8_t  bit;

			// #2 Perform modulo-2 division, a byte at a time.
			for (byte = 0; byte < len; ++byte)
			{
				// #2.1 Bring the next byte into the remainder.
				remainder ^= (((uint8_t)Reflect((data[byte]), 8)) << (width - 8));

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
			return (((uint16_t)Reflect((remainder), width)) ^ finalXorValue);
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
			uint32_t byte;
			uint8_t  bit;

			// #2 Perform modulo-2 division, a byte at a time.
			for (byte = 0; byte < len; ++byte)
			{
				// #2.1 Bring the next byte into the remainder.
				remainder ^= (((uint8_t)Reflect((data[byte]), 8)) << (width - 8));

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
			return (((uint32_t)Reflect((remainder), width)) ^ finalXorValue);
			break;
		}
	}
}
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc)
{
	// #1 Prepare variables
	uint32_t crcLen = 0;
	union
	{
		uint32_t crcToCheck;
		uint8_t raw[4];
	} crc;

	// #2 Get Crc lenght based on cunfiguration
	switch (me->config)
	{
	case AlxCrc_Config_Ccitt:
		{
			crcLen = 2;
			break;
		}
	case AlxCrc_Config_Crc16:
		{
			crcLen = 2;
			break;
		}
	case AlxCrc_Config_Crc32:
		{
			crcLen = 4;
			break;
		}
	}

	// #3 Copy Crc from dataWithCrc to local union
	memcpy(crc.raw, &dataWithCrc[lenWithCrc - crcLen], crcLen);

	// #4 Calculate new Crc with data from dataWithCrc
	uint32_t crcCalc = AlxCrc_Calc(me, dataWithCrc, lenWithCrc - crcLen);

	// #5 Check if Crc-s match
	if (crc.crcToCheck == crcCalc)
	{
		*validatedCrc = crcCalc;
		return true;
	}
	else return false;
}
uint32_t AlxCrc_GetLen(AlxCrc* me)
{
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
	}

}


//******************************************************************************
// Private Functions
//******************************************************************************
uint32_t Reflect(uint32_t data, const uint8_t nBits)
{
	// #1 Prepare variables
	uint32_t reflection = 0x00000000;
	uint8_t  bit;

	// #2 Reflect the data about the center bit.
	for (bit = 0; bit < nBits; ++bit)
	{
		// #3 If the LSB bit is set, set the reflection of it.
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}
		data = (data >> 1);
	}
	return (reflection);
}