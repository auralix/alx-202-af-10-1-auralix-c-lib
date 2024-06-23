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


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
uint32_t AlxParamMgmt_GetNumOfParamItems(AlxParamMgmt* me)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->numOfParamItems;
}


//------------------------------------------------------------------------------
// By Index
//------------------------------------------------------------------------------
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
uint32_t AlxParamMgmt_ByIndex_GetId(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetId(ptr);
}
uint32_t AlxParamMgmt_ByIndex_GetGroupId(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetGroupId(ptr);
}
AlxParamItem_DataType AlxParamMgmt_ByIndex_GetDataType(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetDataType(ptr);
}
void* AlxParamMgmt_ByIndex_GetValPtr(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetValPtr(ptr);
}
uint32_t AlxParamMgmt_ByIndex_GetValLen(AlxParamMgmt* me, uint32_t index)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Return
	return AlxParamItem_GetValLen(ptr);
}
Alx_Status AlxParamMgmt_ByIndex_GetVal_StrFormat(AlxParamMgmt* me, uint32_t index, char* val, uint32_t maxLenWithNullTerm)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_MGMT_ASSERT(index < me->numOfParamItems);

	// Get pointer
	AlxParamItem* ptr = &me->paramItemArr[index];

	// Get val
	Alx_Status status = AlxParamItem_GetVal_StrFormat(ptr, val, maxLenWithNullTerm);

	// Return
	return status;
}


//------------------------------------------------------------------------------
// By Key
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_ByKey_SetVal_StrFormat(AlxParamMgmt* me, char* key, char* val)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get key
		const char* _key = AlxParamItem_GetKey(ptr);

		// If key match, set value
		if (strcmp(key, _key) == 0)
		{
			// Set
			Alx_Status status = AlxParamItem_SetVal_StrFormat(ptr, val);

			// Return
			return status;
		}
	}

	// If we are here, key was not found, so we return ERROR
	return Alx_Err;
}


//------------------------------------------------------------------------------
// By ID
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_ById_GetValLen(AlxParamMgmt* me, uint32_t id, uint32_t* valLen)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get id
		uint32_t _id = AlxParamItem_GetId(ptr);

		// If id match, set value
		if (id == _id)
		{
			// Get
			uint32_t _valLen = AlxParamItem_GetValLen(ptr);

			// Set
			*valLen = _valLen;

			// Return
			return Alx_Ok;
		}
	}

	// If we are here, id was not found, so we return ERROR
	return Alx_Err;
}
Alx_Status AlxParamMgmt_ById_Get(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get id
		uint32_t _id = AlxParamItem_GetId(ptr);

		// If id match, get value
		if (id == _id)
		{
			// Get valPtr
			void* valPtr = AlxParamItem_GetValPtr(ptr);

			// Get valLen
			uint32_t valLen = AlxParamItem_GetValLen(ptr);

			// Check len
			ALX_PARAM_MGMT_ASSERT(len >= valLen);

			// Copy
			memcpy(val, valPtr, valLen);

			// Return
			return Alx_Ok;
		}
	}

	// If we are here, id was not found, so we return ERROR
	return Alx_Err;
}
Alx_Status AlxParamMgmt_ById_Set(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get id
		uint32_t _id = AlxParamItem_GetId(ptr);

		// If id match, get value
		if (id == _id)
		{
			// Get valPtr
			void* valPtr = AlxParamItem_GetValPtr(ptr);

			// Get valLen
			uint32_t valLen = AlxParamItem_GetValLen(ptr);

			// Check len
			ALX_PARAM_MGMT_ASSERT(len >= valLen);

			// Copy
			memcpy(valPtr, val, valLen);

			// Return
			return Alx_Ok;
		}
	}

	// If we are here, id was not found, so we return ERROR
	return Alx_Err;
}


//------------------------------------------------------------------------------
// Set Default
//------------------------------------------------------------------------------
void AlxParamMgmt_SetValToDef_Group(AlxParamMgmt* me, uint32_t groupId)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get groupId
		uint32_t _groupId = AlxParamItem_GetGroupId(ptr);

		// If groupId match, set value to default
		if (groupId == _groupId)
		{
			AlxParamItem_SetValToDef(ptr);
		}
	}
}
void AlxParamMgmt_SetValToDef_All(AlxParamMgmt* me)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Set value to default
		AlxParamItem_SetValToDef(ptr);
	}
}


//------------------------------------------------------------------------------
// Load & Store
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_LoadVal_Group(AlxParamMgmt* me, uint32_t groupId)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get groupId
		uint32_t _groupId = AlxParamItem_GetGroupId(ptr);

		// If groupId match, load value
		if (groupId == _groupId)
		{
			Alx_Status status = AlxParamItem_LoadVal(ptr);
			if (status != Alx_Ok)
			{
				return status;
			}
		}
	}

	// If we are here, all parameters were loaded successfully, so we return OK
	return Alx_Ok;
}
Alx_Status AlxParamMgmt_LoadVal_All(AlxParamMgmt* me)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Load value
		Alx_Status status = AlxParamItem_LoadVal(ptr);
		if (status != Alx_Ok)
		{
			return status;
		}
	}

	// If we are here, all parameters were loaded successfully, so we return OK
	return Alx_Ok;
}
Alx_Status AlxParamMgmt_StoreVal_Group(AlxParamMgmt* me, uint32_t groupId)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Get groupId
		uint32_t _groupId = AlxParamItem_GetGroupId(ptr);

		// If groupId match, store value
		if (groupId == _groupId)
		{
			Alx_Status status = AlxParamItem_StoreVal(ptr);
			if (status != Alx_Ok)
			{
				return status;
			}
		}
	}

	// If we are here, all parameters were stored successfully, so we return OK
	return Alx_Ok;
}
Alx_Status AlxParamMgmt_StoreVal_All(AlxParamMgmt* me)
{
	// Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);

	// Loop through all parameters
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get pointer
		AlxParamItem* ptr = &me->paramItemArr[i];

		// Store value
		Alx_Status status = AlxParamItem_StoreVal(ptr);
		if (status != Alx_Ok)
		{
			return status;
		}
	}

	// If we are here, all parameters were stored successfully, so we return OK
	return Alx_Ok;
}


#endif // #if defined(ALX_C_LIB)
