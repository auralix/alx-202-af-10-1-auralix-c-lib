/**
  ******************************************************************************
  * @file		alxLinFun.c
  * @brief		Auralix C Library - ALX Linear Function Module
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
#include "alxLinFun.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions Declarations
//******************************************************************************
static float Get_X(AlxLinFun* me, float y);
static float Get_Y(AlxLinFun* me, float x);
//static int32_t Get_X_Int(AlxLinFunInt* me, int32_t y);	// TODO
static int32_t Get_Y_Int(AlxLinFunInt* me, int32_t x);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLinFun_Ctor
(
	AlxLinFun *me,
	float x1,
	float point1_y,
	float point2_x,
	float point2_y,
	float min,
	float max,
	bool isLimitOnX	// If true, min/max values all applicable for X axis, else for Y axis
)
{
	// Parameters
	me->x1 = x1;
	me->y1 = point1_y;
	me->x2 = point2_x;
	me->y2 = point2_y;
	me->min = min;
	me->max = max;
	me->isLimitOnX = isLimitOnX;

	// Variables
	me->coefK = (point2_y - point1_y) / (point2_x - x1);
	me->coefN = point1_y - me->coefK * x1;

	// Info
	me->wasCtorCalled = true;

	// Check if min is smaller than max
	ALX_LIN_FUN_ASSERT(me->min < me->max);
}
void AlxLinFunInt_Ctor
(
	AlxLinFunInt *me,
	int32_t x1,
	int32_t point1_y,
	int32_t point2_x,
	int32_t point2_y,
	int32_t min,
	int32_t max,
	bool isLimitOnX	// If true, min/max values all applicable for X axis, else for Y axis
)
{
	// Parameters
	me->x1 = x1;
	me->y1 = point1_y;
	me->x2 = point2_x;
	me->y2 = point2_y;
	me->min = min;
	me->max = max;
	me->isLimitOnX = isLimitOnX;

	// Info
	me->wasCtorCalled = true;

	// Check if min is smaller than max
	ALX_LIN_FUN_ASSERT(me->min < me->max);
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLinFun_GetX_WithStatus(AlxLinFun* me, float y, float* x)
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	if (me->isLimitOnX)
	{
		float xRaw = Get_X(me, y);		// Calculate X

		if (xRaw < me->min)				// Check if X is out of bounds
		{
			*x = me->min;				// Clip X at X min
			return AlxRange_ErrMin;
		}
		else if (xRaw > me->max)		// Check if X is out of bounds
		{
			*x = me->max;				// Clip X at X max
			return AlxRange_ErrMax;
		}
		else
		{
			*x = xRaw;					// If within bounds, return calculated X
			return Alx_Ok;
		}
	}
	else
	{
		if (y < me->min)				// Check if Y is out of bounds
		{
			*x = Get_X(me, me->min);	// Clip Y at Y min
			return AlxRange_ErrMin;
		}
		else if (y > me->max)			// Check if Y is out of bounds
		{
			*x = Get_X(me, me->max);	// Clip Y at Y max
			return AlxRange_ErrMax;
		}
		else
		{
			*x = Get_X(me, y);			// If within bounds, return calculated X
			return Alx_Ok;
		}
	}
}
Alx_Status AlxLinFun_GetY_WithStatus(AlxLinFun* me, float x, float* y)
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	if (me->isLimitOnX)
	{
		if (x < me->min)				// Check if X is out of bounds
		{
			*y = Get_Y(me, me->min);	// Clip X at X min
			return AlxRange_ErrMin;
		}
		else if (x > me->max)			// Check if X is out of bounds
		{
			*y = Get_Y(me, me->max);	// Clip X at X max
			return AlxRange_ErrMax;
		}
		else
		{
			*y = Get_Y(me, x);			// If within bounds, return calculated Y
			return Alx_Ok;
		}
	}
	else
	{
		float yRaw = Get_Y(me, x);		// Calculate Y

		if (yRaw < me->min)				// Check if Y is out of bounds
		{
			*y = me->min;				// Clip Y at Y min
			return AlxRange_ErrMin;
		}
		else if (yRaw > me->max)		// Check if Y is out of bounds
		{
			*y = me->max;				// Clip Y at Y max
			return AlxRange_ErrMax;
		}
		else
		{
			*y = yRaw;					// If within bounds, return calculated Y
			return Alx_Ok;
		}
	}
}
float AlxLinFun_GetY(AlxLinFun* me, float x) // Return float, ignore Alx_Status
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	float y = 0;
	AlxLinFun_GetY_WithStatus(me, x, &y);
	return y;
}
float AlxLinFun_GetX(AlxLinFun* me, float y) // Return float, ignore Alx_Status
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	float x = 0;
	AlxLinFun_GetX_WithStatus(me, y, &x);
	return x;
}
Alx_Status AlxLinFunInt_GetY_WithStatus(AlxLinFunInt* me, int32_t x, int32_t* y)
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	if (me->isLimitOnX)
	{
		if (x < me->min)					// Check if X is out of bounds
		{
			*y = Get_Y_Int(me, me->min);	// Clip X at X min
			return AlxRange_ErrMin;
		}
		else if (x > me->max)				// Check if X is out of bounds
		{
			*y = Get_Y_Int(me, me->max);	// Clip X at X max
			return AlxRange_ErrMax;
		}
		else
		{
			*y = Get_Y_Int(me, x);			// If within bounds, return calculated Y
			return Alx_Ok;
		}
	}
	else
	{
		float yRaw = Get_Y_Int(me, x);		// Calculate Y

		if (yRaw < me->min)				// Check if Y is out of bounds
		{
			*y = me->min;				// Clip Y at Y min
			return AlxRange_ErrMin;
		}
		else if (yRaw > me->max)		// Check if Y is out of bounds
		{
			*y = me->max;				// Clip Y at Y max
			return AlxRange_ErrMax;
		}
		else
		{
			*y = yRaw;					// If within bounds, return calculated Y
			return Alx_Ok;
		}
	}
}
int32_t AlxLinFunInt_GetY(AlxLinFunInt* me, int32_t x) // Return float, ignore Alx_Status
{
	ALX_LIN_FUN_ASSERT(me->wasCtorCalled);

	int32_t y = 0;
	AlxLinFunInt_GetY_WithStatus(me, x, &y);
	return y;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static float Get_X(AlxLinFun* me, float y)
{
	return (y - me->coefN) / me->coefK;
}
static float Get_Y(AlxLinFun* me, float x)
{
	return (me->coefK * x) + me->coefN;
}
static int32_t Get_Y_Int(AlxLinFunInt* me, int32_t x)
{
	int32_t dx = me->x2 - me->x1;
	int32_t dy = me->y2 - me->y1;
	int32_t n = me->y1-((dy*me->x1)/dx);
	int32_t kx = (dy+x)/dx;
	int32_t y = kx + n;
	return y;
}


#endif	// #if defined(ALX_C_LIB)
