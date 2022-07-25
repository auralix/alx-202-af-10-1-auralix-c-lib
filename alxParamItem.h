/**
  ******************************************************************************
  * @file		alxParamItem.h
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
#include "alxAssert.h"
#include "alxBound.h"
#include "alxRange.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_ITEM_FILE "alxParamItem.h"

// Assert //
#if defined(_ALX_PARAM_ITEM_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_ITEM_FILE, expr)
#elif defined(_ALX_PARAM_ITEM_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_ITEM_FILE, expr)
#elif defined(_ALX_PARAM_ITEM_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_ITEM_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_ITEM_FILE, expr)
#else
	#define ALX_PARAM_ITEM_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_ITEM_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_ITEM_TRACE(...) ALX_TRACE_STD(ALX_PARAM_ITEM_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_ITEM_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxParamItem_Type_Uint8 = 0,
	AlxParamItem_Type_Uint16 = 1,
	AlxParamItem_Type_Uint32 = 2,
	AlxParamItem_Type_Uint64 = 3,
	AlxParamItem_Type_Int8 = 4,
	AlxParamItem_Type_Int16 = 5,
	AlxParamItem_Type_Int32 = 6,
	AlxParamItem_Type_Int64 = 7,
	AlxParamItem_Type_Float = 8,
	AlxParamItem_Type_Double = 9,
	AlxParamItem_Type_Bool = 10,
	AlxParamItem_Type_Arr = 11,
	AlxParamItem_Type_Str = 12,
	AlxParamItem_Type_None = 255
} AlxParamItem_Type;

typedef enum
{
	AlxParamItem_ValOutOfRangeHandle_Assert = 0,
	AlxParamItem_ValOutOfRangeHandle_Ignore = 1,
	AlxParamItem_ValOutOfRangeHandle_Bound = 2
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
	void* voidPtr;
	char* charPtr;
} AlxParamItem_Val;

typedef struct
{
	// Parameters
	AlxParamItem_Type type;
	const char* name;
	uint32_t id;
	uint32_t groupId;
	AlxParamItem_Val valDef;	// MF: valDefBuff
	AlxParamItem_Val valMin;
	AlxParamItem_Val valMax;
	uint32_t valLen;			// MF: valBuffLen
	uint32_t valDefLen;
	AlxParamItem_ValOutOfRangeHandle valOutOfRangeHandle;

	// Variables
	AlxParamItem_Val val;		// MF: valBuff

	// Info
	bool wasCtorCalled;
} AlxParamItem;


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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
void AlxParamItem_CtorBool
(
	AlxParamItem* me,
	const char* name,
	uint32_t id,
	uint32_t groupId,
	bool valDef
);
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
);
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
);


//******************************************************************************
// Functions
//******************************************************************************
const char*			AlxParamItem_GetName		(AlxParamItem* me);
uint32_t			AlxParamItem_GetId			(AlxParamItem* me);
uint32_t			AlxParamItem_GetGroupId		(AlxParamItem* me);
AlxParamItem_Type	AlxParamItem_GetType		(AlxParamItem* me);
void*				AlxParamItem_GetValPtr		(AlxParamItem* me);
uint32_t			AlxParamItem_GetValLen		(AlxParamItem* me);
void				AlxParamItem_SetValToDef	(AlxParamItem* me);

uint8_t				AlxParamItem_GetValUint8	(AlxParamItem* me);
uint16_t			AlxParamItem_GetValUint16	(AlxParamItem* me);
uint32_t			AlxParamItem_GetValUint32	(AlxParamItem* me);
uint64_t			AlxParamItem_GetValUint64	(AlxParamItem* me);
int8_t				AlxParamItem_GetValInt8		(AlxParamItem* me);
int16_t				AlxParamItem_GetValInt16	(AlxParamItem* me);
int32_t				AlxParamItem_GetValInt32	(AlxParamItem* me);
int64_t				AlxParamItem_GetValInt64	(AlxParamItem* me);
float				AlxParamItem_GetValFloat	(AlxParamItem* me);
double				AlxParamItem_GetValDouble	(AlxParamItem* me);
bool				AlxParamItem_GetValBool		(AlxParamItem* me);

Alx_Status			AlxParamItem_SetValUint8	(AlxParamItem* me, uint8_t val	);
Alx_Status			AlxParamItem_SetValUint16	(AlxParamItem* me, uint16_t val	);
Alx_Status			AlxParamItem_SetValUint32	(AlxParamItem* me, uint32_t val	);
Alx_Status			AlxParamItem_SetValUint64	(AlxParamItem* me, uint64_t val	);
Alx_Status			AlxParamItem_SetValInt8		(AlxParamItem* me, int8_t val	);
Alx_Status			AlxParamItem_SetValInt16	(AlxParamItem* me, int16_t val	);
Alx_Status			AlxParamItem_SetValInt32	(AlxParamItem* me, int32_t val	);
Alx_Status			AlxParamItem_SetValInt64	(AlxParamItem* me, int64_t val	);
Alx_Status			AlxParamItem_SetValFloat	(AlxParamItem* me, float val	);
Alx_Status			AlxParamItem_SetValDouble	(AlxParamItem* me, double val	);
Alx_Status			AlxParamItem_SetValBool		(AlxParamItem* me, bool val		);

void				AlxParamItem_GetValArr		(AlxParamItem* me, void* val);
void				AlxParamItem_SetValArr		(AlxParamItem* me, void* val);
void				AlxParamItem_GetValStr		(AlxParamItem* me, char* val);
Alx_Status			AlxParamItem_SetValStr		(AlxParamItem* me, char* val);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_ITEM_H
