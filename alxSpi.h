/**
  ******************************************************************************
  * @file		alxSpi.h
  * @brief		Auralix C Library - ALX SPI Module
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
#ifndef ALX_SPI_H
#define ALX_SPI_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"

// AlxMcu //
#if  defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxSpi_McuStm32.h"

#elif defined(ALX_LPC55S6X)
#include "alxSpi_McuLpc55S6x.h"

#else
typedef struct { bool dummy; } AlxSpi;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SPI_FILE "alxSpi.h"

// Assert //
#if defined(_ALX_SPI_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_SPI_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SPI_FILE, expr)
#elif defined(_ALX_SPI_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_SPI_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SPI_FILE, expr)
#elif defined(_ALX_SPI_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_SPI_ASSERT(expr) ALX_ASSERT_RST(ALX_SPI_FILE, expr)
#else
	#define ALX_SPI_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_SPI_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_SPI_TRACE(...) ALX_TRACE_STD(ALX_SPI_FILE, __VA_ARGS__)
#else
	#define ALX_SPI_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxSpi_Init(AlxSpi* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxSpi_DeInit(AlxSpi* me);

/**
  * @brief
  * @param[in] me
  * @param[in] writeData
  * @param[in] len
  * @param[in] numOfTries
  * @param[in] timeout_ms
  */
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);

/**
  * @brief
  * @param[in] me
  * @param[out] readData
  * @param[in] len
  * @param[in] numOfTries
  * @param[in] timeout_ms
  */
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);

/**
  * @brief
  * @param[in] me
  * @param[in] writeData
  * @param[out] readData
  * @param[in] len
  * @param[in] numOfTries
  * @param[in] timeout_ms
  */
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);

/**
  * @brief
  * @param[in] me
  */
void AlxSpi_Master_AssertCs(AlxSpi* me);

/**
  * @brief
  * @param[in] me
  */
void AlxSpi_Master_DeAssertCs(AlxSpi* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_SPI_H
