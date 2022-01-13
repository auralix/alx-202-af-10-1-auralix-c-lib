/**
  ******************************************************************************
  * @file alxI2c.h
  * @brief Auralix C Library - ALX I2C Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_I2C_H
#define ALX_I2C_H

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
#include "alxI2c_McuStm32.h"

#elif defined(ALX_LPC80X)
#include "alxI2c_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxI2c;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_I2C_FILE "alxI2c"

// Assert //
#if defined(_ALX_I2C_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_BKPT(ALX_I2C_FILE, expr)
#elif defined(_ALX_I2C_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_TRACE(ALX_I2C_FILE, expr)
#elif defined(_ALX_I2C_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_RST(ALX_I2C_FILE, expr)
#else
	#define ALX_I2C_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_I2C_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_I2C_TRACE(...) ALX_TRACE_STD(ALX_I2C_FILE, __VA_ARGS__)
#else
	#define ALX_I2C_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_I2C_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_I2C_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_I2C_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Master_MemAddrLen_8bit = 1,
	AlxI2c_Master_MemAddrLen_16bit = 2
} AlxI2c_Master_MemAddrLen;


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxI2c_Init(AlxI2c* me);
Alx_Status AlxI2c_DeInit(AlxI2c* me);
Alx_Status AlxI2c_Master_StartRead					(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartReadStop				(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartReadMemStop			(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWrite					(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteStop				(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteMemStop_Single	(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t data, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi	(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_Stop						(AlxI2c* me, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_IsSlaveReady				(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms);


//******************************************************************************
// Common Functions
//******************************************************************************


#ifdef __cplusplus
}
#endif

#endif // ALX_I2C_H
