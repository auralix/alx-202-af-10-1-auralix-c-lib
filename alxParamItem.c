/**
  ******************************************************************************
  * @file		alxParamItem.c
  * @brief		Auralix C Library - ALX Parameter Item Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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
static void AlxParamItem_Ctor
(
	AlxParamItem* me,
	AlxParamItem_Type type,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	void* valDef,
	void* valMin,
	void* valMax,
	uint32_t valLen,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	void* enumArr,
	uint8_t numOfEnums,
	uint8_t* buff,
	uint32_t buffLen
);
static bool AlxParamItem_AreEnumArrValFromLowToHigh(AlxParamItem* me);
static bool AlxParamItem_IsEnumOnList(AlxParamItem* me, void* enumVal);
static void* AlxParamItem_GetValPtr_Private(AlxParamItem* me);
static Alx_Status AlxParamItem_SetVal(AlxParamItem* me, void* val);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorUint8
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint8_t valDef,
	uint8_t valMin,
	uint8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint8_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Uint8,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(uint8_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramKvStore
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorUint16
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint16_t valDef,
	uint16_t valMin,
	uint16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint16_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Uint16,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(uint16_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorUint32
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint32_t valDef,
	uint32_t valMin,
	uint32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint32_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Uint32,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(uint32_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorUint64
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	uint64_t valDef,
	uint64_t valMin,
	uint64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint64_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Uint64,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(uint64_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorInt8
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int8_t valDef,
	int8_t valMin,
	int8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int8_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Int8,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(int8_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorInt16
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int16_t valDef,
	int16_t valMin,
	int16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int16_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Int16,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(int16_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorInt32
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int32_t valDef,
	int32_t valMin,
	int32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int32_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Int32,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(int32_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorInt64
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	int64_t valDef,
	int64_t valMin,
	int64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int64_t* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Int64,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(int64_t),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramKvStore
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorFloat
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
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
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Float,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(float),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valMin
  * @param[in]		valMax
  * @param[in]		valOutOfRangeHandle
  */
void AlxParamItem_CtorDouble
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	double valDef,
	double valMin,
	double valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	double* enumArr,
	uint8_t numOfEnums
)
{
	AlxParamItem_Ctor
	(
		me,
		AlxParamItem_Type_Double,
		paramKvStore,
		key,
		id,
		groupId,
		&valDef,
		&valMin,
		&valMax,
		sizeof(double),
		valOutOfRangeHandle,
		enumArr,
		numOfEnums,
		ALX_NULL_PTR,
		ALX_NULL
	);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramKvStore
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  */
void AlxParamItem_CtorBool
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	bool valDef
)
{
	// Parameters
	me->type = AlxParamItem_Type_Bool;
	me->paramKvStore = paramKvStore;
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

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valBuff
  * @param[in]		valDefBuff
  * @param[in]		valBuffLen
  * @param[in]		valOutOfRangeHandle
  */
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

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramKvStore
  * @param[in]		key
  * @param[in]		id
  * @param[in]		groupId
  * @param[in]		valDef
  * @param[in]		valOutOfRangeHandle
  * @param[in,out]	buff
  * @param[in]		buffLen
  */
void AlxParamItem_CtorStr
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
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
	me->paramKvStore = paramKvStore;
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

//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxParamItem_GetKey(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->key;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxParamItem_GetId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->id;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxParamItem_GetGroupId(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->groupId;
}

/**
  * @brief
  * @param[in,out]	me
  * @retva			AlxParamItem_Type_Uint8
  * @retva			AlxParamItem_Type_Uint16
  * @retva			AlxParamItem_Type_Uint32
  * @retva			AlxParamItem_Type_Uint64
  * @retva			AlxParamItem_Type_Int8
  * @retva			AlxParamItem_Type_Int16
  * @retva			AlxParamItem_Type_Int32
  * @retva			AlxParamItem_Type_Int64
  * @retva			AlxParamItem_Type_Float
  * @retva			AlxParamItem_Type_Double
  * @retva			AlxParamItem_Type_Bool
  * @retva			AlxParamItem_Type_Arr
  * @retva			AlxParamItem_Type_Str
  * @retva			AlxParamItem_Type_None
  */
AlxParamItem_Type AlxParamItem_GetType(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->type;
}

/**
  * @brief
  * @param[in,out]	me
  */
