/**
  ******************************************************************************
  * @file		alxParamStore.c
  * @brief		Auralix C Library - ALX Parameter Store Module
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
#include "alxParamStore.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxParamStore_IsParamGroupInitOk(AlxParamStore* me);
static void AlxParamStore_Handle_1xGroup(AlxParamStore* me);
static void AlxParamStore_Handle_2xGroup(AlxParamStore* me);
static void AlxParamStore_Handle_4xGroup(AlxParamStore* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramGroupArr
  * @param[in]		numOfParamGroups
  */
void AlxParamStore_Ctor
(
	AlxParamStore* me,
	AlxParamGroup** paramGroupArr,
	uint32_t numOfParamGroups
)
{
	// Parameters
	me->paramGroupArr = paramGroupArr;
	me->numOfParamGroups = numOfParamGroups;

	// Variables
	me->st = AlxParamStore_St_Init;
	me->isParamGroupInitErr = false;

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
  * @param[out]		status
  * @param[in]		numOfParamGroups
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamStore_Init(AlxParamStore* me, Alx_Status* statusParamGroup, uint32_t numOfParamGroups)
{
	// Assert
	ALX_PARAM_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_STORE_ASSERT(me->isInit == false);
	ALX_PARAM_STORE_ASSERT(numOfParamGroups == me->numOfParamGroups);

	// #1 Prepare variables
	Alx_Status status = Alx_Ok;

	// #2 Init
	for (uint32_t i = 0; i < me->numOfParamGroups; i++)
	{
		statusParamGroup[i] = AlxParamGroup_Init(*(me->paramGroupArr + i));
		if (statusParamGroup[i] == Alx_Err)
		{
			me->isParamGroupInitErr = true;
			status = Alx_Err;
		}
	}

	// #3 Set isInit
	me->isInit = true;

	// #4 Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamStore_Handle(AlxParamStore* me)
{
	// Assert
	ALX_PARAM_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_STORE_ASSERT(me->isInit == true);

	// #1 Config
	switch (me->numOfParamGroups)
	{
		case 1:
		{
			AlxParamStore_Handle_1xGroup(me);
			break;
		}
		case 2:
		{
			AlxParamStore_Handle_2xGroup(me);
			break;
		}
		case 4:
		{
			AlxParamStore_Handle_4xGroup(me);
			break;
		}
		default:
		{
			ALX_PARAM_STORE_ASSERT(false);	// We should never get here
			return;
		}
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamStore_IsErr(AlxParamStore* me)
{
	// Assert
	ALX_PARAM_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_STORE_ASSERT(me->isInit == true);

	// #1 Return
	if (me->st == AlxParamStore_St_Err)
		return true;
	else
		return false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxParamStore_IsParamGroupInitOk(AlxParamStore* me)
{
	return !(me->isParamGroupInitErr);
}
static void AlxParamStore_Handle_1xGroup(AlxParamStore* me)
{
	switch (me->st)
	{
		case AlxParamStore_St_Init:
		{
			// #1 Transition
			if(AlxParamStore_IsParamGroupInitOk(me))
			{
				// #1.1 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.2 Trace
				ALX_PARAM_STORE_TRACE_WRN("InitOk");
			}

			// #2 Do Nothing

			// #3 Break
			break;
		}
		case AlxParamStore_St_Err:
		{
			// #1 Do Nothing

			// #2 Break
			break;
		}
		case AlxParamStore_St_CheckingGroup1:
		{
			// #1 Transition
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroupArr[0]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroupArr[0]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroupArr[0]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup1;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_WriteStart");
			}

			// #2.1 Refresh values
			AlxParamGroup_ParamItemsValToValBuff(me->paramGroupArr[0]);

			// #3 Do Nothing

			// #4 Break
			break;
		}
		case AlxParamStore_St_WritingGroup1:
		{
			// #1 Transition
			if(AlxParamGroup_IsWriteDone(me->paramGroupArr[0]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroupArr[0]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_WriteDone");
			}

			// #2 Transition
			else if(AlxParamGroup_IsWriteErr(me->paramGroupArr[0]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_Err");
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		default:
		{
			// #1 Assert
			ALX_PARAM_STORE_ASSERT(false); 	// We should never get here

			// #2 Break
			break;
		}
	}
}
static void AlxParamStore_Handle_2xGroup(AlxParamStore* me)
{
	switch (me->st)
	{
		case AlxParamStore_St_Init:
		{
			// #1 Transition
			if(AlxParamStore_IsParamGroupInitOk(me))
			{
				// #1.1 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.2 Trace
				ALX_PARAM_STORE_TRACE_WRN("InitOk");
			}

			// #2 Do Nothing

			// #3 Break
			break;
		}
		case AlxParamStore_St_Err:
		{
			// #1 Do Nothing

			// #2 Break
			break;
		}
		case AlxParamStore_St_CheckingGroup1:
		{
			// #1 Transition
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroupArr[0]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroupArr[0]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroupArr[0]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup1;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_WriteStart");
			}
			// #2 Transition
			else
			{
				// #2.1 Refresh values
				AlxParamGroup_ParamItemsValToValBuff(me->paramGroupArr[0]);

				// #2.2 Change state
				me->st = AlxParamStore_St_CheckingGroup2;
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		case AlxParamStore_St_WritingGroup1:
		{
			// #1 Transition
			if (AlxParamGroup_IsWriteDone(me->paramGroupArr[0]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroupArr[0]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup2;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_WriteDone");
			}

			// #2 Transition
			else if (AlxParamGroup_IsWriteErr(me->paramGroupArr[0]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group1_Err");
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		case AlxParamStore_St_CheckingGroup2:
		{
			// #1 Transition
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroupArr[1]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroupArr[1]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroupArr[1]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup2;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group2_WriteStart");
			}
			// #2 Transition
			else
			{
				// #2.1 Refresh values
				AlxParamGroup_ParamItemsValToValBuff(me->paramGroupArr[1]);

				// #2.2 Change state
				me->st = AlxParamStore_St_CheckingGroup1;
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		case AlxParamStore_St_WritingGroup2:
		{
			// #1 Transition
			if (AlxParamGroup_IsWriteDone(me->paramGroupArr[1]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroupArr[1]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group2_WriteDone");
			}

			// #2 Transition
			else if (AlxParamGroup_IsWriteErr(me->paramGroupArr[1]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE_WRN("Group2_Err");
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		default:
		{
			// #1 Assert
			ALX_PARAM_STORE_ASSERT(false);	// We should never get here

			// #2 Break
			break;
		}
	}
}
static void AlxParamStore_Handle_4xGroup(AlxParamStore* me)
{
	// TV: TODO
	(void)me;
	ALX_PARAM_STORE_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
