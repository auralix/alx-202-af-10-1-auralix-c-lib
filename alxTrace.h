/**
  ******************************************************************************
  * @file		alxTrace.h
  * @brief		Auralix C Library - ALX Trace Module
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

// AlxMcu //
#if (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)) && (!defined(ALX_MBED))
#include "alxTrace_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxTrace_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxTrace_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxTrace_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxTrace_McuLpc55S6x.h"

#elif defined(ALX_PC) || defined(ALX_MBED)
#include "alxTrace_Printf.h"

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
#define ALX_TRACE_STR(str) do								{ AlxTrace_WriteStr(&alxTrace, str); } while(false)
#define ALX_TRACE_FORMAT(...) do							{ AlxTrace_WriteFormat(&alxTrace, __VA_ARGS__); } while(false)
#define ALX_TRACE_STD(file, ...) do							{ AlxTrace_WriteStd(&alxTrace, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#define ALX_TRACE_SM(smLevel, smName, stName, acName) do	{ AlxTrace_WriteSm(&alxTrace, smLevel, smName, stName, acName); } while(false)


//******************************************************************************
// Variables
//******************************************************************************
extern AlxTrace alxTrace;


//******************************************************************************
// Specific Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxTrace_Init(AlxTrace* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxTrace_DeInit(AlxTrace* me);

/**
  * @brief
  * @param[in] me
  */
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str);


//******************************************************************************
// Common Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxTrace_WriteFormat(AlxTrace* me, const char* format, ...);

/**
  * @brief
  * @param[in] me
  */
void AlxTrace_WriteStd(AlxTrace* me, const char* file, uint32_t line, const char* fun, const char* format, ...);

/**
  * @brief
  * @param[in] me
  */
void AlxTrace_WriteSm(AlxTrace* me, uint8_t smLevel, const char* smName, const char* stName, const char* acName);

/**
  * @brief
  * @param[in] me
  */
void AlxTrace_GetSmLevelStr(uint32_t smLevel, char* smLevelStr);


#endif // #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_TRACE_H
