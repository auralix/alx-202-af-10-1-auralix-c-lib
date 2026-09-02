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
static Alx_Status AlxFifo_ReadStrUntil_Private(AlxFifo* me, char* str, const char* delim, uint32_t len, uint32_t* lenActual, bool delimIsSet);


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
  * @brief								Reads one line from FIFO, terminated by the FIRST occurrence of the FULL delim SEQUENCE (e.g. "\r\n" = the two bytes CR,LF adjacent)
  * @param[in,out]	me
  * @param[out]		str					Line INCLUDING terminator, ALWAYS null-terminated; empty string on every non-Ok return
  * @param[in]		delim				Delimiter sequence - all chars must occur adjacent, in order
  * @param[in]		len					Size of str buffer INCLUDING null terminator - max line = len-1 chars
  * @param[out]		lenActual			Line length EXCLUDING null terminator, 0 on every non-Ok return, NULL allowed
  * @retval			Alx_Ok				Line delivered & consumed (empty line = terminator only)
  * @retval			AlxFifo_ErrEmpty	FIFO empty
  * @retval			AlxFifo_ErrNoDelim	No terminator yet & FIFO not full - FIFO left untouched
  * @retval			AlxFifo_ErrTooLong	Undeliverable line DISCARDED: line found but > len-1 chars (only that line discarded), or FIFO full without terminator (whole content discarded - line can never complete)
  */
Alx_Status AlxFifo_ReadStrUntil(AlxFifo* me, char* str, const char* delim, uint32_t len, uint32_t* lenActual)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT(0 < len);

	// Read
	return AlxFifo_ReadStrUntil_Private(me, str, delim, len, lenActual, false);
}

/**
  * @brief								Reads one line from FIFO, terminated by the FIRST occurrence of ANY SINGLE char from delimSet (e.g. "\r\n" = CR or LF, whichever comes first)
  * @param[in,out]	me
  * @param[out]		str					Line INCLUDING terminator, ALWAYS null-terminated; empty string on every non-Ok return
  * @param[in]		delimSet			Set of single-char delimiters - byte 0x00 is never a set member
  * @param[in]		len					Size of str buffer INCLUDING null terminator - max line = len-1 chars
  * @param[out]		lenActual			Line length EXCLUDING null terminator, 0 on every non-Ok return, NULL allowed
  * @retval			Alx_Ok				Line delivered & consumed (empty line = terminator only, lenActual == 1)
  * @retval			AlxFifo_ErrEmpty	FIFO empty
  * @retval			AlxFifo_ErrNoDelim	No terminator yet & FIFO not full - FIFO left untouched
  * @retval			AlxFifo_ErrTooLong	Undeliverable line DISCARDED: line found but > len-1 chars (only that line discarded), or FIFO full without terminator (whole content discarded - line can never complete)
  */
