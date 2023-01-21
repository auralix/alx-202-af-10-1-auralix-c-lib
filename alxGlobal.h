/**
  ******************************************************************************
  * @file		alxGlobal.h
  * @brief		Auralix C Library - ALX Global Module
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
#ifndef ALX_GLOBAL_H
#define ALX_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes - ALX Config
//******************************************************************************
#include "alxConfig.h"


//------------------------------------------------------------------------------
// Includes - Newlib Nano
//------------------------------------------------------------------------------
#if defined(ALX_NEWLIB_NANO)
#include <assert.h> // All headers from C11 Standard, commented out what not compatible with C99
#include <complex.h>
#include <ctype.h>
#include <errno.h>
//#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
//#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <tgmath.h>
//#include <threads.h>
#include <time.h>
//#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

#include "../include/_newlib_version.h"
#endif


//------------------------------------------------------------------------------
// Includes - Redlib
//------------------------------------------------------------------------------
#if defined(ALX_REDLIB)
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif


//------------------------------------------------------------------------------
// Includes - ARM CMSIS DSP Library
//------------------------------------------------------------------------------
#if defined(ALX_CMSIS_DSP)
#include <arm_math.h>
#endif


//------------------------------------------------------------------------------
// Includes - FreeRTOS
//------------------------------------------------------------------------------
#if defined(ALX_FREE_RTOS)
#include "FreeRTOS.h"
#include "croutine.h"
#include "event_groups.h"
#include "list.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"
#include "timers.h"
#endif


//------------------------------------------------------------------------------
// Includes - PC
//------------------------------------------------------------------------------
#if defined(ALX_PC)
#include <assert.h>		// TV: All headers from C11 Standard, commented out everything, so that project compiles
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
//#include <stdalign.h>
#include <stdarg.h>
//#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdnoreturn.h>
#include <string.h>
#include <tgmath.h>
//#include <threads.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>
#endif


//------------------------------------------------------------------------------
// Includes - MCU
//------------------------------------------------------------------------------
#if defined(ALX_STM32F0)
#include "alxGlobal_McuStm32F0.h"
#define ALX_STM32

#elif defined(ALX_STM32F1)
#include "alxGlobal_McuStm32F1.h"
#define ALX_STM32

#elif defined(ALX_STM32F4)
#include "alxGlobal_McuStm32F4.h"
#define ALX_STM32

#elif defined(ALX_STM32G4)
#include "alxGlobal_McuStm32G4.h"
#define ALX_STM32

#elif defined(ALX_STM32L0)
#include "alxGlobal_McuStm32L0.h"
#define ALX_STM32

#elif defined(ALX_STM32L4)
#include "alxGlobal_McuStm32L4.h"
#define ALX_STM32

#elif defined(ALX_LPC1769)
#include "alxGlobal_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxGlobal_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxGlobal_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxGlobal_McuLpc55S6x.h"

#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_GLOBAL_FILE "alxGlobal.h"

// Assert //
#if defined(_ALX_GLOBAL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_GLOBAL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_GLOBAL_FILE, expr)
#elif defined(_ALX_GLOBAL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_GLOBAL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_GLOBAL_FILE, expr)
#elif defined(_ALX_GLOBAL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_GLOBAL_ASSERT(expr) ALX_ASSERT_RST(ALX_GLOBAL_FILE, expr)
#else
	#define ALX_GLOBAL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_GLOBAL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_GLOBAL_TRACE(...) ALX_TRACE_STD(ALX_GLOBAL_FILE, __VA_ARGS__)
#else
	#define ALX_GLOBAL_TRACE(...) do{} while (false)
#endif

#define ALX_STR(s) #s
#define ALX_xSTR(s) ALX_STR(s)
#define ALX_xVER3(major, minor , patch) (((major) << 24) | ((minor) << 16) | (patch))
#define ALX_ARR_LEN(x) (sizeof(x) / sizeof((x)[0]))

#if defined (USE_FULL_ASSERT)
	#define ALX_STM32_ASSERT
#endif

#if defined(__GNUC__)
	#define ALX_INLINE __attribute__((always_inline))
	#define ALX_STATIC_INLINE static __attribute__((always_inline))
	#define ALX_WEAK __attribute__ ((weak))
	#define ALX_BKPT asm("bkpt 255")
#endif

#if defined(_MSC_VER)
	#define ALX_WEAK
	#define ALX_BKPT __debugbreak()
#endif

#define ALX_NULL 0
#define ALX_NULL_PTR NULL


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxGlobal_BaudRate_9600		= 9600,
	AlxGlobal_BaudRate_19200	= 19200,
	AlxGlobal_BaudRate_115200	= 115200,
	AlxGlobal_BaudRate_800000	= 800000,
	AlxGlobal_BaudRate_921600	= 921600,
	AlxGlobal_BaudRate_1000000	= 1000000,
	AlxGlobal_BaudRate_1600000	= 1600000,
	AlxGlobal_BaudRate_2000000	= 2000000,
	AlxGlobal_BaudRate_4000000	= 4000000,
	AlxGlobal_BaudRate_8000000	= 8000000
} AlxGlobal_BaudRate;

typedef enum
{
	Alx_Ok = 0,
	Alx_Err = 1,
	Alx_ErrNumOfTries,
	AlxBound_ErrMin,
	AlxBound_ErrMax,
	AlxBound_ErrLen,
	AlxFifo_ErrFull,
	AlxFifo_ErrEmpty,
	AlxFifo_ErrNoDelim,
	AlxMemSafe_CrcOkSame_UsedCopyA,
	AlxMemSafe_CrcOkDiff_UsedCopyA,
	AlxMemSafe_CopyANokCopyBOk_UsedCopyB,
	AlxMemSafe_CopyAOkCopyBNok_UsedCopyA,
	AlxMemSafe_BothNok_ResetToDef,
	AlxParamItem_ErrConv,
	AlxParamItem_ErrEnum,
	AlxRange_ErrMin,
	AlxRange_ErrMax,
	AlxRange_ErrLen
} Alx_Status;

typedef enum
{
	Alx_Ch_0  =  0,
	Alx_Ch_1  =  1,
	Alx_Ch_2  =  2,
	Alx_Ch_3  =  3,
	Alx_Ch_4  =  4,
	Alx_Ch_5  =  5,
	Alx_Ch_6  =  6,
	Alx_Ch_7  =  7,
	Alx_Ch_8  =  8,
	Alx_Ch_9  =  9,
	Alx_Ch_10 = 10,
	Alx_Ch_11 = 11,
	Alx_Ch_12 = 12,
	Alx_Ch_13 = 13,
	Alx_Ch_14 = 14,
	Alx_Ch_15 = 15,
	Alx_Ch_16 = 16,
	Alx_Ch_17 = 17,
	Alx_Ch_18 = 18,
	Alx_Ch_19 = 19,
	Alx_Ch_20 = 20,

	Alx_Ch_McuStm32_Vref = 101,
	Alx_Ch_McuStm32_Vbat = 102,
	Alx_Ch_McuStm32_Vlcd = 103,
	Alx_Ch_McuStm32_TempSens = 104
} Alx_Ch;

typedef enum
{
	#if defined(ALX_STM32)
	AlxClk_Clk_McuStm32_SystemCoreClock,
	AlxClk_Clk_McuStm32_Sysclk,
	AlxClk_Clk_McuStm32_Hclk,
	AlxClk_Clk_McuStm32_Pclk1Apb1,
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	AlxClk_Clk_McuStm32_Pclk2Apb2,
	#endif

	AlxClk_Clk_McuStm32_SystemCoreClock_Ctor,
	AlxClk_Clk_McuStm32_Sysclk_Ctor,
	AlxClk_Clk_McuStm32_Hclk_Ctor,
	AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor,
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor,
	#endif
	AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor,
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor,
	#endif

	AlxClk_Clk_McuStm32_MainPllClk,
	AlxClk_Clk_McuStm32_MainPllInputClk_Ctor,
	#endif

	#if defined(ALX_LPC8XX) || defined(ALX_LPC80X)
	AlxClk_Clk_McuLpc8xx_CoreSysClk,
	AlxClk_Clk_McuLpc8xx_MainClk,
	AlxClk_Clk_McuLpc8xx_Fro,
	AlxClk_Clk_McuLpc8xx_FroDiv,
	AlxClk_Clk_McuLpc8xx_ExtClk,
	AlxClk_Clk_McuLpc8xx_Frg0,

	AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor,
	AlxClk_Clk_McuLpc8xx_MainClk_Ctor,
	AlxClk_Clk_McuLpc8xx_Fro_Ctor,
	AlxClk_Clk_McuLpc8xx_LPO_Ctor,
	#endif

	#if defined(ALX_LPC55S6X)
	AlxClk_Clk_McuLpc55s6x_SystemCoreClock,
	AlxClk_Clk_McuLpc55s6x_AhbClk,
	AlxClk_Clk_McuLpc55s6x_MainClk,
	AlxClk_Clk_McuLpc55s6x_FroOsc_1MHz,

	AlxClk_Clk_McuLpc55s6x_SystemCoreClock_Ctor,
	AlxClk_Clk_McuLpc55s6x_AhbClk_Ctor,
	AlxClk_Clk_McuLpc55s6x_MainClk_Ctor,
	AlxClk_Clk_McuLpc55s6x_FroOsc_1MHz_Ctor,
	#endif
} AlxClk_Clk;

typedef enum
{
	AlxClk_Tick_1us   = 1,
	AlxClk_Tick_10us  = 10,
	AlxClk_Tick_100us = 100,
	AlxClk_Tick_200us = 200,
	AlxClk_Tick_250us = 250,
	AlxClk_Tick_300us = 300,
	AlxClk_Tick_400us = 400,
	AlxClk_Tick_500us = 500,
	AlxClk_Tick_1ms   = 1000,
	AlxClk_Tick_10ms  = 10000,
	AlxClk_Tick_100ms = 100000,
} AlxClk_Tick;

typedef enum
{
	Alx_IrqPriority_0  =  0,
	Alx_IrqPriority_1  =  1,
	Alx_IrqPriority_2  =  2,
	Alx_IrqPriority_3  =  3,
	Alx_IrqPriority_4  =  4,
	Alx_IrqPriority_5  =  5,
	Alx_IrqPriority_6  =  6,
	Alx_IrqPriority_7  =  7,
	Alx_IrqPriority_8  =  8,
	Alx_IrqPriority_9  =  9,
	Alx_IrqPriority_10 = 10,
	Alx_IrqPriority_11 = 11,
	Alx_IrqPriority_12 = 12,
	Alx_IrqPriority_13 = 13,
	Alx_IrqPriority_14 = 14,
	Alx_IrqPriority_15 = 15,
	Alx_IrqPriority_16 = 16,
	Alx_IrqPriority_17 = 17,
	Alx_IrqPriority_18 = 18,
	Alx_IrqPriority_19 = 19,
	Alx_IrqPriority_20 = 20,
} Alx_IrqPriority;


//******************************************************************************
// Functions
//******************************************************************************
void AlxGlobal_DisableIrq(void);
void AlxGlobal_EnableIrq(void);
void AlxGlobal_Uint64ToStr(uint64_t uint64, char* str);
uint32_t AlxGlobal_Ntohl(uint32_t val);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_H
