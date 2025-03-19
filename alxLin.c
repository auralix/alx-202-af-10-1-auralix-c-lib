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
static Alx_Status AlxLin_Init(AlxLin* me, bool isMaster);
static Alx_Status AlxLin_DeInit(AlxLin* me, bool isMaster);
static uint8_t AlxLin_GetDataLenFromId(uint8_t id);
static uint8_t AlxLin_CalcProtectedId(uint8_t id);
static uint8_t AlxLin_CalcEnhancedChecksum(uint8_t protectedId, uint8_t* data, uint32_t len);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		alxSerialPort
  * @param[in]		do_BREAK
  */
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort,
	AlxIoPin* do_BREAK
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;
	me->do_BREAK = do_BREAK;

	// Info
	me->wasCtorCalled = true;
	me->isInitMaster = false;
	me->isInitSlave = false;
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
Alx_Status AlxLin_Master_Init(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == false);
	ALX_LIN_ASSERT(me->isInitSlave == false);

	// Return
	return AlxLin_Init(me, true);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_DeInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == true);
	ALX_LIN_ASSERT(me->isInitSlave == false);

	// Return
	return AlxLin_DeInit(me, true);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Slave_Init(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == false);
	ALX_LIN_ASSERT(me->isInitSlave == false);
	ALX_LIN_ASSERT(me->do_BREAK == NULL);

	// Return
	return AlxLin_Init(me, false);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Slave_DeInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == false);
	ALX_LIN_ASSERT(me->isInitSlave == true);
	ALX_LIN_ASSERT(me->do_BREAK == NULL);

	// Return
	return AlxLin_DeInit(me, false);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxLin_Master_IsInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isInitMaster;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxLin_Slave_IsInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isInitSlave;
}

