/**
  ******************************************************************************
  * @file		alxCan_McuStm32.c
  * @brief		Auralix C Library - ALX CAN MCU STM32 Module
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
#include "alxCan_McuStm32.h"
#include "alxCan.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (((defined(ALX_STM32F4) || defined(ALX_STM32L4)) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED)))


//******************************************************************************
// Private Variables
//******************************************************************************
static AlxCan* alxCan_Can1 = NULL;
static AlxCan* alxCan_Can2 = NULL;
#if defined(ALX_STM32G4)
static AlxCan* alxCan_Can3 = NULL;
#endif


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxCan_TxMsg_TryAddToHwMailbox(AlxCan* me);
static void AlxCan_RxMsg_TryReadFromHwFifo(AlxCan* me);
#if defined(ALX_STM32G4)
static uint32_t AlxCan_GetDataLenCode(uint8_t len);
static uint8_t AlxCan_GetDataLen(uint32_t lenCode);
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxCan_Ctor_IsClkOk(AlxCan* me);
static void AlxCan_Periph_EnableClk(AlxCan* me);
static void AlxCan_Periph_DisableClk(AlxCan* me);
static void AlxCan_Periph_ForceReset(AlxCan* me);
static void AlxCan_Periph_ReleaseReset(AlxCan* me);
static void AlxCan_Periph_EnableIrq(AlxCan* me);
static void AlxCan_Periph_DisableIrq(AlxCan* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		can
  * @param[in]		do_CAN_TX
  * @param[in]		di_CAN_RX
  * @param[in]		clk
  * @param[in]		canClk
  * @param[in]		txFifoBuff
  * @param[in]		txFifoBuffLen
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		txIrqPriority
  * @param[in]		rxIrqPriority
  */
