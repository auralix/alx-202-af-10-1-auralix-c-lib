﻿/**
  ******************************************************************************
  * @file		alxNtcg103jf103ft1s.c
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
// Includes
//******************************************************************************
#include "alxNtcg103jf103ft1s.h"


//******************************************************************************
// Variables
//******************************************************************************
const AlxNtcg103jf103ft1s alxNtcg103jf103ft1s =
{
	{
		188.5,
		178.6,
		169.2,
		160.4,
		152.1,
		144.3,
		136.9,
		130,
		123.4,
		117.2,
		111.3,
		105.8,
		100.6,
		95.64,
		90.97,
		86.56,
		82.38,
		78.43,
		74.69,
		71.14,
		67.79,
		64.61,
		61.6,
		58.74,
		56.03,
		53.46,
		51.03,
		48.71,
		46.52,
		44.43,
		42.45,
		40.57,
		38.78,
		37.08,
		35.46,
		33.93,
		32.46,
		31.07,
		29.75,
		28.49,
		27.28,
		26.14,
		25.05,
		24.01,
		23.02,
		22.07,
		21.17,
		20.31,
		19.49,
		18.71,
		17.96,
		17.25,
		16.57,
		15.91,
		15.29,
		14.7,
		14.13,
		13.59,
		13.07,
		12.57,
		12.09,
		11.64,
		11.2,
		10.78,
		10.38,
		10,
		9.633,
		9.282,
		8.945,
		8.622,
		8.312,
		8.015,
		7.73,
		7.456,
		7.194,
		6.942,
		6.7,
		6.468,
		6.245,
		6.031,
		5.826,
		5.628,
		5.438,
		5.255,
		5.08,
		4.911,
		4.749,
		4.592,
		4.442,
		4.297,
		4.158,
		4.024,
		3.895,
		3.771,
		3.651,
		3.536,
		3.425,
		3.318,
		3.215,
		3.115,
		3.019,
		2.927,
		2.837,
		2.751,
		2.668,
		2.588,
		2.511,
		2.436,
		2.364,
		2.295,
		2.227,
		2.163,
		2.1,
		2.039,
		1.981,
		1.924,
		1.869,
		1.817,
		1.765,
		1.716,
		1.668,
		1.622,
		1.577,
		1.534,
		1.492,
		1.451,
		1.412,
		1.374,
		1.337,
		1.302,
		1.267,
		1.234,
		1.201,
		1.17,
		1.139,
		1.11,
		1.081,
		1.053,
		1.027,
		1.001,
		0.975,
		0.951,
		0.927,
		0.904,
		0.881,
		0.86,
		0.838,
		0.818,
		0.798,
		0.779,
		0.76,
		0.742,
		0.724,
		0.707,
		0.69,
		0.674,
		0.658,
		0.643,
		0.628,
		0.613,
		0.599,
		0.585,
		0.572,
		0.559,
		0.546,
		0.534,
		0.522,
		0.511,
		0.499,
		0.488,
		0.478,
		0.467,
		0.457,
		0.447,
		0.437,
		0.428,
		0.419,
		0.41,
		0.401,
		0.393,
		0.385,
		0.376,
		0.369,
		0.361,
		0.354,
		0.346,
		0.339,
		0.332,
		0.326,
		0.319,
		0.313
	},
	{
		-40,
		-39,
		-38,
		-37,
		-36,
		-35,
		-34,
		-33,
		-32,
		-31,
		-30,
		-29,
		-28,
		-27,
		-26,
		-25,
		-24,
		-23,
		-22,
		-21,
		-20,
		-19,
		-18,
		-17,
		-16,
		-15,
		-14,
		-13,
		-12,
		-11,
		-10,
		-9,
		-8,
		-7,
		-6,
		-5,
		-4,
		-3,
		-2,
		-1,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15,
		16,
		17,
		18,
		19,
		20,
		21,
		22,
		23,
		24,
		25,
		26,
		27,
		28,
		29,
		30,
		31,
		32,
		33,
		34,
		35,
		36,
		37,
		38,
		39,
		40,
		41,
		42,
		43,
		44,
		45,
		46,
		47,
		48,
		49,
		50,
		51,
		52,
		53,
		54,
		55,
		56,
		57,
		58,
		59,
		60,
		61,
		62,
		63,
		64,
		65,
		66,
		67,
		68,
		69,
		70,
		71,
		72,
		73,
		74,
		75,
		76,
		77,
		78,
		79,
		80,
		81,
		82,
		83,
		84,
		85,
		86,
		87,
		88,
		89,
		90,
		91,
		92,
		93,
		94,
		95,
		96,
		97,
		98,
		99,
		100,
		101,
		102,
		103,
		104,
		105,
		106,
		107,
		108,
		109,
		110,
		111,
		112,
		113,
		114,
		115,
		116,
		117,
		118,
		119,
		120,
		121,
		122,
		123,
		124,
		125,
		126,
		127,
		128,
		129,
		130,
		131,
		132,
		133,
		134,
		135,
		136,
		137,
		138,
		139,
		140,
		141,
		142,
		143,
		144,
		145,
		146,
		147,
		148,
		149,
		150
	},
	ALX_NTCG103JF103FT1S_NUM_OF_POINTS
};
