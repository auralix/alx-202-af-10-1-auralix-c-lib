/**
  ******************************************************************************
  * @file		alxAssert.h
  * @brief		Auralix C Library - ALX Assert Module
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
#ifndef ALX_ASSERT_H
#define ALX_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ASSERT_BKPT(file, expr) if (expr) {} else { AlxAssert_Bkpt(file, __LINE__, __func__); }
#define ALX_ASSERT_TRACE(file, expr) if (expr) {} else { AlxAssert_Trace(file, __LINE__, __func__); }
#define ALX_ASSERT_RST(file, expr) if (expr) {} else { AlxAssert_Rst(file, __LINE__, __func__); }


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun);

/**
  * @brief
  * @param[in] me
  */
void AlxAssert_Trace(const char* file, uint32_t line, const char* fun);

/**
  * @brief
  * @param[in] me
  */
void AlxAssert_Rst(const char* file, uint32_t line, const char* fun);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_ASSERT_H