/**
  * @brief										As a LIN master device, transmit frame header to specified ID and receive frame response from slave device
  *													This will probably be an ID of LIN slave device
  *													If it's our own ID, then we will need to respond to ourselves
  *													In LIN terminology when you are responding to received frame header, this is considered SUBSCRIBING
  *													AlxLin_Master_TxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[in]		id							ID which will be transmitted in the frame header
  *													Acceptable values: 0x00 .. 0x3F
  * @param[out]		data						Pointer to location to which received frame response data will be copied
  * @param[in]		len							Received frame response data length
  *													If variableLenEnable = false, then length is LIN compliant and:
  *														Acceptable values are: 2, 4, 8
  *														Length must be set accordingly to specified ID
  *															If ID = 0x00 .. 0x1F, then 2
  *															If ID = 0x20 .. 0x2F, then 4
  *															If ID = 0x30 .. 0x3F, then 8
  *													Else if variableLenEnable = true, then len should be 0, or ASSERT will happen
  * @param[in]		slaveResponseWaitTime_ms	Time in ms for which, we as LIN master device, will wait for slave device to transmit whole frame response
  *													Wait will start right after we transmitted frame header
  *													When determining this value you must consider:
  *														Selected LIN bit rate
  *														Frame response data length
  *														How fast is slave device, how quickly after received frame header does it start transmitting frame response
  *													This operation is blocking, so it's recommend that the wait time is not too large
  * @param[in]		numOfTries					Number of tries
  * @param[in]		variableLenEnable			Bool for specifying if variable data length is enabled
  * @param[in]		variableLenEnable_maxLen	Maximum frame response data length, that the user of this function can receive
  *													If received frame response data length > variableLenEnable_maxLen, Alx_Err is returned
  *													ONLY APPLICABLE if variableLenEnable = true
  * @param[out]		variableLenEnable_actualLen	Pointer to variable which will be set with actual frame response data length received after successful communication
  *													ONLY APPLICABLE if variableLenEnable = true
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Read(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, uint16_t slaveResponseWaitTime_ms, uint8_t numOfTries, bool variableLenEnable, uint32_t variableLenEnable_maxLen, uint32_t* variableLenEnable_actualLen)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == true);
	ALX_LIN_ASSERT(me->isInitSlave == false);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	if (variableLenEnable)
	{
		ALX_LIN_ASSERT(len == 0);
		ALX_LIN_ASSERT((0 < variableLenEnable_maxLen) && (variableLenEnable_maxLen <= ALX_LIN_BUFF_LEN));
	}
	else
	{
		ALX_LIN_ASSERT(len == AlxLin_GetDataLenFromId(id));
		ALX_LIN_ASSERT(variableLenEnable_maxLen == 0);
		ALX_LIN_ASSERT(variableLenEnable_actualLen == ALX_NULL);
	}

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(id);
	uint8_t txFrame[2] = {};						// SYNC + Protected ID
	uint8_t rxFrame[3 + ALX_LIN_BUFF_LEN + 1] = {};	// Break + SYNC + Protected ID + Data + Enhanced Checksum
	uint32_t rxFrameDataLen = 0;

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Prepare frame header
		// Break					// Frame Header Break - Send automatically by STM32 HW
		txFrame[0] = 0x55;			// Frame Header SYNC
		txFrame[1] = protectedId;	// Frame Header Protected ID

		// Flush serial port RX FIFO
		AlxSerialPort_FlushRxFifo(me->alxSerialPort);

		// Transmit frame header
		uint8_t txFrameLen = 2;		// SYNC + Protected ID
		status = AlxSerialPort_Write(me->alxSerialPort, txFrame, txFrameLen);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			continue;
		}

		// Wait for slave to transmit whole frame response
		#if defined(ALX_FREE_RTOS)
		AlxOsDelay_ms(&alxOsDelay, slaveResponseWaitTime_ms);
		#else
		AlxDelay_ms(slaveResponseWaitTime_ms);
		#endif

		// Check if serial port RX FIFO number of entries is OK
		uint32_t rxFrameLen = AlxSerialPort_GetRxFifoNumOfEntries(me->alxSerialPort);
		rxFrameDataLen = rxFrameLen - 4;	// We must subtract: Break, SYNC, Protected ID, Enhanced Checksum
		if (variableLenEnable)
		{
			if (rxFrameDataLen > variableLenEnable_maxLen)
			{
				ALX_LIN_TRACE_WRN("Err");
				status = Alx_Err;
				continue;
			}
		}
		else
		{
			if (rxFrameDataLen != len)
			{
				ALX_LIN_TRACE_WRN("Err");
				status = Alx_Err;
				continue;
			}
		}

		// Read received frame from serial port RX FIFO
		status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			continue;
		}

		// Check if protected ID is OK
		uint8_t protectedId_Actual = rxFrame[2];
		uint8_t protectedId_Expected = protectedId;
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if enhanced checksum is OK
		uint8_t enhancedChecksum_Actual = rxFrame[3 + rxFrameDataLen];
		uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId, &rxFrame[3], rxFrameDataLen);
		if (enhancedChecksum_Actual != enhancedChecksum_Expected)
		{
			ALX_LIN_TRACE_WRN("Err");
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
	memcpy(data, &rxFrame[3], rxFrameDataLen);

	// Return frame response actual data length
	if (variableLenEnable)
	{
		*variableLenEnable_actualLen = rxFrameDataLen;
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief								As a LIN master device, transmit frame header and frame response to specified ID
  *											This will probably be an ID of LIN slave device
  *											If it's our own ID, then we will transmit frame to ourselves
  *											In LIN terminology when you are transmitting frame response, this is considered PUBLISHING
  *											Alternative name: AlxLin_Master_TxFrameHeader_TxFrameResponse
  * @param[in,out]	me
  * @param[in]		id					ID which will be transmitted in the frame header
  *											Acceptable values: 0x00 .. 0x3F
  * @param[in]		data				Pointer to data which will be transmitted in frame response
  * @param[in]		len					Transmitted frame response data length
  *											If variableLenEnable = false, then length is LIN compliant and:
  *												Acceptable values are: 2, 4, 8
  *												Length must be set accordingly to specified ID
  *													If ID = 0x00 .. 0x1F, then 2
  *													If ID = 0x20 .. 0x2F, then 4
  *													If ID = 0x30 .. 0x3F, then 8
  *											Else if variableLenEnable = true, then 0 < len <= ALX_LIN_BUFF_LEN
  * @param[in]		variableLenEnable	Bool for specifying if variable data length is enabled
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Write(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, bool variableLenEnable)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == true);
	ALX_LIN_ASSERT(me->isInitSlave == false);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	if (variableLenEnable)
	{
		ALX_LIN_ASSERT((0 < len) && (len <= ALX_LIN_BUFF_LEN));
	}
	else
	{
		ALX_LIN_ASSERT(len == AlxLin_GetDataLenFromId(id));
	}


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(id);
	uint8_t enhancedChecksum = AlxLin_CalcEnhancedChecksum(protectedId, data, len);
	uint8_t txFrame[2 + ALX_LIN_BUFF_LEN + 1] = {};	// SYNC + Protected ID + Data + Enhanced Checksum


	//------------------------------------------------------------------------------
	// Generate Break
	//------------------------------------------------------------------------------
	if (me->do_BREAK != NULL)
	{
		// DeInit serial port
		status = AlxSerialPort_DeInit(me->alxSerialPort);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			return status;
		}

		// Init - Sets GPIO to LOW
		AlxIoPin_Init(me->do_BREAK);

		// Wait
		AlxDelay_ms(2);

		// DeInit - Sets GPIO to HIGH
		AlxIoPin_DeInit(me->do_BREAK);

		// Init serial port
		status = AlxSerialPort_Init(me->alxSerialPort);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			return status;
		}
	}


	//------------------------------------------------------------------------------
	// Transmit Frame Header + Frame Response
	//------------------------------------------------------------------------------

	// Prepare
	// Break								// Frame Header Break - Send automatically by HW or by SW above
	txFrame[0] = 0x55;						// Frame Header SYNC
	txFrame[1] = protectedId;				// Frame Header Protected ID
	memcpy(&txFrame[2], data, len);			// Frame Response Data
	txFrame[2 + len] = enhancedChecksum;	// Frame Response Enhanced Checksum
	uint8_t txFrameLen = 2 + len + 1;	// SYNC + Protected ID + Data + Enhanced Checksum

	// Write
	status = AlxSerialPort_Write(me->alxSerialPort, txFrame, txFrameLen);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}

/**
  * @brief													As a LIN slave device, receive frame header or receive frame header & frame response from master device
  *																AlxLin_Slave_RxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[out]		id										Pointer to variable which will be set with ID which was received in the frame header
  * @param[out]		data									Pointer to location to which received frame response data will be copied
  * @param[in]		len										Received frame response data length which will be read
  * @param[in]		timeout_ms								Time in ms for which, we as LIN slave device, will wait after RX FIFO Flush for master device to transmit whole frame response with specified data length
  * @param[in]		numOfTries								Number of tries
  * @param[in]		rxFifoNumOfEntriesNewCheckWaitTime_ms	Time in ms for which we will wait for new RX FIFO number of entries check
  * @param[in]		handleBreakSync							Bool for specifying if Break & SYNC will be handled
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Slave_ReadLen(AlxLin* me, uint8_t* id, uint8_t* data, uint32_t len, uint16_t timeout_ms, uint8_t numOfTries, uint16_t rxFifoNumOfEntriesNewCheckWaitTime_ms, bool handleBreakSync)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == false);
	ALX_LIN_ASSERT(me->isInitSlave == true);
	ALX_LIN_ASSERT(me->do_BREAK == NULL);
	ALX_LIN_ASSERT((0 <= len) && (len <= ALX_LIN_BUFF_LEN));


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t id_Actual = 0;
	uint8_t rxFrame[3 + ALX_LIN_BUFF_LEN + 1] = {};	// Break + SYNC + Protected ID + Data + Enhanced Checksum
	uint32_t rxFrameLen_Actual = 0;
	uint32_t rxFrameLen_Expected = 0;


	//------------------------------------------------------------------------------
	// Set rxFrameLen_Expected
	//------------------------------------------------------------------------------
	if (len == 0)
	{
		if (handleBreakSync)
		{
			rxFrameLen_Expected = 3;	// Break, SYNC, Protected ID
		}
		else
		{
			rxFrameLen_Expected = 1;	// Protected ID
		}
	}
	else
	{
		if (handleBreakSync)
		{
			rxFrameLen_Expected = 3 + len + 1;	// Break + SYNC + Protected ID + Data + Enhanced Checksum
		}
		else
		{
			rxFrameLen_Expected = 1 + len + 1;	// Protected ID + Data + Enhanced Checksum
		}
	}


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Flush serial port RX FIFO
		AlxSerialPort_FlushRxFifo(me->alxSerialPort);

		// Create timer
		AlxTimSw alxTimSw;
		AlxTimSw_Ctor(&alxTimSw, false);

		// Start timer
		AlxTimSw_Start(&alxTimSw);

		// Wait for master to transmit whole frame response with specified data length
		while (1)
		{
			// Check if serial port RX FIFO number of entries is OK
			rxFrameLen_Actual = AlxSerialPort_GetRxFifoNumOfEntries(me->alxSerialPort);

			// Check if data length is OK
			if(rxFrameLen_Actual == rxFrameLen_Expected)
			{
				status = Alx_Ok;
				break;
			}

			// Delay
			#if defined(ALX_FREE_RTOS)
			AlxOsDelay_ms(&alxOsDelay, rxFifoNumOfEntriesNewCheckWaitTime_ms);
			#else
			AlxDelay_ms(rxFifoNumOfEntriesNewCheckWaitTime_ms);
			#endif

			// Check if timeout
			if (AlxTimSw_IsTimeout_ms(&alxTimSw, timeout_ms))
			{
				ALX_LIN_TRACE_WRN("Err");
				status = Alx_Err;
				break;
			}
		}

		// If we are here and status is NOT OK, timeout occured
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Read received frame from serial port RX FIFO
		status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen_Actual);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if protected ID is OK
		uint8_t protectedId_Actual = 0;
		if (handleBreakSync)
		{
			protectedId_Actual = rxFrame[2];
			id_Actual = rxFrame[2] & 0x3F;
		}
		else
		{
			protectedId_Actual = rxFrame[0];
			id_Actual = rxFrame[0] & 0x3F;
		}
		uint8_t protectedId_Expected = AlxLin_CalcProtectedId(id_Actual);
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if enhanced checksum is OK
		if (handleBreakSync)
		{
			if (5 <= rxFrameLen_Actual)
			{
				uint8_t enhancedChecksum_Actual = rxFrame[3 + len];
				uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId_Actual, &rxFrame[3], len);
				if (enhancedChecksum_Actual != enhancedChecksum_Expected)
				{
					ALX_LIN_TRACE_WRN("Err");
					status = Alx_Err;
					continue;
				}
			}
		}
		else
		{
			if (3 <= rxFrameLen_Actual)
			{
				uint8_t enhancedChecksum_Actual = rxFrame[1 + len];
				uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId_Actual, &rxFrame[1], len);
				if (enhancedChecksum_Actual != enhancedChecksum_Expected)
				{
					ALX_LIN_TRACE_WRN("Err");
					status = Alx_Err;
					continue;
				}
			}
		}

		// Break
		break;
	}

	// If we are here and status is NOT OK, number of tries error occured
	if (status != Alx_Ok)
	{
		return Alx_ErrNumOfTries;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Return protected ID
	*id = id_Actual;

	// Return frame response data
	if (handleBreakSync)
	{
		memcpy(data, &rxFrame[3], len);
	}
	else
	{
		memcpy(data, &rxFrame[1], len);
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxLin_IrqHandler(AlxLin* me)
{
	AlxSerialPort_IrqHandler(me->alxSerialPort);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxLin_Init(AlxLin* me, bool isMaster)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Init serial port
	status = AlxSerialPort_Init(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE_WRN("Err"); return status; }

	// Info
	if (isMaster)
	{
		me->isInitMaster = true;
	}
	else
	{
		me->isInitSlave = true;
	}

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLin_DeInit(AlxLin* me, bool isMaster)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit serial port
	status = AlxSerialPort_DeInit(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE_WRN("Err"); return status; }

	// Info
	if (isMaster)
	{
		me->isInitMaster = false;
	}
	else
	{
		me->isInitSlave = false;
	}

	// Return
	return Alx_Ok;
}
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
static uint8_t AlxLin_CalcEnhancedChecksum(uint8_t protectedId, uint8_t* data, uint32_t len)
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
