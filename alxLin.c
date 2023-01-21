/**
  ******************************************************************************
  * @file		alxLin.c
  * @brief		Auralix C Library - ALX LIN Module
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
#include "alxLin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxLin_GetDataLenFromId(uint8_t id);
static uint8_t AlxLin_CalcProtectedId(uint8_t id);
static uint8_t AlxLin_CalcEnhancedChecksum(uint8_t protectedId, uint8_t* data, uint8_t len);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		alxSerialPort
  */
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Init(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init serial port
	status = AlxSerialPort_Init(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE("Err"); return status; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_DeInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit serial port
	status = AlxSerialPort_DeInit(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE("Err"); return status; }

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief						As a LIN master device, transmit frame header and frame response to specified ID
  *									This will probably be an ID of LIN slave device
  *									If it's our own ID, then we will transmit frame to ourselves
  *									In LIN terminology when you are transmitting frame response, this is considered PUBLISHING
  *									Alternative name: AlxLin_Master_TxFrameHeader_TxFrameResponse
  * @param[in,out]	me
  * @param[in]		id			ID which will be transmitted in the frame header
  *									Acceptable values: 0x00 .. 0x3F
  * @param[in]		data		Pointer to data which will be transmitted in frame response
  * @param[in]		len			Transmitted frame reponse data length
  *									Acceptable values: 2, 4, 8
  *									Must be set accordingly to specified ID
  *										If ID = 0 .. 0x1F, then 2
  *										If ID = 0x20 .. 0x2F, then 4
  *										If ID = 0x30 .. 0x3F, then 8
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Publish(AlxLin* me, uint8_t id, uint8_t* data, uint8_t len)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	ALX_LIN_ASSERT(len == AlxLin_GetDataLenFromId(id));

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(id);
	uint8_t enhancedChecksum = AlxLin_CalcEnhancedChecksum(protectedId, data, len);
	uint8_t frame[16] = {};

	// Prepare frame header + frame response
	// Break							// Frame Header Break - Send automatically by STM32 HW
	frame[0] = 0x55;					// Frame Header SYNC
	frame[1] = protectedId;				// Frame Header Protected ID
	memcpy(&frame[2], data, len);		// Frame Response Data
	frame[2 + len] = enhancedChecksum;	// Frame Response Enhanced Checksum

	// Transmit frame header + frame response
	uint8_t txFrameLen = 2 + len + 1;	// SYNC + Protected ID + Data Length + Enhanced Checksum
	status = AlxSerialPort_Write(me->alxSerialPort, frame, txFrameLen);
	if (status != Alx_Ok) { ALX_LIN_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief					As a LIN master device, transmit frame header to specified ID and receive frame response from slave device
  *								This will probably be an ID of LIN slave device
  *								If it's our own ID, then we will need to respond to ourselves
  *								In LIN terminology when you are responding to received frame header, this is considered SUBSCRIBING
  *								AlxLin_Master_TxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[in]		id		ID which will be transmitted in the frame header
  *								Acceptable values: 0x00 .. 0x3F
  * @param[out]		data	Pointer to location to which received frame response data will be copied
  * @param[in]		len		Received frame response data length
  *								Acceptable values: 2, 4, 8
  *								Must be set accordingly to specified ID
  *									If ID = 0 .. 0x1F, then 2
  *									If ID = 0x20 .. 0x2F, then 4
  *									If ID = 0x30 .. 0x3F, then 8
  * @param[in]		slaveResponseWaitTime_ms	Time in ms for which, we as LIN master device, will wait for slave device to transmit whole frame response
  *													Wait will start right after we transmitted frame header
  *													When determining this value you must consider:
  *														Selected LIN bit rate
  *														Frame response data length
  *														How fast is slave device, how quickly after received frame header does it start transmitting frame response
  *													This operation is blocking, so it's recommend that the wait time it's not too large
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Subscribe(AlxLin* me, uint8_t id, uint8_t* data, uint8_t len, uint8_t slaveResponseWaitTime_ms, uint8_t numOfTries)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	ALX_LIN_ASSERT(len == AlxLin_GetDataLenFromId(id));

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(id);
	uint8_t frame[16] = {};

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Prepare frame header
		// Break				// Frame Header Break - Send automatically by STM32 HW
		frame[0] = 0x55;		// Frame Header SYNC
		frame[1] = protectedId;	// Frame Header Protected ID

		// Flush serial port RX FIFO
		AlxSerialPort_FlushRxFifo(me->alxSerialPort);

		// Transmit frame header
		uint8_t txFrameLen = 1 + 1;	// SYNC + Protected ID
		status = AlxSerialPort_Write(me->alxSerialPort, frame, txFrameLen);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE("Err");
			continue;
		}

		// Wait for slave to transmit whole frame response
		AlxDelay_ms(slaveResponseWaitTime_ms);

		// Read received frame from serial port RX FIFO
		uint8_t rxFrameLen = 1 + 1 + 1 + len + 1;	// Break + SYNC + Protected ID + Data Length + Enhanced Checksum
		status = AlxSerialPort_Read(me->alxSerialPort, frame, rxFrameLen);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE("Err");
			continue;
		}

		// Check if protected ID is OK
		uint8_t protectedId_Actual = frame[2];
		uint8_t protectedId_Expected = protectedId;
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE("Err");
			status = Alx_Err;
			continue;
		}

		// Check if enhanced checksum is OK
		uint8_t enhancedChecksum_Actual = frame[3 + len];
		uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId, &frame[3], len);
		if (enhancedChecksum_Actual != enhancedChecksum_Expected)
		{
			ALX_LIN_TRACE("Err");
			status = Alx_Err;
			continue;
		}

		// Break
		break;
	}

	// If we are here and status is NOT OK, number of tries error occured
	if (status != Alx_Ok)
	{
		return Alx_ErrNumOfTries;
	}

	// Return frame response data
	memcpy(data, &frame[3], len);

	// Return
	return Alx_Ok;
}
void AlxLin_IrqHandler(AlxLin* me)
{
	AlxSerialPort_IrqHandler(me->alxSerialPort);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxLin_GetDataLenFromId(uint8_t id)
{
	// Get
	if ((0 <= id) && (id <= 0x1F))
	{
		return 2;
	}
	else if ((0x20 <= id) && (id <= 0x2F))
	{
		return 4;
	}
	else if ((0x30 <= id) && (id <= 0x3F))
	{
		return 8;
	}

	// Assert
	ALX_LIN_ASSERT(false);	// We should not get here
	return 0;
}
static uint8_t AlxLin_CalcProtectedId(uint8_t id)
{
	// Calculate
	uint8_t b6 = (id^(id>>1)^(id>>2)^(id>>4))&0x01;
	uint8_t b7 = (~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01;

	// Return
	return (id&0x3f)|(b6<<6)|(b7<<7);
}
static uint8_t AlxLin_CalcEnhancedChecksum(uint8_t protectedId, uint8_t* data, uint8_t len)
{
	// Local variables
	uint32_t sum = protectedId;

	// Calculate
	for (uint32_t i = 0; i < len; i++)
	{
		sum = sum + data[i];
		if (sum >= 256)
		{
			sum = sum - 255;
		}
	}

	// Invert
	sum = ~sum;

	// Return
	return sum;
}


#endif	// #if defined(ALX_C_LIB)
