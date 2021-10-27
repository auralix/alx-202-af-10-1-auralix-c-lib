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
	// Ctor
	
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
			const uint8_t width = (8 * sizeof(uint16_t));
			const uint16_t Polynomial = 0x1021;
			const uint16_t InitialRemainder = 0xFFFF;
			const uint16_t FinalXorValue = 0x0000;
			//const uint16_t CheckValue = 0x29B1;

			uint16_t remainder = InitialRemainder;
			uint32_t byte;
			uint8_t  bit;
			/*
			 * Perform modulo-2 division, a byte at a time.
			 */
			for (byte = 0; byte < len; ++byte)
			{
				/*
				 * Bring the next byte into the remainder.
				 */
				remainder ^= (data[byte] << (width - 8));
				/*
				 * Perform modulo-2 division, a bit at a time.
				 */
				for (bit = 8; bit > 0; --bit)
				{
					/*
					 * Try to divide the current data bit.
					 */
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ Polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}
			/*
			 * The final remainder is the CRC result.
			 */
			return (remainder ^ FinalXorValue);
			break;
		}
		case AlxCrc_Config_Crc16:
		{
			const uint8_t width = (8 * sizeof(uint16_t));
			const uint16_t Polynomial = 0x8005;
			const uint16_t InitialRemainder = 0x0000;
			const uint16_t FinalXorValue = 0x0000;
			//const uint16_t CheckValue = 0xBB3D;

			uint16_t remainder = InitialRemainder;
			uint32_t byte;
			uint8_t  bit;
			/*
			 * Perform modulo-2 division, a byte at a time.
			 */
			for (byte = 0; byte < len; ++byte)
			{
				remainder ^= (((uint8_t)Reflect((data[byte]), 8)) << (width - 8));
				/*
				 * Perform modulo-2 division, a bit at a time.
				 */
				for (bit = 8; bit > 0; --bit)
				{
					/*
					 * Try to divide the current data bit.
					 */
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ Polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}
			/*
			 * The final remainder is the CRC result.
			 */
			return (((uint16_t)Reflect((remainder), width)) ^ FinalXorValue);
			break;
		}
		case AlxCrc_Config_Crc32:
		{
			const uint8_t width = (8 * sizeof(uint32_t));
			const uint32_t Polynomial = 0x04C11DB7;
			const uint32_t InitialRemainder = 0xFFFFFFFF;
			const uint32_t FinalXorValue = 0xFFFFFFFF;
			//const uint32_t CheckValue = 0xCBF43926;

			uint32_t remainder = InitialRemainder;
			uint32_t byte;
			uint8_t  bit;
			/*
			 * Perform modulo-2 division, a byte at a time.
			 */
			for (byte = 0; byte < len; ++byte)
			{
				/*
				 * Bring the next byte into the remainder.
				 */
				remainder ^= (((uint8_t)Reflect((data[byte]), 8)) << (width - 8));
				/*
				 * Perform modulo-2 division, a bit at a time.
				 */
				for (bit = 8; bit > 0; --bit)
				{
					/*
					 * Try to divide the current data bit.
					 */
					if (remainder & (1 << (width - 1)))
					{
						remainder = (remainder << 1) ^ Polynomial;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
			}
			/*
			 * The final remainder is the CRC result.
			 */
			return (((uint32_t)Reflect((remainder), width)) ^ FinalXorValue);
			break;
		}
	}
}
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc)
{
	const uint32_t crcLen = 4;	// Manjka switch case za vsako dolžino?

	union
	{
		uint32_t crcToCheck;
		uint8_t raw[4];
	} crc;
	
	memcpy(crc.raw, &dataWithCrc[lenWithCrc - crcLen], crcLen);
	uint32_t crcCalc = AlxCrc_Calc(me, dataWithCrc, lenWithCrc - crcLen);

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
			return 1;
		}
	case AlxCrc_Config_Crc16:
		{
			return 2;
		}
	case AlxCrc_Config_Crc32:
		{
			return 4;
		}
	}
	
}


//******************************************************************************
// Private Functions
//******************************************************************************
uint32_t Reflect(uint32_t data, const uint8_t nBits)
{
	uint32_t reflection = 0x00000000;
	uint8_t  bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit)
	{
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}
		data = (data >> 1);
	}
	return (reflection);
}