/**
  ******************************************************************************
  * @file alxCan_McuStm32.c
  * @brief Auralix C Library - ALX CAN Module
  * @version $LastChangedRevision: 5932 $
  * @date $LastChangedDate: 2021-09-03 18:54:49 +0200 (Fri, 03 Sep 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCan.h"

//******************************************************************************
// Module Guard
//******************************************************************************
#if (defined(ALX_STM32F4) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED))


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
static bool AlxCan_Ctor_IsClkOk(AlxCan* me);
static void AlxCan_Periph_EnableClk(AlxCan* me);
static void AlxCan_Periph_DisableClk(AlxCan* me);
static void AlxCan_Periph_ForceReset(AlxCan* me);
static void AlxCan_Periph_ReleaseReset(AlxCan* me);
static void AlxCan_Periph_EnableIrq(AlxCan* me);
static void AlxCan_Periph_DisableIrq(AlxCan* me);
static void AlxCan_TxMsg_TryAddToHwMailbox(AlxCan* me);
static void AlxCan_RxMsg_TryReadFromHwFifo(AlxCan* me);
#if defined(ALX_STM32G4)
static uint32_t AlxCan_GetDataLenCode(uint8_t len);
static uint8_t AlxCan_GetDataLen(uint32_t lenCode);
#endif


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCan_Ctor
(
	AlxCan* me,
	#if defined(ALX_STM32F4)
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
	// Ctor
	AlxFifo_Ctor(&me->txFifo, txFifoBuff, txFifoBuffLen);
	AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);

	// Objects - External
	me->do_CAN_TX = do_CAN_TX;
	me->di_CAN_RX = di_CAN_RX;
	me->clk = clk;

	// Parameters
	me->hcan.Instance = can;
	me->canClk = canClk;
	#if defined(ALX_STM32F4)
	me->canRxFifo = CAN_RX_FIFO0;
	#endif
	#if defined(ALX_STM32G4)
	me->canRxFifo = FDCAN_RX_FIFO0;
	#endif
	me->txFifoBuff = txFifoBuff;
	me->txFifoBuffLen = txFifoBuffLen;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->txIrqPriority = txIrqPriority;
	me->rxIrqPriority = rxIrqPriority;
	ALX_CAN_ASSERT(AlxCan_Ctor_IsClkOk(me));

	// Private Variables
	#if defined(ALX_STM32F4)
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

	// Variables

	//******************************************************************************
	// STM32F4
	//******************************************************************************
	#if defined(ALX_STM32F4)
	me->hcan.Init.Mode = CAN_MODE_NORMAL;
	me->hcan.Init.TimeTriggeredMode = DISABLE;
	me->hcan.Init.AutoBusOff = DISABLE;
	me->hcan.Init.AutoWakeUp = DISABLE;
	me->hcan.Init.AutoRetransmission = ENABLE;
	me->hcan.Init.ReceiveFifoLocked = DISABLE;
	me->hcan.Init.TransmitFifoPriority = DISABLE;

	if(me->canClk == AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz)
	{
		// Used calculator: http://www.bittiming.can-wiki.info/#STMicro, Target SamplePoint = 87.5%, Actual BitRate = 250kbps +/-0%, Actual SamplePoint = 88.9%, SJW = 1
		me->hcan.Init.Prescaler = 10;
		me->hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
		me->hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
		me->hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
	}
	else { ALX_CAN_ASSERT(false); return; } // We shouldn't get here

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


	//******************************************************************************
	// STM32G4
	//******************************************************************************
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

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
	me->isErr = false;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxCan_Init(AlxCan* me)
{
	ALX_CAN_ASSERT(me->isInit == false);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	// #1 Flush TX & RX FIFO
	AlxFifo_Flush(&me->txFifo);
	AlxFifo_Flush(&me->rxFifo);

	// #2 Init GPIO
	AlxIoPin_Init(me->do_CAN_TX);
	AlxIoPin_Init(me->di_CAN_RX);

	// #3 Release CAN Periphery Reset
	AlxCan_Periph_ReleaseReset(me);

	// #4 Enable CAN Periphery Clock
	AlxCan_Periph_EnableClk(me);
	#if defined(ALX_STM32G4)
	if(HAL_RCCEx_PeriphCLKConfig(&me->periphClkInit) != HAL_OK)	{ ALX_CAN_TRACE("ErrPClk"); };
	#endif

	// #5 Init CAN
	#if defined(ALX_STM32F4)
	if(HAL_CAN_Init(&me->hcan) != HAL_OK)						{ ALX_CAN_TRACE("ErrInit"); return Alx_Err; };
	if(HAL_CAN_ConfigFilter(&me->hcan, &me->fcan) != HAL_OK)	{ ALX_CAN_TRACE("ErrFilter"); return Alx_Err; };
	if(HAL_CAN_Start(&me->hcan) != HAL_OK)						{ ALX_CAN_TRACE("ErrStart"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_Init(&me->hcan) != HAL_OK)																												{ ALX_CAN_TRACE("ErrInit"); return Alx_Err; };
	if(HAL_FDCAN_ConfigFilter(&me->hcan, &me->fcan) != HAL_OK)																							{ ALX_CAN_TRACE("ErrFilter"); return Alx_Err; };
	if(HAL_FDCAN_ConfigGlobalFilter(&me->hcan, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)	{ ALX_CAN_TRACE("ErrGlobalFilter"); return Alx_Err; }
	if(HAL_FDCAN_Start(&me->hcan) != HAL_OK)
	#endif

	// #6 Enable CAN TX & RX IRQ
	#if defined(ALX_STM32F4)
	if(HAL_CAN_ActivateNotification(&me->hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { ALX_CAN_TRACE("ErrIrq"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_ActivateNotification(&me->hcan, FDCAN_IT_TX_FIFO_EMPTY | FDCAN_IT_RX_FIFO0_NEW_MESSAGE, ALX_NULL) != HAL_OK) { ALX_CAN_TRACE("ErrIrq"); return Alx_Err; };
	#endif

	// #ifdefined above does not work for some reason. Moving this line out of it fixes the issue for G4
	if(HAL_FDCAN_ActivateNotification(&me->hcan, FDCAN_IT_TX_FIFO_EMPTY | FDCAN_IT_RX_FIFO0_NEW_MESSAGE, ALX_NULL) != HAL_OK) { ALX_CAN_TRACE("ErrIrq"); return Alx_Err; };

	// #6 Enable CAN TX & RX IRQ
	AlxCan_Periph_EnableIrq(me);

	// #7 Set isInit
	me->isInit = true;

	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxCan_DeInit(AlxCan* me)
{
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	// #1 Disable CAN TX & RX IRQ
	#if defined(ALX_STM32F4)
	AlxCan_Periph_DisableIrq(me);
	if(HAL_CAN_DeactivateNotification(&me->hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { ALX_CAN_TRACE("ErrIrq"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	// #1 Disable CAN TX & RX IRQ
	AlxCan_Periph_DisableIrq(me);
	if (HAL_FDCAN_DeactivateNotification(&me->hcan, FDCAN_IT_TX_FIFO_EMPTY | FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != HAL_OK) { ALX_CAN_TRACE("ErrIrq"); return Alx_Err; };
	#endif

	// #2 DeInit CAN
	#if defined(ALX_STM32F4)
	if(HAL_CAN_DeInit(&me->hcan) != HAL_OK) { ALX_CAN_TRACE("ErrDeInit"); return Alx_Err; };
	#endif
	#if defined(ALX_STM32G4)
	if(HAL_FDCAN_DeInit(&me->hcan) != HAL_OK) { ALX_CAN_TRACE("ErrDeInit"); return Alx_Err; };
	#endif

	// #3 Force CAN Periphery Reset
	AlxCan_Periph_ForceReset(me);

	// #4 Disable CAN Periphery Clock
	AlxCan_Periph_DisableClk(me);

	// #5 DeInit GPIO
	AlxIoPin_DeInit(me->do_CAN_TX);
	AlxIoPin_DeInit(me->di_CAN_RX);

	// #6 Flush TX & RX FIFO
	AlxFifo_Flush(&me->txFifo);
	AlxFifo_Flush(&me->rxFifo);

	// #7 Reset isErr
	me->isErr = false;

	// #8 Reset isInit
	me->isInit = false;

	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxCan_ReInit(AlxCan* me)
{
	if(AlxCan_DeInit(me) != Alx_Ok) return Alx_Err;
	if(AlxCan_Init(me) != Alx_Ok) return Alx_Err;

	return Alx_Ok;
}
Alx_Status AlxCan_TxMsg(AlxCan* me, AlxCan_Msg msg)
{
	return AlxCan_TxMsgMulti(me, &msg, 1);
}
Alx_Status AlxCan_TxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg)
{
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);
	ALX_CAN_ASSERT((0 < numOfMsg) && (numOfMsg <= me->txFifoBuffLen));

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Add TX messages to ALX TX FIFO
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

	// #3 Try to add TX messages to HW mailbox
	AlxGlobal_DisableIrq();
	AlxCan_TxMsg_TryAddToHwMailbox(me);
	AlxGlobal_EnableIrq();

	// #4 Return status
	return status;
}
Alx_Status AlxCan_RxMsg(AlxCan* me, AlxCan_Msg* msg)
{
	return AlxCan_RxMsgMulti(me, msg, 1);
}
Alx_Status AlxCan_RxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg)
{
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);
	ALX_CAN_ASSERT((0 < numOfMsg) && (numOfMsg <= me->rxFifoBuffLen));

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read RX messages from ALX RX FIFO
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

	// #3 Return status
	return status;
}
bool AlxCan_IsErr(AlxCan* me)
{
	ALX_CAN_ASSERT(me->isInit == true);
	ALX_CAN_ASSERT(me->wasCtorCalled == true);

	return me->isErr;
}
void AlxCan_Foreground_Handle(AlxCan* me)
{
	#if defined(ALX_STM32F4)
	HAL_CAN_IRQHandler(&me->hcan);
	#endif
	#if defined(ALX_STM32G4)
	HAL_FDCAN_IRQHandler(&me->hcan);
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxCan_Ctor_IsClkOk(AlxCan* me)
{
	#if defined(ALX_STM32F4)
	if
	(
		#if defined(CAN1)
			(me->hcan.Instance == CAN1)
		#endif
		#if defined(CAN2)
		||	(me->hcan.Instance == CAN2)
		 #endif
	)
	{
		if(me->canClk == AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz)
		{
			if(45000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	#if defined(ALX_STM32G4)
	if
	(
		#if defined(FDCAN1)
			(me->hcan.Instance == FDCAN1)
		#endif
		#if defined(FDCAN2)
		||	(me->hcan.Instance == FDCAN2)
		 #endif
		#if defined(FDCAN3)
		||	(me->hcan.Instance == FDCAN3)
		#endif
	)
	{
		if (me->canClk == AlxCan_Clk_McuStm32G4_CanClk_250kbps_Pclk1Apb1_170MHz)
		{
			if (170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif

	ALX_CAN_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxCan_Periph_EnableClk(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_CLK_ENABLE(); isErr = false;
	#endif

	if(isErr)						{ ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_Periph_DisableClk(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_CLK_DISABLE(); isErr = false; }
	#endif

	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_CLK_DISABLE(); isErr = false;
	#endif

	if(isErr)						{ ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_Periph_ForceReset(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_FORCE_RESET(); isErr = false; }
	#endif

	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	 __HAL_RCC_FDCAN_FORCE_RESET(); isErr = false;
	#endif

	if(isErr)						{ ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_Periph_ReleaseReset(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)	{ __HAL_RCC_CAN1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)	{ __HAL_RCC_CAN2_RELEASE_RESET(); isErr = false; }
	#endif

	#if defined(FDCAN1) || defined(FDCAN2) || defined(FDCAN3)
	__HAL_RCC_FDCAN_RELEASE_RESET(); isErr = false;
	#endif

	if(isErr)						{ ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_Periph_EnableIrq(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)
	{
		HAL_NVIC_SetPriority(CAN1_TX_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, me->rxIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)
	{
		HAL_NVIC_SetPriority(CAN2_TX_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
		HAL_NVIC_SetPriority(CAN2_RX0_IRQn, me->rxIrqPriority, 0);
		HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
		isErr = false;
	}
	#endif

	#if defined(FDCAN1)
	if (me->hcan.Instance == FDCAN1)
	{
		HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(FDCAN2)
	if (me->hcan.Instance == FDCAN2)
	{
		HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(FDCAN3)
	if (me->hcan.Instance == FDCAN3)
	{
		HAL_NVIC_SetPriority(FDCAN3_IT0_IRQn, me->txIrqPriority, 0);
		HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);
		isErr = false;
	}
	#endif

	if(isErr) { ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_Periph_DisableIrq(AlxCan* me)
{
	bool isErr = true;

	#if defined(CAN1)
	if (me->hcan.Instance == CAN1)
	{
		HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN1_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN1_RX0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(CAN2)
	if (me->hcan.Instance == CAN2)
	{
		HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN2_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
		HAL_NVIC_ClearPendingIRQ(CAN2_RX0_IRQn);
		isErr = false;
	}
	#endif

	#if defined(FDCAN1)
	if (me->hcan.Instance == FDCAN1)
	{
		HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN1_IT0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(FDCAN2)
	if (me->hcan.Instance == FDCAN2)
	{
		HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN2_IT0_IRQn);
		isErr = false;
	}
	#endif
	#if defined(FDCAN3)
	if (me->hcan.Instance == FDCAN3)
	{
		HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);
		HAL_NVIC_ClearPendingIRQ(FDCAN3_IT0_IRQn);
		isErr = false;
	}
	#endif

	if(isErr) { ALX_CAN_ASSERT(false); } // We shouldn't get here
}
static void AlxCan_TxMsg_TryAddToHwMailbox(AlxCan* me)
{
	#if defined(ALX_STM32F4)
	while(HAL_CAN_GetTxMailboxesFreeLevel(&me->hcan) != 0)	// Add TX messages to HW mailboxes until all HW mailboxes are full
	{
		// #1 Prepare variables
		AlxCan_Msg alxTxMsg;
		CAN_TxHeaderTypeDef halTxMsg;

		// #2 Get TX message from ALX TX FIFO
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

		// #3 Prepare TX message
		{
			// #3.1 Set ID
			if(alxTxMsg.isExtendedId)
			{
				halTxMsg.StdId = 0;
				halTxMsg.ExtId = alxTxMsg.id;
				halTxMsg.IDE = CAN_ID_EXT;
			}
			else // Standard ID
			{
				halTxMsg.StdId = alxTxMsg.id;	// TODO assert if above 2048
				halTxMsg.ExtId = 0;
				halTxMsg.IDE = CAN_ID_STD;
			}

			// #3.2 Set Frame Type
			if(alxTxMsg.isDataFrame)
				halTxMsg.RTR = CAN_RTR_DATA;
			else // Remote Frame
				halTxMsg.RTR = CAN_RTR_REMOTE;

			// #3.3 Set Other
			halTxMsg.DLC = alxTxMsg.dataLen;
			halTxMsg.TransmitGlobalTime = DISABLE;
		}

		// #4 Add TX message to HW mailbox
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
		// #1 Prepare variables
		AlxCan_Msg alxTxMsg;
		FDCAN_TxHeaderTypeDef halTxMsg;

		// #2 Get TX message from ALX TX FIFO
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

		// #3 Prepare TX message
		{
			// #3.1 Set ID
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

			// #3.2 Set Frame Type
			if(alxTxMsg.isDataFrame)
				halTxMsg.TxFrameType = FDCAN_DATA_FRAME;
			else // Remote Frame
				halTxMsg.TxFrameType = FDCAN_REMOTE_FRAME;

			// #3.3 Set data length
			halTxMsg.DataLength = AlxCan_GetDataLenCode(alxTxMsg.dataLen);

			// #3.4 Set Other
			halTxMsg.ErrorStateIndicator = FDCAN_ESI_PASSIVE;		// JK: I dont understand this	????
			halTxMsg.BitRateSwitch = FDCAN_BRS_OFF;					// JK: Bit rate switch should be off - function of CAN FD
			halTxMsg.FDFormat = FDCAN_CLASSIC_CAN;					// JK: We use classic CAN and NOT CAN FD
			halTxMsg.TxEventFifoControl = FDCAN_NO_TX_EVENTS;		// JK: I dont understand this	????
			halTxMsg.MessageMarker = 0;								// JK: I dont understand this	????
		}

		// #4 Add TX message to HW fifo
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
	#if defined(ALX_STM32F4)
	while(HAL_CAN_GetRxFifoFillLevel(&me->hcan, me->canRxFifo) != 0)	// Read RX messages from HW RX FIFO until it is empty
	{
		// #1 Prepare variables
		AlxCan_Msg alxRxMsg;
		CAN_RxHeaderTypeDef halRxMsg;

		// #2 Get RX message from HW RX FIFO
		HAL_StatusTypeDef halStatus = HAL_CAN_GetRxMessage(&me->hcan, me->canRxFifo, &halRxMsg, alxRxMsg.data);
		if (halStatus != HAL_OK)
		{
			me->isErr = true;	// HAL error, module goes to error state
			return;
		}

		// #3 Prepare RX message
		{
			// #3.1 Set ID
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

			// #3.2 Set Frame Type
			if(halRxMsg.RTR == CAN_RTR_DATA)
				alxRxMsg.isDataFrame = true;
			else // Remote Frame
				alxRxMsg.isDataFrame = false;

			// #3.3 Set Other
			alxRxMsg.dataLen = halRxMsg.DLC;
		}

		// #4 Add RX message to ALX RX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_WriteMulti(&me->rxFifo, (uint8_t*)&alxRxMsg, sizeof(AlxCan_Msg));
		if (alxStatus == AlxFifo_ErrFull)
		{
			// TODO: Decide if we will handle ALX RX FIFO overflow as error, or we will discard overflow data..
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
		// #1 Prepare variables
		AlxCan_Msg alxRxMsg;
		FDCAN_RxHeaderTypeDef halRxMsg;

		// #2 Get RX message from HW RX FIFO
		HAL_StatusTypeDef halStatus = HAL_FDCAN_GetRxMessage(&me->hcan, me->canRxFifo, &halRxMsg, alxRxMsg.data);
		if (halStatus != HAL_OK)
		{
			me->isErr = true; 	// HAL error, module goes to error state
			return;
		}

		// #3 Prepare RX message
		{
			// #3.1 Set ID
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

			// #3.2 Set Frame Type
			if(halRxMsg.RxFrameType == FDCAN_DATA_FRAME)
				alxRxMsg.isDataFrame = true;
			else // Remote Frame
				alxRxMsg.isDataFrame = false;

			// #3.3 Set Lenght
			alxRxMsg.dataLen = AlxCan_GetDataLen(halRxMsg.DataLength);
		}

		// #4 Add RX message to ALX RX FIFO
		Alx_Status alxStatus = Alx_Err;
		alxStatus = AlxFifo_WriteMulti(&me->rxFifo, (uint8_t*)&alxRxMsg, sizeof(AlxCan_Msg));
		if (alxStatus == AlxFifo_ErrFull)
		{
			// TODO: Decide if we will handle ALX RX FIFO overflow as error, or we will discard overflow data..
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


//******************************************************************************
// Callback Functions
//******************************************************************************
#if defined(ALX_STM32F4)
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

#endif // (defined(ALX_STM32F4) || defined(ALX_STM32G4)) && defined(HAL_CAN_MODULE_ENABLED)
