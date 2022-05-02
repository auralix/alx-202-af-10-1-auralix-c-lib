/**
  ******************************************************************************
  * @file alxParamStore.c
  * @brief Auralix C Library - ALX Parameter Store Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxParamStore.h"


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
void AlxParamStore_Ctor
(
	AlxParamStore* me,
	AlxParamGroup** paramGroups,
	uint32_t numOfParamGroups
)
{
	// Parameters
	me->paramGroups = paramGroups;
	me->numOfParamGroups = numOfParamGroups;

	// Variables
	me->isParamGroupInitErr = false;

	// SM
	me->st = AlxParamStore_St_Init;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamStore_Init(AlxParamStore* me, Alx_Status* statusParamGroup, uint8_t numOfParamGroups)
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
		statusParamGroup[i] = AlxParamGroup_Init(*(me->paramGroups + i));
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
			ALX_PARAM_STORE_ASSERT(false);	// We should not get here
			return;
		}
	}
}
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
				ALX_PARAM_STORE_TRACE("InitOk");
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
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroups[0]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroups[0]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroups[0]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup1;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE("Group1_WriteStart");
			}

			// #2.1 Refresh values
			AlxParamGroup_ParamItemsValToValBuff(me->paramGroups[0]);

			// #3 Do Nothing

			// #4 Break
			break;
		}
	case AlxParamStore_St_WritingGroup1:
		{
			// #1 Transition
			if(AlxParamGroup_IsWriteDone(me->paramGroups[0]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroups[0]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE("Group1_WriteDone");
			}

			// #2 Transition
			else if(AlxParamGroup_IsWriteErr(me->paramGroups[0]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE("Group1_Err");
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
				ALX_PARAM_STORE_TRACE("InitOk");
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
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroups[0]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroups[0]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroups[0]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup1;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE("Group1_WriteStart");
			} 
			// #2 Transition
			else
			{
				// #2.1 Refresh values
				AlxParamGroup_ParamItemsValToValBuff(me->paramGroups[0]);

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
			if (AlxParamGroup_IsWriteDone(me->paramGroups[0]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroups[0]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup2;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE("Group1_WriteDone");
			}

			// #2 Transition
			else if (AlxParamGroup_IsWriteErr(me->paramGroups[0]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE("Group1_Err");
			}

			// #3 Do Nothing

			// #4 Break
			break;
		}
		case AlxParamStore_St_CheckingGroup2:
		{
			// #1 Transition
			if(AlxParamGroup_IsValStoredBuffDiff(me->paramGroups[1]))
			{
				// #1.1 Prepare values to store
				AlxParamGroup_ValBuffToValToStoreBuff(me->paramGroups[1]);

				// #1.2 Start writing
				AlxParamGroup_Write(me->paramGroups[1]);

				// #1.3 Change state
				me->st = AlxParamStore_St_WritingGroup2;

				// #1.4 Trace
				ALX_PARAM_STORE_TRACE("Group2_WriteStart");
			}
			// #2 Transition
			else
			{
				// #2.1 Refresh values
				AlxParamGroup_ParamItemsValToValBuff(me->paramGroups[1]);

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
			if (AlxParamGroup_IsWriteDone(me->paramGroups[1]))
			{
				// #1.1 Update stored values
				AlxParamGroup_ValToStoreBuffToValStoredBuff(me->paramGroups[1]);

				// #1.2 Change state
				me->st = AlxParamStore_St_CheckingGroup1;

				// #1.3 Trace
				ALX_PARAM_STORE_TRACE("Group2_WriteDone");
			}

			// #2 Transition
			else if (AlxParamGroup_IsWriteErr(me->paramGroups[1]))
			{
				// #2.1 Change state
				me->st = AlxParamStore_St_Err;

				// #2.2 Trace
				ALX_PARAM_STORE_TRACE("Group2_Err");
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
	// TODO
	(void)me;
	ALX_PARAM_STORE_ASSERT(false);
}
