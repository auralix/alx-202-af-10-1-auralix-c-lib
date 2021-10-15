/**
  ******************************************************************************
  * @file alxRange.c
  * @brief Auralix C Library - ALX Range Module
  * @version $LastChangedRevision: 5338 $
  * @date $LastChangedDate: 2021-06-07 11:51:21 +0200 (Mon, 07 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxRange.h"


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRange_CheckUint8(uint8_t val, uint8_t valMin, uint8_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint16(uint16_t val, uint16_t valMin, uint16_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint32(uint32_t val, uint32_t valMin, uint32_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint64(uint64_t val, uint64_t valMin, uint64_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt8(int8_t val, int8_t valMin, int8_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt16(int16_t val, int16_t valMin, int16_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt32(int32_t val, int32_t valMin, int32_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt64(int64_t val, int64_t valMin, int64_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckFloat(float val, float valMin, float valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckDouble(double val, double valMin, double valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
/*Alx_Status AlxRange_CheckArr(void* val, uint32_t valLenMax)
{
	ALX_RANGE_ASSERT(0 < valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen > (valLenMax - 1))
		return Alx_ErrOutOfRangeLen;
	else
		return Alx_Ok;
}*/
Alx_Status AlxRange_CheckStr(char* val, uint32_t valLenMax)
{
	ALX_RANGE_ASSERT(0 <= valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen > (valLenMax - 1))
		return Alx_ErrOutOfRangeLen;
	else
		return Alx_Ok;
}
