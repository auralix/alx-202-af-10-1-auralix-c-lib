/**
  ******************************************************************************
  * @file		alxParamMgmt.c
  * @brief		Auralix C Library - ALX Parameter Management Module
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
#include "alxParamMgmt.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramItemArr
  * @param[in]		numOfParamItems
  */
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem* paramItemArr,
	uint32_t numOfParamItems
)
{
	// Parameters
	me->paramItemArr = paramItemArr;
	me->numOfParamItems = numOfParamItems;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxParamMgmt_GetNumOfParamItems(AlxParamMgmt* me)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->numOfParamItems;
}
const char* AlxParamMgmt_ByIndex_GetKey(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetKey(ptr);
}
AlxParamItem_Type AlxParamMgmt_ByIndex_GetType(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetType(ptr);
}
Alx_Status AlxParamMgmt_ByIndex_GetVal_StrFormat(AlxParamMgmt* me, uint32_t index, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Get type
	AlxParamItem_Type type = AlxParamItem_GetType(ptr);

	// Execute
	Alx_Status status = Alx_Err;
	if (type == AlxParamItem_Type_Uint8)
	{
		status = AlxParamItem_GetValUint8_StrFormat(ptr, val, maxLenWithNullTerm);
	}
	else if (type == AlxParamItem_Type_Uint16)
	{
		status = AlxParamItem_GetValUint16_StrFormat(ptr, val, maxLenWithNullTerm);
	}
	else if (type == AlxParamItem_Type_Uint32)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Uint64)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Int8)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Int16)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Int32)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Int64)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Float)
	{
		status = AlxParamItem_GetValFloat_StrFormat(ptr, val, maxLenWithNullTerm);
	}
	else if (type == AlxParamItem_Type_Double)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Bool)
	{
		status = AlxParamItem_GetValBool_StrFormat(ptr, val, maxLenWithNullTerm);
	}
	else if (type == AlxParamItem_Type_Arr)
	{
		ALX_PARAM_MGMT_ASSERT(false);
	}
	else if (type == AlxParamItem_Type_Str)
	{
		status = AlxParamItem_GetValUint8_StrFormat(ptr, val, maxLenWithNullTerm);
	}
	else
	{
		ALX_PARAM_MGMT_ASSERT(false);	// We should never get here
	}

	// Return
	return status;
}


#endif // #if defined(ALX_C_LIB)
