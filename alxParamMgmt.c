/**
  ******************************************************************************
  * @file alxParamMgmt.c
  * @brief Auralix C Library - ALX Parameter Management Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxParamMgmt.h"


//******************************************************************************
// Private Functions
//******************************************************************************
//static void AlxParamMgmt_Ctor(AlxParamMgmt);



//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem** paramItemArr,
	uint8_t numOfParamItems
)
{
	// Objects - External
	me->paramItemArr = paramItemArr;

	// Parameters
	me->numOfParamItems = numOfParamItems;

	// Variables

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxParamMgmt_GetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len)
{
	// On HOLD, we must define system for this
	
	
	
	// #1 Assert
	ALX_PARAM_MGMT_ASSERT(me->wasCtorCalled == true);
	
	// #2 Search by name
	for (uint32_t i = 0; i < me->numOfParamItems; i++)
	{
		// #2.1 Get name
		const char* paramItem_Name = AlxParamItem_GetName(*(me->paramItemArr + i));
		
		// #2.2 Compare
		if ((strcmp(name, paramItem_Name) == 0))
		{
			// #2.2.1 Get type
			AlxParamItem_Type paramItem_Type = AlxParamItem_GetType(*(me->paramItemArr + i));
			
			// #2.2.2 Get value length
			uint32_t paramItem_ValLen = AlxParamItem_GetValLen(*(me->paramItemArr + i));
			
			// #2.2.3 Check value length
			ALX_PARAM_MGMT_ASSERT(paramItem_ValLen == len);
			
			switch (paramItem_Type)
			{
				case AlxParamItem_Type_Uint8:
				{
					uint8_t uint8 = AlxParamItem_GetValUint8(*(me->paramItemArr + i));
					memcpy(val, &uint8, len);
					break;
				}
				default:
				{
					ALX_PARAM_MGMT_ASSERT(false);	// We should never get here
					break;
				}
			}
			

		}
	}
	
	ALX_PARAM_MGMT_ASSERT(false);	// We should never get here (user entered "Name" that doesn't exist)
}
void AlxParamMgmt_GetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len)
{

}
void AlxParamMgmt_SetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len)
{

}
void AlxParamMgmt_SetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len)
{

}
void AlxParamMgmt_SetToDefByName(AlxParamMgmt* me, const char* name, uint32_t len)
{

}
void AlxParamMgmt_SetToDefById(AlxParamMgmt* me, uint32_t id, uint32_t len)
{

}
void AlxParamMgmt_SetToDefGroup(AlxParamMgmt* me, uint8_t groupId)
{

}
void AlxParamMgmt_SetToDefAll(AlxParamMgmt* me)
{

}
