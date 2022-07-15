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
// Constructor
//******************************************************************************
void AlxParamItem_CtorUint8
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint8 = valDef;
	me->valMin.uint8 = valMin;
	me->valMax.uint8 = valMax;
	me->valLen = sizeof(uint8_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.uint8 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint16
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint16 = valDef;
	me->valMin.uint16 = valMin;
	me->valMax.uint16 = valMax;
	me->valLen = sizeof(uint16_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.uint16 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint32
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint32 = valDef;
	me->valMin.uint32 = valMin;
	me->valMax.uint32 = valMax;
	me->valLen = sizeof(uint32_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.uint32 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorUint64
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.uint64 = valDef;
	me->valMin.uint64 = valMin;
	me->valMax.uint64 = valMax;
	me->valLen = sizeof(uint64_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.uint64 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt8
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int8 = valDef;
	me->valMin.int8 = valMin;
	me->valMax.int8 = valMax;
	me->valLen = sizeof(int8_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.int8 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt16
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int16 = valDef;
	me->valMin.int16 = valMin;
	me->valMax.int16 = valMax;
	me->valLen = sizeof(int16_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.int16 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt32
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int32 = valDef;
	me->valMin.int32 = valMin;
	me->valMax.int32 = valMax;
	me->valLen = sizeof(int32_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.int32 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorInt64
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef.int64 = valDef;
	me->valMin.int64 = valMin;
	me->valMax.int64 = valMax;
	me->valLen = sizeof(int64_t);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val.int64 = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorFloat
(
	AlxParamItem* me,
	const char* name,
	uint32_t id,
	uint32_t groupId,
	float valDef,
	float valMin,
	float valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Parameters
	me->type = AlxParamItem_Type_Float;
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef._float = valDef;
	me->valMin._float = valMin;
	me->valMax._float = valMax;
	me->valLen = sizeof(float);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val._float = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorDouble
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef._double = valDef;
	me->valMin._double = valMin;
	me->valMax._double = valMax;
	me->valLen = sizeof(double);
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	me->val._double = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorBool
(
	AlxParamItem* me,
	const char* name,
	uint32_t id,
	uint32_t groupId,
	bool valDef
)
{
	// Parameters
	me->type = AlxParamItem_Type_Bool;
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->valDef._bool = valDef;
	me->valMin._bool = ALX_NULL;
	me->valMax._bool = ALX_NULL;
	me->valLen = sizeof(bool);
	me->valOutOfRangeHandle = ALX_NULL;

	// Variables
	me->val._bool = valDef;

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorArr
(
	AlxParamItem* me,
	const char* name,
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
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->val.voidPtr = valBuff;
	me->valDef.voidPtr = valDefBuff;
	me->valMin.voidPtr = ALX_NULL_PTR;
	me->valMax.voidPtr = ALX_NULL_PTR;
	me->valLen = valBuffLen;
	me->valDefLen = ALX_NULL;
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	memcpy(me->valDef.voidPtr, valDef, me->valLen);				// Copy default value to default value buffer
	memcpy(me->val.voidPtr, me->valDef.voidPtr, me->valLen);	// Copy default value buffer to main buffer

	// Info
	me->wasCtorCalled = true;
}
void AlxParamItem_CtorStr
(
	AlxParamItem* me,
	const char* name,
	uint32_t id,
	uint32_t groupId,
	const char* valDef,
	char* valBuff,
	char* valDefBuff,
	uint32_t strMaxLen,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle
)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(strlen(valDefBuff) <= strMaxLen);

	// Parameters
	me->type = AlxParamItem_Type_Str;
	me->name = name;
	me->id = id;
	me->groupId = groupId;
	me->val.charPtr = valBuff;			// Set char pointer value to address of the buffer
	me->valDef.charPtr = valDefBuff;	// Set char pointer value to address of the buffer
	me->valLen = strMaxLen;
	me->valOutOfRangeHandle = valOutOfRangeHandle;

	// Variables
	strcpy(valDefBuff, valDef);			// Save default string to default buffer
	strcpy(valBuff, valDef);			// Save default string to buffer

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
const char* AlxParamItem_GetName(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->name;
}
uint32_t AlxParamItem_GetId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return me->id;
}
uint32_t AlxParamItem_GetGroupId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->groupId;
}
AlxParamItem_Type AlxParamItem_GetType(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->type;
}
void* AlxParamItem_GetValPtr(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	if((me->type == AlxParamItem_Type_Str) || (me->type == AlxParamItem_Type_Arr))
	{
		void* ptr;
		memcpy(&ptr, &me->val, 4);
		return ptr;
	}
	else return &me->val;
}
uint32_t AlxParamItem_GetValLen(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->valLen;
}
void AlxParamItem_SetValToDef(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// #2 Set value to default
	if		(me->type == AlxParamItem_Type_Uint8)	{ me->val.uint8 = me->valDef.uint8; }
	else if (me->type == AlxParamItem_Type_Uint16)	{ me->val.uint16 = me->valDef.uint16; }
	else if (me->type == AlxParamItem_Type_Uint32)	{ me->val.uint32 = me->valDef.uint32; }
	else if (me->type == AlxParamItem_Type_Uint64)	{ me->val.uint64 = me->valDef.uint64; }
	else if (me->type == AlxParamItem_Type_Int8)	{ me->val.int8 = me->valDef.int8; }
	else if (me->type == AlxParamItem_Type_Int16)	{ me->val.int16 = me->valDef.int16; }
	else if (me->type == AlxParamItem_Type_Int32)	{ me->val.int32 = me->valDef.int32; }
	else if (me->type == AlxParamItem_Type_Int64)	{ me->val.int64 = me->valDef.int64; }
	else if (me->type == AlxParamItem_Type_Float)	{ me->val._float = me->valDef._float; }
	else if (me->type == AlxParamItem_Type_Double)	{ me->val._double = me->valDef._double; }
	else if (me->type == AlxParamItem_Type_Bool)	{ me->val._bool = me->valDef._bool; }
	else if (me->type == AlxParamItem_Type_Uint16)	{ me->val.voidPtr = me->valDef.voidPtr; }
	else if (me->type == AlxParamItem_Type_Uint16)	{ strcpy(me->val.charPtr, me->valDef.charPtr); }
	else											{ ALX_PARAM_ITEM_ASSERT(false); }	// We should never get here
}
uint8_t AlxParamItem_GetValUint8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// #1 Return
	return me->val.uint8;
}
uint16_t AlxParamItem_GetValUint16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// #1 Return
	return me->val.uint16;
}
uint32_t AlxParamItem_GetValUint32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// #1 Return
	return me->val.uint32;
}
uint64_t AlxParamItem_GetValUint64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// #1 Return
	return me->val.uint64;
}
int8_t AlxParamItem_GetValInt8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// #1 Return
	return me->val.int8;
}
int16_t AlxParamItem_GetValInt16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// #1 Return
	return me->val.int16;
}
int32_t AlxParamItem_GetValInt32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// #1 Return
	return me->val.int32;
}
int64_t AlxParamItem_GetValInt64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// #1 Return
	return me->val.int64;
}
float AlxParamItem_GetValFloat(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// #1 Return
	return me->val._float;
}
double AlxParamItem_GetValDouble(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// #1 Return
	return me->val._double;
}
bool AlxParamItem_GetValBool(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);

	// #1 Return
	return me->val._bool;
}
Alx_Status AlxParamItem_SetValUint8(AlxParamItem* me, uint8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValUint16(AlxParamItem* me, uint16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint16_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValUint32(AlxParamItem* me, uint32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint32_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValUint64(AlxParamItem* me, uint64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint64_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValInt8(AlxParamItem* me, int8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	int8_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValInt16(AlxParamItem* me, int16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	int16_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValInt32(AlxParamItem* me, int32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	int32_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValInt64(AlxParamItem* me, int64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	int64_t _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValFloat(AlxParamItem* me, float val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	float _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValDouble(AlxParamItem* me, double val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	double _val = val;

	// #2 Handle value out of range
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

	// #3 Return
	return status;
}
Alx_Status AlxParamItem_SetValBool(AlxParamItem* me, bool val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);
	ALX_PARAM_ITEM_ASSERT(val == true || val == false);

	// #1 Handle value
	me->val._bool = val;

	// #2 Return
	return Alx_Ok;
}
void AlxParamItem_GetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// #1 Copy
	memcpy(val, me->val.voidPtr, me->valLen);
}
void AlxParamItem_SetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// #1 Copy
	memcpy(me->val.voidPtr, val, me->valLen);
}
void AlxParamItem_GetValStr(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Str);

	// #1 Copy
	strcpy(val, me->val.charPtr);
}
Alx_Status AlxParamItem_SetValStr(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Str);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Handle value out of range
	switch(me->valOutOfRangeHandle)
	{
		case AlxParamItem_ValOutOfRangeHandle_Assert:
		{
			status = AlxRange_CheckStr(val, me->valLen);
			if (status == Alx_Ok)
			{
				strcpy(me->val.charPtr, val);
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
			status = AlxRange_CheckStr(val, me->valLen);
			if (status == Alx_Ok)
			{
				strcpy(me->val.charPtr, val);
			}
			break;
		}
		case AlxParamItem_ValOutOfRangeHandle_Bound:
		{
			status = AlxBound_Str(me->val.charPtr, val, me->valLen);
			break;
		}
		default:
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
			status = Alx_Err;
			break;
		}
	}

	// #3 Return
	return status;
}
