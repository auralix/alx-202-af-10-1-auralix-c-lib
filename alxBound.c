/**
  ******************************************************************************
  * @file alxBound.c
  * @brief Auralix C Library - ALX Bound Module
  * @version $LastChangedRevision: 5338 $
  * @date $LastChangedDate: 2021-06-07 11:51:21 +0200 (Mon, 07 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBound.h"


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBound_Uint8(uint8_t* val, uint8_t valMin, uint8_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint16(uint16_t* val, uint16_t valMin, uint16_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint32(uint32_t* val, uint32_t valMin, uint32_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint64(uint64_t* val, uint64_t valMin, uint64_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int8(int8_t* val, int8_t valMin, int8_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int16(int16_t* val, int16_t valMin, int16_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int32(int32_t* val, int32_t valMin, int32_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int64(int64_t* val, int64_t valMin, int64_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Float(float* val, float valMin, float valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Double(double* val, double valMin, double valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
/*Alx_Status AlxBound_Arrr(char* valBounded, char* val, uint32_t valLenMax)
{
	ALX_BOUND_ASSERT(0 < valLenMax);
	
	uint32_t valLen = strlen(val);
	if (valLen >= (valLenMax - 1))
	{
		memcpy(valBounded, val, valLenMax - 1);
		valBounded[valLenMax - 1] = 0;	// Terminate string
		return Alx_ErrBoundLen;
	}
	else
	{
		strcpy(valBounded, val);
		return Alx_Ok;
	}
}*/
Alx_Status AlxBound_Str(char* valBounded, char* val, uint32_t valLenMax)
{
	ALX_BOUND_ASSERT(0 < valLenMax);
	
	uint32_t valLen = strlen(val);
	if (valLen > (valLenMax - 1))
	{
		memcpy(valBounded, val, valLenMax - 1);
		valBounded[valLenMax - 1] = 0;	// Terminate string
		return Alx_ErrBoundLen;
	}
	else
	{
		strcpy(valBounded, val);
		return Alx_Ok;
	}
}