void* AlxParamItem_GetValPtr(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return AlxParamItem_GetValPtr_Private(me);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxParamItem_GetValLen(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->valLen;
}


//------------------------------------------------------------------------------
// Get
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxParamItem_GetValUint8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// Return
	return me->val.uint8;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint16_t AlxParamItem_GetValUint16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// Return
	return me->val.uint16;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxParamItem_GetValUint32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// Return
	return me->val.uint32;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxParamItem_GetValUint64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// Return
	return me->val.uint64;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
int8_t AlxParamItem_GetValInt8(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// Return
	return me->val.int8;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
int16_t AlxParamItem_GetValInt16(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// Return
	return me->val.int16;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
int32_t AlxParamItem_GetValInt32(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// Return
	return me->val.int32;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
int64_t AlxParamItem_GetValInt64(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// Return
	return me->val.int64;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxParamItem_GetValFloat(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// Return
	return me->val._float;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
double AlxParamItem_GetValDouble(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// Return
	return me->val._double;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamItem_GetValBool(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);

	// Return
	return me->val._bool;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		val
  */
void AlxParamItem_GetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// Copy
	memcpy(val, me->val.arr, me->valLen);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		val
  * @param[in]		maxLenWithNullTerm
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
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


//------------------------------------------------------------------------------
// Set
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValUint8(AlxParamItem* me, uint8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint8);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValUint16(AlxParamItem* me, uint16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint16);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValUint32(AlxParamItem* me, uint32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint32);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValUint64(AlxParamItem* me, uint64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Uint64);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValInt8(AlxParamItem* me, int8_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int8);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValInt16(AlxParamItem* me, int16_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int16);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValInt32(AlxParamItem* me, int32_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int32);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValInt64(AlxParamItem* me, int64_t val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Int64);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValFloat(AlxParamItem* me, float val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Float);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValDouble(AlxParamItem* me, double val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Double);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValBool(AlxParamItem* me, bool val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Bool);

	// Set
	return AlxParamItem_SetVal(me, &val);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  */
void AlxParamItem_SetValArr(AlxParamItem* me, void* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Arr);

	// Set
	memcpy(me->val.arr, val, me->valLen);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetValStr(AlxParamItem* me, char* val)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_ITEM_ASSERT(me->type == AlxParamItem_Type_Str);

	// Set
	return AlxParamItem_SetVal(me, val);
}


//------------------------------------------------------------------------------
// Set Default
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out] me
  */
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


//------------------------------------------------------------------------------
// Get & Set String Format
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		val
  * @param[in]		maxLenWithNullTerm
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_GetVal_StrFormat(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);
	if (me->type != AlxParamItem_Type_Str)
	{
		ALX_PARAM_ITEM_ASSERT(maxLenWithNullTerm <= ALX_PARAM_ITEM_BUFF_LEN);
	}


	//------------------------------------------------------------------------------
	// Local variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	char valStr[ALX_PARAM_ITEM_BUFF_LEN] = "";




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle String
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if (me->type == AlxParamItem_Type_Str)
	{
		// Check
		status = AlxRange_CheckStr(me->val.str, maxLenWithNullTerm);
		if (status != Alx_Ok)
		{
			return status;
		}

		// Copy
		strcpy(val, me->val.str);

		// Return
		return status;
	}




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle Uint8/16/32/64, Int8/16/32/64, Float, Double, Bool
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------


	//------------------------------------------------------------------------------
	// Convert
	//------------------------------------------------------------------------------
	if (me->type == AlxParamItem_Type_Uint8)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%u", me->val.uint8) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Uint16)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%hu", me->val.uint16) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Uint32)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%lu", me->val.uint32) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Uint64)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Int8)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%d", me->val.int8) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Int16)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%hd", me->val.int16) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Int32)
	{
		ALX_PARAM_ITEM_ASSERT(sprintf(valStr, "%ld", me->val.int32) >= 0);
	}
	else if (me->type == AlxParamItem_Type_Int64)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Float)
	{
		AlxFtoa(me->val._float, valStr, ALX_PARAM_ITEM_FTOA_PRECISION);
	}
	else if (me->type == AlxParamItem_Type_Double)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Bool)
	{
		if (me->val._bool)
		{
			strcpy(valStr, "true");
		}
		else
		{
			strcpy(valStr, "false");
		}
	}
	else if (me->type == AlxParamItem_Type_Arr)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Str)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
	}
	else
	{
		ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	status = AlxRange_CheckStr(valStr, maxLenWithNullTerm);
	if (status != Alx_Ok)
	{
		return status;
	}


	//------------------------------------------------------------------------------
	// Copy
	//------------------------------------------------------------------------------
	strcpy(val, valStr);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamItem_SetVal_StrFormat(AlxParamItem* me, char* val)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Local variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Convert
	//------------------------------------------------------------------------------
	if (me->type == AlxParamItem_Type_Uint8)
	{
		// Convert
		uint8_t valNum = 0;
		if (sscanf(val, "%hu", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValUint8(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Uint16)
	{
		// Convert
		uint16_t valNum = 0;
		if (sscanf(val, "%hu", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}
		status = AlxParamItem_SetValUint16(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Uint32)
	{
		// Convert
		uint32_t valNum = 0;
		if (sscanf(val, "%lu", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValUint32(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Uint64)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Int8)
	{
		// Convert
		int8_t valNum = 0;
		if (sscanf(val, "%hd", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValInt8(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Int16)
	{
		// Convert
		int16_t valNum = 0;
		if (sscanf(val, "%hd", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValInt16(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Int32)
	{
		// Convert
		int32_t valNum = 0;
		if (sscanf(val, "%ld", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValInt32(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Int64)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Float)
	{
		// Convert
		float valNum = 0;
		if (sscanf(val, "%f", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValFloat(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Double)
	{
		// Convert
		double valNum = 0;
		if (sscanf(val, "%lf", &valNum) != 1)
		{
			return AlxParamItem_ErrConv;
		}

		// Set
		status = AlxParamItem_SetValDouble(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Bool)
	{
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

		// Set
		status = AlxParamItem_SetValBool(me, valNum);
	}
	else if (me->type == AlxParamItem_Type_Arr)
	{
		ALX_PARAM_ITEM_ASSERT(false);	// TODO
	}
	else if (me->type == AlxParamItem_Type_Str)
	{
		// Set
		status = AlxParamItem_SetValStr(me, val);
	}
	else
	{
		ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}


//------------------------------------------------------------------------------
// Load & Store
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
Alx_Status AlxParamItem_LoadVal(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Local variables
	void* buff = NULL;
	uint32_t buffLen = 0;
	uint32_t actualValLen = 0;
	Alx_Status status = Alx_Err;

	// Set buffLen
	if (me->type == AlxParamItem_Type_Str)
	{
		buffLen = me->buffLen;
	}
	else
	{
		buffLen = me->valLen;
	}

	// Allocate memory
	buff = calloc(buffLen, sizeof(uint8_t));
	if(buff == NULL) { ALX_PARAM_ITEM_TRACE("Err"); free(buff); return Alx_Err; }

	// Get value from Param KV Store
	status = AlxParamKvStore_Get(me->paramKvStore, me->key, buff, buffLen, &actualValLen);
	if (status != Alx_Ok)
	{
		// Trace
		ALX_PARAM_ITEM_TRACE_FORMAT("ALX param KV store get error - Key not found, default will be used - %s\r\n", me->key);

		// Free memory
		free(buff);

		// Return
		return Alx_Ok;
	}

	// Set Param Item
	status = AlxParamItem_SetVal(me, buff);

	// Free memory
	free(buff);

	// If Param Item set error
	if (status != Alx_Ok)
	{
		// Remove Param KV Store key
		status = AlxParamKvStore_Remove(me->paramKvStore, me->key);
		if(status != Alx_Ok) { ALX_PARAM_ITEM_TRACE("Err"); return Alx_Err; }

		// Trace
		ALX_PARAM_ITEM_TRACE_FORMAT("ALX param KV store read OK - ALX param item set error - Key was removed, default will be used - %s\r\n", me->key);
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
Alx_Status AlxParamItem_StoreVal(AlxParamItem* me)
{
	// Assert
	ALX_PARAM_ITEM_ASSERT(me->wasCtorCalled == true);

	// Get value pointer
	void* valPtr = AlxParamItem_GetValPtr_Private(me);

	// Set Param KV Store
	Alx_Status status = AlxParamKvStore_Set(me->paramKvStore, me->key, valPtr, me->valLen);
	if(status != Alx_Ok) { ALX_PARAM_ITEM_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxParamItem_Ctor
(
	AlxParamItem* me,
	AlxParamItem_Type type,
	AlxParamKvStore* paramKvStore,
	const char* key,
	uint32_t id,
	uint32_t groupId,
	void* valDef,
	void* valMin,
	void* valMax,
	uint32_t valLen,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	void* enumArr,
	uint8_t numOfEnums,
	uint8_t* buff,
	uint32_t buffLen
)
{
	// Parameters
	me->type = type;
	me->paramKvStore = paramKvStore;
	me->key = key;
	me->id = id;
	me->groupId = groupId;
	memcpy(&me->valDef, valDef, valLen);
	memcpy(&me->valMin, valMin, valLen);
	memcpy(&me->valMax, valMax, valLen);
	me->valLen = valLen;
	me->valOutOfRangeHandle = valOutOfRangeHandle;
	me->enumArr = enumArr;
	me->numOfEnums = numOfEnums;
	me->buff = buff;
	me->buffLen = buffLen;

	// Variables
	memcpy(&me->val, valDef, valLen);

	// Check if enum
	if (me->enumArr != ALX_NULL)
	{
		// Check if enum array value are from low to high
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_AreEnumArrValFromLowToHigh(me) == true);

		// Check if enum number is on the list
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList(me, valDef) == true);
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList(me, valMin) == true);
		ALX_PARAM_ITEM_ASSERT(AlxParamItem_IsEnumOnList(me, valMax) == true);
	}

	// Info
	me->wasCtorCalled = true;
}
static bool AlxParamItem_AreEnumArrValFromLowToHigh(AlxParamItem* me)
{
	// Check if enum array values are from low to high
	for (uint8_t i = 0; i < me->numOfEnums - 1; i++)
	{
		if (me->type == AlxParamItem_Type_Uint8)
		{
			// Dereference
			uint8_t _enumArrValCurrent = *((uint8_t*)me->enumArr + i);
			uint8_t _enumArrValNext = *((uint8_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint16)
		{
			// Dereference
			uint16_t _enumArrValCurrent = *((uint16_t*)me->enumArr + i);
			uint16_t _enumArrValNext = *((uint16_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint32)
		{
			// Dereference
			uint32_t _enumArrValCurrent = *((uint32_t*)me->enumArr + i);
			uint32_t _enumArrValNext = *((uint32_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint64)
		{
			// Dereference
			uint64_t _enumArrValCurrent = *((uint64_t*)me->enumArr + i);
			uint64_t _enumArrValNext = *((uint64_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Int8)
		{
			// Dereference
			int8_t _enumArrValCurrent = *((int8_t*)me->enumArr + i);
			int8_t _enumArrValNext = *((int8_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Int16)
		{
			// Dereference
			uint16_t _enumArrValCurrent = *((uint16_t*)me->enumArr + i);
			uint16_t _enumArrValNext = *((uint16_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Int32)
		{
			// Dereference
			uint32_t _enumArrValCurrent = *((uint32_t*)me->enumArr + i);
			uint32_t _enumArrValNext = *((uint32_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Int64)
		{
			// Dereference
			uint64_t _enumArrValCurrent = *((uint64_t*)me->enumArr + i);
			uint64_t _enumArrValNext = *((uint64_t*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Float)
		{
			// Dereference
			float _enumArrValCurrent = *((float*)me->enumArr + i);
			float _enumArrValNext = *((float*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Double)
		{
			// Dereference
			double _enumArrValCurrent = *((double*)me->enumArr + i);
			double _enumArrValNext = *((double*)me->enumArr + i + 1);

			// If enum array value current is NOT less then enum array value next, return false (enum array values are NOT from low to high)
			if ((_enumArrValCurrent < _enumArrValNext) == false)
			{
				return false;
			}
		}
		else if (me->type == AlxParamItem_Type_Bool)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Arr)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Str)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
	}

	// If we are here, enum array values are from low to high, so return true
	return true;
}
static bool AlxParamItem_IsEnumOnList(AlxParamItem* me, void* enumVal)
{
	// Check if enum value is on the list
	for (uint8_t i = 0; i < me->numOfEnums; i++)
	{
		if (me->type == AlxParamItem_Type_Uint8)
		{
			// Dereference
			uint8_t _enumVal = *(uint8_t*)enumVal;
			uint8_t _enumArrVal = *((uint8_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint16)
		{
			// Dereference
			uint16_t _enumVal = *(uint16_t*)enumVal;
			uint16_t _enumArrVal = *((uint16_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint32)
		{
			// Dereference
			uint32_t _enumVal = *(uint32_t*)enumVal;
			uint32_t _enumArrVal = *((uint32_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint64)
		{
			// Dereference
			uint64_t _enumVal = *(uint64_t*)enumVal;
			uint64_t _enumArrVal = *((uint64_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Int8)
		{
			// Dereference
			int8_t _enumVal = *(int8_t*)enumVal;
			int8_t _enumArrVal = *((int8_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Int16)
		{
			// Dereference
			uint16_t _enumVal = *(uint16_t*)enumVal;
			uint16_t _enumArrVal = *((uint16_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Int32)
		{
			// Dereference
			uint32_t _enumVal = *(uint32_t*)enumVal;
			uint32_t _enumArrVal = *((uint32_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Int64)
		{
			// Dereference
			uint64_t _enumVal = *(uint64_t*)enumVal;
			uint64_t _enumArrVal = *((uint64_t*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Float)
		{
			// Dereference
			float _enumVal = *(float*)enumVal;
			float _enumArrVal = *((float*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Double)
		{
			// Dereference
			double _enumVal = *(double*)enumVal;
			double _enumArrVal = *((double*)me->enumArr + i);

			// If enum value is on the list, return true
			if (_enumVal == _enumArrVal)
			{
				return true;
			}
		}
		else if (me->type == AlxParamItem_Type_Bool)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Arr)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Str)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
	}

	// If we are here, enum value was not found on the list, so return false
	return false;
}
static void* AlxParamItem_GetValPtr_Private(AlxParamItem* me)
{
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
static Alx_Status AlxParamItem_SetVal(AlxParamItem* me, void* val)
{
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle Bool
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if (me->type == AlxParamItem_Type_Bool)
	{
		bool _val = *(bool*)val;
		me->val._bool = _val;
		return Alx_Ok;
	}




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle Uint8/16/32/64, Int8/16/32/64, Float, Double, String
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if enum
	if (me->enumArr != NULL)
	{
		// Check if enum is on the list
		bool isEnumOnList = AlxParamItem_IsEnumOnList(me, val);
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
	if((me->valOutOfRangeHandle == AlxParamItem_ValOutOfRangeHandle_Assert) || (me->valOutOfRangeHandle == AlxParamItem_ValOutOfRangeHandle_Ignore))
	{
		// Set
		if (me->type == AlxParamItem_Type_Uint8)
		{
			uint8_t _val = *(uint8_t*)val;
			status = AlxRange_CheckUint8(_val, me->valMin.uint8, me->valMax.uint8);
			if (status == Alx_Ok)
			{
				me->val.uint8 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint16)
		{
			uint16_t _val = *(uint16_t*)val;
			status = AlxRange_CheckUint16(_val, me->valMin.uint16, me->valMax.uint16);
			if (status == Alx_Ok)
			{
				me->val.uint16 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint32)
		{
			uint32_t _val = *(uint32_t*)val;
			status = AlxRange_CheckUint32(_val, me->valMin.uint32, me->valMax.uint32);
			if (status == Alx_Ok)
			{
				me->val.uint32 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Uint64)
		{
			uint64_t _val = *(uint64_t*)val;
			status = AlxRange_CheckUint64(_val, me->valMin.uint64, me->valMax.uint64);
			if (status == Alx_Ok)
			{
				me->val.uint64 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Int8)
		{
			int8_t _val = *(int8_t*)val;
			status = AlxRange_CheckInt8(_val, me->valMin.int8, me->valMax.int8);
			if (status == Alx_Ok)
			{
				me->val.int8 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Int16)
		{
			int16_t _val = *(int16_t*)val;
			status = AlxRange_CheckInt16(_val, me->valMin.int16, me->valMax.int16);
			if (status == Alx_Ok)
			{
				me->val.int16 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Int32)
		{
			int32_t _val = *(int32_t*)val;
			status = AlxRange_CheckInt32(_val, me->valMin.int32, me->valMax.int32);
			if (status == Alx_Ok)
			{
				me->val.int32 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Int64)
		{
			int64_t _val = *(int64_t*)val;
			status = AlxRange_CheckInt64(_val, me->valMin.int64, me->valMax.int64);
			if (status == Alx_Ok)
			{
				me->val.int64 = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Float)
		{
			float _val = *(float*)val;
			status = AlxRange_CheckFloat(_val, me->valMin._float, me->valMax._float);
			if (status == Alx_Ok)
			{
				me->val._float = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Double)
		{
			double _val = *(double*)val;
			status = AlxRange_CheckDouble(_val, me->valMin._double, me->valMax._double);
			if (status == Alx_Ok)
			{
				me->val._double = _val;
			}
		}
		else if (me->type == AlxParamItem_Type_Bool)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Arr)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Str)
		{
			char* _val = (char*)val;
			status = AlxRange_CheckStr(_val, me->buffLen);
			if (status == Alx_Ok)
			{
				strcpy(me->val.str, _val);
				me->valLen = strlen(me->val.str) + 1;	// Add +1 for null terminator
			}
		}
		else
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}

		// If ValOutOfRangeHandle_Assert and status is NOT OK, assert
		if ((me->valOutOfRangeHandle == AlxParamItem_ValOutOfRangeHandle_Assert) && (status != Alx_Ok))
		{
			ALX_PARAM_ITEM_ASSERT(false);
			status = Alx_Err;
		}
	}
	else if (me->valOutOfRangeHandle == AlxParamItem_ValOutOfRangeHandle_Bound)
	{
		// Set
		if (me->type == AlxParamItem_Type_Uint8)
		{
			uint8_t _val = *(uint8_t*)val;
			status = AlxBound_Uint8(&_val, me->valMin.uint8, me->valMax.uint8);
			me->val.uint8 = _val;
		}
		else if (me->type == AlxParamItem_Type_Uint16)
		{
			uint16_t _val = *(uint16_t*)val;
			status = AlxBound_Uint16(&_val, me->valMin.uint16, me->valMax.uint16);
			me->val.uint16 = _val;
		}
		else if (me->type == AlxParamItem_Type_Uint32)
		{
			uint32_t _val = *(uint32_t*)val;
			status = AlxBound_Uint32(&_val, me->valMin.uint32, me->valMax.uint32);
			me->val.uint32 = _val;
		}
		else if (me->type == AlxParamItem_Type_Uint64)
		{
			uint64_t _val = *(uint64_t*)val;
			status = AlxBound_Uint64(&_val, me->valMin.uint64, me->valMax.uint64);
			me->val.uint64 = _val;
		}
		else if (me->type == AlxParamItem_Type_Int8)
		{
			int8_t _val = *(int8_t*)val;
			status = AlxBound_Int8(&_val, me->valMin.int8, me->valMax.int8);
			me->val.int8 = _val;
		}
		else if (me->type == AlxParamItem_Type_Int16)
		{
			int16_t _val = *(int16_t*)val;
			status = AlxBound_Int16(&_val, me->valMin.int16, me->valMax.int16);
			me->val.int16 = _val;
		}
		else if (me->type == AlxParamItem_Type_Int32)
		{
			int32_t _val = *(int32_t*)val;
			status = AlxBound_Int32(&_val, me->valMin.int32, me->valMax.int32);
			me->val.int32 = _val;
		}
		else if (me->type == AlxParamItem_Type_Int64)
		{
			int64_t _val = *(int64_t*)val;
			status = AlxBound_Int64(&_val, me->valMin.int64, me->valMax.int64);
			me->val.int64 = _val;
		}
		else if (me->type == AlxParamItem_Type_Float)
		{
			float _val = *(float*)val;
			status = AlxBound_Float(&_val, me->valMin._float, me->valMax._float);
			me->val._float = _val;
		}
		else if (me->type == AlxParamItem_Type_Double)
		{
			double _val = *(double*)val;
			status = AlxBound_Double(&_val, me->valMin._double, me->valMax._double);
			me->val._double = _val;
		}
		else if (me->type == AlxParamItem_Type_Bool)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Arr)
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
		else if (me->type == AlxParamItem_Type_Str)
		{
			char* _val = (char*)val;
			status = AlxBound_Str(me->val.str, _val, me->buffLen);
			me->valLen = strlen(me->val.str) + 1;	// Add +1 for null terminator
		}
		else
		{
			ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		}
	}
	else
	{
		ALX_PARAM_ITEM_ASSERT(false);	// We should never get here
		status = Alx_Err;
	}

	// Return
	return status;
}


#endif // #if defined(ALX_C_LIB)
