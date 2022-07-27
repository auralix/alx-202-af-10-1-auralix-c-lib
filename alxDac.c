/**
  ******************************************************************************
  * @file		alxDac.c
  * @brief		Auralix C Library - ALX DAC Module
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
#include "alxDac.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxDac_SetFuncPtr(AlxDac* me);


//******************************************************************************
// Functions
//******************************************************************************
void AlxDac_Ctor(AlxDac* me, AlxDac_Submodule submodule, void *submodulePtr)
{
	// Parameters
	me->submodule = submodule;
	me->submodulePtr = submodulePtr;

	// FunPtr
	AlxDac_SetFuncPtr(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxDac_Init(AlxDac* me)
{
	ALX_DAC_ASSERT(me->isInit == false);
	ALX_DAC_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit
	me->isInit = true;

	// #2 FuncPtr to Init()
	return (*me->AlxDac_Init_FuncPtr[me->submodule])(me->submodulePtr);
}
Alx_Status AlxDac_Init_CalibrateVref(AlxDac* me, float vref_V)
{
	ALX_DAC_ASSERT(me->isInit == false);
	ALX_DAC_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit
	me->isInit = true;

	// #2 FuncPtr to Init_CalibrateVref()
	return (*me->AlxDac_Init_CalibrateVref_FuncPtr[me->submodule])(me->submodulePtr, &vref_V);
}
Alx_Status AlxDac_DeInit(AlxDac* me)
{
	ALX_DAC_ASSERT(me->isInit == true);
	ALX_DAC_ASSERT(me->wasCtorCalled == true);

	// #1 Reset isInit
	me->isInit = false;

	// #2 FuncPtr to DeInit()
	return (*me->AlxDac_DeInit_FuncPtr[me->submodule])(me->submodulePtr);
}
Alx_Status AlxDac_SetVoltage_V(AlxDac* me, Alx_Ch ch, float voltage_V)
{
	ALX_DAC_ASSERT(me->isInit == true);
	ALX_DAC_ASSERT(me->wasCtorCalled == true);

	// #1 FuncPtr to SetVoltage_V()
	return (*me->AlxDac_SetVoltage_V_FuncPtr[me->submodule])(me->submodulePtr, &ch, &voltage_V);
}
Alx_Status AlxDac_SetVoltage_V_CalibrateVref(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V)
{
	ALX_DAC_ASSERT(me->isInit == true);
	ALX_DAC_ASSERT(me->wasCtorCalled == true);

	// #1 FuncPtr to SetVoltage_V_CalibrateVref()
	return (*me->AlxDac_SetVoltage_V_CalibrateVref_FuncPtr[me->submodule])(me->submodulePtr, &ch, &voltage_V, &vref_V);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxDac_SetFuncPtr(AlxDac* me)
{
	bool isErr = true;

	#if defined(ALX_DAC_MCU)
		if (me->submodule == AlxDac_Submodule_Mcu)
		{
			me->AlxDac_Init_FuncPtr[AlxDac_Submodule_Mcu] = AlxDacMcu_Init;
			me->AlxDac_Init_CalibrateVref_FuncPtr[AlxDac_Submodule_Mcu] = AlxDacMcu_Init_CalibrateVref;
			me->AlxDac_DeInit_FuncPtr[AlxDac_Submodule_Mcu] = AlxDacMcu_DeInit;
			me->AlxDac_SetVoltage_V_FuncPtr[AlxDac_Submodule_Mcu] = AlxDacMcu_SetVoltage_V;
			me->AlxDac_SetVoltage_V_CalibrateVref_FuncPtr[AlxDac_Submodule_Mcu] = AlxDacMcu_SetVoltage_V_CalibrateVref;
			isErr = false;
		}
	#endif

	if (isErr) { ALX_DAC_ASSERT(false); }; // We shouldn't get here
}


#endif // #if defined(ALX_C_LIB)
