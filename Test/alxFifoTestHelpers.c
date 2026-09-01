/**
  ******************************************************************************
  * @file		alxFifoTestHelpers.c
  * @brief		Auralix C Library - ALX FIFO Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor/destructor + status enum getters, so the Python
  * test layer (ctypes) never mirrors the AlxFifo struct layout or hardcodes
  * enum values. All testing goes through the public alxFifo.h API.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxFifo.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxFifo* AlxFifoTest_New(uint32_t buffLen);
void AlxFifoTest_Delete(AlxFifo* me);
int32_t AlxFifoTest_Status_Ok(void);
int32_t AlxFifoTest_Status_Err(void);
int32_t AlxFifoTest_Status_ErrFull(void);
int32_t AlxFifoTest_Status_ErrEmpty(void);
int32_t AlxFifoTest_Status_ErrNoDelim(void);
int32_t AlxFifoTest_Status_ErrTooLong(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxFifo* AlxFifoTest_New(uint32_t buffLen)
{
	AlxFifo* me = (AlxFifo*)malloc(sizeof(AlxFifo));
	uint8_t* buff = (uint8_t*)malloc(buffLen);
	if ((me == NULL) || (buff == NULL))
	{
		exit(1);	// test infrastructure - fail fast on OOM
	}
	memset(buff, '\n', buffLen);	// poison with a DELIMITER byte - any scan past the
									// valid entries finds a phantom terminator deterministically
	AlxFifo_Ctor(me, buff, buffLen);
	return me;
}
void AlxFifoTest_Delete(AlxFifo* me)
{
	free(me->buff);
	free(me);
}
int32_t AlxFifoTest_Status_Ok(void)			{ return (int32_t)Alx_Ok; }
int32_t AlxFifoTest_Status_Err(void)		{ return (int32_t)Alx_Err; }
int32_t AlxFifoTest_Status_ErrFull(void)	{ return (int32_t)AlxFifo_ErrFull; }
int32_t AlxFifoTest_Status_ErrEmpty(void)	{ return (int32_t)AlxFifo_ErrEmpty; }
int32_t AlxFifoTest_Status_ErrNoDelim(void)	{ return (int32_t)AlxFifo_ErrNoDelim; }
int32_t AlxFifoTest_Status_ErrTooLong(void)	{ return (int32_t)AlxFifo_ErrTooLong; }
