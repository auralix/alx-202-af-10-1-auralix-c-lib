/**
  ******************************************************************************
  * @file		alxTrace.h
  * @brief		Auralix C Library - ALX Trace Module
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
#ifndef ALX_TRACE_H
#define ALX_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"

#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
#include "alxTrace_McuStm32.h"

#elif defined(ALX_LPC17XX)
#include "alxTrace_McuLpc17xx.h"

#elif defined(ALX_LPC55S6X)
#include "alxTrace_McuLpc55S6x.h"

#elif defined(ALX_LPC80X) || defined(ALX_LPC84X)
#include "alxTrace_McuLpc80x.h"

#elif defined(ALX_ZEPHYR)
#include "alxTrace_McuZephyr.h"

#elif defined(ALX_PC)
typedef struct { bool dummy; } AlxTrace;

#else
typedef struct { bool dummy; } AlxTrace;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TRACE_LEN_MAX 256

#define ALX_TRACE_LEVEL_OFF 0
#define ALX_TRACE_LEVEL_FTL 1
#define ALX_TRACE_LEVEL_ERR 2
#define ALX_TRACE_LEVEL_WRN 3
#define ALX_TRACE_LEVEL_INF 4
#define ALX_TRACE_LEVEL_DBG 5
#define ALX_TRACE_LEVEL_VRB 6

#define ALX_TRACE_STR(str) do			{ AlxTrace_WriteStr(&alxTrace, str); } while(false)
#define ALX_TRACE_FORMAT(...) do		{ AlxTrace_WriteFormat(&alxTrace, __VA_ARGS__); } while(false)

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_FTL
	#define ALX_TRACE_FTL(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_FTL, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_FTL(file, ...) do {} while(false)
#endif

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_ERR
	#define ALX_TRACE_ERR(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_ERR, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_ERR(file, ...) do {} while(false)
#endif

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_WRN
	#define ALX_TRACE_WRN(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_WRN, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_WRN(file, ...) do {} while(false)
#endif

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_INF
	#define ALX_TRACE_INF(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_INF, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_INF(file, ...) do {} while(false)
#endif

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_DBG
	#define ALX_TRACE_DBG(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_DBG, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_DBG(file, ...) do {} while(false)
#endif

#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
	#define ALX_TRACE_VRB(file, ...) do { AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_VRB, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#else
	#define ALX_TRACE_VRB(file, ...) do {} while(false)
#endif


//******************************************************************************
// Variables
//******************************************************************************
extern AlxTrace alxTrace;


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me);
Alx_Status AlxTrace_DeInit(AlxTrace* me);
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str);
void AlxTrace_WriteFormat(AlxTrace* me, const char* format, ...);
void AlxTrace_WriteLevel(AlxTrace* me, uint8_t level, const char* module, uint32_t line, const char* fun, const char* format, ...);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TRACE_H
