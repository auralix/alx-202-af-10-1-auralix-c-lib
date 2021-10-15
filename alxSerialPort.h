/**
  ******************************************************************************
  * @file alxSerialPort.h
  * @brief Auralix C Library - ALX Serial Port Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_SERIAL_PORT_H
#define ALX_SERIAL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"

#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxSerialPort_McuStm32.h"

#else
typedef struct { bool dummy; } AlxSerialPort;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SERIAL_PORT_FILE "alxSerialPort"

// Assert //
#if defined(_ALX_SERIAL_PORT_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SERIAL_PORT_FILE, expr)
#elif defined(_ALX_SERIAL_PORT_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SERIAL_PORT_FILE, expr)
#elif defined(_ALX_SERIAL_PORT_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_RST(ALX_SERIAL_PORT_FILE, expr)
#else
	#define ALX_SERIAL_PORT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_SERIAL_PORT_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_SERIAL_PORT_TRACE(...) ALX_TRACE_STD(ALX_SERIAL_PORT_FILE, __VA_ARGS__)
#else
	#define ALX_SERIAL_PORT_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_SERIAL_PORT_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_SERIAL_PORT_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_SERIAL_PORT_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxSerialPort_Init			(AlxSerialPort* me);
Alx_Status AlxSerialPort_DeInit			(AlxSerialPort* me);
Alx_Status AlxSerialPort_Read			(AlxSerialPort* me, uint8_t* data, uint32_t len);
Alx_Status AlxSerialPort_ReadStrUntil	(AlxSerialPort* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead);
Alx_Status AlxSerialPort_Write			(AlxSerialPort* me, uint8_t data);
Alx_Status AlxSerialPort_WriteMulti		(AlxSerialPort* me, const uint8_t* data, uint32_t len);
Alx_Status AlxSerialPort_WriteStr		(AlxSerialPort* me, const char* str);
void AlxSerialPort_Foreground_Handle	(AlxSerialPort* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_SERIAL_PORT_H
