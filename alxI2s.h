/**
  ******************************************************************************
  * @file alxI2s.h
  * @brief Auralix C Library - ALX I2S Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_I2S_H
#define ALX_I2S_H

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
#if defined(ALX_STM32F4)
#include "alxI2s_McuStm32.h"

#else
typedef struct { bool dummy; } AlxI2s;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_I2S_FILE "alxI2s"

// Assert //
#if defined(_ALX_I2S_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_BKPT(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_TRACE(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_RST(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_MOCK) || defined(_ALX_ASSERT_MOCK_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_MOCK(ALX_I2S_FILE, expr)
#else
	#define ALX_I2S_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_I2S_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_I2S_TRACE(...) ALX_TRACE_STD(ALX_I2S_FILE, __VA_ARGS__)
#else
	#define ALX_I2S_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_I2S_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_I2S_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_I2S_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxI2s_Init(AlxI2s* me);
Alx_Status AlxI2s_DeInit(AlxI2s* me);
void AlxI2s_Foreground_Handle(AlxI2s* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_I2S_H



























//void AlxI2s_Start_Write			(AlxI2s* me, uint8_t* data, uint16_t len);
//void AlxI2s_Stop_Write			(AlxI2s* me);
//void AlxI2s_Start_Read			(AlxI2s* me, uint8_t* data);
//void AlxI2s_Stop_Read			(AlxI2s* me);
//void AlxI2s_Foreground_Handle	(AlxI2s* me);
//uint8_t AlxI2s_GetFrameLength	(AlxI2s* me);