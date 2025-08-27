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

/**
  * @brief
  * @param[in,out]	me
  * @param[in,out]	buff
  * @param[in]		buffLen
  */
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
	me->head = 0;
	me->tail = 0;
	me->numOfEntries = 0;
	me->numOfEntriesSinceFlush = 0;
	me->isFull = false;
	me->isEmpty = true;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxFifo_Flush(AlxFifo* me)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->head = 0;
	me->tail = 0;
	me->numOfEntries = 0;
	me->numOfEntriesSinceFlush = 0;
	me->isFull = false;
	me->isEmpty = true;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  * @retval			AlxFifo_ErrEmpty
  */
Alx_Status AlxFifo_Read(AlxFifo* me, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT(0 < len && len <= me->buffLen);

	// Read
	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxFifo_ReadByte(me, &data[i]);
		if (status != Alx_Ok)
		{
			return status;
		}
	}

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		str
  * @param[in]		delim
  * @param[in]		maxLen
  * @param[out]		numRead
  * @retval			Alx_Ok
  * @retval			Alx_Err
  * @retval			AlxFifo_ErrEmpty
  * @retval			AlxFifo_ErrNoDelim
  */
Alx_Status AlxFifo_ReadStrUntil(AlxFifo* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT(0 < maxLen && maxLen <= me->buffLen);


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	if (me->isEmpty == true)
	{
		status = AlxFifo_ErrEmpty;
		if (numRead != NULL)
		{
			*numRead = 0;
		}
	}
	else
	{
		status = AlxFifo_ErrNoDelim;
		if (numRead != NULL)
		{
			*numRead = 0;
		}

		// Search for "delim" in _buff
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

				// Checks if there is "delim" in FIFO
				if ((strlen(delim) == countDelimChar) && (strlen(delim) <= me->numOfEntries))
				{
					numBytesRead = ptrDelim - AlxFifo_GetTailPtr(me);
					uint8_t dummy = 0;

					// Handle ReadUntil
					if (numBytesRead < maxLen)
					{
						for (uint32_t k = 0; k < numBytesRead; k++)
						{
							AlxFifo_ReadByte(me, (uint8_t*)&str[k]);
						}
						str[numBytesRead] = '\0';

						if (numRead != NULL)
						{
							*numRead = numBytesRead;
						}
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

						if (numRead != NULL)
						{
							*numRead = maxLen;
						}
					}

					// Delete "delim" from FIFO
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


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  * @retval			AlxFifo_ErrFull
  */
Alx_Status AlxFifo_Write(AlxFifo* me, const uint8_t* data, uint32_t len)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT(0 < len && len <= me->buffLen);

	// Write
	Alx_Status status = Alx_Err;
	for (uint32_t i = 0; i < len; i++)
	{
		status = AlxFifo_WriteByte(me, data[i]);
		if (status != Alx_Ok)
		{
			return status;
		}
	}

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  * @retval			AlxFifo_ErrFull
  */
Alx_Status AlxFifo_WriteStr(AlxFifo* me, const char* str)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	// Return
	return AlxFifo_Write(me, (uint8_t*)str, strlen(str));
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxFifo_GetNumOfEntries(AlxFifo* me)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->numOfEntries;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		len
  * @return			numOfEntriesRewinded
  */
uint32_t AlxFifo_Rewind(AlxFifo* me, uint32_t len)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);

	// Check length
	if (len == 0)
	{
		return 0;
	}

	// Bound numOfEntriesUnused
	if (me->buffLen > me->numOfEntries)
	{
		uint32_t numOfEntriesUnused = me->buffLen - me->numOfEntries;
		AlxBound_Uint32(&len, 1, numOfEntriesUnused);
	}
	else
	{
		return 0;
	}

	// Bound numOfEntriesRewindable
	if (me->numOfEntriesSinceFlush > me->numOfEntries)
	{
		uint64_t numOfEntriesRewindable = me->numOfEntriesSinceFlush - me->numOfEntries;
		uint64_t _len = len;
		AlxBound_Uint64(&_len, 1, numOfEntriesRewindable);
		len = (uint32_t)_len;
	}
	else
	{
		return 0;
	}

	// Handle rewind
	me->tail = (me->tail + me->buffLen - len) % me->buffLen;	// Decrement tail, rewind if necessary
	me->numOfEntries = me->numOfEntries + len;
	me->isEmpty = false;										// Fifo not empty anymore

	// Update isFull flag if needed
	if (me->numOfEntries == me->buffLen)
	{
		me->isFull = true;
	}

	// Return
	return len;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxFifo_ReadByte(AlxFifo* me, uint8_t* data)
{
	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if fifo empty
	if(me->isEmpty)
	{
		status = AlxFifo_ErrEmpty;
	}
	else
	{
		// Handle fifo read
		*data = me->buff[me->tail];
		me->tail = (me->tail + 1) % me->buffLen;	// Increment tail, rewind if necessary
		me->numOfEntries--;
		me->isFull = false;							// Fifo not full anymore

		// Update isEmpty flag if needed
		if (me->numOfEntries == 0)
		{
			me->isEmpty = true;
		}

		// Set
		status = Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}
static Alx_Status AlxFifo_WriteByte(AlxFifo* me, uint8_t data)
{
	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if fifo full
	if(me->isFull)
	{
		status = AlxFifo_ErrFull;
	}
	else
	{
		// Handle fifo write
		me->buff[me->head] = data;
		me->head = (me->head + 1) % me->buffLen;	// Increment head, rewind if necessary
		me->numOfEntries++;
		me->numOfEntriesSinceFlush++;
		me->isEmpty = false;						// Fifo not empty anymore

		// Update isFull flag if needed
		if (me->numOfEntries == me->buffLen)
		{
			me->isFull = true;
		}

		// Set
		status = Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}
static uint8_t* AlxFifo_GetTailPtr(AlxFifo* me)
{
	return (uint8_t*)me->buff + me->tail;
}


#endif	// #if defined(ALX_C_LIB)
