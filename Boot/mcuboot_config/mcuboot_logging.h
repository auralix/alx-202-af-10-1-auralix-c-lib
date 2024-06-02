/**
  ******************************************************************************
  * @file		mcuboot_logging.h
  * @brief		Auralix C Library - ALX MCUboot Logging File
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
#ifndef __MCUBOOT_LOGGING_H__
#define __MCUBOOT_LOGGING_H__

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Defines
//******************************************************************************
#define MCUBOOT_LOG_LEVEL_OFF		0
#define MCUBOOT_LOG_LEVEL_ERROR		1
#define MCUBOOT_LOG_LEVEL_WARNING	2
#define MCUBOOT_LOG_LEVEL_INFO		3
#define MCUBOOT_LOG_LEVEL_DEBUG		4


//******************************************************************************
// Trace Macros
//******************************************************************************
#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_ERROR
#define MCUBOOT_LOG_ERR(_fmt, ...) do { AlxTrace_WriteFormat(&alxTrace, "[ERR] " _fmt "\r\n", ##__VA_ARGS__); } while (false)
#else
#define MCUBOOT_LOG_ERR(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_WARNING
#define MCUBOOT_LOG_WRN(_fmt, ...) do { AlxTrace_WriteFormat(&alxTrace, "[WRN] " _fmt "\r\n", ##__VA_ARGS__); } while (false)
#else
#define MCUBOOT_LOG_WRN(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_INFO
#define MCUBOOT_LOG_INF(_fmt, ...) do { AlxTrace_WriteFormat(&alxTrace, "[INF] " _fmt "\r\n", ##__VA_ARGS__); } while (false)
#else
#define MCUBOOT_LOG_INF(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_DEBUG
#define MCUBOOT_LOG_DBG(_fmt, ...) do { AlxTrace_WriteFormat(&alxTrace, "[DBG] " _fmt "\r\n", ##__VA_ARGS__); } while (false)
#else
#define MCUBOOT_LOG_DBG(...) IGNORE(__VA_ARGS__)
#endif

#define MCUBOOT_LOG_SIM(...) IGNORE(__VA_ARGS__)


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef __MCUBOOT_LOGGING_H__
