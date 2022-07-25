/**
  ******************************************************************************
  * @file		alxNtcg103jf103ft1s.h
  * @brief		Auralix C Library - ALX NTC Thermistors NTCG103JF103FT1S Module
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
// Include Guard
//******************************************************************************
#ifndef ALX_NTCG103JF103FT1S_H
#define ALX_NTCG103JF103FT1S_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_NTCG103JF103FT1S_NUM_OF_POINTS 191


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	float xPointRes_kOhm[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	float yPointTemp_degC[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	uint32_t numOfPoints;
} AlxNtcg103jf103ft1s;


//******************************************************************************
// Variables
//******************************************************************************
extern const AlxNtcg103jf103ft1s alxNtcg103jf103ft1s;


#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_NTCG103JF103FT1S_H
