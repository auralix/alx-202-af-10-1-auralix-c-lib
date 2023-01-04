/**
  ******************************************************************************
  * @file		alxFifo.c
  * @brief		Auralix C Library - ALX FIFO Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxFifo.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxFifo_ReadByte(AlxFifo* me, uint8_t* data);
static Alx_Status AlxFifo_WriteByte(AlxFifo* me, uint8_t data);
static uint8_t* AlxFifo_GetTailPtr(AlxFifo* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFifo_Ctor
(
	AlxFifo* me,
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
void AlxFifo_Flush(AlxFifo* me)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	for (uint32_t i = 0; i < me->buffLen; i++) me->buff[i] = 0;
	me->head = 0;
	me->tail = 0;
	me->numOfEntries = 0;
	me->isFull = false;
	me->isEmpty = true;
}
Alx_Status AlxFifo_Read(AlxFifo* me, uint8_t* data, uint32_t len)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT((0 < len) && (len <= me->buffLen));

	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxFifo_ReadByte(me, &data[i]);
		if (status != Alx_Ok) return status;
	}

	return status;
}
Alx_Status AlxFifo_ReadStrUntil(AlxFifo* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead)
{
	// #0 Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	//ALX_FIFO_ASSERT((delim != "") && (strlen(delim) <= me->buffLen)); // TV: Warning  comparison with string literal results in unspecified behavior
	ALX_FIFO_ASSERT((0 < maxLen) && (maxLen <= me->buffLen));
	(void)numRead;

	Alx_Status status = Alx_Err;

	// #1 Return if fifo empty
	if (me->isEmpty == true)
	{
		status = AlxFifo_ErrEmpty;
		if (numRead != NULL) { *numRead = 0; }
	}
	else
	{
		status = AlxFifo_ErrNoDelim;
		if (numRead != NULL) { *numRead = 0; }

		// #2 search for "delim" in _buff
		uint8_t* ptrDelim = NULL;
		uint32_t countDelimChar = 0;
		uint32_t numBytesRead = 0;
		for (uint32_t i = 0; i < me->numOfEntries; i++)
		{
			if ((me->buff[(i + me->tail) % me->buffLen]) == (delim[0]))
			{
				ptrDelim = AlxFifo_GetTailPtr(me) + i;
				for (uint32_t j = 1; j < strlen(delim); j++)
				{
					if ((me->buff[(i + me->tail + j) % me->buffLen] == delim[0 + j]) && ((i + j) < me->numOfEntries))
					{
						countDelimChar++;
					}
				}
				countDelimChar++;

				// #3 Checks if there is "delim" in FIFO and
				if ((strlen(delim) == countDelimChar) && (strlen(delim) <= me->numOfEntries))
				{
					numBytesRead = ptrDelim - AlxFifo_GetTailPtr(me);
					uint8_t dummy = 0;

					// #3.1 Handle ReadUntil
					if (numBytesRead < maxLen)
					{
						for (uint32_t k = 0; k < numBytesRead; k++)
						{
							AlxFifo_ReadByte(me, (uint8_t*)&str[k]);
						}
						str[numBytesRead] = '\0';

						if (numRead != NULL) { *numRead = numBytesRead; }
					}
					else
					{
						// Delete bytes that are before maxLen
						for (uint32_t l = 0; l < numBytesRead - maxLen; l++)
						{
							AlxFifo_ReadByte(me, &dummy);
						}

						for (uint32_t m = 0; m < maxLen; m++)
						{
							AlxFifo_ReadByte(me, (uint8_t*)&str[m]);
						}

						if (numRead != NULL) { *numRead = maxLen; }
					}

					// #3.2 Delete "delim" from FIFO
					for (uint32_t n = 0; n < strlen(delim); n++)
					{
						AlxFifo_ReadByte(me, &dummy);
					}

					status = Alx_Ok;
					break;
				}
				countDelimChar = 0;
			}
		}
	}

	return status;
}
Alx_Status AlxFifo_Write(AlxFifo* me, uint8_t data)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	(void)data;

	return AlxFifo_WriteMulti(me, &data, 1);
}
Alx_Status AlxFifo_WriteMulti(AlxFifo* me, const uint8_t* data, uint32_t len)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT((0 < len) && (len <= me->buffLen));

	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxFifo_WriteByte(me, data[i]);
		if (status != Alx_Ok) return status;
	}

	return status;
}
Alx_Status AlxFifo_WriteStr(AlxFifo* me, const char* str)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	return AlxFifo_WriteMulti(me, (uint8_t*)str, strlen(str));
}
uint32_t AlxFifo_GetNumOfEntries(AlxFifo* me)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	return me->numOfEntries;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxFifo_ReadByte(AlxFifo* me, uint8_t* data)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

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

	ALX_FIFO_ASSERT((0 <= me->head) && (me->head <= me->buffLen));
	ALX_FIFO_ASSERT((0 <= me->tail) && (me->tail <= me->buffLen));
	ALX_FIFO_ASSERT((0 <= me->numOfEntries) && (me->numOfEntries <= me->buffLen));

	return status;
}
static Alx_Status AlxFifo_WriteByte(AlxFifo* me, uint8_t data)
{
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
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

	ALX_FIFO_ASSERT((0 <= me->head) && (me->head <= me->buffLen));
	ALX_FIFO_ASSERT((0 <= me->tail) && (me->tail <= me->buffLen));
	ALX_FIFO_ASSERT((0 <= me->numOfEntries) && (me->numOfEntries <= me->buffLen));

	return status;
}
static uint8_t* AlxFifo_GetTailPtr(AlxFifo* me)
{
	return (uint8_t*)me->buff + me->tail;
}


#endif	// #if defined(ALX_C_LIB)
