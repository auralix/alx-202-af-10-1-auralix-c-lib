/**
  ******************************************************************************
  * @file		alxAdau1961.c
  * @brief		Auralix C Library - ALX Audio Codec ADAU1961 Module
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
#include "alxAdau1961.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdau1961_RegStruct_SetAddr(AlxAdau1961* me);
static void AlxAdau1961_RegStruct_SetLen(AlxAdau1961* me);
static void AlxAdau1961_RegStruct_SetValToZero(AlxAdau1961* me);
static void AlxAdau1961_RegStruct_SetNonClkValToDefault(AlxAdau1961* me);
static Alx_Status AlxAdau1961_Reg_Write(AlxAdau1961* me, void* reg);
static Alx_Status AlxAdau1961_Reg_Read(AlxAdau1961* me, void* reg);
static Alx_Status AlxAdau1961_Reg_WriteNonClkVal(AlxAdau1961* me);
static uint8_t AlxAdau1961_CalcRegVal_InDiffGain(AlxAdau1961* me, float gain_dB);
static AlxAdau1961_General_GainMixer1Mixer2 AlxAdau1961_CalcRegVal_InAuxGain(AlxAdau1961* me, int8_t gain_dB);
static uint8_t AlxAdau1961_CalcRegVal_OutLineGain(AlxAdau1961* me, int8_t gain_dB);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxAdau1961_RegStruct_SetClkVal(AlxAdau1961* me);
void AlxAdau1961_RegStruct_SetNonClkVal(AlxAdau1961* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdau1961_Ctor
(
	AlxAdau1961* me,
	AlxI2s* i2s,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters - Const
	me->MUTE_GAIN_dB = -120;

	me->IN_DIFF_GAIN_MIN_dB = -12.f;
	me->IN_DIFF_GAIN_MAX_dB = 55.5f;
	me->IN_DIFF_GAIN_PER_BIT_dB = 0.75f;
	me->IN_DIFF_GAIN_OFFSET_dB = -12.f;
	me->IN_DIFF_PGA_BOOST_THRESHOLD_dB = 35.25f;
	me->IN_DIFF_PGA_BOOST_REDUCE_VAL_dB = 20.25f;

	me->IN_AUX_GAIN_MIN_dB = -12;
	me->IN_AUX_GAIN_MAX_dB = 6;
	me->IN_AUX_GAIN_PER_BIT_dB = 3;
	me->IN_AUX_GAIN_OFFSET_dB = -12;
	me->IN_AUX_MUTE_OFFSET_dB = 1;

	me->OUT_LINE_GAIN_MIN_dB = -57;
	me->OUT_LINE_GAIN_MAX_dB = 6;
	me->OUT_LINE_GAIN_PER_BIT_dB = 1;
	me->OUT_LINE_GAIN_OFFSET_dB = -57;

	// Objects - External
	me->i2s = i2s;
	me->i2c = i2c;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxAdau1961_RegStruct_SetAddr(me);
	AlxAdau1961_RegStruct_SetLen(me);
	AlxAdau1961_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdau1961_Init(AlxAdau1961* me)
{
	ALX_ADAU1961_ASSERT(me->isInit == false);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Init I2S
	status = AlxI2s_Init(me->i2s);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_AlxI2s_Init"); return status; }

	// #2 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_AlxI2c_Init"); return status; }

	// #3 Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 3, 1000);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }

	// #4 Set clock register values - WEAK
	AlxAdau1961_RegStruct_SetClkVal(me);

	//// TV: TODO future ////
	// #4.2 If MCLK is not used
//	if(me->i2s->isMclkUsed != true)
//	{
//		// #4.2.1 Write Clk R1_PllControl register
//		status = AlxAdau1961_Reg_Write(me, &me->reg.R1_PllControl);
//		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R1_PllControl"); return status; }
//
//		// #4.2.2 Poll PLL lock bit in R1
//		status = AlxAdau1961_WaitUntilPllLock(me);
//		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R1_PllLockBit"); return status; }
//	}

	// #5 Write clock register R0_ClockControl
	status = AlxAdau1961_Reg_Write(me, &me->reg.R0_ClockControl);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R0_ClockControl"); return status; }

	// #5 Set non-clock registers values to default
	 AlxAdau1961_RegStruct_SetNonClkValToDefault(me);

	// #6 Set non-clock registers values - WEAK
	AlxAdau1961_RegStruct_SetNonClkVal(me);

	// #7 Write non-clock registers
	status = AlxAdau1961_Reg_WriteNonClkVal(me);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_Reg_WriteNonClkVal"); return status;}

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_DeInit(AlxAdau1961* me)
{
	// TODO
	ALX_ADAU1961_ASSERT(false);

	return Alx_Err;
}
void AlxAdau1961_Foreground_Handle(AlxAdau1961* me)
{
	AlxI2s_Foreground_Handle(me->i2s);
}
Alx_Status AlxAdau1961_InDiffL_SetGain_dB(AlxAdau1961* me, float gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);
	ALX_ADAU1961_ASSERT(fmodf(gain_dB, me->IN_DIFF_GAIN_PER_BIT_dB) == 0);

	Alx_Status status = Alx_Err;

	// #1 Set Mute
	if (gain_dB == (float)me->MUTE_GAIN_dB)
	{
		me->reg.R8_LeftDiffInputVol.val.LDMUTE = DiffInputMuteControl_Mute;
		Alx_Status status = AlxAdau1961_Reg_Write(me, &me->reg.R8_LeftDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}
	// #2 Set Gain - Gain is less or equal than 35.25dB
	else if (gain_dB <= me->IN_DIFF_PGA_BOOST_THRESHOLD_dB)
	{
		ALX_ADAU1961_ASSERT((me->IN_DIFF_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->IN_DIFF_PGA_BOOST_THRESHOLD_dB));

		// #2.1 Set PGA gain boost 0dB
		me->reg.R5_RecMixerLeft1.val.LDBOOST = DiffPGAInputGain_0dB;
		status = AlxAdau1961_Reg_Write(me, &me->reg.R5_RecMixerLeft1);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

		// #2.2 Un-Mute & Set PGA gain
		float _gain_dB = gain_dB;	// We do not need to substract anything
		me->reg.R8_LeftDiffInputVol.val.LDMUTE = DiffInputMuteControl_UnMute;
		me->reg.R8_LeftDiffInputVol.val.LDVOL = AlxAdau1961_CalcRegVal_InDiffGain(me, _gain_dB);
		status = AlxAdau1961_Reg_Write(me, &me->reg.R8_LeftDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}
	// #3 Set Gain - Gain is larger than 35.25dB
	else
	{
		ALX_ADAU1961_ASSERT((me->IN_DIFF_PGA_BOOST_THRESHOLD_dB < gain_dB) && (gain_dB <= me->IN_DIFF_GAIN_MAX_dB));

		// #3.1 Set PGA gain boost +20dB
		me->reg.R5_RecMixerLeft1.val.LDBOOST = DiffPGAInputGain_20dB;
		status = AlxAdau1961_Reg_Write(me, &me->reg.R5_RecMixerLeft1);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

		// #3.2 Un-Mute & Set PGA gain
		float _gain_dB = gain_dB - me->IN_DIFF_PGA_BOOST_REDUCE_VAL_dB;	// We must reduce set gain because PGA gain boost is set to +20dB (we reduce it by 20.25dB, to get number which is multiple of 0.75dB to have uniform 0.75dB/bit, for easier handling)
		ALX_ADAU1961_ASSERT(fmodf(_gain_dB, me->IN_DIFF_GAIN_PER_BIT_dB) == 0);
		me->reg.R8_LeftDiffInputVol.val.LDMUTE = DiffInputMuteControl_UnMute;
		me->reg.R8_LeftDiffInputVol.val.LDVOL = AlxAdau1961_CalcRegVal_InDiffGain(me, _gain_dB);
		status = AlxAdau1961_Reg_Write(me, &me->reg.R8_LeftDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_InDiffR_SetGain_dB(AlxAdau1961* me, float gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);
	ALX_ADAU1961_ASSERT(fmodf(gain_dB, me->IN_DIFF_GAIN_PER_BIT_dB) == 0);

	Alx_Status status = Alx_Err;

	// #1 Set Mute
	if (gain_dB == (float)me->MUTE_GAIN_dB)
	{
		me->reg.R9_RightDiffInputVol.val.RDMUTE = DiffInputMuteControl_Mute;
		Alx_Status status = AlxAdau1961_Reg_Write(me, &me->reg.R9_RightDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}
	// #2 Set Gain - Gain is less or equal than 35.25dB
	else if (gain_dB <= me->IN_DIFF_PGA_BOOST_THRESHOLD_dB)
	{
		ALX_ADAU1961_ASSERT((me->IN_DIFF_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->IN_DIFF_PGA_BOOST_THRESHOLD_dB));

		// #2.1 Set PGA gain boost 0dB
		me->reg.R7_RecMixerRight1.val.RDBOOST = DiffPGAInputGain_0dB;
		status = AlxAdau1961_Reg_Write(me, &me->reg.R7_RecMixerRight1);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

		// #2.2 Un-Mute & Set PGA gain
		float _gain_dB = gain_dB;	// We do not need to substract anything
		me->reg.R9_RightDiffInputVol.val.RDMUTE = DiffInputMuteControl_UnMute;
		me->reg.R9_RightDiffInputVol.val.RDVOL = AlxAdau1961_CalcRegVal_InDiffGain(me, _gain_dB);
		status = AlxAdau1961_Reg_Write(me, &me->reg.R9_RightDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}
	// #3 Set Gain - Gain is larger than 35.25dB
	else
	{
		ALX_ADAU1961_ASSERT((me->IN_DIFF_PGA_BOOST_THRESHOLD_dB < gain_dB) && (gain_dB <= me->IN_DIFF_GAIN_MAX_dB));

		// #3.1 Set PGA gain boost +20dB
		me->reg.R7_RecMixerRight1.val.RDBOOST = DiffPGAInputGain_20dB;
		status = AlxAdau1961_Reg_Write(me, &me->reg.R7_RecMixerRight1);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

		// #3.2 Un-Mute & Set PGA gain
		float _gain_dB = gain_dB - me->IN_DIFF_PGA_BOOST_REDUCE_VAL_dB;	// We must reduce set gain because PGA gain boost is set to +20dB (we reduce it by 20.25dB, to get number which is multiple of 0.75dB to have uniform 0.75dB/bit, for easier handling)
		ALX_ADAU1961_ASSERT(fmodf(_gain_dB, me->IN_DIFF_GAIN_PER_BIT_dB) == 0);
		me->reg.R9_RightDiffInputVol.val.RDMUTE = DiffInputMuteControl_UnMute;
		me->reg.R9_RightDiffInputVol.val.RDVOL = AlxAdau1961_CalcRegVal_InDiffGain(me, _gain_dB);
		status = AlxAdau1961_Reg_Write(me, &me->reg.R9_RightDiffInputVol);
		if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }
	}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_InAuxL_SetGain_dB(AlxAdau1961* me, int8_t gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);
	ALX_ADAU1961_ASSERT((gain_dB % me->IN_AUX_GAIN_PER_BIT_dB) == 0);

	// #1 Set Mute
	if (gain_dB == me->MUTE_GAIN_dB)
	{
		me->reg.R5_RecMixerLeft1.val.MX1AUXG = GainMixer1Mixer2_Mute;
	}
	// #2 Set Gain
	else
	{
		ALX_ADAU1961_ASSERT((me->IN_AUX_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->IN_AUX_GAIN_MAX_dB));

		me->reg.R5_RecMixerLeft1.val.MX1AUXG = AlxAdau1961_CalcRegVal_InAuxGain(me, gain_dB);
	}

	// #3 Write Reg
	Alx_Status status = AlxAdau1961_Reg_Write(me, &me->reg.R5_RecMixerLeft1);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_InAuxR_SetGain_dB(AlxAdau1961* me, int8_t gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);
	ALX_ADAU1961_ASSERT((gain_dB % me->IN_AUX_GAIN_PER_BIT_dB) == 0);

	// #1 Mute
	if (gain_dB == me->MUTE_GAIN_dB)
	{
		me->reg.R7_RecMixerRight1.val.MX2AUXG = GainMixer1Mixer2_Mute;
	}
	// #2 Set Gain
	else
	{
		ALX_ADAU1961_ASSERT((me->IN_AUX_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->IN_AUX_GAIN_MAX_dB));

		me->reg.R7_RecMixerRight1.val.MX2AUXG = AlxAdau1961_CalcRegVal_InAuxGain(me, gain_dB);
	}

	// #3 Write Reg
	Alx_Status status = AlxAdau1961_Reg_Write(me, &me->reg.R7_RecMixerRight1);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_OutLineL_SetGain_dB(AlxAdau1961* me, int8_t gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Mute
	if (gain_dB == me->MUTE_GAIN_dB)
	{
		me->reg.R31_LineOutputLeftVol.val.LOUTM = OutputMute_Mute;
	}
	// #2 Un-Mute & Set Gain
	else
	{
		ALX_ADAU1961_ASSERT((me->OUT_LINE_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->OUT_LINE_GAIN_MAX_dB));

		me->reg.R31_LineOutputLeftVol.val.LOUTVOL = AlxAdau1961_CalcRegVal_OutLineGain(me, gain_dB);
		me->reg.R31_LineOutputLeftVol.val.LOUTM = OutputMute_UnMute;
	}

	// #3 Write Reg
	status = AlxAdau1961_Reg_Write(me, &me->reg.R31_LineOutputLeftVol);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status;}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdau1961_OutLineR_SetGain_dB(AlxAdau1961* me, int8_t gain_dB)
{
	ALX_ADAU1961_ASSERT(me->isInit == true);
	ALX_ADAU1961_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Mute
	if (gain_dB == me->MUTE_GAIN_dB)
	{
		me->reg.R32_LineOutputRightVol.val.ROUTM = OutputMute_Mute;
	}
	// #2 Un-Mute & Set Gain
	else
	{
		ALX_ADAU1961_ASSERT((me->OUT_LINE_GAIN_MIN_dB <= gain_dB) && (gain_dB <= me->OUT_LINE_GAIN_MAX_dB));

		me->reg.R32_LineOutputRightVol.val.ROUTVOL = AlxAdau1961_CalcRegVal_OutLineGain(me, gain_dB);
		me->reg.R32_LineOutputRightVol.val.ROUTM = OutputMute_UnMute;
	}

	// #3 Write Reg
	status = AlxAdau1961_Reg_Write(me, &me->reg.R32_LineOutputRightVol);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err"); return status;}

	// #4 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdau1961_RegStruct_SetAddr(AlxAdau1961* me)
{
	me->reg.R0_ClockControl			.addr = 0x4000;
	me->reg.R1_PllControl			.addr = 0x4002;
	me->reg.R2_DigMicJackDetect		.addr = 0x4008;
//	me->reg.R3_Reserved				.addr = 0x4009;
	me->reg.R4_RecMixerLeft0		.addr = 0x400A;
	me->reg.R5_RecMixerLeft1		.addr = 0x400B;
	me->reg.R6_RecMixerRight0		.addr = 0x400C;
	me->reg.R7_RecMixerRight1		.addr = 0x400D;
	me->reg.R8_LeftDiffInputVol		.addr = 0x400E;
	me->reg.R9_RightDiffInputVol	.addr = 0x400F;
	me->reg.R10_RecordMicBias		.addr = 0x4010;
	me->reg.R11_Alc0				.addr = 0x4011;
	me->reg.R12_Alc1				.addr = 0x4012;
	me->reg.R13_Alc2				.addr = 0x4013;
	me->reg.R14_Alc3				.addr = 0x4014;
	me->reg.R15_SerialPort0			.addr = 0x4015;
	me->reg.R16_SerialPort1			.addr = 0x4016;
	me->reg.R17_Converter0			.addr = 0x4017;
	me->reg.R18_Converter1			.addr = 0x4018;
	me->reg.R19_AdcControl			.addr = 0x4019;
	me->reg.R20_LeftDigitalVol		.addr = 0x401A;
	me->reg.R21_RightDigitalVol		.addr = 0x401B;
	me->reg.R22_PlayMixerLeft0		.addr = 0x401C;
	me->reg.R23_PlayMixerLeft1		.addr = 0x401D;
	me->reg.R24_PlayMixerRight0		.addr = 0x401E;
	me->reg.R25_PlayMixerRight1		.addr = 0x401F;
	me->reg.R26_PlayLrMixerLeft		.addr = 0x4020;
	me->reg.R27_PlayLrMixerRight	.addr = 0x4021;
	me->reg.R28_PlayLrMixerMono		.addr = 0x4022;
	me->reg.R29_PlayHpLeftVol		.addr = 0x4023;
	me->reg.R30_PlayHpRightVol		.addr = 0x4024;
	me->reg.R31_LineOutputLeftVol	.addr = 0x4025;
	me->reg.R32_LineOutputRightVol	.addr = 0x4026;
	me->reg.R33_PlayMonoOutput		.addr = 0x4027;
	me->reg.R34_PopClickSuppress	.addr = 0x4028;
	me->reg.R35_PlayPowerMgmt		.addr = 0x4029;
	me->reg.R36_DacControl0			.addr = 0x402A;
	me->reg.R37_DacControl1			.addr = 0x402B;
	me->reg.R38_DacControl2			.addr = 0x402C;
	me->reg.R39_SerialPortPad		.addr = 0x402D;
	me->reg.R40_ControlPortPad0		.addr = 0x402F;
	me->reg.R41_ControlPortPad1		.addr = 0x4030;
	me->reg.R42_JackDetectPin		.addr = 0x4031;
	me->reg.R67_DejitterControl		.addr = 0x4036;
}
static void AlxAdau1961_RegStruct_SetLen(AlxAdau1961* me)
{
	me->reg.R0_ClockControl			.len = sizeof(me->reg.R0_ClockControl			.val);
	me->reg.R1_PllControl			.len = sizeof(me->reg.R1_PllControl				.val);
	me->reg.R2_DigMicJackDetect		.len = sizeof(me->reg.R2_DigMicJackDetect		.val);
//	me->reg.R3_Reserved				.len = sizeof(me->reg.R3_Reserved				.val);
	me->reg.R4_RecMixerLeft0		.len = sizeof(me->reg.R4_RecMixerLeft0			.val);
	me->reg.R5_RecMixerLeft1		.len = sizeof(me->reg.R5_RecMixerLeft1			.val);
	me->reg.R6_RecMixerRight0		.len = sizeof(me->reg.R6_RecMixerRight0			.val);
	me->reg.R7_RecMixerRight1		.len = sizeof(me->reg.R7_RecMixerRight1			.val);
	me->reg.R8_LeftDiffInputVol		.len = sizeof(me->reg.R8_LeftDiffInputVol		.val);
	me->reg.R9_RightDiffInputVol	.len = sizeof(me->reg.R9_RightDiffInputVol		.val);
	me->reg.R10_RecordMicBias		.len = sizeof(me->reg.R10_RecordMicBias			.val);
	me->reg.R11_Alc0				.len = sizeof(me->reg.R11_Alc0					.val);
	me->reg.R12_Alc1				.len = sizeof(me->reg.R12_Alc1					.val);
	me->reg.R13_Alc2				.len = sizeof(me->reg.R13_Alc2					.val);
	me->reg.R14_Alc3				.len = sizeof(me->reg.R14_Alc3					.val);
	me->reg.R15_SerialPort0			.len = sizeof(me->reg.R15_SerialPort0			.val);
	me->reg.R16_SerialPort1			.len = sizeof(me->reg.R16_SerialPort1			.val);
	me->reg.R17_Converter0			.len = sizeof(me->reg.R17_Converter0			.val);
	me->reg.R18_Converter1			.len = sizeof(me->reg.R18_Converter1			.val);
	me->reg.R19_AdcControl			.len = sizeof(me->reg.R19_AdcControl			.val);
	me->reg.R20_LeftDigitalVol		.len = sizeof(me->reg.R20_LeftDigitalVol		.val);
	me->reg.R21_RightDigitalVol		.len = sizeof(me->reg.R21_RightDigitalVol		.val);
	me->reg.R22_PlayMixerLeft0		.len = sizeof(me->reg.R22_PlayMixerLeft0		.val);
	me->reg.R23_PlayMixerLeft1		.len = sizeof(me->reg.R23_PlayMixerLeft1		.val);
	me->reg.R24_PlayMixerRight0		.len = sizeof(me->reg.R24_PlayMixerRight0		.val);
	me->reg.R25_PlayMixerRight1		.len = sizeof(me->reg.R25_PlayMixerRight1		.val);
	me->reg.R26_PlayLrMixerLeft		.len = sizeof(me->reg.R26_PlayLrMixerLeft		.val);
	me->reg.R27_PlayLrMixerRight	.len = sizeof(me->reg.R27_PlayLrMixerRight		.val);
	me->reg.R28_PlayLrMixerMono		.len = sizeof(me->reg.R28_PlayLrMixerMono		.val);
	me->reg.R29_PlayHpLeftVol		.len = sizeof(me->reg.R29_PlayHpLeftVol			.val);
	me->reg.R30_PlayHpRightVol		.len = sizeof(me->reg.R30_PlayHpRightVol		.val);
	me->reg.R31_LineOutputLeftVol	.len = sizeof(me->reg.R31_LineOutputLeftVol		.val);
	me->reg.R32_LineOutputRightVol	.len = sizeof(me->reg.R32_LineOutputRightVol	.val);
	me->reg.R33_PlayMonoOutput		.len = sizeof(me->reg.R33_PlayMonoOutput		.val);
	me->reg.R34_PopClickSuppress	.len = sizeof(me->reg.R34_PopClickSuppress		.val);
	me->reg.R35_PlayPowerMgmt		.len = sizeof(me->reg.R35_PlayPowerMgmt			.val);
	me->reg.R36_DacControl0			.len = sizeof(me->reg.R36_DacControl0			.val);
	me->reg.R37_DacControl1			.len = sizeof(me->reg.R37_DacControl1			.val);
	me->reg.R38_DacControl2			.len = sizeof(me->reg.R38_DacControl2			.val);
	me->reg.R39_SerialPortPad		.len = sizeof(me->reg.R39_SerialPortPad			.val);
	me->reg.R40_ControlPortPad0		.len = sizeof(me->reg.R40_ControlPortPad0		.val);
	me->reg.R41_ControlPortPad1		.len = sizeof(me->reg.R41_ControlPortPad1		.val);
	me->reg.R42_JackDetectPin		.len = sizeof(me->reg.R42_JackDetectPin			.val);
	me->reg.R67_DejitterControl		.len = sizeof(me->reg.R67_DejitterControl		.val);
}
static void AlxAdau1961_RegStruct_SetValToZero(AlxAdau1961* me)
{
	me->reg.R0_ClockControl			.val.raw	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[0]	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[1]	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[2]	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[3]	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[4]	= 0b00000000;
	me->reg.R1_PllControl			.val.raw[5]	= 0b00000000;
	me->reg.R2_DigMicJackDetect		.val.raw	= 0b00000000;
//	me->reg.R3_Reserved				.val.raw	= 0b00000000;
	me->reg.R4_RecMixerLeft0		.val.raw	= 0b00000000;
	me->reg.R5_RecMixerLeft1		.val.raw	= 0b00000000;
	me->reg.R6_RecMixerRight0		.val.raw	= 0b00000000;
	me->reg.R7_RecMixerRight1		.val.raw	= 0b00000000;
	me->reg.R8_LeftDiffInputVol		.val.raw	= 0b00000000;
	me->reg.R9_RightDiffInputVol	.val.raw	= 0b00000000;
	me->reg.R10_RecordMicBias		.val.raw	= 0b00000000;
	me->reg.R11_Alc0				.val.raw	= 0b00000000;
	me->reg.R12_Alc1				.val.raw	= 0b00000000;
	me->reg.R13_Alc2				.val.raw	= 0b00000000;
	me->reg.R14_Alc3				.val.raw	= 0b00000000;
	me->reg.R15_SerialPort0			.val.raw	= 0b00000000;
	me->reg.R16_SerialPort1			.val.raw	= 0b00000000;
	me->reg.R17_Converter0			.val.raw	= 0b00000000;
	me->reg.R18_Converter1			.val.raw	= 0b00000000;
	me->reg.R19_AdcControl			.val.raw	= 0b00000000;
	me->reg.R20_LeftDigitalVol		.val.raw	= 0b00000000;
	me->reg.R21_RightDigitalVol		.val.raw	= 0b00000000;
	me->reg.R22_PlayMixerLeft0		.val.raw	= 0b00000000;
	me->reg.R23_PlayMixerLeft1		.val.raw	= 0b00000000;
	me->reg.R24_PlayMixerRight0		.val.raw	= 0b00000000;
	me->reg.R25_PlayMixerRight1		.val.raw	= 0b00000000;
	me->reg.R26_PlayLrMixerLeft		.val.raw	= 0b00000000;
	me->reg.R27_PlayLrMixerRight	.val.raw	= 0b00000000;
	me->reg.R28_PlayLrMixerMono		.val.raw	= 0b00000000;
	me->reg.R29_PlayHpLeftVol		.val.raw	= 0b00000000;
	me->reg.R30_PlayHpRightVol		.val.raw	= 0b00000000;
	me->reg.R31_LineOutputLeftVol	.val.raw	= 0b00000000;
	me->reg.R32_LineOutputRightVol	.val.raw	= 0b00000000;
	me->reg.R33_PlayMonoOutput		.val.raw	= 0b00000000;
	me->reg.R34_PopClickSuppress	.val.raw	= 0b00000000;
	me->reg.R35_PlayPowerMgmt		.val.raw	= 0b00000000;
	me->reg.R36_DacControl0			.val.raw	= 0b00000000;
	me->reg.R37_DacControl1			.val.raw	= 0b00000000;
	me->reg.R38_DacControl2			.val.raw	= 0b00000000;
	me->reg.R39_SerialPortPad		.val.raw	= 0b00000000;
	me->reg.R40_ControlPortPad0		.val.raw	= 0b00000000;
	me->reg.R41_ControlPortPad1		.val.raw	= 0b00000000;
	me->reg.R42_JackDetectPin		.val.raw	= 0b00000000;
	me->reg.R67_DejitterControl		.val.raw	= 0b00000000;
}
static void AlxAdau1961_RegStruct_SetNonClkValToDefault(AlxAdau1961* me)
{
	me->reg.R2_DigMicJackDetect		.val.raw	= 0b00000000;
//	me->reg.R3_Reserved				.val.raw	= 0b00000000;
	me->reg.R4_RecMixerLeft0		.val.raw	= 0b00000000;
	me->reg.R5_RecMixerLeft1		.val.raw	= 0b00000000;
	me->reg.R6_RecMixerRight0		.val.raw	= 0b00000000;
	me->reg.R7_RecMixerRight1		.val.raw	= 0b00000000;
	me->reg.R8_LeftDiffInputVol		.val.raw	= 0b00000000;
	me->reg.R9_RightDiffInputVol	.val.raw	= 0b00000000;
	me->reg.R10_RecordMicBias		.val.raw	= 0b00000000;
	me->reg.R11_Alc0				.val.raw	= 0b00000000;
	me->reg.R12_Alc1				.val.raw	= 0b00000000;
	me->reg.R13_Alc2				.val.raw	= 0b00000000;
	me->reg.R14_Alc3				.val.raw	= 0b00000000;
	me->reg.R15_SerialPort0			.val.raw	= 0b00000000;
	me->reg.R16_SerialPort1			.val.raw	= 0b00000000;
	me->reg.R17_Converter0			.val.raw	= 0b00000000;
	me->reg.R18_Converter1			.val.raw	= 0b00000000;
	me->reg.R19_AdcControl			.val.raw	= 0b00010000;
	me->reg.R20_LeftDigitalVol		.val.raw	= 0b00000000;
	me->reg.R21_RightDigitalVol		.val.raw	= 0b00000000;
	me->reg.R22_PlayMixerLeft0		.val.raw	= 0b00000000;
	me->reg.R23_PlayMixerLeft1		.val.raw	= 0b00000000;
	me->reg.R24_PlayMixerRight0		.val.raw	= 0b00000000;
	me->reg.R25_PlayMixerRight1		.val.raw	= 0b00000000;
	me->reg.R26_PlayLrMixerLeft		.val.raw	= 0b00000000;
	me->reg.R27_PlayLrMixerRight	.val.raw	= 0b00000000;
	me->reg.R28_PlayLrMixerMono		.val.raw	= 0b00000000;
	me->reg.R29_PlayHpLeftVol		.val.raw	= 0b00000010;
	me->reg.R30_PlayHpRightVol		.val.raw	= 0b00000010;
	me->reg.R31_LineOutputLeftVol	.val.raw	= 0b00000010;
	me->reg.R32_LineOutputRightVol	.val.raw	= 0b00000010;
	me->reg.R33_PlayMonoOutput		.val.raw	= 0b00000010;
	me->reg.R34_PopClickSuppress	.val.raw	= 0b00000000;
	me->reg.R35_PlayPowerMgmt		.val.raw	= 0b00000000;
	me->reg.R36_DacControl0			.val.raw	= 0b00000000;
	me->reg.R37_DacControl1			.val.raw	= 0b00000000;
	me->reg.R38_DacControl2			.val.raw	= 0b00000000;
	me->reg.R39_SerialPortPad		.val.raw	= 0b10101010;
	me->reg.R40_ControlPortPad0		.val.raw	= 0b10101010;
	me->reg.R41_ControlPortPad1		.val.raw	= 0b00000000;
	me->reg.R42_JackDetectPin		.val.raw	= 0b00001000;
	me->reg.R67_DejitterControl		.val.raw	= 0b00000011;
}
static Alx_Status AlxAdau1961_Reg_Write(AlxAdau1961* me, void* reg)
{
	uint16_t regAddr = *((uint16_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(uint16_t));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(uint16_t) + sizeof(uint8_t);

	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_16bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
}
static Alx_Status AlxAdau1961_Reg_Read(AlxAdau1961* me, void* reg)
{
	uint16_t regAddr = *((uint16_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(uint16_t));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(uint16_t) + sizeof(uint8_t);

	return AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_16bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
}
static Alx_Status AlxAdau1961_Reg_WriteNonClkVal(AlxAdau1961* me)
{
	Alx_Status status = AlxAdau1961_Reg_Write(me, &me->reg.R2_DigMicJackDetect);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R2_DigMicJackDetect		"); return status;}

	//status = AlxAdau1961_Reg_Write(me, &me->reg.R3_Reserved			);
	//if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R3_Reserved			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R4_RecMixerLeft0		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R4_RecMixerLeft0		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R5_RecMixerLeft1		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R5_RecMixerLeft1		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R6_RecMixerRight0		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R6_RecMixerRight0		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R7_RecMixerRight1		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R7_RecMixerRight1		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R8_LeftDiffInputVol		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R8_LeftDiffInputVol		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R9_RightDiffInputVol	);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R9_RightDiffInputVol	"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R10_RecordMicBias		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R10_RecordMicBias		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R11_Alc0				);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R11_Alc0				"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R12_Alc1				);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R12_Alc1				"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R13_Alc2				);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R13_Alc2				"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R14_Alc3				);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R14_Alc3				"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R15_SerialPort0			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R15_SerialPort0			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R16_SerialPort1			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R16_SerialPort1			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R17_Converter0			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R17_Converter0			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R18_Converter1			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R18_Converter1			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R19_AdcControl			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R19_AdcControl			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R20_LeftDigitalVol		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R20_LeftDigitalVol		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R21_RightDigitalVol		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R21_RightDigitalVol		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R22_PlayMixerLeft0		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R22_PlayMixerLeft0		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R23_PlayMixerLeft1		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R23_PlayMixerLeft1		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R24_PlayMixerRight0		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R24_PlayMixerRight0		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R25_PlayMixerRight1		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R25_PlayMixerRight1		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R26_PlayLrMixerLeft		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R26_PlayLrMixerLeft		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R27_PlayLrMixerRight	);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R27_PlayLrMixerRight	"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R28_PlayLrMixerMono		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R28_PlayLrMixerMono		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R29_PlayHpLeftVol		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R29_PlayHpLeftVol		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R30_PlayHpRightVol		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R30_PlayHpRightVol		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R31_LineOutputLeftVol	);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R31_LineOutputLeftVol	"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R32_LineOutputRightVol	);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R32_LineOutputRightVol	"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R33_PlayMonoOutput		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R33_PlayMonoOutput		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R34_PopClickSuppress	);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R34_PopClickSuppress	"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R35_PlayPowerMgmt		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R35_PlayPowerMgmt		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R36_DacControl0			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R36_DacControl0			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R37_DacControl1			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R37_DacControl1			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R38_DacControl2			);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R38_DacControl2			"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R39_SerialPortPad		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R39_SerialPortPad		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R40_ControlPortPad0		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R40_ControlPortPad0		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R41_ControlPortPad1		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R41_ControlPortPad1		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R42_JackDetectPin		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R42_JackDetectPin		"); return status;}

	status = AlxAdau1961_Reg_Write(me, &me->reg.R67_DejitterControl		);
	if (status != Alx_Ok) { ALX_ADAU1961_TRACE("Err_R67_DejitterControl		"); return status;}

	return Alx_Ok;
}
static uint8_t AlxAdau1961_CalcRegVal_InDiffGain(AlxAdau1961* me, float gain_dB)
{
	uint8_t regVal = (uint8_t)((gain_dB - me->IN_DIFF_GAIN_OFFSET_dB) / me->IN_DIFF_GAIN_PER_BIT_dB);
	return regVal;
}
static AlxAdau1961_General_GainMixer1Mixer2 AlxAdau1961_CalcRegVal_InAuxGain(AlxAdau1961* me, int8_t gain_dB)
{
	AlxAdau1961_General_GainMixer1Mixer2 regVal = (AlxAdau1961_General_GainMixer1Mixer2)(((gain_dB - me->IN_AUX_GAIN_OFFSET_dB) / me->IN_AUX_GAIN_PER_BIT_dB) + me->IN_AUX_MUTE_OFFSET_dB);
	return regVal;
}
static uint8_t AlxAdau1961_CalcRegVal_OutLineGain(AlxAdau1961* me, int8_t gain_dB)
{
	uint8_t regVal = (uint8_t)((gain_dB - me->OUT_LINE_GAIN_OFFSET_dB) / me->OUT_LINE_GAIN_PER_BIT_dB);
	return regVal;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxAdau1961_RegStruct_SetClkVal(AlxAdau1961* me)
{
	(void)me;

	ALX_ADAU1961_TRACE("Define 'AlxAdau1961_RegStruct_SetClkVal' function in your application.");
	ALX_ADAU1961_ASSERT(false);
}
ALX_WEAK void AlxAdau1961_RegStruct_SetNonClkVal(AlxAdau1961* me)
{
	(void)me;

	ALX_ADAU1961_TRACE("Define 'AlxAdau1961_RegStruct_SetNonClkVal' function in your application.");
	ALX_ADAU1961_ASSERT(false);
}


#endif // #if defined(ALX_C_LIB)
