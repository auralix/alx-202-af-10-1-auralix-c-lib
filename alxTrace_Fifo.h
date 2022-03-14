/**
  ******************************************************************************
  * @file alxTrace_Fifo.h
  * @brief Auralix C Library - ALX Trace FIFO Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_FIFO_H
#define ALX_TRACE_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TRACE_FIFO_FILE "alxTraceFifo"


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
} AlxTrace_Fifo;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Fifo_Ctor
(
	AlxTrace_Fifo* me,
	uint8_t* buff,
	uint32_t buffLen
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxTrace_Fifo_Flush(AlxTrace_Fifo* me);
Alx_Status AlxTrace_Fifo_Read(AlxTrace_Fifo* me, uint8_t* data, uint32_t len);
Alx_Status AlxTrace_Fifo_Write(AlxTrace_Fifo* me, const uint8_t* data, uint32_t len);
uint32_t AlxTrace_Fifo_GetNumOfEntries(AlxTrace_Fifo* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_FIFO_H
