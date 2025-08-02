/**
  ******************************************************************************
  * @file		alxParamItem.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_PARAM_ITEM_H
#define ALX_PARAM_ITEM_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxBound.h"
#include "alxRange.h"
#include "alxFtoa.h"
#include "alxParamKvStore.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_ITEM_FILE "alxParamItem.h"

// Assert //
#if defined(ALX_PARAM_ITEM_ASSERT_BKPT_ENABLE)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_ITEM_FILE, expr)
#elif defined(ALX_PARAM_ITEM_ASSERT_TRACE_ENABLE)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_ITEM_FILE, expr)
#elif defined(ALX_PARAM_ITEM_ASSERT_RST_ENABLE)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_ITEM_FILE, expr)
#else
	#define ALX_PARAM_ITEM_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PARAM_ITEM_TRACE_ENABLE)
	#define ALX_PARAM_ITEM_TRACE_WRN(...) ALX_TRACE_WRN(ALX_PARAM_ITEM_FILE, __VA_ARGS__)
	#define ALX_PARAM_ITEM_TRACE_INF(...) ALX_TRACE_INF(ALX_PARAM_ITEM_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_ITEM_TRACE_WRN(...) do{} while (false)
	#define ALX_PARAM_ITEM_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxParamItem_Uint8,
	AlxParamItem_Uint16,
	AlxParamItem_Uint32,
	AlxParamItem_Uint64,
	AlxParamItem_Int8,
	AlxParamItem_Int16,
	AlxParamItem_Int32,
	AlxParamItem_Int64,
	AlxParamItem_Float,
	AlxParamItem_Double,
	AlxParamItem_Bool,
	AlxParamItem_Arr,
	AlxParamItem_Str
} AlxParamItem_DataType;

typedef enum
{
	AlxParamItem_Param,
	AlxParamItem_Var,
	AlxParamItem_Flag,
	AlxParamItem_Const,
	AlxParamItem_Trig
} AlxParamItem_ParamType;

typedef enum
{
	AlxParamItem_Assert,
	AlxParamItem_Ignore,
	AlxParamItem_Bound
} AlxParamItem_ValOutOfRangeHandle;

typedef union
{
	uint8_t uint8;
	uint16_t uint16;
	uint32_t uint32;
	uint64_t uint64;
	int8_t int8;
	int16_t int16;
	int32_t int32;
	int64_t int64;
	float _float;
	double _double;
	bool _bool;
	void* arr;
	char* str;
} AlxParamItem_Val;

typedef struct
{
	// Defines
	#define ALX_PARAM_ITEM_BUFF_LEN 128
	#define ALX_PARAM_ITEM_FTOA_PRECISION 6

	// Parameters
	AlxParamItem_DataType dataType;								// General
	AlxParamKvStore* paramKvStore;								// N/A
	AlxParamItem_ParamType paramType;							// General
	const char* key;											// General
	uint32_t id;												// General
	const char* groupKey;										// General
	uint32_t groupId;											// General
	AlxParamItem_Val valDef;									// Default / Min / Max
	AlxParamItem_Val valMin;									// Default / Min / Max
	AlxParamItem_Val valMax;									// Default / Min / Max
	uint32_t valLen;											// General
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle;		// General
	void* enumArr;												// Enum
	uint8_t enumArrLen;											// Enum
	const char* valUnit;										// General
	bool valChangeTakesEffectAfterReset;						// General
	uint8_t* buff;												// N/A
	uint32_t buffLen;											// General

	// Variables
	AlxParamItem_Val val;										// General
	bool pendingStore;											// N/A

	// Info
	bool wasCtorCalled;											// N/A
} AlxParamItem;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamItem_CtorUint8
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	uint8_t valDef,
	uint8_t valMin,
	uint8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint8_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorUint16
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	uint16_t valDef,
	uint16_t valMin,
	uint16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint16_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorUint32
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	uint32_t valDef,
	uint32_t valMin,
	uint32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint32_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorUint64
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	uint64_t valDef,
	uint64_t valMin,
	uint64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	uint64_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorInt8
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	int8_t valDef,
	int8_t valMin,
	int8_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int8_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorInt16
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	int16_t valDef,
	int16_t valMin,
	int16_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int16_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorInt32
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	int32_t valDef,
	int32_t valMin,
	int32_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int32_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorInt64
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	int64_t valDef,
	int64_t valMin,
	int64_t valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	int64_t* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorFloat
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	float valDef,
	float valMin,
	float valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	float* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorDouble
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	double valDef,
	double valMin,
	double valMax,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	double* enumArr,
	uint8_t enumArrLen,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorBool
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	bool valDef,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorArr
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	void* valDef,
	void* valBuff,
	void* valDefBuff,
	uint32_t valBuffLen,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset
);
void AlxParamItem_CtorStr
(
	AlxParamItem* me,
	AlxParamKvStore* paramKvStore,
	AlxParamItem_ParamType paramType,
	const char* key,
	uint32_t id,
	const char* groupKey,
	uint32_t groupId,
	const char* valDef,
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle,
	const char* valUnit,
	bool valChangeTakesEffectAfterReset,
	uint8_t* buff,
	uint32_t buffLen
);


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
AlxParamItem_DataType AlxParamItem_GetDataType(AlxParamItem* me);
AlxParamItem_ParamType AlxParamItem_GetParamType(AlxParamItem* me);
const char* AlxParamItem_GetKey(AlxParamItem* me);
uint32_t AlxParamItem_GetId(AlxParamItem* me);
const char* AlxParamItem_GetGroupKey(AlxParamItem* me);
uint32_t AlxParamItem_GetGroupId(AlxParamItem* me);
uint32_t AlxParamItem_GetValLen(AlxParamItem* me);
AlxParamItem_ValOutOfRangeHandle AlxParamItem_GetValOutOfRangeHandle(AlxParamItem* me);
const char* AlxParamItem_GetValUnit(AlxParamItem* me);
bool AlxParamItem_GetValChangeTakesEffectAfterReset(AlxParamItem* me);
uint32_t AlxParamItem_GetBuffLen(AlxParamItem* me);
void* AlxParamItem_GetValPtr(AlxParamItem* me);


//------------------------------------------------------------------------------
// Get
//------------------------------------------------------------------------------
uint8_t AlxParamItem_GetValUint8(AlxParamItem* me);
uint16_t AlxParamItem_GetValUint16(AlxParamItem* me);
uint32_t AlxParamItem_GetValUint32(AlxParamItem* me);
uint64_t AlxParamItem_GetValUint64(AlxParamItem* me);
int8_t AlxParamItem_GetValInt8(AlxParamItem* me);
int16_t AlxParamItem_GetValInt16(AlxParamItem* me);
int32_t AlxParamItem_GetValInt32(AlxParamItem* me);
int64_t AlxParamItem_GetValInt64(AlxParamItem* me);
float AlxParamItem_GetValFloat(AlxParamItem* me);
double AlxParamItem_GetValDouble(AlxParamItem* me);
bool AlxParamItem_GetValBool(AlxParamItem* me);
void AlxParamItem_GetValArr(AlxParamItem* me, void* val);
Alx_Status AlxParamItem_GetValStr(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm);


//------------------------------------------------------------------------------
// Set
//------------------------------------------------------------------------------
Alx_Status AlxParamItem_SetValUint8(AlxParamItem* me, uint8_t val);
Alx_Status AlxParamItem_SetValUint16(AlxParamItem* me, uint16_t val);
Alx_Status AlxParamItem_SetValUint32(AlxParamItem* me, uint32_t val);
Alx_Status AlxParamItem_SetValUint64(AlxParamItem* me, uint64_t val);
Alx_Status AlxParamItem_SetValInt8(AlxParamItem* me, int8_t val);
Alx_Status AlxParamItem_SetValInt16(AlxParamItem* me, int16_t val);
Alx_Status AlxParamItem_SetValInt32(AlxParamItem* me, int32_t val);
Alx_Status AlxParamItem_SetValInt64(AlxParamItem* me, int64_t val);
Alx_Status AlxParamItem_SetValFloat(AlxParamItem* me, float val);
Alx_Status AlxParamItem_SetValDouble(AlxParamItem* me, double val);
Alx_Status AlxParamItem_SetValBool(AlxParamItem* me, bool val);
void AlxParamItem_SetValArr(AlxParamItem* me, void* val);
Alx_Status AlxParamItem_SetValStr(AlxParamItem* me, char* val);


//------------------------------------------------------------------------------
// Default / Min / Max
//------------------------------------------------------------------------------
void AlxParamItem_SetValToDef(AlxParamItem* me);
uint8_t AlxParamItem_GetValDefUint8(AlxParamItem* me);
uint16_t AlxParamItem_GetValDefUint16(AlxParamItem* me);
uint32_t AlxParamItem_GetValDefUint32(AlxParamItem* me);
uint64_t AlxParamItem_GetValDefUint64(AlxParamItem* me);
int8_t AlxParamItem_GetValDefInt8(AlxParamItem* me);
int16_t AlxParamItem_GetValDefInt16(AlxParamItem* me);
int32_t AlxParamItem_GetValDefInt32(AlxParamItem* me);
int64_t AlxParamItem_GetValDefInt64(AlxParamItem* me);
float AlxParamItem_GetValDefFloat(AlxParamItem* me);
double AlxParamItem_GetValDefDouble(AlxParamItem* me);
bool AlxParamItem_GetValDefBool(AlxParamItem* me);
void* AlxParamItem_GetValDefArr(AlxParamItem* me);
const char* AlxParamItem_GetValDefStr(AlxParamItem* me);
uint8_t AlxParamItem_GetValMinUint8(AlxParamItem* me);
uint16_t AlxParamItem_GetValMinUint16(AlxParamItem* me);
uint32_t AlxParamItem_GetValMinUint32(AlxParamItem* me);
uint64_t AlxParamItem_GetValMinUint64(AlxParamItem* me);
int8_t AlxParamItem_GetValMinInt8(AlxParamItem* me);
int16_t AlxParamItem_GetValMinInt16(AlxParamItem* me);
int32_t AlxParamItem_GetValMinInt32(AlxParamItem* me);
int64_t AlxParamItem_GetValMinInt64(AlxParamItem* me);
float AlxParamItem_GetValMinFloat(AlxParamItem* me);
double AlxParamItem_GetValMinDouble(AlxParamItem* me);
uint8_t AlxParamItem_GetValMaxUint8(AlxParamItem* me);
uint16_t AlxParamItem_GetValMaxUint16(AlxParamItem* me);
uint32_t AlxParamItem_GetValMaxUint32(AlxParamItem* me);
uint64_t AlxParamItem_GetValMaxUint64(AlxParamItem* me);
int8_t AlxParamItem_GetValMaxInt8(AlxParamItem* me);
int16_t AlxParamItem_GetValMaxInt16(AlxParamItem* me);
int32_t AlxParamItem_GetValMaxInt32(AlxParamItem* me);
int64_t AlxParamItem_GetValMaxInt64(AlxParamItem* me);
float AlxParamItem_GetValMaxFloat(AlxParamItem* me);
double AlxParamItem_GetValMaxDouble(AlxParamItem* me);


//------------------------------------------------------------------------------
// Enum
//------------------------------------------------------------------------------
void AlxParamItem_GetEnumArrUint8(AlxParamItem* me, uint8_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrUint16(AlxParamItem* me, uint16_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrUint32(AlxParamItem* me, uint32_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrUint64(AlxParamItem* me, uint64_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrInt8(AlxParamItem* me, int8_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrInt16(AlxParamItem* me, int16_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrInt32(AlxParamItem* me, int32_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrInt64(AlxParamItem* me, int64_t** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrFloat(AlxParamItem* me, float** enumArr, uint8_t* enumArrLen);
void AlxParamItem_GetEnumArrDouble(AlxParamItem* me, double** enumArr, uint8_t* enumArrLen);


//------------------------------------------------------------------------------
// Get & Set String Format
//------------------------------------------------------------------------------
Alx_Status AlxParamItem_GetVal_StrFormat(AlxParamItem* me, char* val, uint32_t maxLenWithNullTerm);
Alx_Status AlxParamItem_SetVal_StrFormat(AlxParamItem* me, char* val);


//------------------------------------------------------------------------------
// Load & Store
//------------------------------------------------------------------------------
Alx_Status AlxParamItem_LoadVal(AlxParamItem* me);
Alx_Status AlxParamItem_StoreVal(AlxParamItem* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_ITEM_H
