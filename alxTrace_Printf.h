/**
  ******************************************************************************
  * @file		alxTrace_Printf.h
  * @brief		Auralix C Library - ALX Trace Printf Module
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
#ifndef ALX_TRACE_PRINTF_H
#define ALX_TRACE_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_PC) || defined(ALX_MBED))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Dummy
	bool dummy;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxTrace_Ctor
(
	AlxTrace* me
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_PC) || defined(ALX_MBED))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TRACE_PRINTF_H
