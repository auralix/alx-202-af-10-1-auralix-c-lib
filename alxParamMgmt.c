/**
  ******************************************************************************
  * @file		alxParamMgmt.c
  * @brief		Auralix C Library - ALX Parameter Management Module
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
