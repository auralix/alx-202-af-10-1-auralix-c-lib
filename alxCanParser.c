/**
  ******************************************************************************
  * @file		alxCanParser.c
  * @brief		Auralix C Library - ALX CAN Parser Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
#include "alxCanParser.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
typedef union
{
	float number;
	uint8_t bytes[4];
}AlxCanParser_FloatUnion;

typedef union
{
	double number;
	uint8_t bytes[8];
}AlxCanParser_DoubleUnion;


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxCanParser_BitGet(uint8_t var, uint8_t bit);
static void AlxCanParser_BitSet(uint8_t *var, uint8_t bit, bool state);


//******************************************************************************
// Functions
//******************************************************************************
void AlxCanParser_SetBit(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, bool value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset < 8);	// Byte offset out of packet range
	ALX_CAN_PARSER_ASSERT(bitOffset < 8);		// Bit offset out of byte range
	AlxCanParser_BitSet(&msg->data[byteOffset], bitOffset, value);
}	// Tested
void AlxCanParser_SetUint8(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1)); 	// Byte offset out of packet range
	msg->data[byteOffset] = value;
}	// Not tested
void AlxCanParser_SetInt8(AlxCan_Msg* msg, uint8_t byteOffset, int8_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1));  	// Byte offset out of packet range
	msg->data[byteOffset] = (uint8_t)value ;
}		// Not tested
void AlxCanParser_SetUint16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint16_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 2)); 	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 1] = (uint8_t)value;
		break;

	case Little:
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 0] = (uint8_t)value;
		break;

	default: break;
	}
}	// Tested
void AlxCanParser_SetInt16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int16_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 2)); 	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 1] = (uint8_t)value;
		break;

	case Little:
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 0] = (uint8_t)value;
		break;

	default: break;
	}
}	// Not tested
void AlxCanParser_SetUint32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint32_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4)); 	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 1] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 3] = (uint8_t)value;
		break;
	case Little:
		msg->data[byteOffset + 0] = (uint8_t)value;
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 24);
		break;
	default: break;
	}
}	// Not tested
void AlxCanParser_SetInt32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int32_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4));  	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 1] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 3] = (uint8_t)value;
		break;
	case Little:
		msg->data[byteOffset + 0] = (uint8_t)value;
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 24);
		break;
	default: break;
	}
}	// Not tested
void AlxCanParser_SetUint64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint64_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));  	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 56);
		msg->data[byteOffset + 1] = (uint8_t)(value >> 48);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 40);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 32);
		msg->data[byteOffset + 4] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 5] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 6] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 7] = (uint8_t)value;
		break;
	case Little:
		msg->data[byteOffset + 0] = (uint8_t)value;
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 32);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 40);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 48);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 56);
		break;
	default: break;
	}
}	// Not tested
void AlxCanParser_SetInt64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int64_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));   	// Byte offset out of packet range

	// #1 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = (uint8_t)(value >> 56);
		msg->data[byteOffset + 1] = (uint8_t)(value >> 48);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 40);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 32);
		msg->data[byteOffset + 4] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 5] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 6] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 7] = (uint8_t)value;
		break;
	case Little:
		msg->data[byteOffset + 0] = (uint8_t)value;
		msg->data[byteOffset + 1] = (uint8_t)(value >> 8);
		msg->data[byteOffset + 2] = (uint8_t)(value >> 16);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 24);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 32);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 40);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 48);
		msg->data[byteOffset + 3] = (uint8_t)(value >> 56);
		break;
	default: break;
	}
}	// Not tested
void AlxCanParser_SetFloat(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, float value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4));   	// Byte offset out of packet range

	// #1 Prepare variables
	AlxCanParser_FloatUnion myFloat;
	myFloat.number = value;

	// #2 Copy data to message according to endian selection
	switch (endian)
	{
	case Big:
		msg->data[byteOffset + 0] = myFloat.bytes[3];
		msg->data[byteOffset + 1] = myFloat.bytes[2];
		msg->data[byteOffset + 2] = myFloat.bytes[1];
		msg->data[byteOffset + 3] = myFloat.bytes[0];
		break;
	case Little:
		msg->data[byteOffset + 0] = myFloat.bytes[0];
		msg->data[byteOffset + 1] = myFloat.bytes[1];
		msg->data[byteOffset + 2] = myFloat.bytes[2];
		msg->data[byteOffset + 3] = myFloat.bytes[3];
		break;

	default: break;
	}
}		// Not tested
void AlxCanParser_SetDouble(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, double value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));   	// Byte offset out of packet range

	// #1 Prepare variables
	AlxCanParser_DoubleUnion myDouble;
	myDouble.number = value;

	// #2 Copy data to message according to endian selection
	switch(endian)
	{
	case Big:
		msg->data[byteOffset + 0] = myDouble.bytes[7];
		msg->data[byteOffset + 1] = myDouble.bytes[6];
		msg->data[byteOffset + 2] = myDouble.bytes[5];
		msg->data[byteOffset + 3] = myDouble.bytes[4];
		msg->data[byteOffset + 4] = myDouble.bytes[3];
		msg->data[byteOffset + 5] = myDouble.bytes[2];
		msg->data[byteOffset + 6] = myDouble.bytes[1];
		msg->data[byteOffset + 7] = myDouble.bytes[0];
		break;
	case Little:
		msg->data[byteOffset + 0] = myDouble.bytes[0];
		msg->data[byteOffset + 1] = myDouble.bytes[1];
		msg->data[byteOffset + 2] = myDouble.bytes[2];
		msg->data[byteOffset + 3] = myDouble.bytes[3];
		msg->data[byteOffset + 4] = myDouble.bytes[4];
		msg->data[byteOffset + 5] = myDouble.bytes[5];
		msg->data[byteOffset + 6] = myDouble.bytes[6];
		msg->data[byteOffset + 7] = myDouble.bytes[7];
		break;

	default: break;
	}
}	// Not tested
void AlxCanParser_SetEnum(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, uint8_t nOfBits, uint8_t value)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1));  	// Byte offset out of packet range
	ALX_CAN_PARSER_ASSERT(bitOffset < 8);		// Bit offset out of byte range
	ALX_CAN_PARSER_ASSERT(nOfBits < 8);		// Number of bits out of range

	// #1 Copy enum bits to selected positions
	for(uint8_t i = 0 ; i < nOfBits ; i++)
	{
		AlxCanParser_BitSet(&msg->data[byteOffset], bitOffset + i, AlxCanParser_BitGet(value, i));
	}
}	// Tested
bool AlxCanParser_GetBit(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1));  	// Byte offset out of packet range
	ALX_CAN_PARSER_ASSERT(bitOffset < 8);		// Bit offset out of byte range

	return AlxCanParser_BitGet(msg->data[byteOffset], bitOffset);
}	// Tested
uint8_t AlxCanParser_GetUint8(AlxCan_Msg* msg, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1)); 	// Byte offset out of packet range
	return msg->data[byteOffset];
}	// Tested
int8_t AlxCanParser_GetInt8(AlxCan_Msg* msg, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1));  	// Byte offset out of packet range
	return (int8_t)msg->data[byteOffset];
}	// Not tested
uint16_t AlxCanParser_GetUint16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 2)); 	// Byte offset out of packet range

	// #1 Prepare variables
	uint16_t value;

	// #2 Copy data according to endian selection
	switch (endian)
	{
	case Big:
		value =		(uint16_t)(msg->data[byteOffset + 1] << 0)
				|	(uint16_t)(msg->data[byteOffset + 0] << 8);
		break;
	case Little:
		value =		(uint16_t)(msg->data[byteOffset + 1] << 8)
				|	(uint16_t)(msg->data[byteOffset + 0] << 0);
		break;
	default: break;
	}
	return value;
}	// Not Tested
int16_t	AlxCanParser_GetInt16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 2));  	// Byte offset out of packet range

	// #1 Prepare variables
	uint16_t value;

	// #2 Copy data according to endian selection
	switch (endian)
	{
	case Big:
		value =		(uint16_t)(msg->data[byteOffset + 1] << 0)
				|	(uint16_t)(msg->data[byteOffset + 0] << 8);
		break;
	case Little:
		value =		(uint16_t)(msg->data[byteOffset + 1] << 8)
				|	(uint16_t)(msg->data[byteOffset + 0] << 0);
		break;
	default: break;
	}
	return (int16_t)value;
}	// Not tested
uint32_t AlxCanParser_GetUint32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4));  	// Byte offset out of packet range

	// #1 Prepare variables
	uint32_t value;

	// #2 Copy data according to endian selection
	switch (endian)
	{
	case Big:
		value =		(uint32_t)(msg->data[byteOffset + 3] << 0)
				|	(uint32_t)(msg->data[byteOffset + 2] << 8)
				|	(uint32_t)(msg->data[byteOffset + 1] << 16)
				|	(uint32_t)(msg->data[byteOffset + 0] << 24);
		break;
	case Little:
		value =		(uint32_t)(msg->data[byteOffset + 3] << 24)
				|	(uint32_t)(msg->data[byteOffset + 2] << 16)
				|	(uint32_t)(msg->data[byteOffset + 1] << 8)
				|	(uint32_t)(msg->data[byteOffset + 0] << 0);
		break;
	default: break;
	}

	return value;
}	// Tested
int32_t AlxCanParser_GetInt32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4));  	// Byte offset out of packet range

	// #1 Prepare variables
	int32_t value;

	// #2 Copy data according to endian selection
	switch (endian)
	{
	case Big:
		value =		(int32_t)(msg->data[byteOffset + 3] << 0)
				|	(int32_t)(msg->data[byteOffset + 2] << 8)
				|	(int32_t)(msg->data[byteOffset + 1] << 16)
				|	(int32_t)(msg->data[byteOffset + 0] << 24);
		break;
	case Little:
		value =		(int32_t)(msg->data[byteOffset + 3] << 24)
				|	(int32_t)(msg->data[byteOffset + 2] << 16)
				|	(int32_t)(msg->data[byteOffset + 1] << 8)
				|	(int32_t)(msg->data[byteOffset + 0] << 0);
		break;
	default: break;
	}

	return value;
} 	// Not tested
uint64_t AlxCanParser_GetUint64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));  	// Byte offset out of packet range

	// #1 Prepare variables
	uint64_t value;

	// #2 Copy data according to endian selection
	switch (endian)
	{
	case Big:
		value =		((uint64_t)(msg->data[byteOffset + 7]) << 0)
				|	((uint64_t)(msg->data[byteOffset + 6]) << 8)
				|	((uint64_t)(msg->data[byteOffset + 5]) << 16)
				|	((uint64_t)(msg->data[byteOffset + 4]) << 24)
				|	((uint64_t)(msg->data[byteOffset + 3]) << 32)
				|	((uint64_t)(msg->data[byteOffset + 2]) << 40)
				|	((uint64_t)(msg->data[byteOffset + 1]) << 48)
				|	((uint64_t)(msg->data[byteOffset + 0]) << 56);
		break;
	case Little:
		value =		((uint64_t)(msg->data[byteOffset + 7]) << 56)
				|	((uint64_t)(msg->data[byteOffset + 6]) << 48)
				|	((uint64_t)(msg->data[byteOffset + 5]) << 40)
				|	((uint64_t)(msg->data[byteOffset + 4]) << 32)
				|	((uint64_t)(msg->data[byteOffset + 3]) << 24)
				|	((uint64_t)(msg->data[byteOffset + 2]) << 16)
				|	((uint64_t)(msg->data[byteOffset + 1]) << 8)
				|	((uint64_t)(msg->data[byteOffset + 0]) << 0);
		break;
	default: break;
	}

	return value;
} // Not tested
int64_t AlxCanParser_GetInt64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));  	// Byte offset out of packet range

	// #1 Prepare variables
	int64_t value;

	// #2 Copy data according to endian selection
	switch(endian)
	{
	case Big:
		value =		((int64_t)(msg->data[byteOffset + 7]) << 0)
				|	((int64_t)(msg->data[byteOffset + 6]) << 8)
				|	((int64_t)(msg->data[byteOffset + 5])<< 16)
				|	((int64_t)(msg->data[byteOffset + 4]) << 24)
				|	((int64_t)(msg->data[byteOffset + 3]) << 32)
				|	((int64_t)(msg->data[byteOffset + 2]) << 40)
				|	((int64_t)(msg->data[byteOffset + 1]) << 48)
				|	((int64_t)(msg->data[byteOffset + 0]) << 56);
		break;
	case Little:
		value =		((int64_t)(msg->data[byteOffset + 7]) << 56)
				|	((int64_t)(msg->data[byteOffset + 6]) << 48)
				|	((int64_t)(msg->data[byteOffset + 5]) << 40)
				|	((int64_t)(msg->data[byteOffset + 4]) << 32)
				|	((int64_t)(msg->data[byteOffset + 3]) << 24)
				|	((int64_t)(msg->data[byteOffset + 2]) << 16)
				|	((int64_t)(msg->data[byteOffset + 1]) << 8)
				|	((int64_t)(msg->data[byteOffset + 0]) << 0);
		break;
	default: break;
	}

	return value;
} 	// Not tested
float AlxCanParser_GetFloat(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 4));  	// Byte offset out of packet range

	// #1 Prepare variables
	AlxCanParser_FloatUnion myFloat;

	// #2 Copy data to float union according to endian selection
	switch (endian)
	{
	case Big:
		myFloat.bytes[3] = msg->data[byteOffset + 0];
		myFloat.bytes[2] = msg->data[byteOffset + 1];
		myFloat.bytes[1] = msg->data[byteOffset + 2];
		myFloat.bytes[0] = msg->data[byteOffset + 3];
		break;

	case Little:
		myFloat.bytes[0] = msg->data[byteOffset + 0];
		myFloat.bytes[1] = msg->data[byteOffset + 1];
		myFloat.bytes[2] = msg->data[byteOffset + 2];
		myFloat.bytes[3] = msg->data[byteOffset + 3];
		break;
	default: break;
	}
	return myFloat.number;
} 	// Not tested
double AlxCanParser_GetDouble(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 8));	// Byte offset out of packet range

	// #1 Prepare variables
	AlxCanParser_DoubleUnion myDouble;

	// #2 Copy data to float union according to endian selection
	switch (endian)
	{
	case Big:
		myDouble.bytes[3] = msg->data[byteOffset + 0];
		myDouble.bytes[2] = msg->data[byteOffset + 1];
		myDouble.bytes[1] = msg->data[byteOffset + 2];
		myDouble.bytes[0] = msg->data[byteOffset + 3];
		break;

	case Little:
		myDouble.bytes[0] = msg->data[byteOffset + 0];
		myDouble.bytes[1] = msg->data[byteOffset + 1];
		myDouble.bytes[2] = msg->data[byteOffset + 2];
		myDouble.bytes[3] = msg->data[byteOffset + 3];
		break;
	default: break;
	}
	return myDouble.number;
}	// Not tested
uint8_t AlxCanParser_GetEnum(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, uint8_t nOfBits)
{
	ALX_CAN_PARSER_ASSERT(byteOffset <= (8 - 1));	// Byte offset out of packet range
	ALX_CAN_PARSER_ASSERT(bitOffset < 8);			// Bit offset out of byte range
	ALX_CAN_PARSER_ASSERT(nOfBits < 8);			// Number of bits out of range

	// #1 Prepare variables
	uint8_t value = 0;

	// #2 Set enum bits to selected positions
	for (uint8_t i = 0; i < nOfBits; i++)				// copy bits to selected positions
	{
		AlxCanParser_BitSet(&value, i, AlxCanParser_BitGet(msg->data[byteOffset], bitOffset + i));
	}
	return value;
}	// Tested


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxCanParser_BitGet(uint8_t var, uint8_t bit)
{
	ALX_CAN_PARSER_ASSERT(bit < 8);  		// Bit offset out of byte range

	// #1 Prepare variables
	uint8_t mask = 0x01;

	// #2 Prepare mask
	mask = mask << bit;

	// #3 Check bit state
	if((var&mask) > 0){ return true; }
	return false;
}
static void AlxCanParser_BitSet(uint8_t *var, uint8_t bit, bool state)
{
	ALX_CAN_PARSER_ASSERT(bit < 8);  		// Bit offset out of byte range

	// #1 Set bit
	if(state) {
		*var |=  (0x01 << bit);
	}
	// #2 Reset bit
	else {
		*var &= ~(0x01 << bit);
	}
}


#endif // #if defined(ALX_C_LIB)
