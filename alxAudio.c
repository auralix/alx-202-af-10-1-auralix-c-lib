/**
  ******************************************************************************
  * @file		alxAudio.c
  * @brief		Auralix C Library - ALX Audio Module
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
#include "alxAudio.h"


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudio_LinerPcmInt8ToFloat(int8_t inSample)
{
	float outSample = (float)inSample / 128.f;
	return outSample;
}
float AlxAudio_LinerPcmUint8ToFloat(uint8_t inSample)
{
	uint8_t sampleUint8 = inSample - (uint8_t)128;	// Converts decimal to 2's complement
	float outSample = (float)((int8_t)sampleUint8 / 128.f);
	return outSample;
}
float AlxAudio_LinerPcmInt16ToFloat(int16_t inSample)
{
	float outSample = (float)(inSample / 32768.f);
	return outSample;
}
float AlxAudio_LinerPcmUint16ToFloat(uint16_t inSample)
{
	uint16_t dummy = inSample - 32768U;	// Converts decimal to 2's complement
	float outSample = (float)((int16_t)dummy / 32768.f);
	return outSample;
}
int8_t AlxAudio_FloatToLinerPcmInt8(float inSample)
{
	ALX_AUDIO_ASSERT((-1.f <= inSample) && (inSample <= 1.f));

	int8_t outSample = (int8_t)(inSample * 127.f);
	return outSample;
}
int16_t AlxAudio_FloatToLinerPcmInt16(float inSample)
{
	ALX_AUDIO_ASSERT((-1.f <= inSample) && (inSample <= 1.f));

	int16_t outSample = (int16_t)(inSample * 32767.f);
	return outSample;
}
float AlxAudio_StereoToMono(float sampleL, float sampleR)
{
	ALX_AUDIO_ASSERT((-1.f <= sampleL) && (sampleL <= 1.f));
	ALX_AUDIO_ASSERT((-1.f <= sampleR) && (sampleR <= 1.f));

	float outSample = (sampleL + sampleR) / 2.f;
	return outSample;
}
