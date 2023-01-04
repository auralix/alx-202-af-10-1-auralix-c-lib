/**
  ******************************************************************************
  * @file		alxOsMutex.h
  * @brief		Auralix C Library - ALX OS Mutex Module
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
#ifndef ALX_OS_MUTEX_H
#define ALX_OS_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"	// TV: alxOsMutex is needed by Trace & Assert so it must NOT contain alxTrace & alxAssert


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	SemaphoreHandle_t mutex;
	#endif

	// Info
	bool wasCtorCalled;
} AlxOsMutex;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Lock(AlxOsMutex* me);

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Unlock(AlxOsMutex* me);

/**
  * @brief
  * @param[in]	me
  * @retval		false
  * @retval		true
  */
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me);	// TV: Not tested


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_MUTEX_H