Alx_Status AlxFifo_ReadStrUntilAny(AlxFifo* me, char* str, const char* delimSet, uint32_t len, uint32_t* lenActual)
{
	// Assert
	ALX_FIFO_ASSERT(me->wasCtorCalled == true);
	ALX_FIFO_ASSERT(0 < len);

	// Read
	return AlxFifo_ReadStrUntil_Private(me, str, delimSet, len, lenActual, true);
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
	return AlxFifo_Write(me, (const uint8_t*)str, strlen(str));
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxFifo_GetNumOfEntries(const AlxFifo* me)
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
		me->tail++;									// Increment tail, rewind if necessary - compare instead of %, Cortex-M0+ has no divider
		if (me->tail == me->buffLen)
		{
			me->tail = 0;
		}
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
		me->head++;									// Increment head, rewind if necessary - compare instead of %, Cortex-M0+ has no divider
		if (me->head == me->buffLen)
		{
			me->head = 0;
		}
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
static Alx_Status AlxFifo_ReadStrUntil_Private(AlxFifo* me, char* str, const char* delim, uint32_t len, uint32_t* lenActual, bool delimIsSet)
{
	//------------------------------------------------------------------------------
	// Prepare Out Params - str always null-terminated, empty until a line is delivered
	//------------------------------------------------------------------------------
	str[0] = '\0';
	if (lenActual != NULL)
	{
		*lenActual = 0;
	}


	//------------------------------------------------------------------------------
	// Handle Empty
	//------------------------------------------------------------------------------
	if (me->isEmpty == true)
	{
		return AlxFifo_ErrEmpty;
	}


	//------------------------------------------------------------------------------
	// Search Line End - lineLen = line length INCLUDING terminator, 0 = not found
	//------------------------------------------------------------------------------
	uint32_t delimLen;
	if (delimIsSet)
	{
		delimLen = 1;
	}
	else
	{
		delimLen = (uint32_t)strlen(delim);
	}
	uint32_t lineLen = 0;
	uint32_t idx = me->tail;	// Buffer index of byte i - advanced with a wrap compare instead of (tail + i) % buffLen, Cortex-M0+ has no divider
	for (uint32_t i = 0; (i + delimLen) <= me->numOfEntries; i++)
	{
		char ch = (char)me->buff[idx];
		if (delimIsSet)
		{
			// Set semantics - terminate at first char that is a member of delim set (0x00 is never a member)
			if ((ch != '\0') && (strchr(delim, ch) != NULL))
			{
				lineLen = i + 1;
				break;
			}
		}
		else
		{
			// Sequence semantics - terminate at first occurrence of the full delim sequence
			if (ch == delim[0])
			{
				bool match = true;
				uint32_t idxJ = idx;	// Buffer index of byte i + j, same wrap-by-compare
				for (uint32_t j = 1; j < delimLen; j++)
				{
					idxJ++;
					if (idxJ == me->buffLen)
					{
						idxJ = 0;
					}
					if ((char)me->buff[idxJ] != delim[j])
					{
						match = false;
						break;
					}
				}
				if (match)
				{
					lineLen = i + delimLen;
					break;
				}
			}
		}

		// Next byte
		idx++;
		if (idx == me->buffLen)
		{
			idx = 0;
		}
	}


	//------------------------------------------------------------------------------
	// Handle No Line
	//------------------------------------------------------------------------------
	if (lineLen == 0)
	{
		if (me->isFull)
		{
			// Line can never complete (nothing more can enter) - discard everything to restore liveness
			uint8_t dummy = 0;
			uint32_t numOfEntries = me->numOfEntries;
			for (uint32_t k = 0; k < numOfEntries; k++)
			{
				AlxFifo_ReadByte(me, &dummy);
			}
			return AlxFifo_ErrTooLong;
		}
		return AlxFifo_ErrNoDelim;	// Line may still complete - FIFO left untouched
	}


	//------------------------------------------------------------------------------
	// Handle Line Too Long - line + null terminator must fit into len
	//------------------------------------------------------------------------------
	if ((lineLen + 1) > len)	// line + null terminator must fit (no uint32 underflow for len == 0)
	{
		// Discard ONLY this line incl. terminator - data behind it survives
		uint8_t dummy = 0;
		for (uint32_t k = 0; k < lineLen; k++)
		{
			AlxFifo_ReadByte(me, &dummy);
		}
		return AlxFifo_ErrTooLong;
	}


	//------------------------------------------------------------------------------
	// Read Line - incl. terminator
	//------------------------------------------------------------------------------
	for (uint32_t k = 0; k < lineLen; k++)
	{
		AlxFifo_ReadByte(me, (uint8_t*)&str[k]);
	}
	str[lineLen] = '\0';
	if (lenActual != NULL)
	{
		*lenActual = lineLen;
	}
	return Alx_Ok;
}	// NOTE: AI Created - ALX-1514


#endif	// #if defined(ALX_C_LIB)
