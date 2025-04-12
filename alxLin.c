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
static Alx_Status AlxLin_GetFrameConfigFromId(AlxLin* me, uint8_t id, AlxLin_FrameConfig* frameConfig);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxLin_Master_Subscribe_Callback(AlxLin* me, AlxLin_Frame* frame);
void AlxLin_Slave_Subscribe_Callback(AlxLin* me, AlxLin_Frame frame);
void AlxLin_Slave_Publish_Callback(AlxLin* me, AlxLin_Frame* frame);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		alxSerialPort
  * @param[in]		masterReadSwHandleBreak
  * @param[in]		slaveReadSwHandleBreakSync
  * @param[in]		frameConfigArr
  * @param[in]		frameConfigArrLen
  */
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort,
	bool masterReadSwHandleBreak,
	bool slaveReadSwHandleBreakSync,
	AlxLin_FrameConfig* frameConfigArr,
	uint8_t frameConfigArrLen
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;
	me->masterReadSwHandleBreak = masterReadSwHandleBreak;
	me->slaveReadSwHandleBreakSync = slaveReadSwHandleBreakSync;
	me->frameConfigArr = frameConfigArr;
	me->frameConfigArrLen = frameConfigArrLen;

	// Parameters - Private
	if (slaveReadSwHandleBreakSync)
	{
		me->slaveReadBreakSyncOffset = 0;
	}
	else
	{
		me->slaveReadBreakSyncOffset = 2;
	}

	// Variables
	memset(&me->rxb, 0, sizeof(me->rxb));

	// Info
	me->wasCtorCalled = true;
	me->isInitMaster = false;
	me->isInitSlave = false;
}


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Master
//------------------------------------------------------------------------------

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
  *													If variableLen = false, then length is LIN compliant and:
  *														Acceptable values are: 2, 4, 8
  *														Length must be set accordingly to specified ID
  *															If ID = 0x00 .. 0x1F, then 2
  *															If ID = 0x20 .. 0x2F, then 4
  *															If ID = 0x30 .. 0x3F, then 8
  *													Else if variableLen = true, then len should be 0, or ASSERT will happen
  * @param[in]		slaveResponseWaitTime_ms	Time in ms for which, we as LIN master device, will wait for slave device to transmit whole frame response
  *													Wait will start right after we transmitted frame header
  *													When determining this value you must consider:
  *														Selected LIN bit rate
  *														Frame response data length
  *														How fast is slave device, how quickly after received frame header does it start transmitting frame response
  *													This operation is blocking, so it's recommend that the wait time is not too large
  * @param[in]		numOfTries					Number of tries
  * @param[in]		variableLen					Bool for specifying if variable data length is enabled
  * @param[in]		variableLen_maxLen			Maximum frame response data length, that the user of this function can receive
  *													If received frame response data length > variableLen_maxLen, Alx_Err is returned
  *													ONLY APPLICABLE if variableLen = true
  * @param[out]		variableLen_actualLen		Pointer to variable which will be set with actual frame response data length received after successful communication
  *													ONLY APPLICABLE if variableLen = true
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Read(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, uint16_t slaveResponseWaitTime_ms, uint8_t numOfTries, bool variableLen, uint32_t variableLen_maxLen, uint32_t* variableLen_actualLen)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == true);
	ALX_LIN_ASSERT(me->isInitSlave == false);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	if (variableLen)
	{
		ALX_LIN_ASSERT(len == 0);
		ALX_LIN_ASSERT((0 < variableLen_maxLen) && (variableLen_maxLen <= ALX_LIN_BUFF_LEN));
	}
	else
	{
		ALX_LIN_ASSERT(len == AlxLin_GetDataLenFromId(id));
		ALX_LIN_ASSERT(variableLen_maxLen == 0);
		ALX_LIN_ASSERT(variableLen_actualLen == ALX_NULL);
	}


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(id);
	uint8_t txFrame[2] = {};						// SYNC + Protected ID
	uint8_t rxFrame[3 + ALX_LIN_BUFF_LEN + 1] = {};	// Break + SYNC + Protected ID + Data + Enhanced Checksum
	uint32_t rxFrameDataLen = 0;
	uint32_t breakOffset = 0;
	if (me->masterReadSwHandleBreak)
	{
		breakOffset = 0;
	}
	else
	{
		breakOffset = 1;
	}


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Flush serial port RX FIFO
		AlxSerialPort_FlushRxFifo(me->alxSerialPort);

		// Prepare frame header
		// Break					// Frame Header Break - Send by AlxSerialPort_Write
		txFrame[0] = 0x55;			// Frame Header SYNC
		txFrame[1] = protectedId;	// Frame Header Protected ID
		uint8_t txFrameLen = 2;		// SYNC + Protected ID

		// Transmit frame header
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
		rxFrameDataLen = rxFrameLen - (4 - breakOffset);	// We must subtract: *Break, SYNC, Protected ID, Enhanced Checksum
		if (variableLen)
		{
			if (rxFrameDataLen > variableLen_maxLen)
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

		// Read received frame response from serial port RX FIFO
		status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			continue;
		}

		// Check if protected ID is OK
		uint8_t protectedId_Actual = rxFrame[2 - breakOffset];
		uint8_t protectedId_Expected = protectedId;
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if enhanced checksum is OK
		uint8_t enhancedChecksum_Actual = rxFrame[(3 - breakOffset) + rxFrameDataLen];
		uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId, &rxFrame[3 - breakOffset], rxFrameDataLen);
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


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Return frame response data
	memcpy(data, &rxFrame[3 - breakOffset], rxFrameDataLen);

	// Return frame response actual data length
	if (variableLen)
	{
		*variableLen_actualLen = rxFrameDataLen;
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief							As a LIN master device, transmit frame header and frame response to specified ID
  *										This will probably be an ID of LIN slave device
  *										If it's our own ID, then we will transmit frame to ourselves
  *										In LIN terminology when you are transmitting frame response, this is considered PUBLISHING
  *										Alternative name: AlxLin_Master_TxFrameHeader_TxFrameResponse
  * @param[in,out]	me
  * @param[in]		id				ID which will be transmitted in the frame header
  *										Acceptable values: 0x00 .. 0x3F
  * @param[in]		data			Pointer to data which will be transmitted in frame response
  * @param[in]		len				Transmitted frame response data length
  *										If variableLen = false, then length is LIN compliant and:
  *											Acceptable values are: 2, 4, 8
  *											Length must be set accordingly to specified ID
  *												If ID = 0x00 .. 0x1F, then 2
  *												If ID = 0x20 .. 0x2F, then 4
  *												If ID = 0x30 .. 0x3F, then 8
  *										Else if variableLen = true, then 0 < len <= ALX_LIN_BUFF_LEN
  * @param[in]		variableLen		Bool for specifying if variable data length is enabled
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Write(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, bool variableLen)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == true);
	ALX_LIN_ASSERT(me->isInitSlave == false);
	ALX_LIN_ASSERT((0 <= id) && (id <= 0x3F));
	if (variableLen)
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
	// Transmit Frame Header + Frame Response
	//------------------------------------------------------------------------------

	// Prepare frame header
	// Break								// Frame Header Break - Send by AlxSerialPort_Write
	txFrame[0] = 0x55;						// Frame Header SYNC
	txFrame[1] = protectedId;				// Frame Header Protected ID
	memcpy(&txFrame[2], data, len);			// Frame Response Data
	txFrame[2 + len] = enhancedChecksum;	// Frame Response Enhanced Checksum
	uint8_t txFrameLen = 2 + len + 1;		// SYNC + Protected ID + Data + Enhanced Checksum

	// Transmit frame header
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


