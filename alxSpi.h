/**
  ******************************************************************************
  * @file alxSpi.h
  * @brief Auralix C Library - ALX SPI Module
  * @version $LastChangedRevision: 5451 $
  * @date $LastChangedDate: 2021-06-18 23:31:45 +0200 (Fri, 18 Jun 2021) $
  ******************************************************************************
  */

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
// Preprocessor
//******************************************************************************
#define ALX_SPI_FILE "alxSpi"

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

// DbgPin //
#if defined(_ALX_SPI_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_SPI_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_SPI_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSpi_Init(AlxSpi* me);
Alx_Status AlxSpi_DeInit(AlxSpi* me);
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
void AlxSpi_Master_AssertCs(AlxSpi* me);
void AlxSpi_Master_DeAssertCs(AlxSpi* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_SPI_H
