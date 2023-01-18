/**
  ******************************************************************************
  * @file		alxVnh7040.h
  * @brief		Auralix C Library - ALX H-Bridge VNH7040 Module
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
// Include Guard
//******************************************************************************
#ifndef ALX_VNH7040_H
#define ALX_VNH7040_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxIoPin.h"
#include "alxAdc.h"
#include "alxPwm.h"
#include "alxDelay.h"
#include "alxVdiv.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_VNH7040_FILE "alxVnh7040.h"

// Assert //
#if defined(_ALX_VNH7040_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_VNH7040_ASSERT(expr) ALX_ASSERT_BKPT(ALX_VNH7040_FILE, expr)
#elif defined(_ALX_VNH7040_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_VNH7040_ASSERT(expr) ALX_ASSERT_TRACE(ALX_VNH7040_FILE, expr)
#elif defined(_ALX_VNH7040_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_VNH7040_ASSERT(expr) ALX_ASSERT_RST(ALX_VNH7040_FILE, expr)
#else
	#define ALX_VNH7040_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_VNH7040_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_VNH7040_TRACE(...) ALX_TRACE_STD(ALX_VNH7040_FILE, __VA_ARGS__)
#else
	#define ALX_VNH7040_TRACE(...) do{} while (false)
#endif

#ifndef ALX_VNH7040_NO_PWM
	#define ALX_VNH7040_PWM
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxVnh7040_Sm_StOk = 0,
	AlxVnh7040_Sm_StErr = 2
} AlxVnh7040_Sm_St;

typedef enum
{
	AlxVnh7040_Status_Init = 0,
	AlxVnh7040_Status_RotateCw = 1,
	AlxVnh7040_Status_RotateCcw = 2,
	AlxVnh7040_Status_BreakGnd = 3,
	AlxVnh7040_Status_BreakVcc = 4,
	AlxVnh7040_Status_BreakOpen = 5,
	AlxVnh7040_Status_Err = 6
} AlxVnh7040_Status;

typedef enum
{
	AlxVnh7040_BreakType_Gnd = 0,
	AlxVnh7040_BreakType_Vcc = 1,
	AlxVnh7040_BreakType_Open = 2
} AlxVnh7040_BreakType;

typedef enum
{
	AlxVnh7040_MonitoringType_HSA   = 0,
	AlxVnh7040_MonitoringType_HSB   = 1,
	AlxVnh7040_MonitoringType_Tchip = 2,
	AlxVnh7040_MonitoringType_Vcc   = 3
} AlxVnh7040_MonitoringType;

typedef struct
{
	// Parameters Const
	uint32_t MONITORING_PERIOD_MIN_us;
	uint32_t MULTI_SENS_CURRENT_COEFF;
	uint32_t MULTI_SENS_ERR_INDICATE_PIN_VOLTAGE_THRESHOLD_mV;
	int32_t dVSENSE_TC_dT_uV_K;									// uV/K
	int32_t VSENSE_TC_T0_uV;									// 2076-(25*(-5.5)) uV
	uint32_t dVSENSE_Vcc_k;										// Transfer function for VSENSE_VCC

	// Objects - External
	AlxAdc* adc;
	Alx_Ch multiSensAdcCh;
	#ifdef ALX_VNH7040_PWM
	AlxPwm* pwm;
	Alx_Ch pwmCh;
	#else
	AlxIoPin* do_PWM;
	#endif
	AlxIoPin* do_INA;
	AlxIoPin* do_INB;
	AlxIoPin* do_SEL0;
	AlxIoPin* do_SEL1;
	AlxIoPin* do_SENS_EN;

	// Objects - Internal
	AlxTimSw timMonitoringPeriod;
	AlxTimSw tim;

	// Parameters
	AlxVnh7040_BreakType breakType;
	uint32_t multiSensResHigh_ohm;
	uint32_t multiSensResLow_ohm;
	uint32_t restartWaitTime_ms;

	// Variables
	int16_t duty_permil;
	bool do_PWM_val;
	bool do_INA_val;
	bool do_INB_val;
//	bool do_SEL0_val;
//	bool do_SEL1_val;

	AlxVnh7040_Status status;

	uint32_t multiSensAdcVoltage_mV;
	uint32_t multiSensPinVoltage_mV;
	uint32_t multiSensVDivCurrent_uA;

	uint32_t currentHsA_mA;
	uint32_t currentHsB_mA;
	uint32_t supplyVoltage_mV;
	volatile int32_t temp_degC;

	bool currentHsA_clamp;
	bool currentHsB_clamp;

	uint32_t current_mA;

	//	bool isMultiSensErr_BreakGnd_LsB;
	AlxVnh7040_MonitoringType monitoringTypeSelected;

	// SM
	AlxVnh7040_Sm_St st;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxVnh7040;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxVnh7040_Ctor
(
	AlxVnh7040* me,
	AlxAdc* adc,
	Alx_Ch multiSensAdcCh,
	#ifdef ALX_VNH7040_PWM
	AlxPwm* pwm,
	Alx_Ch pwmCh,
	#else
	AlxIoPin* do_PWM,
	#endif
	AlxIoPin* do_INA,
	AlxIoPin* do_INB,
	AlxIoPin* do_SEL0,
	AlxIoPin* do_SEL1,
	AlxIoPin* do_SENS_EN,
	AlxVnh7040_BreakType breakTypeDefault,
	uint32_t multiSensResHigh_ohm,
	uint32_t multiSensResLow_ohm,
	uint32_t restartWaitTime_ms
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxVnh7040_Init(AlxVnh7040* me);
void AlxVnh7040_DeInit(AlxVnh7040* me);
void AlxVnh7040_Handle(AlxVnh7040* me);
void AlxVnh7040_SetDuty_permil(AlxVnh7040* me, int16_t duty_permil);
uint32_t AlxVnh7040_GetCurrent_mA(AlxVnh7040* me);
uint32_t AlxVnh7040_GetCurrentHsA_mA(AlxVnh7040* me);
uint32_t AlxVnh7040_GetCurrentHsB_mA(AlxVnh7040* me);
uint32_t AlxVnh7040_GetSupplyVoltage_mV(AlxVnh7040* me);
int32_t AlxVnh7040_GetTemp_degC(AlxVnh7040* me);
int32_t AlxVnh7040_GetIsError(AlxVnh7040* me);
void AlxVnh7040_Config_BreakType(AlxVnh7040* me, AlxVnh7040_BreakType breakType);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_VNH7040_H