//------------------------------------------------------------------------------
// Slave
//------------------------------------------------------------------------------

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

	// Return
	return AlxLin_DeInit(me, false);
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
  * @brief													As a LIN slave device, receive frame header or receive frame header & frame response from master device
  *																AlxLin_Slave_RxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[out]		id										Pointer to variable which will be set with ID which was received in the frame header
  * @param[out]		data									Pointer to location to which received frame response data will be copied
  * @param[in]		len										Received frame response data length which will be read
  * @param[in]		timeout_ms								Time in ms for which, we as LIN slave device, will wait after RX FIFO Flush for master device to transmit whole frame response with specified data length
  * @param[in]		numOfTries								Number of tries
  * @param[in]		rxFifoNumOfEntriesNewCheckWaitTime_ms	Time in ms for which we will wait for new RX FIFO number of entries check
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Slave_Read(AlxLin* me, uint8_t* id, uint8_t* data, uint32_t len, uint16_t timeout_ms, uint8_t numOfTries, uint16_t rxFifoNumOfEntriesNewCheckWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInitMaster == false);
	ALX_LIN_ASSERT(me->isInitSlave == true);
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
		rxFrameLen_Expected = 3 - me->slaveReadBreakSyncOffset;	// *Break + *SYNC + Protected ID
	}
	else
	{
		rxFrameLen_Expected = (3 - me->slaveReadBreakSyncOffset) + len + 1;	// *Break + *SYNC + Protected ID + Data + Enhanced Checksum
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
			// Check if serial port RX FIFO number of entries are OK
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

		// Read received frame response from serial port RX FIFO
		status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen_Actual);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if protected ID is OK
		uint8_t protectedId_Actual = rxFrame[2 - me->slaveReadBreakSyncOffset];
		id_Actual = rxFrame[2 - me->slaveReadBreakSyncOffset] & 0x3F;
		uint8_t protectedId_Expected = AlxLin_CalcProtectedId(id_Actual);
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE_WRN("Err");
			status = Alx_Err;
			continue;
		}

		// Check if enhanced checksum is OK
		if ((5 - me->slaveReadBreakSyncOffset) <= rxFrameLen_Actual)
		{
			uint8_t enhancedChecksum_Actual = rxFrame[(3 - me->slaveReadBreakSyncOffset) + len];
			uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId_Actual, &rxFrame[3 - me->slaveReadBreakSyncOffset], len);
			if (enhancedChecksum_Actual != enhancedChecksum_Expected)
			{
				ALX_LIN_TRACE_WRN("Err");
				status = Alx_Err;
				continue;
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

	// Return ID
	*id = id_Actual;

	// Return frame response data
	memcpy(data, &rxFrame[3 - me->slaveReadBreakSyncOffset], len);

	// Return
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// RX Buffer
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  */
void AlxLin_RxBuff_Flush(AlxLin* me)
{
	memset(&me->rxb, 0, sizeof(me->rxb));
	me->rxb.active = true;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  */
void AlxLin_RxBuff_Handle(AlxLin* me, uint8_t data)
{
	//------------------------------------------------------------------------------
	// Check if Active
	//------------------------------------------------------------------------------
	if (me->rxb.active == false)
	{
		return;
	}


	//------------------------------------------------------------------------------
	// Handle Write
	//------------------------------------------------------------------------------
	me->rxb.buff[me->rxb.i] = data;


	//------------------------------------------------------------------------------
	// Handle Frame ID, Protected ID, Data Length
	//------------------------------------------------------------------------------
	if (me->rxb.i == 0)
	{
		// Check protected ID
		uint8_t protectedId_Actual = me->rxb.buff[0];
		uint8_t id_Actual = protectedId_Actual & 0x3F;
		uint8_t protectedId_Expected = AlxLin_CalcProtectedId(id_Actual);
		if (protectedId_Actual != protectedId_Expected)
		{
			ALX_LIN_TRACE_DBG("FAIL: CheckProtectedId() protectedId_Actual %u protectedId_Expected %u", protectedId_Actual, protectedId_Expected);
			me->rxb.active = false;
			return;
		}

		// Get frame config from ID
		AlxLin_FrameConfig frameConfig = {};
		Alx_Status status = AlxLin_GetFrameConfigFromId(me, id_Actual, &frameConfig);
		if (status != Alx_Ok)
		{
			ALX_LIN_TRACE_DBG("FAIL: AlxLin_GetFrameConfigFromId() status %ld id_Actual %u", status, id_Actual);
			me->rxb.active = false;
			return;
		}

		// Set frame
		me->rxb.frame.id = id_Actual;
		me->rxb.frame.protectedId = protectedId_Actual;
		me->rxb.frame.dataLen = frameConfig.dataLen;

		// Set frame config
		me->rxb.frameConfig = frameConfig;

		// Slave publish
		if (me->rxb.frameConfig.publish)
		{
			// Callback
			AlxLin_Slave_Publish_Callback(me, &me->rxb.frame);

			// Set enhanced checksum
			me->rxb.frame.enhancedChecksum = AlxLin_CalcEnhancedChecksum(me->rxb.frame.protectedId, me->rxb.frame.data, me->rxb.frame.dataLen);

			// Clear active
			me->rxb.active = false;

			// Transmit frame response
			Alx_Status status = AlxSerialPort_Write(me->alxSerialPort, me->rxb.frame.data, me->rxb.frame.dataLen + 1);
			if (status != Alx_Ok)
			{
				ALX_LIN_TRACE_DBG("FAIL: AlxSerialPort_Write() status %ld", status);
				return;
			}
		}
	}


	//------------------------------------------------------------------------------
	// Handle Data
	//------------------------------------------------------------------------------
	else if (me->rxb.i == me->rxb.frame.dataLen)
	{
		for (uint8_t i = 0; i < me->rxb.frame.dataLen; i++)
		{
			me->rxb.frame.data[i] = me->rxb.buff[i + 1];
		}
	}


	//------------------------------------------------------------------------------
	// Handle Enhanced Checksum
	//------------------------------------------------------------------------------
	else if (me->rxb.i == me->rxb.frame.dataLen + 1)
	{
		// Check enhanced checksum
		uint8_t enhancedChecksum_Actual = me->rxb.buff[me->rxb.i];
		uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(me->rxb.frame.protectedId, me->rxb.frame.data, me->rxb.frame.dataLen);
		if (enhancedChecksum_Actual != enhancedChecksum_Expected)
		{
			ALX_LIN_TRACE_DBG("FAIL: CheckEnhancedChecksum() enhancedChecksum_Actual %u enhancedChecksum_Expected %u", enhancedChecksum_Actual, enhancedChecksum_Expected);
			me->rxb.active = false;
			return;
		}

		// Set frame
		me->rxb.frame.enhancedChecksum = enhancedChecksum_Actual;

		// Callback
		AlxLin_Slave_Subscribe_Callback(me, me->rxb.frame);
	}


	//------------------------------------------------------------------------------
	// Increment
	//------------------------------------------------------------------------------
	me->rxb.i++;
}


//------------------------------------------------------------------------------
// IRQ
//------------------------------------------------------------------------------

/**
  * @brief
  * @param[in,out]	me
  */
void AlxLin_Irq_Handle(AlxLin* me)
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
static Alx_Status AlxLin_GetFrameConfigFromId(AlxLin* me, uint8_t id, AlxLin_FrameConfig* frameConfig)
{
	for (uint32_t i = 0; i < me->frameConfigArrLen; i++)
	{
		if (me->frameConfigArr[i].id == id)
		{
			*frameConfig = me->frameConfigArr[i];
			return Alx_Ok;
		}
	}

	// Return
	return Alx_Err;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxLin_Slave_Subscribe_Callback(AlxLin* me, AlxLin_Frame frame)
{
	(void)me;
	(void)frame;
}
ALX_WEAK void AlxLin_Slave_Publish_Callback(AlxLin* me, AlxLin_Frame* frame)
{
	(void)me;
	(void)frame;
}


#endif	// #if defined(ALX_C_LIB)
