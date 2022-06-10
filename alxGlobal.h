/**
  ******************************************************************************
  * @file alxGlobal.h
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 6167 $
  * @date $LastChangedDate: 2021-10-01 17:19:36 +0200 (Fri, 01 Oct 2021) $
  ******************************************************************************
  */

#ifndef ALX_GLOBAL_H
#define ALX_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes - ALX Config
//******************************************************************************
#include <alxConfig.h>


//******************************************************************************
// Includes - Newlib Nano
//******************************************************************************
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


//******************************************************************************
// Includes - Redlib
//******************************************************************************
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


//******************************************************************************
// Includes - Redlib
//******************************************************************************
#if defined(ALX_CMSIS_DSP)
#include <arm_math.h>
#endif


//******************************************************************************
// Includes - FreeRtos
//******************************************************************************
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


//******************************************************************************
// Includes - MCU
//******************************************************************************
#if defined(ALX_STM32F1)
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

#elif defined(ALX_LPC1769)
#include "alxGlobal_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxGlobal_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxGlobal_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxGlobal_McuLpc55S6x.h"

#elif defined(ALX_PC)
#include "alxGlobal_Pc.h"

#else
#error "Please select platform for your application!"
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_GLOBAL_FILE "alxGlobal"

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

#define ALX_I2C_BUFF_LEN 256
#define ALX_ADC_BUFF_LEN 16
#define ALX_DAC_BUFF_LEN 2
#define ALX_PWM_BUFF_LEN 6
#define ALX_IO_PIN_IRQ_BUFF_LEN 32

#define ALX_NULL 0
#define ALX_NULL_PTR NULL


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxGlobal_BaudRate_9600    = 9600,
	AlxGlobal_BaudRate_115200  = 115200,
	AlxGlobal_BaudRate_800000  = 800000,
	AlxGlobal_BaudRate_921600  = 921600,
	AlxGlobal_BaudRate_1000000 = 1000000,
	AlxGlobal_BaudRate_1600000 = 1600000,
	AlxGlobal_BaudRate_2000000 = 2000000,
	AlxGlobal_BaudRate_4000000 = 4000000,
	AlxGlobal_BaudRate_8000000 = 8000000
} AlxGlobal_BaudRate;

typedef enum
{
	Alx_Bit_8  =  8,
	Alx_Bit_16 = 16,
	Alx_Bit_32 = 32,
	Alx_Bit_64 = 64
} Alx_Bit;

typedef enum
{
	Alx_Ok = 0,
	Alx_Err = 1,
	Alx_ErrReInit = 2,
	Alx_ErrNumOfTries = 3,
	Alx_ErrOutOfRangeMin = 4,
	Alx_ErrOutOfRangeMax = 5,
	AlxI2c_ErrNack = 6,
	AlxI2c_ErrTimeout = 7,
	AlxFifo_ErrFull = 8,
	AlxFifo_ErrEmpty = 9,
	AlxFifo_ErrNoDelim = 10,
	Alx_ErrOutOfRangeLen = 11,
	Alx_ErrBoundMin = 12,
	Alx_ErrBoundMax = 13,
	Alx_ErrBoundLen = 14,
	AlxMemSafe_CrcOkSame_UsedCopyA = 15,
	AlxMemSafe_CrcOkDiff_UsedCopyA = 16,
	AlxMemSafe_CopyANokCopyBOk_UsedCopyB = 17,
	AlxMemSafe_CopyAOkCopyBNok_UsedCopyA = 18,
	AlxMemSafe_BothNok_ResetToDef = 19
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
	AlxClk_Clk_McuStm32_SystemCoreClock = 0,
	AlxClk_Clk_McuStm32_Sysclk = 1,
	AlxClk_Clk_McuStm32_Hclk = 2,
	AlxClk_Clk_McuStm32_Pclk1Apb1 = 3,
	AlxClk_Clk_McuStm32_Pclk2Apb2 = 4,

	AlxClk_Clk_McuStm32_SystemCoreClock_Ctor = 5,
	AlxClk_Clk_McuStm32_Sysclk_Ctor = 6,
	AlxClk_Clk_McuStm32_Hclk_Ctor = 7,
	AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor = 8,
	AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor = 9,
	AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor = 10,
	AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor = 11,

	AlxClk_Clk_McuStm32_MainPllClk = 12,
	AlxClk_Clk_McuStm32_MainPllInputClk_Ctor = 13,
	#endif

	#if defined(ALX_LPC8XX) || defined(ALX_LPC80X)
	AlxClk_Clk_McuLpc8xx_CoreSysClk = 14,
	AlxClk_Clk_McuLpc8xx_MainClk = 15,
	AlxClk_Clk_McuLpc8xx_Fro = 16,
	AlxClk_Clk_McuLpc8xx_FroDiv = 17,
	AlxClk_Clk_McuLpc8xx_ExtClk = 18,
	AlxClk_Clk_McuLpc8xx_Frg0 = 19,

	AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor = 20,
	AlxClk_Clk_McuLpc8xx_MainClk_Ctor = 21,
	AlxClk_Clk_McuLpc8xx_Fro_Ctor = 22,
	AlxClk_Clk_McuLpc8xx_LPO_Ctor = 23,
	#endif

	#if defined(ALX_LPC55S6X)
	AlxClk_Clk_McuLpc55s6x_SystemCoreClock = 24,
	AlxClk_Clk_McuLpc55s6x_AhbClk = 25,
	AlxClk_Clk_McuLpc55s6x_MainClk = 26,

	AlxClk_Clk_McuLpc55s6x_SystemCoreClock_Ctor = 27,
	AlxClk_Clk_McuLpc55s6x_AhbClk_Ctor = 28,
	AlxClk_Clk_McuLpc55s6x_MainClk_Ctor = 29,
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
Alx_Status AlxGlobal_BoundUint32(uint32_t* valPtr, uint32_t valMin, uint32_t valMax);
Alx_Status AlxGlobal_BoundFloat(float* valPtr, float valMin, float valMax);
uint32_t AlxGlobal_Round(float val);
bool AlxGlobal_IsOutOfRangeUint8(uint8_t val, uint8_t valMin, uint8_t valMax);
uint32_t AlxGlobal_Ntohl(uint32_t val);


#ifdef __cplusplus
}
#endif

#endif // ALX_GLOBAL_H
