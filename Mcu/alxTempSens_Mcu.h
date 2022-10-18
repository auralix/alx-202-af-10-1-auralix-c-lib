/**
  ******************************************************************************
  * @file		alxTempSens_Mcu.h
  * @brief		Auralix C Library - ALX Temperature Sensor Module
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
#ifndef ALX_TEMP_SENS_MCU_H
#define ALX_TEMP_SENS_MCU_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

// AlxMcu //
#if defined(ALX_TEMP_SENS_MCU_DUMMY)
#include "alxTempSens_Dummy.h"

#else
typedef struct {} AlxTempSens_Mcu;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_MCU_FILE "alxTempSens_Mcu.h"

// Assert //
#if defined(_ALX_TEMP_SENS_MCU_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TEMP_SENS_MCU_FILE, expr)
#elif defined(_ALX_TEMP_SENS_MCU_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TEMP_SENS_MCU_FILE, expr)
#elif defined(_ALX_TEMP_SENS_MCU_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_RST(ALX_TEMP_SENS_MCU_FILE, expr)
#else
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TEMP_SENS_MCU_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TEMP_SENS_MCU_TRACE(...) ALX_TRACE_STD(ALX_TEMP_SENS_MCU_FILE, __VA_ARGS__)
#else
	#define ALX_TEMP_SENS_MCU_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTempSensMcu_Init(AlxTempSens_Mcu* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTempSensMcu_DeInit(AlxTempSens_Mcu* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxTempSensMcu_GetTemp_degC(AlxTempSens_Mcu* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TEMP_SENS_MCU_H