void AlxCan_Ctor
(
	AlxCan* me,
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	CAN_TypeDef* can,
	#endif
	#if defined(ALX_STM32G4)
	FDCAN_GlobalTypeDef* can,
	#endif
	AlxIoPin* do_CAN_TX,
	AlxIoPin* di_CAN_RX,
	AlxClk* clk,
	AlxCan_Clk canClk,
	uint8_t* txFifoBuff,
	uint32_t txFifoBuffLen,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority txIrqPriority,
	Alx_IrqPriority rxIrqPriority
)
{
	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->hcan.Instance = can;
	me->do_CAN_TX = do_CAN_TX;
	me->di_CAN_RX = di_CAN_RX;
	me->clk = clk;
	me->canClk = canClk;
	me->txFifoBuff = txFifoBuff;
	me->txFifoBuffLen = txFifoBuffLen;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->txIrqPriority = txIrqPriority;
	me->rxIrqPriority = rxIrqPriority;
	ALX_CAN_ASSERT(AlxCan_Ctor_IsClkOk(me));


	//------------------------------------------------------------------------------
	// Private Variables
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
		#if defined(CAN1)
		if (me->hcan.Instance == CAN1)			{ alxCan_Can1 = me; }
		#endif
		#if defined(CAN2)
		else if (me->hcan.Instance == CAN2)		{ alxCan_Can2 = me; }
		#endif
		else									{ ALX_CAN_ASSERT(false); }	// We should never get here
	#endif
	#if defined(ALX_STM32G4)
		#if defined(FDCAN1)
		if(me->hcan.Instance == FDCAN1)			{ alxCan_Can1 = me; }
		#endif
		#if defined(FDCAN2)
		else if (me->hcan.Instance == FDCAN2)	{ alxCan_Can2 = me; }
		ALX_CAN_ASSERT(false);	// Only FDCAN1 is supported
		#endif
		#if defined(FDCAN3)
		else if(me->hcan.Instance == FDCAN3)	{ alxCan_Can2 = me; }
		ALX_CAN_ASSERT(false);	// Only FDCAN1 is supported
		#endif
		else									{ ALX_CAN_ASSERT(false); }	// We should never get here
	#endif


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------
	AlxFifo_Ctor(&me->txFifo, txFifoBuff, txFifoBuffLen);
	AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	me->canRxFifo = CAN_RX_FIFO0;
	#endif
	#if defined(ALX_STM32G4)
	me->canRxFifo = FDCAN_RX_FIFO0;
	#endif


	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	me->hcan.Init.Mode = CAN_MODE_NORMAL;
	me->hcan.Init.TimeTriggeredMode = DISABLE;
	me->hcan.Init.AutoBusOff = DISABLE;
	me->hcan.Init.AutoWakeUp = DISABLE;
	me->hcan.Init.AutoRetransmission = ENABLE;
	me->hcan.Init.ReceiveFifoLocked = DISABLE;
	me->hcan.Init.TransmitFifoPriority = DISABLE;

	#if defined(ALX_STM32F4)
	if(me->canClk == AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz)
	{
		// Used calculator: http://www.bittiming.can-wiki.info/#STMicro, Target SamplePoint = 87.5%, Actual BitRate = 250kbps +/-0%, Actual SamplePoint = 88.9%, SJW = 1
		me->hcan.Init.Prescaler = 10;
		me->hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
		me->hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
		me->hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
	}
	else { ALX_CAN_ASSERT(false); return; } // We shouldn't get here
	#endif
	#if defined(ALX_STM32L4)
	if(me->canClk == AlxCan_Clk_McuStm32L4_CanClk_250kbps_Pclk1Apb1_80MHz)
	{
		// Used calculator: http://www.bittiming.can-wiki.info/#STMicro, Target SamplePoint = 87.5%, Actual BitRate = 250kbps +/-0%, Actual SamplePoint = 87.5%, SJW = 1
		me->hcan.Init.Prescaler = 20;
		me->hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
		me->hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
		me->hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
	}
	else { ALX_CAN_ASSERT(false); return; } // We shouldn't get here
	#endif

	me->fcan.FilterIdHigh = 0x00000000;
	me->fcan.FilterIdLow = 0x00000000;
	me->fcan.FilterMaskIdHigh = 0x00000000;
	me->fcan.FilterMaskIdLow = 0x00000000;
	me->fcan.FilterFIFOAssignment = me->canRxFifo;
	me->fcan.FilterBank = 0;
	me->fcan.FilterMode = CAN_FILTERMODE_IDMASK;
	me->fcan.FilterScale = CAN_FILTERSCALE_32BIT;
	me->fcan.FilterActivation = ENABLE;
	me->fcan.SlaveStartFilterBank = 14;
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	me->periphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
	me->periphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;

	me->hcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;			// JK: Only non FD option
	me->hcan.Init.Mode = FDCAN_MODE_NORMAL;						// JK: Other options: Restricted Operation mode ,Bus Monitoring mode,Internal LoopBack mode,External LoopBack mode
	me->hcan.Init.AutoRetransmission = ENABLE;					// JK: It keeps sending messages if not acknowledged
	me->hcan.Init.TransmitPause = DISABLE;						// JK: Tx pauses for 2 bits, to avoid "babbling idiot failure"
	me->hcan.Init.ProtocolException = DISABLE;					// JK: Can FD optional function
	me->hcan.Init.StdFiltersNbr = 1;							// JK: Specifies number of standard ID filters
	me->hcan.Init.ExtFiltersNbr = 0;							// JK: Specifies number of extended ID filters
	me->hcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;	// JK: FIFO mode, other option is QUEUE mode

	if(me->canClk == AlxCan_Clk_McuStm32G4_CanClk_250kbps_Pclk1Apb1_170MHz)
	{
		// Used calculator: http://www.bittiming.can-wiki.info/#STMicro, Target SamplePoint = 87.5%, Actual BitRate = 250kbps +/-0%, Actual SamplePoint = 88.2%, SJW = 1
		me->hcan.Init.ClockDivider = FDCAN_CLOCK_DIV1;
		me->hcan.Init.NominalSyncJumpWidth = 4;
		me->hcan.Init.NominalPrescaler = 40;
		me->hcan.Init.NominalTimeSeg1 = 14;
		me->hcan.Init.NominalTimeSeg2 = 2;
		me->hcan.Init.DataSyncJumpWidth = ALX_NULL;
		me->hcan.Init.DataPrescaler = ALX_NULL;
		me->hcan.Init.DataTimeSeg1 = ALX_NULL;
		me->hcan.Init.DataTimeSeg2 = ALX_NULL;
	}
	else { ALX_CAN_ASSERT(false); return; } // We shouldn't get here

	me->fcan.IdType = FDCAN_STANDARD_ID;
	me->fcan.FilterIndex = 0;
	me->fcan.FilterType = FDCAN_FILTER_RANGE;
	me->fcan.FilterConfig = FDCAN_FILTER_DISABLE;
	me->fcan.FilterID1 = 0x000;
	me->fcan.FilterID2 = 0x7FF;
	#endif


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->isInit = false;
	me->wasCtorCalled = true;
	me->isErr = false;
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
Alx_Status AlxCan_Init(AlxCan* me)
{
	// Assert
	ALX_CAN_ASSERT(me->isInit == false);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	// Flush TX & RX FIFO
	AlxFifo_Flush(&me->txFifo);
	AlxFifo_Flush(&me->rxFifo);

	// Init GPIO
	AlxIoPin_Init(me->do_CAN_TX);
	AlxIoPin_Init(me->di_CAN_RX);

	// Release CAN Periphery Reset
	AlxCan_Periph_ReleaseReset(me);

	// Enable CAN Periphery Clock
	AlxCan_Periph_EnableClk(me);
	#if defined(ALX_STM32G4)
	if(HAL_RCCEx_PeriphCLKConfig(&me->periphClkInit) != HAL_OK)	{ ALX_CAN_TRACE("Err"); };
	#endif

	// Init CAN
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	if(HAL_CAN_Init(&me->hcan) != HAL_OK)						{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	if(HAL_CAN_ConfigFilter(&me->hcan, &me->fcan) != HAL_OK)	{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	if(HAL_CAN_Start(&me->hcan) != HAL_OK)						{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_Init(&me->hcan) != HAL_OK)																												{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	if(HAL_FDCAN_ConfigFilter(&me->hcan, &me->fcan) != HAL_OK)																							{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	if(HAL_FDCAN_ConfigGlobalFilter(&me->hcan, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)	{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	if(HAL_FDCAN_Start(&me->hcan) != HAL_OK)																											{ ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif

	// Enable CAN TX & RX IRQ
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	if(HAL_CAN_ActivateNotification(&me->hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_ActivateNotification(&me->hcan, FDCAN_IT_TX_FIFO_EMPTY | FDCAN_IT_RX_FIFO0_NEW_MESSAGE, ALX_NULL) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif

	// Enable CAN TX & RX IRQ
	AlxCan_Periph_EnableIrq(me);

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
Alx_Status AlxCan_DeInit(AlxCan* me)
{
	// Assert
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	// Disable CAN TX & RX IRQ
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	AlxCan_Periph_DisableIrq(me);
	if(HAL_CAN_DeactivateNotification(&me->hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	AlxCan_Periph_DisableIrq(me);
	if (HAL_FDCAN_DeactivateNotification(&me->hcan, FDCAN_IT_TX_FIFO_EMPTY | FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif

	// DeInit CAN
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	if(HAL_CAN_DeInit(&me->hcan) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_DeInit(&me->hcan) != HAL_OK) { ALX_CAN_TRACE("Err"); return Alx_Err; };
	#endif

	// Force CAN Periphery Reset
	AlxCan_Periph_ForceReset(me);

	// Disable CAN Periphery Clock
	AlxCan_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_CAN_TX);
	AlxIoPin_DeInit(me->di_CAN_RX);

	// Flush TX & RX FIFO
	AlxFifo_Flush(&me->txFifo);
	AlxFifo_Flush(&me->rxFifo);

	// Clear isErr
	me->isErr = false;

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxCan_ReInit(AlxCan* me)
{
	// ReInit
	if(AlxCan_DeInit(me) != Alx_Ok) return Alx_Err;
	if(AlxCan_Init(me) != Alx_Ok) return Alx_Err;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		msg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxCan_TxMsg(AlxCan* me, AlxCan_Msg msg)
{
	return AlxCan_TxMsgMulti(me, &msg, 1);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		msg
  * @param[in]		numOfMsg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxCan_TxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg)
{
	// Assert
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);
	ALX_CAN_ASSERT((0 < numOfMsg) && (numOfMsg <= me->txFifoBuffLen));

	// Local variables
	Alx_Status status = Alx_Err;

	// Add TX messages to ALX TX FIFO
	for (uint32_t i = 0; i < numOfMsg; i++)
	{
		uint32_t msgLen = sizeof(AlxCan_Msg);
		uint8_t* ptr = (uint8_t*)&msg[i * msgLen];

		AlxGlobal_DisableIrq();
		status = AlxFifo_WriteMulti(&me->txFifo, ptr, msgLen);
		AlxGlobal_EnableIrq();

		if(status == AlxFifo_ErrFull)
		{
			break;	// We break if ALX TX FIFO is full
		}
		else if(status != Alx_Ok)
		{
			ALX_CAN_ASSERT(false);	// We should never get here
			break;
		}
	}

	// Try to add TX messages to HW mailbox
	AlxGlobal_DisableIrq();
	AlxCan_TxMsg_TryAddToHwMailbox(me);
	AlxGlobal_EnableIrq();

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		msg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxCan_RxMsg(AlxCan* me, AlxCan_Msg* msg)
{
	return AlxCan_RxMsgMulti(me, msg, 1);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		msg
  * @param[in]		numOfMsg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxCan_RxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg)
{
	// Assert
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);
	ALX_CAN_ASSERT((0 < numOfMsg) && (numOfMsg <= me->rxFifoBuffLen));

	// Local variables
	Alx_Status status = Alx_Err;

	// Read RX messages from ALX RX FIFO
	for (uint32_t i = 0; i < numOfMsg; i++)
	{
		uint32_t msgLen = sizeof(AlxCan_Msg);
		uint8_t* ptr = (uint8_t*)&msg[i * msgLen];

		AlxGlobal_DisableIrq();
		status = AlxFifo_Read(&me->rxFifo, ptr, msgLen);
		AlxGlobal_EnableIrq();

		if(status == AlxFifo_ErrEmpty)
		{
			break;	// We break if there are no more messages in ALX RX FIFO
		}
		else if(status != Alx_Ok)
		{
			ALX_CAN_ASSERT(false);	// We should never get here
			break;
		}
	}

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxCan_IsErr(AlxCan* me)
{
	// Assert
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	// Check if bus is in bus off state and set isErr accordingly
	#if defined(ALX_STM32G4)
	FDCAN_ProtocolStatusTypeDef protocolStatus;
	HAL_StatusTypeDef status = HAL_FDCAN_GetProtocolStatus(&me->hcan, &protocolStatus);
	if ((status != HAL_OK) || protocolStatus.BusOff)
		me->isErr = true;
	else
		me->isErr = false;
	#endif

	// Return
	return me->isErr;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxCan_IrqHandler(AlxCan* me)
{
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	HAL_CAN_IRQHandler(&me->hcan);
	#endif
	#if defined(ALX_STM32G4)
	HAL_FDCAN_IRQHandler(&me->hcan);
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxCan_TxMsg_TryAddToHwMailbox(AlxCan* me)
{
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	while(HAL_CAN_GetTxMailboxesFreeLevel(&me->hcan) != 0)	// Add TX messages to HW mailboxes until all HW mailboxes are full
	{
		// Local variables
		AlxCan_Msg alxTxMsg;
		CAN_TxHeaderTypeDef halTxMsg;

		// Get TX message from ALX TX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_Read(&me->txFifo, (uint8_t*)&alxTxMsg, sizeof(AlxCan_Msg));
		if(alxStatus == AlxFifo_ErrEmpty)
		{
			return;	// ALX TX FIFO is empty, we don't have anything to send, so we return
		}
		else if(alxStatus != Alx_Ok)
		{
			ALX_CAN_ASSERT(false);	// We should never get here
			return;
		}

		// Prepare TX message
		{
			// Set ID
			if(alxTxMsg.isExtendedId)
			{
				halTxMsg.StdId = 0;
				halTxMsg.ExtId = alxTxMsg.id;
				halTxMsg.IDE = CAN_ID_EXT;
			}
			else // Standard ID
			{
				halTxMsg.StdId = alxTxMsg.id;	// TV: TODO, assert if above 2048
				halTxMsg.ExtId = 0;
				halTxMsg.IDE = CAN_ID_STD;
			}

			// Set Frame Type
			if(alxTxMsg.isDataFrame)
				halTxMsg.RTR = CAN_RTR_DATA;
			else // Remote Frame
				halTxMsg.RTR = CAN_RTR_REMOTE;

			// Set Other
			halTxMsg.DLC = alxTxMsg.dataLen;
			halTxMsg.TransmitGlobalTime = DISABLE;
		}

		// Add TX message to HW mailbox
		uint32_t txMailbox = 0;
		HAL_StatusTypeDef halStatus = HAL_CAN_AddTxMessage(&me->hcan, &halTxMsg, alxTxMsg.data, &txMailbox);
		if (halStatus != HAL_OK)
		{
			me->isErr = true;	// HAL error, module goes to error state
			return;
		}
	}
	#endif
	#if defined(ALX_STM32G4)
	while(HAL_FDCAN_GetTxFifoFreeLevel(&me->hcan) != 0)	// Add TX messages to CAN TX FIFO until it's full
	{
		// Local variables
		AlxCan_Msg alxTxMsg;
		FDCAN_TxHeaderTypeDef halTxMsg;

		// Get TX message from ALX TX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_Read(&me->txFifo, (uint8_t*)&alxTxMsg, sizeof(AlxCan_Msg));
		if (alxStatus == AlxFifo_ErrEmpty)
		{
			return;	// ALX TX FIFO is empty, we don't have anything to send, so we return
		}
		else if (alxStatus != Alx_Ok)
		{
			ALX_CAN_ASSERT(false); 	// We should never get here
			return;
		}

		// Prepare TX message
		{
			// Set ID
			if(alxTxMsg.isExtendedId)
			{
				halTxMsg.IdType = FDCAN_EXTENDED_ID;
				halTxMsg.Identifier = alxTxMsg.id;		// JK: Add ASSERT if out of range: 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
			}
			else // Standard ID
			{
				halTxMsg.IdType = FDCAN_STANDARD_ID;
				halTxMsg.Identifier = alxTxMsg.id;		// JK: Add ASSERT if out of range: 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID
			}

			// Set Frame Type
			if(alxTxMsg.isDataFrame)
				halTxMsg.TxFrameType = FDCAN_DATA_FRAME;
			else // Remote Frame
				halTxMsg.TxFrameType = FDCAN_REMOTE_FRAME;

			// Set data length
			halTxMsg.DataLength = AlxCan_GetDataLenCode(alxTxMsg.dataLen);

			// Set Other
			halTxMsg.ErrorStateIndicator = FDCAN_ESI_PASSIVE;		// JK: I dont understand this	????
			halTxMsg.BitRateSwitch = FDCAN_BRS_OFF;					// JK: Bit rate switch should be off - function of CAN FD
			halTxMsg.FDFormat = FDCAN_CLASSIC_CAN;					// JK: We use classic CAN and NOT CAN FD
			halTxMsg.TxEventFifoControl = FDCAN_NO_TX_EVENTS;		// JK: I dont understand this	????
			halTxMsg.MessageMarker = 0;								// JK: I dont understand this	????
		}

		// Add TX message to HW fifo
		HAL_StatusTypeDef halStatus = HAL_FDCAN_AddMessageToTxFifoQ(&me->hcan, &halTxMsg, alxTxMsg.data);
		if (halStatus != HAL_OK)
		{
			me->isErr = true; 	// HAL error, module goes to error state
			return;
		}
	}
	#endif
}
static void AlxCan_RxMsg_TryReadFromHwFifo(AlxCan* me)
{
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	while(HAL_CAN_GetRxFifoFillLevel(&me->hcan, me->canRxFifo) != 0)	// Read RX messages from HW RX FIFO until it is empty
	{
		// Local variables
		AlxCan_Msg alxRxMsg;
		CAN_RxHeaderTypeDef halRxMsg;

		// Get RX message from HW RX FIFO
		HAL_StatusTypeDef halStatus = HAL_CAN_GetRxMessage(&me->hcan, me->canRxFifo, &halRxMsg, alxRxMsg.data);
		if (halStatus != HAL_OK)
		{
			me->isErr = true;	// HAL error, module goes to error state
			return;
		}

		// Prepare RX message
		{
			// Set ID
			if (halRxMsg.IDE == CAN_ID_EXT)
			{
				alxRxMsg.id = halRxMsg.ExtId;
				alxRxMsg.isExtendedId = true;
			}
			else // Standard ID
			{
				alxRxMsg.id = halRxMsg.StdId;
				alxRxMsg.isExtendedId = false;
			}

			// Set Frame Type
			if(halRxMsg.RTR == CAN_RTR_DATA)
				alxRxMsg.isDataFrame = true;
			else // Remote Frame
				alxRxMsg.isDataFrame = false;

			// Set Other
			alxRxMsg.dataLen = halRxMsg.DLC;
		}

		// Add RX message to ALX RX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_WriteMulti(&me->rxFifo, (uint8_t*)&alxRxMsg, sizeof(AlxCan_Msg));
		if (alxStatus == AlxFifo_ErrFull)
		{
			// TV: TODO, decide if we will handle ALX RX FIFO overflow as error, or we will discard overflow data..
			// For now we will discard additional overflow data..
			//me->isErr = true;	// ALX FIFO RX is full, we will handle this the same way as HAL error, module goes to error state
			return;
		}
		else if (alxStatus != Alx_Ok)
		{
			ALX_CAN_ASSERT(false);	// We should never get here
			return;
		}
	}
	#endif
	#if defined(ALX_STM32G4)
	while(HAL_FDCAN_GetRxFifoFillLevel(&me->hcan, me->canRxFifo) != 0)	// Read RX messages from HW RX FIFO until it is empty
	{
		// Local variables
		AlxCan_Msg alxRxMsg;
		FDCAN_RxHeaderTypeDef halRxMsg;

		// Get RX message from HW RX FIFO
		HAL_StatusTypeDef halStatus = HAL_FDCAN_GetRxMessage(&me->hcan, me->canRxFifo, &halRxMsg, alxRxMsg.data);
		if (halStatus != HAL_OK)
		{
			me->isErr = true; 	// HAL error, module goes to error state
			return;
		}

		// Prepare RX message
		{
			// Set ID
			if(halRxMsg.IdType == FDCAN_EXTENDED_ID)
			{
				alxRxMsg.id = halRxMsg.Identifier;
				alxRxMsg.isExtendedId = true;
			}
			else // Standard ID
			{
				alxRxMsg.id = halRxMsg.Identifier;
				alxRxMsg.isExtendedId = false;
			}

			// Set Frame Type
			if(halRxMsg.RxFrameType == FDCAN_DATA_FRAME)
				alxRxMsg.isDataFrame = true;
			else // Remote Frame
				alxRxMsg.isDataFrame = false;

			// Set Lenght
			alxRxMsg.dataLen = AlxCan_GetDataLen(halRxMsg.DataLength);
		}

		// Add RX message to ALX RX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_WriteMulti(&me->rxFifo, (uint8_t*)&alxRxMsg, sizeof(AlxCan_Msg));
		if (alxStatus == AlxFifo_ErrFull)
		{
			// TV: TODO, decide if we will handle ALX RX FIFO overflow as error, or we will discard overflow data..
			// For now we will discard additional overflow data..
			//me->isErr = true;	// ALX FIFO RX is full, we will handle this the same way as HAL error, module goes to error state
			return;
		}
		else if (alxStatus != Alx_Ok)
		{
			ALX_CAN_ASSERT(false); 	// We should never get here
			return;
		}
	}
	#endif
}
#if defined(ALX_STM32G4)
static uint32_t AlxCan_GetDataLenCode(uint8_t dataLen)
{
	if (dataLen == 0) return FDCAN_DLC_BYTES_0;
	if (dataLen == 1) return FDCAN_DLC_BYTES_1;
	if (dataLen == 2) return FDCAN_DLC_BYTES_2;
	if (dataLen == 3) return FDCAN_DLC_BYTES_3;
	if (dataLen == 4) return FDCAN_DLC_BYTES_4;
	if (dataLen == 5) return FDCAN_DLC_BYTES_5;
	if (dataLen == 6) return FDCAN_DLC_BYTES_6;
	if (dataLen == 7) return FDCAN_DLC_BYTES_7;
	if (dataLen == 8) return FDCAN_DLC_BYTES_8;

	ALX_CAN_ASSERT(false); 	// Messages longer than 8 bytes are no supported
	return 0;
}
static uint8_t AlxCan_GetDataLen(uint32_t dataLenCode)
{
	if (dataLenCode == FDCAN_DLC_BYTES_0) return 0;
	if (dataLenCode == FDCAN_DLC_BYTES_1) return 1;
	if (dataLenCode == FDCAN_DLC_BYTES_2) return 2;
	if (dataLenCode == FDCAN_DLC_BYTES_3) return 3;
	if (dataLenCode == FDCAN_DLC_BYTES_4) return 4;
	if (dataLenCode == FDCAN_DLC_BYTES_5) return 5;
	if (dataLenCode == FDCAN_DLC_BYTES_6) return 6;
	if (dataLenCode == FDCAN_DLC_BYTES_7) return 7;
	if (dataLenCode == FDCAN_DLC_BYTES_8) return 8;

	ALX_CAN_ASSERT(false); 	// Messages longer than 8 bytes are no supported
	return 0;
}
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxCan_Ctor_IsClkOk(AlxCan* me)
{
	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(me->canClk == AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz)
	{
		if(45000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if (me->canClk == AlxCan_Clk_McuStm32G4_CanClk_250kbps_Pclk1Apb1_170MHz)
	{
		if (170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if(me->canClk == AlxCan_Clk_McuStm32L4_CanClk_250kbps_Pclk1Apb1_80MHz)
	{
		if(80000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_CAN_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxCan_Periph_EnableClk(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_CLK_ENABLE(); return; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_CLK_ENABLE(); return; }
	#endif
	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_CLK_ENABLE(); return;
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}
static void AlxCan_Periph_DisableClk(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_CLK_DISABLE(); return; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_CLK_DISABLE(); return; }
	#endif
	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_CLK_DISABLE(); return;
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}
static void AlxCan_Periph_ForceReset(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_FORCE_RESET(); return; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_FORCE_RESET(); return; }
	#endif
	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	 __HAL_RCC_FDCAN_FORCE_RESET(); return;
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}
static void AlxCan_Periph_ReleaseReset(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_RELEASE_RESET(); return; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_RELEASE_RESET(); return; }
	#endif
	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_RELEASE_RESET(); return;
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}
static void AlxCan_Periph_EnableIrq(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)
	{
		HAL_NVIC_SetPriority(CAN1_TX_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, me->rxIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
		return;
	}
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)
	{
		HAL_NVIC_SetPriority(CAN2_TX_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
		HAL_NVIC_SetPriority(CAN2_RX0_IRQn, me->rxIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN1)
	if (me->hcan.Instance == FDCAN1)
	{
		HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN2)
	if (me->hcan.Instance == FDCAN2)
	{
		HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN3)
	if (me->hcan.Instance == FDCAN3)
	{
		HAL_NVIC_SetPriority(FDCAN3_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);
		return;
	}
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}
static void AlxCan_Periph_DisableIrq(AlxCan* me)
{
	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)
	{
		HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN1_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN1_RX0_IRQn);
		return;
	}
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)
	{
		HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN2_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN2_RX0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN1)
	if (me->hcan.Instance == FDCAN1)
	{
		HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN1_IT0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN2)
	if (me->hcan.Instance == FDCAN2)
	{
		HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN2_IT0_IRQn);
		return;
	}
	#endif
	#if defined(FDCAN3)
	if (me->hcan.Instance == FDCAN3)
	{
		HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN3_IT0_IRQn);
		return;
	}
	#endif

	ALX_CAN_ASSERT(false);	// We should not get here
}


//******************************************************************************
// Callback Functions
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	#if defined(CAN1)
	if (hcan->Instance == CAN1)			{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can1); }
	#endif
	#if defined(CAN2)
	else if (hcan->Instance == CAN2)	{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can2); }
	#endif
	else								{ ALX_CAN_ASSERT(false); }	// We should never get here
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	#if defined(CAN1)
	if (hcan->Instance == CAN1)			{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can1); }
	#endif
	#if defined(CAN2)
	else if (hcan->Instance == CAN2)	{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can2); }
	#endif
	else								{ ALX_CAN_ASSERT(false); }	// We should never get here
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	#if defined(CAN1)
	if (hcan->Instance == CAN1)			{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can1); }
	#endif
	#if defined(CAN2)
	else if (hcan->Instance == CAN2)	{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can2); }
	#endif
	else								{ ALX_CAN_ASSERT(false); }	// We should never get here
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	#if defined(CAN1)
	if (hcan->Instance == CAN1)			{ AlxCan_RxMsg_TryReadFromHwFifo(alxCan_Can1); }
	#endif
	#if defined(CAN2)
	else if (hcan->Instance == CAN2)	{ AlxCan_RxMsg_TryReadFromHwFifo(alxCan_Can2); }
	#endif
	else								{ ALX_CAN_ASSERT(false); }	// We should never get here
}
#endif
#if defined(ALX_STM32G4)
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hcan)
{
	#if defined(FDCAN1)
	if (hcan->Instance == FDCAN1)		{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can1); }
	#endif
	#if defined(FDCAN2)
	else if (hcan->Instance == FDCAN2)	{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can2); }
	#endif
	#if defined(FDCAN3)
	else if (hcan->Instance == FDCAN3)	{ AlxCan_TxMsg_TryAddToHwMailbox(alxCan_Can3); }
	#endif
	else								{ ALX_CAN_ASSERT(false); };	// We should never get here
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs)
{
	#if defined(FDCAN1)
	if (hcan->Instance == FDCAN1)		{ AlxCan_RxMsg_TryReadFromHwFifo(alxCan_Can1); }
	#endif
	#if defined(FDCAN2)
	else if (hcan->Instance == FDCAN2)	{ AlxCan_RxMsg_TryReadFromHwFifo(alxCan_Can2); }
	#endif
	#if defined(FDCAN3)
	else if (hcan->Instance == FDCAN2)	{ AlxCan_RxMsg_TryReadFromHwFifo(alxCan_Can3); }
	#endif
	else								{ ALX_CAN_ASSERT(false); }	// We should never get here
}
#endif


#endif	// #if defined(ALX_C_LIB) && (((defined(ALX_STM32F4) || defined(ALX_STM32L4)) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED)))
