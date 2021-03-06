/**
  ******************************************************************************
  * @file		alxAudioVol.h
  * @brief		Auralix C Library - ALX Audio Volume Module
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

#ifndef ALX_AUDIO_VOL_H
#define ALX_AUDIO_VOL_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxLinFun.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_VOL_FILE "alxAudioVol"

// Assert //
#if defined(_ALX_AUDIO_VOL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_VOL_FILE, expr)
#elif defined(_ALX_AUDIO_VOL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_VOL_FILE, expr)
#elif defined(_ALX_AUDIO_VOL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_VOL_FILE, expr)
#else
	#define ALX_AUDIO_VOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_AUDIO_VOL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_AUDIO_VOL_TRACE(...) ALX_TRACE_STD(ALX_AUDIO_VOL_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_VOL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_AUDIO_VOL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_AUDIO_VOL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_AUDIO_VOL_DBG_PIN(...) do{} while (false)
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


#ifdef __cplusplus
}
#endif

#endif // ALX_AUDIO_VOL_H
