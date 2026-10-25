/**
  ******************************************************************************
  * @file		alxCanFake.c
  * @brief		Auralix C Library - ALX CAN Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A bus is two queues. What the code under test transmits is kept in order so
  * a test can read the frame back byte for byte; what a test queues is handed
  * to the code under test one frame per AlxCan_RxMsg, and AlxFifo_ErrEmpty when
  * empty - which is what the library's own driver answers on an empty receive
  * FIFO, and what a product's receive loop breaks on. Anything else here is an
  * endless loop.
  *
  * Buses are told apart by their ADDRESS, and a bus nobody wrote to answers as
  * an empty one. On an MCU family the module's own constructor exists and is
  * defined here under the same guard the module header uses.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCan.h"
#include "alxFifo.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_CAN_FAKE_NUM_OF_BUSES 8
#define ALX_CAN_FAKE_NUM_OF_MSGS 256

typedef struct
{
	const AlxCan* me;
	bool isErr;
	AlxCan_Msg tx[ALX_CAN_FAKE_NUM_OF_MSGS];
	uint32_t txLen;
	AlxCan_Msg rx[ALX_CAN_FAKE_NUM_OF_MSGS];
	uint32_t rxLen;
	uint32_t rxRead;
	uint32_t rxMsgCount;
} AlxCanFake_Bus;

static AlxCanFake_Bus alxCanFake_bus[ALX_CAN_FAKE_NUM_OF_BUSES];
static uint32_t alxCanFake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxCanFake_Reset(void);
void AlxCanFake_ClearTx(const AlxCan* me);
uint32_t AlxCanFake_TxCount(const AlxCan* me);
bool AlxCanFake_TxMsg(const AlxCan* me, uint32_t i, uint32_t* id, bool* isExtendedId, uint8_t* dlc, uint8_t* data);
bool AlxCanFake_QueueRxMsg(const AlxCan* me, uint32_t id, bool isExtendedId, uint8_t dlc, const uint8_t* data);
uint32_t AlxCanFake_RxMsgCount(const AlxCan* me);
void AlxCanFake_SetErr(const AlxCan* me, bool isErr);


//******************************************************************************
// Private Functions
//******************************************************************************
static AlxCanFake_Bus* AlxCanFake_Slot(const AlxCan* me)
{
	for (uint32_t i = 0; i < alxCanFake_used; i++)
	{
		if (alxCanFake_bus[i].me == me) { return &alxCanFake_bus[i]; }
	}
	if (alxCanFake_used >= ALX_CAN_FAKE_NUM_OF_BUSES) { return &alxCanFake_bus[ALX_CAN_FAKE_NUM_OF_BUSES - 1]; }

	memset(&alxCanFake_bus[alxCanFake_used], 0, sizeof(alxCanFake_bus[0]));
	alxCanFake_bus[alxCanFake_used].me = me;
	return &alxCanFake_bus[alxCanFake_used++];
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxCanFake_Reset(void)
{
	alxCanFake_used = 0;
}

void AlxCanFake_ClearTx(const AlxCan* me)
{
	AlxCanFake_Slot(me)->txLen = 0;
}

uint32_t AlxCanFake_TxCount(const AlxCan* me)
{
	return AlxCanFake_Slot(me)->txLen;
}

bool AlxCanFake_TxMsg(const AlxCan* me, uint32_t i, uint32_t* id, bool* isExtendedId, uint8_t* dlc, uint8_t* data)
{
	AlxCanFake_Bus* bus = AlxCanFake_Slot(me);
	if (i >= bus->txLen) { return false; }
	*id = bus->tx[i].id;
	*isExtendedId = bus->tx[i].isExtendedId;
	*dlc = bus->tx[i].dataLen;
	memcpy(data, bus->tx[i].data, sizeof(bus->tx[i].data));
	return true;
}

bool AlxCanFake_QueueRxMsg(const AlxCan* me, uint32_t id, bool isExtendedId, uint8_t dlc, const uint8_t* data)
{
	AlxCanFake_Bus* bus = AlxCanFake_Slot(me);
	if (bus->rxLen >= ALX_CAN_FAKE_NUM_OF_MSGS) { return false; }
	bus->rx[bus->rxLen].id = id;
	bus->rx[bus->rxLen].isExtendedId = isExtendedId;
	bus->rx[bus->rxLen].isDataFrame = true;
	bus->rx[bus->rxLen].dataLen = dlc;
	memcpy(bus->rx[bus->rxLen].data, data, sizeof(bus->rx[0].data));
	bus->rxLen++;
	return true;
}

uint32_t AlxCanFake_RxMsgCount(const AlxCan* me)
{
	// How many times the code under test asked for a frame, whether one was there or not: a
	// receive loop that never polls is the defect this counts.
	return AlxCanFake_Slot(me)->rxMsgCount;
}

void AlxCanFake_SetErr(const AlxCan* me, bool isErr)
{
	AlxCanFake_Slot(me)->isErr = isErr;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if ((defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4)) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED))
void AlxCan_Ctor
(
	AlxCan* me,
	#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4)
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
	(void)can; (void)do_CAN_TX; (void)di_CAN_RX; (void)clk; (void)canClk;
	(void)txFifoBuff; (void)txFifoBuffLen; (void)rxFifoBuff; (void)rxFifoBuffLen;
	(void)txIrqPriority; (void)rxIrqPriority;
	AlxCanFake_Slot(me);
}
#endif

Alx_Status AlxCan_Init(AlxCan* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxCan_DeInit(AlxCan* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxCan_ReInit(AlxCan* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxCan_TxMsg(AlxCan* me, AlxCan_Msg msg)
{
	AlxCanFake_Bus* bus = AlxCanFake_Slot(me);
	if (bus->txLen >= ALX_CAN_FAKE_NUM_OF_MSGS) { return Alx_Err; }
	bus->tx[bus->txLen++] = msg;
	return Alx_Ok;
}

Alx_Status AlxCan_RxMsg(AlxCan* me, AlxCan_Msg* msg)
{
	AlxCanFake_Bus* bus = AlxCanFake_Slot(me);
	bus->rxMsgCount++;
	if (bus->rxRead >= bus->rxLen) { return AlxFifo_ErrEmpty; }	// what a receive loop breaks on
	*msg = bus->rx[bus->rxRead++];
	return Alx_Ok;
}

bool AlxCan_IsErr(AlxCan* me)
{
	return AlxCanFake_Slot(me)->isErr;
}

void AlxCan_IrqHandler(AlxCan* me)
{
	(void)me;	// no IRQs on the host
}
