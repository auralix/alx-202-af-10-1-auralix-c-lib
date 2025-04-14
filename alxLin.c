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
static uint8_t AlxLin_CalcProtectedId(uint8_t id);
static uint8_t AlxLin_CalcEnhancedChecksum(uint8_t protectedId, uint8_t* data, uint32_t len);
static Alx_Status AlxLin_GetSlaveFrameConfigFromId(AlxLin* me, uint8_t id, AlxLin_SlaveFrameConfig* slaveFrameConfig);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxLin_Master_Subscribe_Callback(AlxLin* me, AlxLin_Frame frame);
void AlxLin_Slave_Subscribe_Callback(AlxLin* me, AlxLin_Frame frame);
void AlxLin_Slave_Publish_Callback(AlxLin* me, AlxLin_Frame* frame);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		alxSerialPort
  * @param[in]		breakSyncOffset
  * @param[in]		slaveFrameConfigArr
  * @param[in]		slaveFrameConfigArrLen
  */
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort,
	uint8_t breakSyncOffset,
	AlxLin_SlaveFrameConfig* slaveFrameConfigArr,
	uint8_t slaveFrameConfigArrLen
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;
	me->breakSyncOffset = breakSyncOffset;
	me->slaveFrameConfigArr = slaveFrameConfigArr;
	me->slaveFrameConfigArrLen = slaveFrameConfigArrLen;

	// Variables
	memset(&me->rxb, 0, sizeof(me->rxb));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
	me->isMaster = false;
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
	ALX_LIN_ASSERT(me->isInit == false);
	ALX_LIN_ASSERT(me->isMaster == false);

	// Init
	Alx_Status status = AlxSerialPort_Init(me->alxSerialPort);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Init() status %ld", status);
		return status;
	}

	// Set info
	me->isInit = true;
	me->isMaster = true;

	// Return
	return Alx_Ok;
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
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == true);

	// DeInit
	Alx_Status status = AlxSerialPort_DeInit(me->alxSerialPort);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_DeInit() status %ld", status);
		return status;
	}

	// Clear info
	me->isInit = false;
	me->isMaster = false;

	// Return
	return Alx_Ok;
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
	ALX_LIN_ASSERT(me->isMaster == true);

	// Return
	return me->isInit;
}

