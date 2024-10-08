/**
  ******************************************************************************
  * @file		alxAudio.h
  * @brief		Auralix C Library - ALX Audio Module
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
#ifndef ALX_AUDIO_H
#define ALX_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_FILE "alxAudio.h"

// Assert //
#if defined(ALX_AUDIO_ASSERT_BKPT_ENABLE)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_FILE, expr)
#elif defined(ALX_AUDIO_ASSERT_TRACE_ENABLE)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_FILE, expr)
#elif defined(ALX_AUDIO_ASSERT_RST_ENABLE)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_FILE, expr)
#else
	#define ALX_AUDIO_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_AUDIO_TRACE_ENABLE)
	#define ALX_AUDIO_TRACE(...) ALX_TRACE_WRN(ALX_AUDIO_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxAudio_Encoding_LinerPcm_Int8   = 0,
	AlxAudio_Encoding_LinerPcm_Uint8  = 1,
	AlxAudio_Encoding_LinerPcm_Int16  = 2,
	AlxAudio_Encoding_LinerPcm_Uint16 = 3,
	AlxAudio_Encoding_LinerPcm_Int16_InternalFlash  = 4
} AlxAudio_Encoding;


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudio_LinerPcmInt8ToFloat(int8_t inSample);
float AlxAudio_LinerPcmUint8ToFloat(uint8_t inSample);
float AlxAudio_LinerPcmInt16ToFloat(int16_t inSample);
float AlxAudio_LinerPcmUint16ToFloat(uint16_t inSample);
int8_t AlxAudio_FloatToLinerPcmInt8(float inSample);
int16_t AlxAudio_FloatToLinerPcmInt16(float inSample);
float AlxAudio_StereoToMono(float sampleL, float sampleR);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_AUDIO_H
