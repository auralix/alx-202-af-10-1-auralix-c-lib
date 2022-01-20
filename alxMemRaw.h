/**
  ******************************************************************************
  * @file alxMemRaw.h
  * @brief Auralix C Library - ALX Memory Raw Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_MEM_RAW_H
#define ALX_MEM_RAW_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MEM_RAW_FILE "alxMemRaw.h"

// Assert //
#if defined(_ALX_MEM_RAW_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MEM_RAW_FILE, expr)
#elif defined(_ALX_MEM_RAW_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MEM_RAW_FILE, expr)
#elif defined(_ALX_MEM_RAW_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_RST(ALX_MEM_RAW_FILE, expr)
#else
	#define ALX_MEM_RAW_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_MEM_RAW_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_MEM_RAW_TRACE(...) ALX_TRACE_STD(ALX_MEM_RAW_FILE, __VA_ARGS__)
#else
	#define ALX_MEM_RAW_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_MEM_RAW_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_MEM_RAW_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_MEM_RAW_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMemRaw;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMemRaw_Ctor
(
	AlxMemRaw* me
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMemRaw_Init(AlxMemRaw* me);
Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me);
Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms);


#ifdef __cplusplus
}
#endif

#endif // ALX_MEM_RAW_H
