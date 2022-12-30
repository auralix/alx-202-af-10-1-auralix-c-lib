/**
  ******************************************************************************
  * @file		alxParamItem.c
  * @brief		Auralix C Library - ALX Parameter Item Module
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
#include "alxParamItem.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxParamItem_IsEnumOnList_Float(AlxParamItem* me, float enumVal, float* enumArr, uint8_t numOfEnums);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamItem_CtorUint8
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint8_t valDef,
	uint8_t valMin,
	uint8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Uint8;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint8 = valDef;
	me->valMin.uint8 = valMin;
	me->valMax.uint8 = valMax;
	me->valLen = sizeof(uint8_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.uint8 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint16
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint16_t valDef,
	uint16_t valMin,
	uint16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Uint16;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint16 = valDef;
	me->valMin.uint16 = valMin;
	me->valMax.uint16 = valMax;
	me->valLen = sizeof(uint16_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.uint16 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint32
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint32_t valDef,
	uint32_t valMin,
	uint32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Uint32;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint32 = valDef;
	me->valMin.uint32 = valMin;
	me->valMax.uint32 = valMax;
	me->valLen = sizeof(uint32_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.uint32 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint64
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint64_t valDef,
	uint64_t valMin,
	uint64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Uint64;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint64 = valDef;
	me->valMin.uint64 = valMin;
	me->valMax.uint64 = valMax;
	me->valLen = sizeof(uint64_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.uint64 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt8
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int8_t valDef,
	int8_t valMin,
	int8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Int8;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int8 = valDef;
	me->valMin.int8 = valMin;
	me->valMax.int8 = valMax;
	me->valLen = sizeof(int8_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.int8 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt16
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int16_t valDef,
	int16_t valMin,
	int16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Int16;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int16 = valDef;
	me->valMin.int16 = valMin;
	me->valMax.int16 = valMax;
	me->valLen = sizeof(int16_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.int16 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt32
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int32_t valDef,
	int32_t valMin,
	int32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Int32;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int32 = valDef;
	me->valMin.int32 = valMin;
	me->valMax.int32 = valMax;
	me->valLen = sizeof(int32_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.int32 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt64
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int64_t valDef,
	int64_t valMin,
	int64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Int64;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int64 = valDef;
	me->valMin.int64 = valMin;
	me->valMax.int64 = valMax;
	me->valLen = sizeof(int64_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val.int64 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorFloat
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	float valDef,
	float valMin,
	float valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	float* enumArr,
	uint8_t numOfEnums
)
{
	// Parameters
	me->type = AlxParamItem_Type_Float;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef._float = valDef;
	me->valMin._float = valMin;
	me->valMax._float = valMax;
	me->valLen = sizeof(float);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->enumArr = enumArr;
	me->numOfEnums = numOfEnums;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val._float = valDef;

	// Check if enum
	if (me->enumArr != NULL)
	{
		// Check if enum number is from low to high
		for (uint8_t i = 0; i < numOfEnums - 1; i++)
		{
			ALX_PARAM_ITEM_ASSERT(enumArr[i] < enumArr[i + 1]);	// Enums must be from low to high number
		}

		// Check if enum number is on the list
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList_Float(me, valDef, enumArr, numOfEnums) == true);
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList_Float(me, valMin, enumArr, numOfEnums) == true);
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList_Float(me, valMax, enumArr, numOfEnums) == true);
	}

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorDouble
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	double valDef,
	double valMin,
	double valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Double;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef._double = valDef;
	me->valMin._double = valMin;
	me->valMax._double = valMax;
	me->valLen = sizeof(double);
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val._double = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorBool
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	bool valDef
)
{
	// Parameters
	me->type = AlxParamItem_Type_Bool;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef._bool = valDef;
	me->valMin._bool = ALX_NULL;
	me->valMax._bool = ALX_NULL;
	me->valLen = sizeof(bool);
	me->valOutOfRangeHandle = ALX_NULL;
	me->buff = ALX_NULL_PTR;
	me->buffLen = ALX_NULL;

	// Variables
	me->val._bool = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorArr
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	void* valDef,
	void* valBuff,
	void* valDefBuff,
	uint32_t valBuffLen,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Arr;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->val.arr = valBuff;
	me->valDef.arr = valDefBuff;
	me->valMin.arr = ALX_NULL_PTR;
	me->valMax.arr = ALX_NULL_PTR;
	me->valLen = valBuffLen;
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	memcpy(me->valDef.arr, valDef, me->valLen);			// Copy default value to default value buffer
	memcpy(me->val.arr, me->valDef.arr, me->valLen);	// Copy default value buffer to main buffer

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorStr
(
	AlxParamItem* me,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	const char* valDef,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint8_t* buff,
	uint32_t buffLen
)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(strlen(valDef) < buffLen);

	// Parameters
	me->type = AlxParamItem_Type_Str;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	me->valDef.str = (char*)valDef;
	me->valMin.str = ALX_NULL_PTR;
	me->valMax.str = ALX_NULL_PTR;
	me->valLen = strlen(valDef) + 1;	// Add +1 for null terminator
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->buff = buff;
	me->buffLen = buffLen;

	// Variables
	me->val.str = (char*)buff;
	strcpy((char*)buff, valDef);	// Copy default value to buffer

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
const char* AlxParamItem_GetKey(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->key;
}
uint32_t AlxParamItem_GetId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->id;
}
uint32_t AlxParamItem_GetGroupId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->groupId;
}
AlxParamItem_Type AlxParamItem_GetType(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->type;
}
void* AlxParamItem_GetValPtr(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	if (me->type == AlxParamItem_Type_Arr)
	{
		void* ptr = NULL;
		memcpy(&ptr, &me->val, 4);
		return ptr;
	}
	else if (me->type == AlxParamItem_Type_Str)
	{
		return me->val.str;
	}
	else
	{
		return &me->val;
	}
}
uint32_t AlxParamItem_GetValLen(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->valLen;
}
void AlxParamItem_SetValToDef(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Set value to default
	if (me->type == AlxParamItem_Type_Uint8)
	{
		me->val.uint8 = me->valDef.uint8;
	}
	else if (me->type == AlxParamItem_Type_Uint16)
	{
		me->val.uint16 = me->valDef.uint16;
	}
	else if (me->type == AlxParamItem_Type_Uint32)
	{
		me->val.uint32 = me->valDef.uint32;
	}
	else if (me->type == AlxParamItem_Type_Uint64)
	{
		me->val.uint64 = me->valDef.uint64;
	}
	else if (me->type == AlxParamItem_Type_Int8)
	{
		me->val.int8 = me->valDef.int8;
	}
	else if (me->type == AlxParamItem_Type_Int16)
	{
		me->val.int16 = me->valDef.int16;
	}
	else if (me->type == AlxParamItem_Type_Int32)
	{
		me->val.int32 = me->valDef.int32;
	}
	else if (me->type == AlxParamItem_Type_Int64)
	{
		me->val.int64 = me->valDef.int64;
	}
	else if (me->type == AlxParamItem_Type_Float)
	{
		me->val._float = me->valDef._float;
	}
	else if (me->type == AlxParamItem_Type_Double)
	{
		me->val._double = me->valDef._double;
	}
	else if (me->type == AlxParamItem_Type_Bool)
	{
		me->val._bool = me->valDef._bool;
	}
	else if (me->type == AlxParamItem_Type_Arr)
	{
		me->val.arr = me->valDef.arr;
	}
	else if (me->type == AlxParamItem_Type_Str)
	{
		strcpy(me->val.str, me->valDef.str);
	}
	else
	{
		ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
	}
}
uint8_t AlxParamItem_GetValUint8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// Return
	return me->val.uint8;
}
uint16_t AlxParamItem_GetValUint16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// Return
	return me->val.uint16;
}
uint32_t AlxParamItem_GetValUint32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// Return
	return me->val.uint32;
}
uint64_t AlxParamItem_GetValUint64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// Return
	return me->val.uint64;
}
int8_t AlxParamItem_GetValInt8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// Return
	return me->val.int8;
}
int16_t AlxParamItem_GetValInt16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// Return
	return me->val.int16;
}
int32_t AlxParamItem_GetValInt32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// Return
	return me->val.int32;
}
int64_t AlxParamItem_GetValInt64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// Return
	return me->val.int64;
}
float AlxParamItem_GetValFloat(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// Return
	return me->val._float;
}
double AlxParamItem_GetValDouble(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// Return
	return me->val._double;
}
bool AlxParamItem_GetValBool(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);

	// Return
	return me->val._bool;
}
Alx_Status AlxParamItem_SetValUint8(AlxParamItem* me, uint8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t _val = val;

	// Handle value out of range
	switch(me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckUint8(_val, me->valMin.uint8, me->valMax.uint8);
			if (status == Alx_Ok)
			{
				me->val.uint8 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckUint8(_val, me->valMin.uint8, me->valMax.uint8);
			if (status == Alx_Ok)
			{
				me->val.uint8 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Uint8(&_val, me->valMin.uint8, me->valMax.uint8);
			me->val.uint8 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValUint16(AlxParamItem* me, uint16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// Local variables
	Alx_Status status = Alx_Err;
	uint16_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckUint16(_val, me->valMin.uint16, me->valMax.uint16);
			if (status == Alx_Ok)
			{
				me->val.uint16 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckUint16(_val, me->valMin.uint16, me->valMax.uint16);
			if (status == Alx_Ok)
			{
				me->val.uint16 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Uint16(&_val, me->valMin.uint16, me->valMax.uint16);
			me->val.uint16 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValUint32(AlxParamItem* me, uint32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// Local variables
	Alx_Status status = Alx_Err;
	uint32_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckUint32(_val, me->valMin.uint32, me->valMax.uint32);
			if (status == Alx_Ok)
			{
				me->val.uint32 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckUint32(_val, me->valMin.uint32, me->valMax.uint32);
			if (status == Alx_Ok)
			{
				me->val.uint32 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Uint32(&_val, me->valMin.uint32, me->valMax.uint32);
			me->val.uint32 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValUint64(AlxParamItem* me, uint64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// Local variables
	Alx_Status status = Alx_Err;
	uint64_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckUint64(_val, me->valMin.uint64, me->valMax.uint64);
			if (status == Alx_Ok)
			{
				me->val.uint64 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckUint64(_val, me->valMin.uint64, me->valMax.uint64);
			if (status == Alx_Ok)
			{
				me->val.uint64 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Uint64(&_val, me->valMin.uint64, me->valMax.uint64);
			me->val.uint64 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValInt8(AlxParamItem* me, int8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// Local variables
	Alx_Status status = Alx_Err;
	int8_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckInt8(_val, me->valMin.int8, me->valMax.int8);
			if (status == Alx_Ok)
			{
				me->val.int8 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckInt8(_val, me->valMin.int8, me->valMax.int8);
			if (status == Alx_Ok)
			{
				me->val.int8 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Int8(&_val, me->valMin.int8, me->valMax.int8);
			me->val.int8 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValInt16(AlxParamItem* me, int16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// Local variables
	Alx_Status status = Alx_Err;
	int16_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckInt16(_val, me->valMin.int16, me->valMax.int16);
			if (status == Alx_Ok)
			{
				me->val.int16 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckInt16(_val, me->valMin.int16, me->valMax.int16);
			if (status == Alx_Ok)
			{
				me->val.int16 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Int16(&_val, me->valMin.int16, me->valMax.int16);
			me->val.int16 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValInt32(AlxParamItem* me, int32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// Local variables
	Alx_Status status = Alx_Err;
	int32_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckInt32(_val, me->valMin.int32, me->valMax.int32);
			if (status == Alx_Ok)
			{
				me->val.int32 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckInt32(_val, me->valMin.int32, me->valMax.int32);
			if (status == Alx_Ok)
			{
				me->val.int32 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Int32(&_val, me->valMin.int32, me->valMax.int32);
			me->val.int32 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValInt64(AlxParamItem* me, int64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// Local variables
	Alx_Status status = Alx_Err;
	int64_t _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckInt64(_val, me->valMin.int64, me->valMax.int64);
			if (status == Alx_Ok)
			{
				me->val.int64 = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckInt64(_val, me->valMin.int64, me->valMax.int64);
			if (status == Alx_Ok)
			{
				me->val.int64 = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Int64(&_val, me->valMin.int64, me->valMax.int64);
			me->val.int64 = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValFloat(AlxParamItem* me, float val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// Local variables
	Alx_Status status = Alx_Err;
	float _val = val;

	// Check if enum
	if (me->enumArr != NULL)
	{
		// Check if enum is on the list
		bool isEnumOnList = AlxParamItem_IsEnumOnList_Float(me, _val, me->enumArr, me->numOfEnums);
		if (isEnumOnList == false)
		{
			// If handle assert selected, then assert, else return
			if (me->valOutOfRangeHandle == AlxParamItem_ValOutOfRangeHandle_Assert)
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}

			// Return
			return AlxParamItem_ErrEnum;
		}
	}

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckFloat(_val, me->valMin._float, me->valMax._float);
			if (status == Alx_Ok)
			{
				me->val._float = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckFloat(_val, me->valMin._float, me->valMax._float);
			if (status == Alx_Ok)
			{
				me->val._float = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Float(&_val, me->valMin._float, me->valMax._float);
			me->val._float = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValDouble(AlxParamItem* me, double val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// Local variables
	Alx_Status status = Alx_Err;
	double _val = val;

	// Handle value out of range
	switch (me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckDouble(_val, me->valMin._double, me->valMax._double);
			if (status == Alx_Ok)
			{
				me->val._double = _val;
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckDouble(_val, me->valMin._double, me->valMax._double);
			if (status == Alx_Ok)
			{
				me->val._double = _val;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Double(&_val, me->valMin._double, me->valMax._double);
			me->val._double = _val;
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValBool(AlxParamItem* me, bool val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);
	ALX_PARAM_ITEM_ASSERT(val == true || val == false);

	// Set
	me->val._bool = val;

	// Return
	return Alx_Ok;
}
Alx_Status AlxParamItem_GetValUint16_StrFormat(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);
	ALX_PARAM_ITEM_ASSERT(maxLenWithNullTerm <= ALX_PARAM_ITEM_BUFF_LEN);

	// Convert
	char valStr[ALX_PARAM_ITEM_BUFF_LEN] = "";
	ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%u", me->val.uint16) >= 0);

	// Check & Copy if OK
	Alx_Status status = AlxRange_CheckStr(valStr, maxLenWithNullTerm);
	if (status == Alx_Ok)
	{
		strcpy(val, valStr);
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_GetValFloat_StrFormat(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);
	ALX_PARAM_ITEM_ASSERT(maxLenWithNullTerm <= ALX_PARAM_ITEM_BUFF_LEN);

	// Convert
	char valStr[ALX_PARAM_ITEM_BUFF_LEN] = "";
	AlxFtoa(me->val._float, valStr, ALX_PARAM_ITEM_FTOA_PRECISION);

	// Check & Copy if OK
	Alx_Status status = AlxRange_CheckStr(valStr, maxLenWithNullTerm);
	if (status == Alx_Ok)
	{
		strcpy(val, valStr);
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_GetValBool_StrFormat(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);
	ALX_PARAM_ITEM_ASSERT(maxLenWithNullTerm <= ALX_PARAM_ITEM_BUFF_LEN);

	// Convert
	char valStr[ALX_PARAM_ITEM_BUFF_LEN] = "";
	if (me->val._bool)
	{
		strcpy(valStr, "true");
	}
	else
	{
		strcpy(valStr, "false");
	}

	// Check & Copy if OK
	Alx_Status status = AlxRange_CheckStr(valStr, maxLenWithNullTerm);
	if (status == Alx_Ok)
	{
		strcpy(val, valStr);
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValUint16_StrFormat(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// Convert
	uint16_t valNum = 0;
	if (sscanf(val, "%hu", &valNum) != 1)
	{
		return AlxParamItem_ErrConv;
	}

	// Return
	return AlxParamItem_SetValUint16(me, valNum);
}
Alx_Status AlxParamItem_SetValFloat_StrFormat(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// Convert
	float valNum = 0;
	if (sscanf(val, "%f", &valNum) != 1)
	{
		return AlxParamItem_ErrConv;
	}

	// Return
	return AlxParamItem_SetValFloat(me, valNum);
}
Alx_Status AlxParamItem_SetValBool_StrFormat(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);

	// Convert
	bool valNum = false;
	if (strcmp(val, "true") == 0)
	{
		valNum = true;
	}
	else if (strcmp(val, "True") == 0)
	{
		valNum = true;
	}
	else if (strcmp(val, "TRUE") == 0)
	{
		valNum = true;
	}
	else if (strcmp(val, "false") == 0)
	{
		valNum = false;
	}
	else if (strcmp(val, "False") == 0)
	{
		valNum = false;
	}
	else if (strcmp(val, "FALSE") == 0)
	{
		valNum = false;
	}
	else
	{
		return AlxParamItem_ErrConv;
	}

	// Return
	return AlxParamItem_SetValBool(me, valNum);
}
void AlxParamItem_GetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// Copy
	memcpy(val, me->val.arr, me->valLen);
}
void AlxParamItem_SetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// Set
	memcpy(me->val.arr, val, me->valLen);
}
Alx_Status AlxParamItem_GetValStr(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Str);

	// Check & Copy if OK
	Alx_Status status = AlxRange_CheckStr(me->val.str, maxLenWithNullTerm);
	if (status == Alx_Ok)
	{
		strcpy(val, me->val.str);
	}

	// Return
	return status;
}
Alx_Status AlxParamItem_SetValStr(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Str);

	// Local variables
	Alx_Status status = Alx_Err;

	// Handle value out of range
	switch(me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckStr(val, me->buffLen);
			if (status == Alx_Ok)
			{
				strcpy(me->val.str, val);
				me->valLen = strlen(me->val.str) + 1;	// Add +1 for null terminator
			}
			else
			{
				ALX_PARAM_ITEM_ASSERT(false);
				status = Alx_Err;
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Ignore:
		{
			status = AlxRange_CheckStr(val, me->buffLen);
			if (status == Alx_Ok)
			{
				strcpy(me->val.str, val);
				me->valLen = strlen(me->val.str) + 1;	// Add +1 for null terminator
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Str(me->val.str, val, me->buffLen);
			me->valLen = strlen(me->val.str) + 1;	// Add +1 for null terminator
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// Return
	return status;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxParamItem_IsEnumOnList_Float(AlxParamItem* me, float enumVal, float* enumArr, uint8_t numOfEnums)
{
	// Check if enum number is on the list
	for (uint8_t i = 0; i < numOfEnums; i++)
	{
		if (enumVal == enumArr[i])
		{
			// Return
			return true;	// Number is on the list
		}
	}

	// Return
	return false;	// Number is NOT on the list
}


#endif // #if defined(ALX_C_LIB)
