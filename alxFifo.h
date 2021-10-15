/**
  ******************************************************************************
  * @file alxFifo.h
  * @brief Auralix C Library - ALX FIFO Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_FIFO_H
#define ALX_FIFO_H

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
// Preprocessor
//******************************************************************************
#define ALX_FIFO_FILE "alxFifo"

// Assert //
#if defined(_ALX_FIFO_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FIFO_FILE, expr)
#elif defined(_ALX_FIFO_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FIFO_FILE, expr)
#elif defined(_ALX_FIFO_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_RST(ALX_FIFO_FILE, expr)
#else
	#define ALX_FIFO_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_FIFO_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_FIFO_TRACE(...) ALX_TRACE_STD(ALX_FIFO_FILE, __VA_ARGS__)
#else
	#define ALX_FIFO_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_FIFO_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_FIFO_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_FIFO_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t* buff;
	uint32_t buffLen;
	
	// Variables
	uint32_t head;
	uint32_t tail;
	uint32_t numOfEntries;
	bool isFull;
	bool isEmpty;

	// Info
	bool wasCtorCalled;
} AlxFifo;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFifo_Ctor
(
	AlxFifo* me,
	uint8_t* buff,
	uint32_t buffLen
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxFifo_Flush(AlxFifo* me);
Alx_Status AlxFifo_Read(AlxFifo* me, uint8_t* data, uint32_t len);
Alx_Status AlxFifo_ReadStrUntil(AlxFifo* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead);
Alx_Status AlxFifo_Write(AlxFifo* me, uint8_t data);
Alx_Status AlxFifo_WriteMulti(AlxFifo* me, const uint8_t* data, uint32_t len);
Alx_Status AlxFifo_WriteStr(AlxFifo* me, const char* str);
uint32_t AlxFifo_GetNumOfEntries(AlxFifo* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_FIFO_H
