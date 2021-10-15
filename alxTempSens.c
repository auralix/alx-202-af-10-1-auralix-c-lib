/**
  ******************************************************************************
  * @file alxTempSens.c
  * @brief Auralix C Library - ALX Temperature Sensor Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTempSens.h"


//******************************************************************************
// Private Functions
//******************************************************************************
void AlxTempSens_SetFuncPtr(AlxTempSens* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxTempSens_Ctor(AlxTempSens* me, AlxTempSens_Submodule submodule, void *submodulePtr)
{
	// Parameters
	me->submodule = submodule;
	me->submodulePtr = submodulePtr;

	// FunPtr
	AlxTempSens_SetFuncPtr(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxTempSens_Init(AlxTempSens* me)
{
	ALX_TEMP_SENS_ASSERT(me->isInit == false);
	ALX_TEMP_SENS_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit
	me->isInit = true;
	
	// #2 FuncPtr to Init()
	return (*me->alxTempSens_Init_FuncPtr[me->submodule])(me->submodulePtr);
}
Alx_Status AlxTempSens_DeInit(AlxTempSens* me)
{
	ALX_TEMP_SENS_ASSERT(me->isInit == true);
	ALX_TEMP_SENS_ASSERT(me->wasCtorCalled == true);

	// #1 Reset isInit
	me->isInit = false;
	
	// #2 FuncPtr to DeInit()
	return (*me->alxTempSens_DeInit_FuncPtr[me->submodule])(me->submodulePtr);
}
Alx_Status AlxTempSens_GetTemp_degC(AlxTempSens* me, float* temp_degC)
{
	//ALX_TEMP_SENS_ASSERT(me->isInit == true);
	ALX_TEMP_SENS_ASSERT(me->wasCtorCalled == true);

	// #1 FuncPtr to GetTemp_degC()
	return (*me->alxTempSens_GetTemp_degC_FuncPtr[me->submodule])(me->submodulePtr, temp_degC);
}


//******************************************************************************
// Private Functions
//******************************************************************************
void AlxTempSens_SetFuncPtr(AlxTempSens* me)
{
	bool isErr = true;
	
	#if defined(ALX_TEMP_SENS_MCU)
	if (me->submodule == AlxTempSens_Submodule_Mcu)
	{
		me->alxTempSens_Init_FuncPtr[AlxTempSens_Submodule_Mcu] = AlxTempSensMcu_Init;
		me->alxTempSens_DeInit_FuncPtr[AlxTempSens_Submodule_Mcu] =AlxTempSensMcu_DeInit;
		me->alxTempSens_GetTemp_degC_FuncPtr[AlxTempSens_Submodule_Mcu] = AlxTempSensMcu_GetTemp_degC;
		isErr = false;
	}
	#endif
	#if defined(ALX_TEMP_SENS_RTD_VDIV)
	if (me->submodule == AlxTempSens_Submodule_Rtd_Vdiv)
	{
		me->alxTempSens_Init_FuncPtr[AlxTempSens_Submodule_Rtd_Vdiv] = AlxTempSensRtdVdiv_Init;
		me->alxTempSens_DeInit_FuncPtr[AlxTempSens_Submodule_Rtd_Vdiv] = AlxTempSensRtdVdiv_DeInit;
		me->alxTempSens_GetTemp_degC_FuncPtr[AlxTempSens_Submodule_Rtd_Vdiv] = AlxTempSensRtdVdiv_GetTemp_degC;
		isErr = false;
	}
	#endif

	if (isErr) { ALX_TEMP_SENS_ASSERT(false); }; // We shouldn't get here
}
