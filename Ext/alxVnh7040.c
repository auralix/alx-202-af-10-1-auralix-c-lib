/**
  ******************************************************************************
  * @file		alxVnh7040.c
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
// Includes
//******************************************************************************
#include "alxVnh7040.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxVnh7040_Monitoring_Select_HSA(AlxVnh7040* me);
static void AlxVnh7040_Monitoring_Select_HSB(AlxVnh7040* me);
static void AlxVnh7040_Monitoring_Select_Tchip(AlxVnh7040* me);
static void AlxVnh7040_Monitoring_Select_Vcc(AlxVnh7040* me);
static void AlxVnh7040_Monitoring (AlxVnh7040* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		adc
  * @param[in]		multiSensAdcCh
  * @param[in]		pwm
  * @param[in]		pwmCh
  * @param[in]		do_PWM
  * @param[in]		do_INA
  * @param[in]		do_INB
  * @param[in]		do_SEL0
  * @param[in]		do_SEL1
  * @param[in]		do_SENS_EN
  * @param[in]		breakTypeDefault
  * @param[in]		multiSensResHigh_ohm
  * @param[in]		multiSensResLow_ohm
  * @param[in]		restartWaitTime_ms
  */
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
)
{
	// Info
	me->wasCtorCalled = true;
	me->isInit = false;

	// Ctor
	AlxTimSw_Ctor(&me->timMonitoringPeriod, true);
	AlxTimSw_Ctor(&me->tim, false);

	// Parameters Const
	me->MONITORING_PERIOD_MIN_us = 1000;							// minimum monitoring period
	me->MULTI_SENS_CURRENT_COEFF = 4450;							// K1, Iout/Isense [uA/uA]
	me->MULTI_SENS_ERR_INDICATE_PIN_VOLTAGE_THRESHOLD_mV = 5000;	//
	me->dVSENSE_TC_dT_uV_K = (-5500);								// uV/K
	me->VSENSE_TC_T0_uV = 2214000;									// 2076-(25*(-5.5)) uV
	me->dVSENSE_Vcc_k = 4;											//

	// Objects - External
	me->adc = adc;
	me->multiSensAdcCh = multiSensAdcCh;
	#ifdef ALX_VNH7040_PWM
	me->pwm = pwm;
	me->pwmCh = pwmCh;
	#else
	me->do_PWM = do_PWM;
	#endif
	me->do_INA = do_INA;
	me->do_INB = do_INB;
	me->do_SEL0 = do_SEL0;
	me->do_SEL1 = do_SEL1;
	me->do_SENS_EN = do_SENS_EN;

	// Parameters
	me->breakType = breakTypeDefault;
	me->multiSensResHigh_ohm = multiSensResHigh_ohm;
	me->multiSensResLow_ohm = multiSensResLow_ohm;
	me->restartWaitTime_ms = restartWaitTime_ms;

	// Variables
	me->duty_permil = 0;
	me->status = AlxVnh7040_Status_Init;
	me->current_mA = 0;
	me->currentHsA_mA = 0;
	me->currentHsB_mA = 0;
	me->supplyVoltage_mV = 0;
	me->temp_degC = 0;
	me->multiSensAdcVoltage_mV = 0;
	me->multiSensPinVoltage_mV = 0;
	me->multiSensVDivCurrent_uA = 0;
	me->monitoringTypeSelected = AlxVnh7040_MonitoringType_HSA;

	// SM
	me->st = AlxVnh7040_Sm_StOk;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxVnh7040_Init(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	me->isInit = true;

	// #1 Init GPIO
	AlxIoPin_Init(me->do_INA);
	AlxIoPin_Init(me->do_INB);
	AlxIoPin_Init(me->do_SEL0);
	AlxIoPin_Init(me->do_SEL1);
	AlxIoPin_Init(me->do_SENS_EN);

	// #2 Init PWM/GPIO
	#ifdef ALX_VNH7040_PWM
	if(AlxPwm_Init(me->pwm) != Alx_Ok) { ALX_VNH7040_ASSERT(false); return; }
	#else
	AlxIoPin_Init(me->do_PWM);
	#endif

	// #3 Enable Multisense
	AlxIoPin_Set(me->do_SENS_EN);

	// #4 Delay for device to come from standby mode to operating mode
	AlxDelay_ms(2);

	// #5 Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxVnh7040_DeInit(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	// #1 DeInit PWM/GPIO
	#ifdef ALX_VNH7040_PWM
	if(AlxPwm_DeInit(me->pwm) != Alx_Ok) { ALX_VNH7040_ASSERT(false); return; }
	#else
	AlxIoPin_DeInit(me->do_PWM);
	#endif

	// #2 DeInit GPIO
	AlxIoPin_DeInit(me->do_INA);
	AlxIoPin_DeInit(me->do_INB);
	AlxIoPin_DeInit(me->do_SEL0);
	AlxIoPin_DeInit(me->do_SEL1);
	AlxIoPin_DeInit(me->do_SENS_EN);

	// #3 Reset isInit
	me->isInit = false;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxVnh7040_Handle(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	// #1 Get Multi Sense Voltage
	AlxVnh7040_Monitoring (me);

	// #2 Set Direction
	AlxIoPin_Write(me->do_INA, me->do_INA_val);
	AlxIoPin_Write(me->do_INB, me->do_INB_val);

	// #3 Set Speed
	#ifdef ALX_VNH7040_PWM
	AlxPwm_SetDuty_permil(me->pwm, me->pwmCh, me->duty_permil);
	#else
	AlxIoPin_Write(me->do_PWM, me->do_PWM_val);
	#endif

	if (me->status == AlxVnh7040_Status_RotateCw)
	{
//		// Set Sense
//		AlxIoPin_Write(me->do_SEL0, me->do_SEL0_val);
//		AlxIoPin_Write(me->do_SEL1, me->do_SEL1_val);
//
//		// Set Current
//		me->currentHsA_mA = me->multiSensVDivCurrent_uA * me->MULTI_SENS_CURRENT_COEFF / 1000;
//		me->currentHsB_mA = 0;
		me->current_mA = me->currentHsA_mA;
	}
	else if (me->status == AlxVnh7040_Status_RotateCcw)
	{
//		// Set Sense
//		AlxIoPin_Write(me->do_SEL0, me->do_SEL0_val);
//		AlxIoPin_Write(me->do_SEL1, me->do_SEL1_val);
//
//		// Set Current
//		me->currentHsA_mA = 0;
//		me->currentHsB_mA = me->multiSensVDivCurrent_uA * me->MULTI_SENS_CURRENT_COEFF / 1000;
		me->current_mA = me->currentHsB_mA;
	}
	else if (me->status == AlxVnh7040_Status_BreakGnd)
	{
//		// Set Sense - Detect LSB protection
//		AlxIoPin_Reset(me->do_SEL0);
//		AlxIoPin_Reset(me->do_SEL1);

//		me->multiSensAdcVoltage_mV = AlxAdc_GetVoltage_mV(me->adc, me->multiSensAdcCh);
//		me->multiSensPinVoltage_mV = AlxVdiv_GetVin_mV(me->multiSensAdcVoltage_mV, me->multiSensResHigh_ohm, me->multiSensResLow_ohm);
//		me->multiSensVDivCurrent_uA = AlxVdiv_GetCurrent_uA(me->multiSensAdcVoltage_mV * 1000, me->multiSensResLow_ohm);
//
//		me->isMultiSensErr_BreakGnd_LsB = AlxVnh7040_IsMultiSensErrIndicate(me);
//		if (me->isMultiSensErr_BreakGnd_LsB)
//		{
//			AlxVnh7040_Reset(me);
//
//			// Set Direction
//			AlxIoPin_Reset(me->do_INA);
//			AlxIoPin_Reset(me->do_INB);
//
//			// Set Speed
//			#ifdef ALX_VNH7040_PWM
//			AlxPwm_SetDuty_permil(me->pwm, me->pwmCh, 1000);
//			#else
//			AlxIoPin_Set(me->do_PWM);
//			#endif
//		}
//
//		// Set Sense - Detect LSA protection
//		AlxIoPin_Set(me->do_SEL0);
//		AlxIoPin_Reset(me->do_SEL1);
//
//		me->multiSensAdcVoltage_mV = AlxAdc_GetVoltage_mV(me->adc, me->multiSensAdcCh);
//		me->multiSensPinVoltage_mV = AlxVdiv_GetVin_mV(me->multiSensAdcVoltage_mV, me->multiSensResHigh_ohm, me->multiSensResLow_ohm);
//		me->multiSensVDivCurrent_uA = AlxVdiv_GetCurrent_uA(me->multiSensAdcVoltage_mV * 1000, me->multiSensResLow_ohm);
//
//		me->isMultiSensErr_BreakGnd_LsB = AlxVnh7040_IsMultiSensErrIndicate(me);
//		if (me->isMultiSensErr_BreakGnd_LsB)
//		{
//			AlxVnh7040_Reset(me);
//
//			// Set Direction
//			AlxIoPin_Reset(me->do_INA);
//			AlxIoPin_Reset(me->do_INB);
//
//			// Set Speed
//			#ifdef ALX_VNH7040_PWM
//			AlxPwm_SetDuty_permil(me->pwm, me->pwmCh, 1000);
//			#else
//			AlxIoPin_Set(me->do_PWM);
//			#endif
//		}

		// Set Current
		me->currentHsA_mA = 0;
		me->currentHsB_mA = 0;
		me->current_mA = 0;
	}
	else if (me->status == AlxVnh7040_Status_BreakVcc)
	{
		// Set Current
		me->currentHsA_mA = 0;
		me->currentHsB_mA = 0;
		me->current_mA = 0;
	}
	else if (me->status == AlxVnh7040_Status_BreakOpen)
	{
		// Set Current
		me->currentHsA_mA = 0;
		me->currentHsB_mA = 0;
		me->current_mA = 0;
	}
	else
	{
		ALX_VNH7040_ASSERT(false);	// We should never get here
	}
}

/**
  * @brief
  * @param[in,out] me
  * @param[in] duty_permil
  */
void AlxVnh7040_SetDuty_permil(AlxVnh7040* me, int16_t duty_permil)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);
	ALX_VNH7040_ASSERT((-1000 <= duty_permil) && (duty_permil <= 1000));

	// #1 Break
	if (duty_permil == 0)
	{
		switch (me->breakType)
		{
		case AlxVnh7040_BreakType_Gnd:
			me->duty_permil = 1000;
			me->do_PWM_val = true;
			me->do_INA_val = false;
			me->do_INB_val = false;
			me->status = AlxVnh7040_Status_BreakGnd;
			break;

		case AlxVnh7040_BreakType_Vcc:
			me->duty_permil = 0; 		// PWM val is don't care in this case, we will set it to 0%
			me->do_PWM_val = false; 	// PWM val is don't care in this case, we will set it to false
			me->do_INA_val = true;
			me->do_INB_val = true;
			me->status = AlxVnh7040_Status_BreakVcc;
			break;

		case AlxVnh7040_BreakType_Open:
			me->duty_permil = 0;
			me->do_PWM_val = false;
			me->do_INA_val = false;
			me->do_INB_val = false;
			me->status = AlxVnh7040_Status_BreakOpen;
			break;

		default:
			ALX_VNH7040_ASSERT (false);  // We should never get here
			break;
		}
	}
	// #2 Rotate Clockwise
	else if (duty_permil > 0)
	{
		me->duty_permil = duty_permil;
		me->do_PWM_val = true;
		me->do_INA_val = true;
		me->do_INB_val = false;
		me->status = AlxVnh7040_Status_RotateCw;
	}
	// #3 Rotate Counter-Clockwise
	else if (duty_permil < 0)
	{
		me->duty_permil = duty_permil * (-1);
		me->do_PWM_val = true;
		me->do_INA_val = false;
		me->do_INB_val = true;
		me->status = AlxVnh7040_Status_RotateCcw;
	}
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
uint32_t AlxVnh7040_GetCurrent_mA(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->current_mA;
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
uint32_t AlxVnh7040_GetCurrentHsA_mA(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->currentHsA_mA;
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
uint32_t AlxVnh7040_GetCurrentHsB_mA(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->currentHsB_mA;
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
uint32_t AlxVnh7040_GetSupplyVoltage_mV(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->supplyVoltage_mV;
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
int32_t AlxVnh7040_GetTemp_degC(AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->temp_degC;
}

/**
  * @brief
  * @param[in,out] me
  * @param[in] breakType
  */
int32_t AlxVnh7040_GetIsError (AlxVnh7040* me)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	return me->currentHsA_clamp || me->currentHsB_clamp;	// true-> short circuit or overcurrent protection is acctive on one of half bridges
}

/**
  * @brief
  * @param[in,out] me
  * @return
  */
void AlxVnh7040_Config_BreakType(AlxVnh7040* me, AlxVnh7040_BreakType breakType)
{
	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	me->breakType = breakType;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxVnh7040_Monitoring_Select_HSA(AlxVnh7040* me)
{
	AlxIoPin_Write(me->do_SEL0, true);
	AlxIoPin_Write(me->do_SEL1, false);
	me->monitoringTypeSelected = AlxVnh7040_MonitoringType_HSA;
}
static void AlxVnh7040_Monitoring_Select_HSB(AlxVnh7040* me)
{
	AlxIoPin_Write(me->do_SEL0, false);
	AlxIoPin_Write(me->do_SEL1, false);
	me->monitoringTypeSelected = AlxVnh7040_MonitoringType_HSB;
}
static void AlxVnh7040_Monitoring_Select_Tchip(AlxVnh7040* me)
{
	AlxIoPin_Write(me->do_SEL0, false);
	AlxIoPin_Write(me->do_SEL1, true);
	me->monitoringTypeSelected = AlxVnh7040_MonitoringType_Tchip;
}
static void AlxVnh7040_Monitoring_Select_Vcc(AlxVnh7040* me)
{
	AlxIoPin_Write(me->do_SEL0, true);
	AlxIoPin_Write(me->do_SEL1, true);
	me->monitoringTypeSelected = AlxVnh7040_MonitoringType_Vcc;
}
static void AlxVnh7040_Monitoring (AlxVnh7040* me)
{
	int32_t temp;

	ALX_VNH7040_ASSERT(me->wasCtorCalled == true);
	ALX_VNH7040_ASSERT(me->isInit == true);

	if(AlxTimSw_Get_us(&me->timMonitoringPeriod) >= me->MONITORING_PERIOD_MIN_us)
	{
		AlxTimSw_Start (&me->timMonitoringPeriod);

		// get value on multisense pin
		me->multiSensAdcVoltage_mV = AlxAdc_GetVoltage_mV(me->adc, me->multiSensAdcCh);
		me->multiSensPinVoltage_mV = AlxVdiv_GetVin_mV(me->multiSensAdcVoltage_mV, me->multiSensResHigh_ohm, me->multiSensResLow_ohm);
		me->multiSensVDivCurrent_uA = AlxVdiv_GetCurrent_uA(me->multiSensAdcVoltage_mV * 1000, me->multiSensResLow_ohm);

		// cycle thru all possibilities
		switch (me->monitoringTypeSelected)
		{
			case AlxVnh7040_MonitoringType_HSA:
			// get HSA current
			if (me->multiSensPinVoltage_mV > me->MULTI_SENS_ERR_INDICATE_PIN_VOLTAGE_THRESHOLD_mV)	// check for error flag
			{
				// error flag asserted
				me->currentHsA_clamp = true;
				me->currentHsA_mA = 0;
			}
			else
			{
				// operational
				me->currentHsA_clamp = false;
				me->currentHsA_mA = (me->multiSensVDivCurrent_uA * me->MULTI_SENS_CURRENT_COEFF) / 1000;
			}

			// jump to next measurement - HSB
			AlxVnh7040_Monitoring_Select_HSB (me);
			break;

			case AlxVnh7040_MonitoringType_HSB:
			// get HSB current
			if (me->multiSensPinVoltage_mV > me->MULTI_SENS_ERR_INDICATE_PIN_VOLTAGE_THRESHOLD_mV)	// check for error flag
			{
				// error flag asserted
				me->currentHsB_clamp = true;
				me->currentHsB_mA = 0;
			}
			else
			{
				// operational
				me->currentHsB_clamp = false;
				me->currentHsB_mA = (me->multiSensVDivCurrent_uA * me->MULTI_SENS_CURRENT_COEFF) / 1000;
			}

			// jump to next measurement - Tchip
			AlxVnh7040_Monitoring_Select_Tchip (me);
			break;

			case AlxVnh7040_MonitoringType_Tchip:
			// get Tchip temperature
			me->temp_degC = ((int32_t)(me->multiSensPinVoltage_mV * 1000) - me->VSENSE_TC_T0_uV) / me->dVSENSE_TC_dT_uV_K;

			// jump to next measurement - Vcc
			AlxVnh7040_Monitoring_Select_Vcc (me);
			break;

			case AlxVnh7040_MonitoringType_Vcc:
			// get Vcc voltage
			me->supplyVoltage_mV = me->multiSensPinVoltage_mV * me->dVSENSE_Vcc_k;

			// jump to next measurement - HSA
			AlxVnh7040_Monitoring_Select_HSA (me);
			break;

			default:
				ALX_VNH7040_ASSERT (false);  // We should never get here
				break;
		}
	}
}


#endif	// #if defined(ALX_C_LIB)
