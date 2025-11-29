/**
  ******************************************************************************
  * @file		usbh_conf.h
  * @brief		Auralix C Library - ALX USBH Configuration File
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
#ifndef USBH_CONF_H
#define USBH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


//*******************************************************************************
// Includes
//*******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Configuration
//******************************************************************************
#define USBH_MAX_NUM_ENDPOINTS			2
#define USBH_MAX_NUM_INTERFACES			2
#define USBH_MAX_NUM_CONFIGURATION		1
#define USBH_KEEP_CFG_DESCRIPTOR		1
#define USBH_MAX_NUM_SUPPORTED_CLASS	1
#define USBH_MAX_SIZE_CONFIGURATION		0x200
#define USBH_MAX_DATA_BUFFER			0x200
#define USBH_DEBUG_LEVEL				2
#define USBH_USE_OS						0

#define USBH_malloc		malloc
#define USBH_free		free
#define USBH_memset		memset
#define USBH_memcpy		memcpy

#include "usbh_conf_usr.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define USBH_CONF_FILE "usbh_conf.h"

// Trace //
#if USBH_DEBUG_LEVEL > 0
	#define USBH_ErrLog(...) ALX_TRACE_ERR(USBH_CONF_FILE, __VA_ARGS__)
#else
	#define USBH_ErrLog(...) do {} while (0)
#endif

#if USBH_DEBUG_LEVEL > 1
	#define USBH_UsrLog(...) ALX_TRACE_INF(USBH_CONF_FILE, __VA_ARGS__)
#else
	#define USBH_UsrLog(...) do {} while (0)
#endif

#if USBH_DEBUG_LEVEL > 2
	#define USBH_DbgLog(...) ALX_TRACE_DBG(USBH_CONF_FILE, __VA_ARGS__)
#else
	#define USBH_DbgLog(...) do {} while (0)
#endif


#ifdef __cplusplus
}
#endif

#endif	// #ifndef USBH_CONF_H
