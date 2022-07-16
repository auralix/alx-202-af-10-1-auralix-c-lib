/**
  ******************************************************************************
  * @file		alxAudioVol.c
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

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAudioVol.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAudioVol_Ctor
(
	AlxAudioVol* me,
	float volMin_dB,
	float volMax_dB
)
{
	// Parameters
	me->volMin_dB = volMin_dB;
	me->volMax_dB = volMax_dB;

	// Ctor
	AlxLinFun_Ctor(&me->linFun_x_pct_y_dB, 0.f, me->volMin_dB, 100.f, me->volMax_dB, me->volMin_dB, me->volMax_dB, false);

	// Variables
	me->pctMax = AlxLinFun_GetX(&me->linFun_x_pct_y_dB, me->volMax_dB);
	me->volFactor = 0.f;	// Mute by default
}


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudioVol_Process(AlxAudioVol* me, float inSample)
{
	ALX_AUDIO_VOL_ASSERT((-1.f <= inSample) && (inSample <= 1.f));

	// #1 Calclulate
	float outSample = inSample * me->volFactor;

	// #2 Handle Clipping
	if (outSample > 1.f)		outSample = 1.f;
	else if (outSample < -1.f)	outSample = -1.f;

	// #3 Return
	return outSample;
}
void AlxAudioVol_Set_pct(AlxAudioVol* me, float vol_pct)
{
	ALX_AUDIO_VOL_ASSERT((0.f <= vol_pct) && (vol_pct <= me->pctMax));

	// #1 Set volFactor to 0 if vol = 0%
	if (vol_pct == 0.f)
	{
		me->volFactor = 0.f;
	}
	else
	{
		// #1 Get volume in dB from volume in %
		float vol_dB = AlxLinFun_GetY(&me->linFun_x_pct_y_dB, vol_pct);

		// #2 Set volFactor
		AlxAudioVol_Set_dB(me, vol_dB);
	}
}
void AlxAudioVol_Set_dB(AlxAudioVol* me, float vol_dB)
{
	ALX_AUDIO_VOL_ASSERT((me->volMin_dB <= vol_dB) && (vol_dB <= me->volMax_dB));

	me->volFactor = powf(10.f, (vol_dB / 20.f));
}
