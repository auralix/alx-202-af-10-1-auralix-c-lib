/**
  ******************************************************************************
  * @file		alxParamGroup.c
  * @brief		Auralix C Library - ALX Parameter Group Module
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
#include "alxParamGroup.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxParamGroup_ParamItemsValDefToValToStoreBuff(AlxParamGroup* me);
static void AlxParamGroup_ValStoredBuffToParamItemsVal(AlxParamGroup* me);
static void AlxParamGroup_ValToStoreBuffToValStoredBuff_Private(AlxParamGroup* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		memSafe
  * @param[in]		name
  * @param[in]		len
  * @param[in]		valBuff
  * @param[in]		valStoredBuff
  * @param[in]		valToStoreBuff
  * @param[in]		paramItemArr
  * @param[in]		numOfParamItems
  * @param[in]		initNumOfTries
  */
void AlxParamGroup_Ctor
(
	AlxParamGroup* me,
	AlxMemSafe* memSafe,
	const char* name,
	uint32_t len,
	uint8_t* valBuff,
	uint8_t* valStoredBuff,
	uint8_t* valToStoreBuff,
	AlxParamItem** paramItemArr,
	uint32_t numOfParamItems,
	uint8_t initNumOfTries
)
{
	// Parameters
	me->memSafe = memSafe;
	me->name = name;
	me->len = len;
	me->valBuff = valBuff;
	me->valStoredBuff = valStoredBuff;
	me->valToStoreBuff = valToStoreBuff;
	me->paramItemArr = paramItemArr;
	me->numOfParamItems = numOfParamItems;
	me->initNumOfTries = initNumOfTries;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamGroup_Init(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == false);

	// Prepare variables
	Alx_Status status = Alx_Err;

	// Read memory for initNumOfTries
	for (uint32_t i = 0; i < me->initNumOfTries; i++)
	{
		// Read memory
		Alx_Status statusAlxMemSafeRead = Alx_Err;
		statusAlxMemSafeRead = AlxMemSafe_Read(me->memSafe, me->valStoredBuff, me->len);
		memcpy(me->valBuff, me->valStoredBuff, me->len);

		// Handle result
		switch (statusAlxMemSafeRead)
		{
			case AlxSafe_BothCopyOkCrcSame_OrigDontCare_UseCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE_INF("%s_CrcOkSame_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxSafe_BothCopyOkCrcDiff_OrigDontCare_UseCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE_INF("%s_CrcOkDiff_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxSafe_CopyAErrCopyBOk_OrigDontCare_UseCopyB:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE_INF("%s_CopyANokCopyBOk_UsedCopyB", me->name);

				// Break
				break;
			}
			case AlxSafe_CopyAOkCopyBErr_OrigDontCare_UseCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE_INF("%s_CopyAOkCopyBNok_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxSafe_BothCopyErr_OrigErr:
			{
				// Prepare
				AlxParamGroup_ParamItemsValDefToValToStoreBuff(me);

				// Write
				Alx_Status statusAlxMemSafeWrite = AlxMemSafe_Write(me->memSafe, me->valToStoreBuff, me->len);
				if (statusAlxMemSafeWrite != Alx_Ok)
				{
					// Trace
					ALX_PARAM_GROUP_TRACE_WRN("%s_BothNok_ResetToDef_WriteErr", me->name);

					// Continue
					continue;
				}
				else
				{
					// Update
					AlxParamGroup_ValToStoreBuffToValStoredBuff_Private(me);

					// Trace
					ALX_PARAM_GROUP_TRACE_INF("%s_BothNok_ResToDef", me->name);

					// Break
					break;
				}
			}
			case Alx_Err:
			{
				ALX_PARAM_GROUP_TRACE_WRN("%s_Err", me->name);
				continue;
			}
			default:
			{
				ALX_PARAM_GROUP_ASSERT(false);	// We should never get here
				return Alx_Err;
			}
		}

		// Break
		status = Alx_Ok;
		break;
	}

	// Set isInit
	me->isInit = true;

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamGroup_Write(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Start writing
	AlxMemSafe_Write(me->memSafe, me->valToStoreBuff, me->len);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamGroup_IsWriteDone(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Return
	return AlxMemSafe_IsWriteDone(me->memSafe);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamGroup_IsWriteErr(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Return
	return AlxMemSafe_IsWriteErr(me->memSafe);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamGroup_IsValStoredBuffDiff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Compare valBuff to valStoredBuff
	if (memcmp(me->valBuff, me->valStoredBuff, me->len) != 0)	// Difference detected
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamGroup_ValBuffToValToStoreBuff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Copy
	memcpy(me->valToStoreBuff, me->valBuff, me->len);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamGroup_ValToStoreBuffToValStoredBuff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// Copy
	AlxParamGroup_ValToStoreBuffToValStoredBuff_Private(me);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamGroup_ParamItemsValToValBuff(AlxParamGroup* me)
{
	// Prepare variables
	uint32_t valBuffIndex = 0;

	// Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// Copy
		memcpy(&me->valBuff[valBuffIndex], valPtr, len);

		// Increment
		valBuffIndex = valBuffIndex + len;
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxParamGroup_ParamItemsValDefToValToStoreBuff(AlxParamGroup* me)
{
	// Prepare variables
	uint32_t valToStoreBuffIndex = 0;

	// Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// Copy
		memcpy(&me->valToStoreBuff[valToStoreBuffIndex], valPtr, len);

		// Increment
		valToStoreBuffIndex = valToStoreBuffIndex + len;
	}
}
static void AlxParamGroup_ValStoredBuffToParamItemsVal(AlxParamGroup* me)
{
	// Prepare variables
	uint32_t valStoredBuffIndex = 0;

	// Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// Copy
		memcpy(valPtr, &me->valStoredBuff[valStoredBuffIndex], len);

		// Increment
		valStoredBuffIndex = valStoredBuffIndex + len;
	}
}
static void AlxParamGroup_ValToStoreBuffToValStoredBuff_Private(AlxParamGroup* me)
{
	// Copy
	memcpy(me->valStoredBuff, me->valToStoreBuff, me->len);
}


#endif	// #if defined(ALX_C_LIB)
