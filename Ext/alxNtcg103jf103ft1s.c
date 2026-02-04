/**
  ******************************************************************************
  * @file		alxNtcg103jf103ft1s.c
  * @brief		Auralix C Library - ALX NTC Thermistors NTCG103JF103FT1S Module
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
#include "alxNtcg103jf103ft1s.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Defines
//******************************************************************************
#define LUT_LEN 191
#define LUT_TEMP_MIN_degC -40
#define LUT_TEMP_MAX_degC 150


//******************************************************************************
// Private Variables
//******************************************************************************
static const uint32_t lut[LUT_LEN] =
{
	188500,
	178600,
	169200,
	160400,
	152100,
	144300,
	136900,
	130000,
	123400,
	117200,
	111300,
	105800,
	100600,
	95640,
	90970,
	86560,
	82380,
	78430,
	74690,
	71140,
	67790,
	64610,
	61600,
	58740,
	56030,
	53460,
	51030,
	48710,
	46520,
	44430,
	42450,
	40570,
	38780,
	37080,
	35460,
	33930,
	32460,
	31070,
	29750,
	28490,
	27280,
	26140,
	25050,
	24010,
	23020,
	22070,
	21170,
	20310,
	19490,
	18710,
	17960,
	17250,
	16570,
	15910,
	15290,
	14700,
	14130,
	13590,
	13070,
	12570,
	12090,
	11640,
	11200,
	10780,
	10380,
	10000,
	9633,
	9282,
	8945,
	8622,
	8312,
	8015,
	7730,
	7456,
	7194,
	6942,
	6700,
	6468,
	6245,
	6031,
	5826,
	5628,
	5438,
	5255,
	5080,
	4911,
	4749,
	4592,
	4442,
	4297,
	4158,
	4024,
	3895,
	3771,
	3651,
	3536,
	3425,
	3318,
	3215,
	3115,
	3019,
	2927,
	2837,
	2751,
	2668,
	2588,
	2511,
	2436,
	2364,
	2295,
	2227,
	2163,
	2100,
	2039,
	1981,
	1924,
	1869,
	1817,
	1765,
	1716,
	1668,
	1622,
	1577,
	1534,
	1492,
	1451,
	1412,
	1374,
	1337,
	1302,
	1267,
	1234,
	1201,
	1170,
	1139,
	1110,
	1081,
	1053,
	1027,
	1001,
	975,
	951,
	927,
	904,
	881,
	860,
	838,
	818,
	798,
	779,
	760,
	742,
	724,
	707,
	690,
	674,
	658,
	643,
	628,
	613,
	599,
	585,
	572,
	559,
	546,
	534,
	522,
	511,
	499,
	488,
	478,
	467,
	457,
	447,
	437,
	428,
	419,
	410,
	401,
	393,
	385,
	376,
	369,
	361,
	354,
	346,
	339,
	332,
	326,
	319,
	313
};


//******************************************************************************
// Functions
//******************************************************************************
int16_t AlxNtcg103jf103ft1s_ResToTemp_degC(uint32_t res_Ohm)
{
	// Bound
	AlxBound_Uint32(&res_Ohm, lut[LUT_LEN - 1], lut[0]);

	// Loop
	for (uint32_t i = 0; i < LUT_LEN; i++)
	{
		if (res_Ohm >= lut[i])
		{
			return (int16_t)((int16_t)LUT_TEMP_MIN_degC + (int16_t)i);
		}
	}

	// Assert
	ALX_NTCG103JF103FT1S_ASSERT(false);	// We should never get here
	return 0;
}


#endif	// #if defined(ALX_C_LIB)