/**
  * @brief										As a LIN master device, transmit frame header to specified ID and receive frame response from slave device
  *													This will probably be an ID of LIN slave device
  *													If it's our own ID, then we will need to respond to ourselves
  *													In LIN terminology when you are responding to received frame header, this is considered SUBSCRIBING
  *													AlxLin_Master_TxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[in,out]	frame						Pointer to frame variable in which data will be received and shall already include:
  *													frame.id - Frame Header ID
  *														ID which will be transmitted in the frame header
  *														Acceptable values: 0x00 .. 0x3F
  *													frame.dataLen - Frame Response Data Length
  *														Received frame response data length
  *														0 < frame.dataLen <= ALX_LIN_BUFF_LEN
  *														If we want that data length is LIN compliant then:
  *															Acceptable values are: 2, 4, 8
  *															Length must be set accordingly to specified ID
  *																If ID = 0x00 .. 0x1F, then 2
  *																If ID = 0x20 .. 0x2F, then 4
  *																If ID = 0x30 .. 0x3F, then 8
  *													After successful operation frame variable will be populated with:
  *														frame.protectedId - Frame Header Protected ID
  *														frame.data - Frame Response Data
  *														frame.enhancedChecksum - Frame Response Enhanced Checksum
  * @param[in]		slaveResponseWaitTime_ms	Time in ms for which, we as LIN master device, will wait for slave device to transmit whole frame response
  *													Wait will start right after we transmitted frame header
  *													When determining this value you must consider:
  *														Selected LIN bit rate
  *														Frame response data length
  *														How fast is slave device, how quickly after received frame header does it start transmitting frame response
  *													This operation is blocking, so it's recommend that the wait time is not too large
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Subscribe(AlxLin* me, AlxLin_Frame* frame, uint16_t slaveResponseWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == true);
	ALX_LIN_ASSERT((0 <= frame->id) && (frame->id <= 0x3F));
	ALX_LIN_ASSERT((0 < frame->dataLen) && (frame->dataLen <= ALX_LIN_FRAME_DATA_LEN_MAX));


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(frame->id);
	uint8_t txFrame[2] = {};									// SYNC + Protected ID
	uint8_t rxFrame[3 + ALX_LIN_FRAME_DATA_LEN_MAX + 1] = {};	// Break + SYNC + Protected ID + Data + Enhanced Checksum


	//------------------------------------------------------------------------------
	// Transmit Frame Header
	//------------------------------------------------------------------------------

	// Prepare
	// Break					// Frame Header Break - Send by AlxSerialPort_Write
	txFrame[0] = 0x55;			// Frame Header SYNC
	txFrame[1] = protectedId;	// Frame Header Protected ID
	uint8_t txFrameLen = 2;		// SYNC + Protected ID

	// Flush
	AlxSerialPort_FlushTxFifo(me->alxSerialPort);
	AlxSerialPort_FlushRxFifo(me->alxSerialPort);

	// Transmit
	status = AlxSerialPort_Write(me->alxSerialPort, txFrame, txFrameLen);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Write() status %ld", status);
		return status;
	}


	//------------------------------------------------------------------------------
	// Receive Frame Response
	//------------------------------------------------------------------------------

	// Wait for slave to transmit whole frame response
	#if defined(ALX_FREE_RTOS)
	AlxOsDelay_ms(&alxOsDelay, slaveResponseWaitTime_ms);
	#else
	AlxDelay_ms(slaveResponseWaitTime_ms);
	#endif

	// Check RX frame length
	uint32_t rxFrameLen_Actual = AlxSerialPort_GetRxFifoNumOfEntries(me->alxSerialPort);
	uint32_t rxFrameLen_Expected = me->breakSyncOffset + 1 + frame->dataLen + 1;	// *Break + *SYNC + Protected ID + Data + Enhanced Checksum
	if (rxFrameLen_Actual != rxFrameLen_Expected)
	{
		ALX_LIN_TRACE_WRN("FAIL: CheckRxFrameLen() rxFrameLen_Actual %lu rxFrameLen_Expected %lu", rxFrameLen_Actual, rxFrameLen_Expected);
		return Alx_Err;
	}

	// Read RX frame
	status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen_Actual);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Read() status %ld", status);
		return status;
	}

	// Check protected ID
	uint8_t protectedId_Actual = rxFrame[me->breakSyncOffset];
	uint8_t protectedId_Expected = protectedId;
	if (protectedId_Actual != protectedId_Expected)
	{
		ALX_LIN_TRACE_WRN("FAIL: CheckProtectedId() protectedId_Actual %u protectedId_Expected %u", protectedId_Actual, protectedId_Expected);
		return Alx_Err;
	}

	// Check enhanced checksum
	uint8_t enhancedChecksum_Actual = rxFrame[me->breakSyncOffset + 1 + frame->dataLen];
	uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId, &rxFrame[me->breakSyncOffset + 1], frame->dataLen);
	if (enhancedChecksum_Actual != enhancedChecksum_Expected)
	{
		ALX_LIN_TRACE_WRN("FAIL: CheckEnhancedChecksum() enhancedChecksum_Actual %u enhancedChecksum_Expected %u", enhancedChecksum_Actual, enhancedChecksum_Expected);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Return frame
	frame->protectedId = protectedId_Actual;
	memcpy(frame->data, &rxFrame[me->breakSyncOffset + 1], frame->dataLen);
	frame->enhancedChecksum = enhancedChecksum_Actual;

	// Return
	return Alx_Ok;
}

Alx_Status AlxLin_Master_SubscribeViaCallback(AlxLin* me, AlxLin_Frame frame)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == true);
	ALX_LIN_ASSERT((0 <= frame.id) && (frame.id <= 0x3F));
	ALX_LIN_ASSERT((0 < frame.dataLen) && (frame.dataLen <= ALX_LIN_FRAME_DATA_LEN_MAX));


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(frame.id);
	uint8_t txFrame[2] = {};	// SYNC + Protected ID


	//------------------------------------------------------------------------------
	// Transmit Frame Header
	//------------------------------------------------------------------------------

	// Prepare
	// Break					// Frame Header Break - Send by AlxSerialPort_Write
	txFrame[0] = 0x55;			// Frame Header SYNC
	txFrame[1] = protectedId;	// Frame Header Protected ID
	uint8_t txFrameLen = 2;		// SYNC + Protected ID

	// Flush
	AlxSerialPort_FlushTxFifo(me->alxSerialPort);
	AlxLin_RxBuff_Flush(me);

	// Set frame
	me->rxb.frame.id = frame.id;
	me->rxb.frame.protectedId = protectedId;
	me->rxb.frame.dataLen = frame.dataLen;

	// Transmit
	status = AlxSerialPort_Write(me->alxSerialPort, txFrame, txFrameLen);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Write() status %ld", status);
		return status;
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief					As a LIN master device, transmit frame header and frame response to specified ID
  *								This will probably be an ID of LIN slave device
  *								If it's our own ID, then we will transmit frame to ourselves
  *								In LIN terminology when you are transmitting frame response, this is considered PUBLISHING
  *								Alternative name: AlxLin_Master_TxFrameHeader_TxFrameResponse
  * @param[in,out]	me
  * @param[in]		frame	Frame which will be transmitted and shall include:
  *								frame.id - Frame Header ID
  *									ID which will be transmitted in the frame header
  *									Acceptable values: 0x00 .. 0x3F
  *								frame.dataLen - Frame Response Data Length
  *									Transmitted frame response data length
  *									0 < frame.dataLen <= ALX_LIN_BUFF_LEN
  *									If we want that data length is LIN compliant then:
  *										Acceptable values are: 2, 4, 8
  *										Length must be set accordingly to specified ID
  *											If ID = 0x00 .. 0x1F, then 2
  *											If ID = 0x20 .. 0x2F, then 4
  *											If ID = 0x30 .. 0x3F, then 8
  *								frame.data - Frame Response Data
  *									Data which will be transmitted in frame response
  *								Unused and set internally:
  *									frame.protectedId - Frame Header Protected ID
  *									frame.enhancedChecksum - Frame Response Enhanced Checksum
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Master_Publish(AlxLin* me, AlxLin_Frame frame)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == true);
	ALX_LIN_ASSERT((0 <= frame.id) && (frame.id <= 0x3F));
	ALX_LIN_ASSERT((0 < frame.dataLen) && (frame.dataLen <= ALX_LIN_FRAME_DATA_LEN_MAX));


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t protectedId = AlxLin_CalcProtectedId(frame.id);
	uint8_t enhancedChecksum = AlxLin_CalcEnhancedChecksum(protectedId, frame.data, frame.dataLen);
	uint8_t txFrame[2 + ALX_LIN_FRAME_DATA_LEN_MAX + 1] = {};	// SYNC + Protected ID + Data + Enhanced Checksum


	//------------------------------------------------------------------------------
	// Transmit Frame Header + Frame Response
	//------------------------------------------------------------------------------

	// Prepare
	// Break										// Frame Header Break - Send by AlxSerialPort_Write
	txFrame[0] = 0x55;								// Frame Header SYNC
	txFrame[1] = protectedId;						// Frame Header Protected ID
	memcpy(&txFrame[2], frame.data, frame.dataLen);	// Frame Response Data
	txFrame[2 + frame.dataLen] = enhancedChecksum;	// Frame Response Enhanced Checksum
	uint8_t txFrameLen = 2 + frame.dataLen + 1;		// SYNC + Protected ID + Data + Enhanced Checksum

	// Flush
	AlxSerialPort_FlushTxFifo(me->alxSerialPort);

	// Transmit
	status = AlxSerialPort_Write(me->alxSerialPort, txFrame, txFrameLen);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Write() status %ld", status);
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
	ALX_LIN_ASSERT(me->isInit == false);
	ALX_LIN_ASSERT(me->isMaster == false);

	// Init
	Alx_Status status = AlxSerialPort_Init(me->alxSerialPort);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Init() status %ld", status);
		return status;
	}

	// Set info
	me->isInit = true;
	me->isMaster = false;

	// Return
	return Alx_Ok;
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
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == false);

	// DeInit
	Alx_Status status = AlxSerialPort_DeInit(me->alxSerialPort);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_DeInit() status %ld", status);
		return status;
	}

	// Clear info
	me->isInit = false;
	me->isMaster = false;

	// Return
	return Alx_Ok;
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
	ALX_LIN_ASSERT(me->isMaster == false);

	// Return
	return me->isInit;
}

/**
  * @brief													As a LIN slave device, receive frame header or receive frame header & frame response from master device
  *																AlxLin_Slave_RxFrameHeader_RxFrameResponse
  * @param[in,out]	me
  * @param[in,out]	frame									Pointer to frame variable in which data will be received and shall already include:
  *																frame.dataLen - Frame Response Data Length
  *																	Data length which we will try to receive
  *																	0 < frame.dataLen <= ALX_LIN_BUFF_LEN
  *																After successful operation frame variable will be populated with:
  *																	frame.id - Frame Header ID
  *																	frame.protectedId - Frame Header Protected ID
  *																	frame.data - Frame Response Data
  *																	frame.enhancedChecksum - Frame Response Enhanced Checksum
  * @param[in]		timeout_ms								Time in ms for which, we as LIN slave device, will wait after RX FIFO Flush for master device to transmit whole frame response with specified data length
  * @param[in]		rxFifoNumOfEntriesNewCheckWaitTime_ms	Time in ms for which we will wait for new RX FIFO number of entries check
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLin_Slave_Subscribe(AlxLin* me, AlxLin_Frame* frame, uint16_t timeout_ms, uint16_t rxFifoNumOfEntriesNewCheckWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);
	ALX_LIN_ASSERT(me->isMaster == false);
	ALX_LIN_ASSERT((0 < frame->dataLen) && (frame->dataLen <= ALX_LIN_FRAME_DATA_LEN_MAX));


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint8_t rxFrame[3 + ALX_LIN_FRAME_DATA_LEN_MAX + 1] = {};	// Break + SYNC + Protected ID + Data + Enhanced Checksum


	//------------------------------------------------------------------------------
	// Receive Frame Header + Frame Response
	//------------------------------------------------------------------------------

	// Flush
	AlxSerialPort_FlushRxFifo(me->alxSerialPort);

	// Start timer
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);
	AlxTimSw_Start(&alxTimSw);

	// Wait for master to transmit whole frame response with specified data length
	uint32_t rxFrameLen_Actual = 0;
	while (1)
	{
		// Check RX frame length
		rxFrameLen_Actual = AlxSerialPort_GetRxFifoNumOfEntries(me->alxSerialPort);
		uint32_t rxFrameLen_Expected = me->breakSyncOffset + 1 + frame->dataLen + 1;	// *Break + *SYNC + Protected ID + Data + Enhanced Checksum
		if (rxFrameLen_Actual == rxFrameLen_Expected)
		{
			status = Alx_Ok;
			break;
		}

		// Wait
		#if defined(ALX_FREE_RTOS)
		AlxOsDelay_ms(&alxOsDelay, rxFifoNumOfEntriesNewCheckWaitTime_ms);
		#else
		AlxDelay_ms(rxFifoNumOfEntriesNewCheckWaitTime_ms);
		#endif

		// Check RX frame length too large
		if (rxFrameLen_Actual > rxFrameLen_Expected)
		{
			ALX_LIN_TRACE_WRN("FAIL: CheckRxFrameLenTooLarge() rxFrameLen_Actual %lu rxFrameLen_Expected %lu", rxFrameLen_Actual, rxFrameLen_Expected);
			return Alx_Err;
		}

		// Check timeout
		if (AlxTimSw_IsTimeout_ms(&alxTimSw, timeout_ms))
		{
			ALX_LIN_TRACE_WRN("FAIL: AlxTimSw_IsTimeout_ms()");
			return Alx_Err;
		}
	}

	// Read RX frame
	status = AlxSerialPort_Read(me->alxSerialPort, rxFrame, rxFrameLen_Actual);
	if (status != Alx_Ok)
	{
		ALX_LIN_TRACE_WRN("FAIL: AlxSerialPort_Read() status %ld", status);
		return Alx_Err;
	}

	// Check protected ID
	uint8_t protectedId_Actual = rxFrame[me->breakSyncOffset];
	uint8_t id_Actual = protectedId_Actual & 0x3F;
	uint8_t protectedId_Expected = AlxLin_CalcProtectedId(id_Actual);
	if (protectedId_Actual != protectedId_Expected)
	{
		ALX_LIN_TRACE_WRN("FAIL: CheckProtectedId() protectedId_Actual %u protectedId_Expected %u", protectedId_Actual, protectedId_Expected);
		return Alx_Err;
	}

	// Check enhanced checksum
	uint8_t enhancedChecksum_Actual = rxFrame[me->breakSyncOffset + 1 + frame->dataLen];
	uint8_t enhancedChecksum_Expected = AlxLin_CalcEnhancedChecksum(protectedId_Actual, &rxFrame[me->breakSyncOffset + 1], frame->dataLen);
	if (enhancedChecksum_Actual != enhancedChecksum_Expected)
	{
		ALX_LIN_TRACE_WRN("FAIL: CheckEnhancedChecksum() enhancedChecksum_Actual %u enhancedChecksum_Expected %u", enhancedChecksum_Actual, enhancedChecksum_Expected);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Return frame
	frame->id = id_Actual;
	frame->protectedId = protectedId_Actual;
	memcpy(frame->data, &rxFrame[me->breakSyncOffset + 1], frame->dataLen);
	frame->enhancedChecksum = enhancedChecksum_Actual;

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
	if (me->rxb.i == me->breakSyncOffset)
	{
		if (me->isMaster)
		{
			// Set frame - Already set by AlxLin_Master_SubscribeViaCallback
			// me->rxb.frame.id
			// me->rxb.frame.protectedId
			// me->rxb.frame.dataLen

			// Check protected ID
			uint8_t protectedId_Actual = me->rxb.buff[me->breakSyncOffset];
			uint8_t protectedId_Expected = me->rxb.frame.protectedId;
			if (protectedId_Actual != protectedId_Expected)
			{
				ALX_LIN_TRACE_DBG("FAIL: CheckProtectedId() protectedId_Actual %u protectedId_Expected %u", protectedId_Actual, protectedId_Expected);
				me->rxb.active = false;
				return;
			}
		}
		else
		{
			// Check protected ID
			uint8_t protectedId_Actual = me->rxb.buff[me->breakSyncOffset];
			uint8_t id_Actual = protectedId_Actual & 0x3F;
			uint8_t protectedId_Expected = AlxLin_CalcProtectedId(id_Actual);
			if (protectedId_Actual != protectedId_Expected)
			{
				ALX_LIN_TRACE_DBG("FAIL: CheckProtectedId() protectedId_Actual %u protectedId_Expected %u", protectedId_Actual, protectedId_Expected);
				me->rxb.active = false;
				return;
			}

			// Get slave frame config from ID
			AlxLin_SlaveFrameConfig slaveFrameConfig = {};
			Alx_Status status = AlxLin_GetSlaveFrameConfigFromId(me, id_Actual, &slaveFrameConfig);
			if (status != Alx_Ok)
			{
				ALX_LIN_TRACE_DBG("FAIL: AlxLin_GetSlaveFrameConfigFromId() status %ld id_Actual %u", status, id_Actual);
				me->rxb.active = false;
				return;
			}

			// Set frame
			me->rxb.frame.id = id_Actual;
			me->rxb.frame.protectedId = protectedId_Actual;
			me->rxb.frame.dataLen = slaveFrameConfig.dataLen;

			// Set slave frame config
			me->rxb.slaveFrameConfig = slaveFrameConfig;


			//------------------------------------------------------------------------------
			// Handle Slave Publish
			//------------------------------------------------------------------------------
			if (me->rxb.slaveFrameConfig.publish)
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
	}


	//------------------------------------------------------------------------------
	// Handle Data
	//------------------------------------------------------------------------------
	else if (me->rxb.i == me->breakSyncOffset + me->rxb.frame.dataLen)
	{
		for (uint8_t i = 0; i < me->rxb.frame.dataLen; i++)
		{
			me->rxb.frame.data[i] = me->rxb.buff[me->breakSyncOffset + 1 + i];
		}
	}


	//------------------------------------------------------------------------------
	// Handle Enhanced Checksum
	//------------------------------------------------------------------------------
	else if (me->rxb.i == me->breakSyncOffset + 1 + me->rxb.frame.dataLen)
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
		if (me->isMaster)
		{
			AlxLin_Master_Subscribe_Callback(me, me->rxb.frame);
		}
		else
		{
			AlxLin_Slave_Subscribe_Callback(me, me->rxb.frame);
		}
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
static Alx_Status AlxLin_GetSlaveFrameConfigFromId(AlxLin* me, uint8_t id, AlxLin_SlaveFrameConfig* slaveFrameConfig)
{
	// Get
	for (uint32_t i = 0; i < me->slaveFrameConfigArrLen; i++)
	{
		if (me->slaveFrameConfigArr[i].id == id)
		{
			*slaveFrameConfig = me->slaveFrameConfigArr[i];
			return Alx_Ok;
		}
	}

	// Return
	return Alx_Err;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxLin_Master_Subscribe_Callback(AlxLin* me, AlxLin_Frame frame)
{
	(void)me;
	(void)frame;
}
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
