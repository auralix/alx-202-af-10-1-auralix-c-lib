/**
  ******************************************************************************
  * @file		alxAudioVol.h
  * @brief		Auralix C Library - ALX Audio Volume Module
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
#ifndef ALX_AUDIO_VOL_H
#define ALX_AUDIO_VOL_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxLinFun.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_VOL_FILE "alxAudioVol.h"

// Assert //
#if defined(ALX_AUDIO_VOL_ASSERT_BKPT_ENABLE)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_VOL_FILE, expr)
#elif defined(ALX_AUDIO_VOL_ASSERT_TRACE_ENABLE)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_VOL_FILE, expr)
#elif defined(ALX_AUDIO_VOL_ASSERT_RST_ENABLE)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_VOL_FILE, expr)
#else
	#define ALX_AUDIO_VOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_AUDIO_VOL_TRACE_ENABLE)
	#define ALX_AUDIO_VOL_TRACE(...) ALX_TRACE_WRN(ALX_AUDIO_VOL_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_VOL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float volMin_dB;
	float volMax_dB;

	// Objects - Internal
	AlxLinFun linFun_x_pct_y_dB;

	// Variables
	float pctMax;
	float volFactor;	// Can be negative value for attenuation
} AlxAudioVol;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAudioVol_Ctor
(
	AlxAudioVol* me,
	float volMin_dB,
	float volMax_dB
);


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudioVol_Process(AlxAudioVol* me, float inSample);
void AlxAudioVol_Set_pct(AlxAudioVol* me, float vol_pct);
void AlxAudioVol_Set_dB(AlxAudioVol* me, float vol_dB);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_AUDIO_VOL_H
