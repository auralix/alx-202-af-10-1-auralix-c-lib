/**
  ******************************************************************************
  * @file alxTrace_Fifo.c
  * @brief Auralix C Library - ALX Trace FIFO Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace_Fifo.h"


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Fifo_ReadByte(AlxTrace_Fifo* me, uint8_t* data);
static Alx_Status AlxTrace_Fifo_WriteByte(AlxTrace_Fifo* me, uint8_t data);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Fifo_Ctor
(
	AlxTrace_Fifo* me,
	uint8_t* buff,
	uint32_t buffLen
)
{
	// Parameters
	me->buff = buff;
	me->buffLen = buffLen;

	// Variables
	for (uint32_t i = 0; i < me->buffLen; i++) me->buff[i] = 0;
	me->head = 0;
	me->tail = 0;
	me->numOfEntries = 0;
	me->isFull = false;
	me->isEmpty = true;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxTrace_Fifo_Flush(AlxTrace_Fifo* me)
{
	for (uint32_t i = 0; i < me->buffLen; i++) me->buff[i] = 0;
	me->head = 0;
	me->tail = 0;
	me->numOfEntries = 0;
	me->isFull = false;
	me->isEmpty = true;
}
Alx_Status AlxTrace_Fifo_Read(AlxTrace_Fifo* me, uint8_t* data, uint32_t len)
{
	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxTrace_Fifo_ReadByte(me, &data[i]);
		if (status != Alx_Ok) return status;
	}

	return status;
}
Alx_Status AlxTrace_Fifo_Write(AlxTrace_Fifo* me, const uint8_t* data, uint32_t len)
{
	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxTrace_Fifo_WriteByte(me, data[i]);
		if (status != Alx_Ok) return status;
	}

	return status;
}

uint32_t AlxTrace_Fifo_GetNumOfEntries(AlxTrace_Fifo* me)
{
	return me->numOfEntries;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Fifo_ReadByte(AlxTrace_Fifo* me, uint8_t* data)
{
	Alx_Status status = Alx_Err;

	// #1 Check if fifo empty
	if(me->isEmpty)
	{
		status = AlxFifo_ErrEmpty;
	}
	else
	{
		// #2 Handle fifo read
		*data = me->buff[me->tail];
		me->buff[me->tail] = 0;						// Set read byte to 0
		me->tail = (me->tail + 1) % me->buffLen;	// Increment tail, rewind if necessary
		me->numOfEntries--;
		me->isFull = false;							// Fifo not full anymore

		// #3 Update isEmpty flag if needed
		if (me->numOfEntries <= 0)
		{
			me->isEmpty = true;
		}

		status = Alx_Ok;
	}

	return status;
}
static Alx_Status AlxTrace_Fifo_WriteByte(AlxTrace_Fifo* me, uint8_t data)
{
	(void)data;

	Alx_Status status = Alx_Err;

	// #1 Check if fifo full
	if(me->isFull)
	{
		status = AlxFifo_ErrFull;
	}
	else
	{
		// #2 Handle fifo write
		me->buff[me->head] = data;
		me->head = (me->head + 1) % me->buffLen;	// Increment head, rewind if necessary
		me->numOfEntries++;
		me->isEmpty = false;						// Fifo not empty anymore

		// #3 Update isFull flag if needed
		if (me->numOfEntries >= me->buffLen)
		{
			me->isFull = true;
		}

		status = Alx_Ok;
	}

	return status;
}
