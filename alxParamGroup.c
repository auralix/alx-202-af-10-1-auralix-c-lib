//******************************************************************************
// @file alxParamGroup.c
// @brief Auralix C Library - ALX Parameter Group Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

//******************************************************************************
// Includes
//******************************************************************************
#include "alxParamGroup.h"


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxParamGroup_ParamItemsValDefToValToStoreBuff(AlxParamGroup* me);
static void AlxParamGroup_ValStoredBuffToParamItemsVal(AlxParamGroup* me);


//******************************************************************************
// Constructor
//******************************************************************************
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
Alx_Status AlxParamGroup_Init(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == false);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read memory for initNumOfTries
	for(uint32_t i = 0; i < me->initNumOfTries; i++)
	{
		// #2.1 Read memory
		Alx_Status statusAlxMemSafeRead = Alx_Err;
		statusAlxMemSafeRead = AlxMemSafe_Read(me->memSafe, me->valStoredBuff, me->len);
		memcpy(me->valBuff, me->valStoredBuff, me->len);

		// #2.2 Handle result
		switch(statusAlxMemSafeRead)
		{
			case AlxMemSafe_CrcOkSame_UsedCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE("%s - CrcOkSame_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxMemSafe_CrcOkDiff_UsedCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE("%s - CrcOkDiff_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxMemSafe_CopyANokCopyBOk_UsedCopyB:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE("%s - CopyANokCopyBOk_UsedCopyB", me->name);

				// Break
				break;
			}
			case AlxMemSafe_CopyAOkCopyBNok_UsedCopyA:
			{
				// Update
				AlxParamGroup_ValStoredBuffToParamItemsVal(me);

				// Trace
				ALX_PARAM_GROUP_TRACE("%s - CopyAOkCopyBNok_UsedCopyA", me->name);

				// Break
				break;
			}
			case AlxMemSafe_BothNok_ResetToDef:
			{
				// Prepare
				AlxParamGroup_ParamItemsValDefToValToStoreBuff(me);

				// Write
				Alx_Status statusAlxMemSafeWrite = AlxMemSafe_Write(me->memSafe, me->valToStoreBuff, me->len);
				if (statusAlxMemSafeWrite != Alx_Ok)
				{
					// Trace
					ALX_PARAM_GROUP_TRACE("%s - BothNok_ResetToDef_WriteErr", me->name);

					// Continue
					continue;
				}
				else
				{
					// Update
					me->isInit = true;	// JK: isInit has to be set to true here, otherwise assert is triggered when copying default values
					AlxParamGroup_ValToStoreBuffToValStoredBuff(me);

					// Trace
					ALX_PARAM_GROUP_TRACE("%s - BothNok_ResToDef");

					// Break
					break;
				}
			}
			case Alx_Err:
			{
				ALX_PARAM_GROUP_TRACE("%s - Err", me->name);
				continue;
			}
			default:
			{
				ALX_PARAM_GROUP_ASSERT(false);	// We should never get here
				return Alx_Err;
			}
		}

		// #2.3 Break
		status = Alx_Ok;
		break;
	}

	// #3 Set isInit
	me->isInit = true;

	// #4 Return
	return status;
}
void AlxParamGroup_Write(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Start writing
	AlxMemSafe_Write(me->memSafe, me->valToStoreBuff, me->len);
}
bool AlxParamGroup_IsWriteDone(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Return
	return AlxMemSafe_IsWriteDone(me->memSafe);
}
bool AlxParamGroup_IsWriteErr(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Return
	return AlxMemSafe_IsWriteErr(me->memSafe);
}
bool AlxParamGroup_IsValStoredBuffDiff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Compare valBuff to valStoredBuff
	if (memcmp(me->valBuff, me->valStoredBuff, me->len) != 0)	// Difference detected
		return true;
	else
		return false;
}
void AlxParamGroup_ValBuffToValToStoreBuff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Copy
	memcpy(me->valToStoreBuff, me->valBuff, me->len);
}
void AlxParamGroup_ValToStoreBuffToValStoredBuff(AlxParamGroup* me)
{
	// Assert
	ALX_PARAM_GROUP_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_GROUP_ASSERT(me->isInit == true);

	// #1 Copy
	memcpy(me->valStoredBuff, me->valToStoreBuff, me->len);
}
void AlxParamGroup_ParamItemsValToValBuff(AlxParamGroup* me)
{
	// #1 Prepare variables
	uint32_t valBuffIndex = 0;

	// #2 Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// #2.1 Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// #2.2 Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// #2.3 Copy
		memcpy(&me->valBuff[valBuffIndex], valPtr, len);

		// #2.4 Increment
		valBuffIndex = valBuffIndex + len;
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxParamGroup_ParamItemsValDefToValToStoreBuff(AlxParamGroup* me)
{
	// #1 Prepare variables
	uint32_t valToStoreBuffIndex = 0;

	// #2 Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// #2.1 Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// #2.2 Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// #2.3 Copy
		memcpy(&me->valToStoreBuff[valToStoreBuffIndex], valPtr, len);

		// #2.4 Increment
		valToStoreBuffIndex = valToStoreBuffIndex + len;
	}
}
static void AlxParamGroup_ValStoredBuffToParamItemsVal(AlxParamGroup* me)
{
	// #1 Prepare variables
	uint32_t valStoredBuffIndex = 0;

	// #2 Copy
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// #2.1 Get value pointer
		void* valPtr = AlxParamItem_GetValPtr(*(me->paramItemArr + i));

		// #2.2 Get value length
		uint32_t len = AlxParamItem_GetValLen(*(me->paramItemArr + i));

		// #2.3 Copy
		memcpy(valPtr, &me->valStoredBuff[valStoredBuffIndex], len);

		// #2.4 Increment
		valStoredBuffIndex = valStoredBuffIndex + len;
	}
}
