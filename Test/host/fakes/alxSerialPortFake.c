/**
  ******************************************************************************
  * @file		alxSerialPortFake.c
  * @brief		Auralix C Library - ALX SerialPort Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the serial port, named after the module it fakes. On a
  * PC the port is two host-side AlxFifo instances per registered port: rx is
  * what the code under test receives (tests inject bytes), tx is what it sends
  * (tests read the answers). The wrapper API runs over the library's own FIFO,
  * so a read that stops at a delimiter answers exactly what the real driver
  * answers - the same AlxFifo status codes, the bytes left in place.
  *
  * A port is registered before use: by a test, through AlxSerialPortFake_Register,
  * or by the module's own constructor where an MCU family defines one. A port
  * that was never registered is a harness bug and fails fast.
  *
  * The buffers are sized for a command line that dumps a whole parameter table.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxSerialPort.h"
#include "alxFifo.h"

#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS 8
#define ALX_SERIAL_PORT_FAKE_RX_LEN 16384
#define ALX_SERIAL_PORT_FAKE_TX_LEN 16384
#define ALX_SERIAL_PORT_FAKE_AFTER_FLUSH_LEN 64

typedef struct
{
	const AlxSerialPort* me;
	AlxFifo rxFifo;
	AlxFifo txFifo;
	uint8_t rxBuff[ALX_SERIAL_PORT_FAKE_RX_LEN];
	uint8_t txBuff[ALX_SERIAL_PORT_FAKE_TX_LEN];
	uint8_t afterFlush[ALX_SERIAL_PORT_FAKE_AFTER_FLUSH_LEN];	// what the bus carries once the code listens
	uint32_t afterFlushLen;
	uint32_t afterPolls;		// how many RX FIFO counts the code reads before the bytes land, 0 = at the flush
	bool used;
} AlxSerialPortFake_Slot;

static AlxSerialPortFake_Slot alxSerialPortFake_slot[ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS];
// What the port answers when a test makes it fail - Alx_Ok is the port working - and which of its
// operations fail: a mask of ALX_SERIAL_PORT_FAKE_INIT, _DEINIT, _READ and _WRITE.
#define ALX_SERIAL_PORT_FAKE_INIT 1
#define ALX_SERIAL_PORT_FAKE_DEINIT 2
#define ALX_SERIAL_PORT_FAKE_READ 4
#define ALX_SERIAL_PORT_FAKE_WRITE 8
static int32_t alxSerialPortFake_forcedStatus = Alx_Ok;
static uint32_t alxSerialPortFake_forcedOps = 0;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxSerialPortFake_Reset(void);
void AlxSerialPortFake_Register(const AlxSerialPort* me);
void AlxSerialPortFake_Unregister(const AlxSerialPort* me);
Alx_Status AlxSerialPortFake_InjectRx(const AlxSerialPort* me, const uint8_t* data, uint32_t len);
uint32_t AlxSerialPortFake_TxRead(const AlxSerialPort* me, uint8_t* buff, uint32_t len);
uint32_t AlxSerialPortFake_TxNumOfEntries(const AlxSerialPort* me);
void AlxSerialPortFake_SetForcedStatus(int32_t status, uint32_t ops);
void AlxSerialPortFake_SetRxAfterFlush(const AlxSerialPort* me, const uint8_t* data, uint32_t len);
void AlxSerialPortFake_SetRxAfterPolls(const AlxSerialPort* me, const uint8_t* data, uint32_t len, uint32_t polls);


//******************************************************************************
// Private Functions
//******************************************************************************
static AlxSerialPortFake_Slot* AlxSerialPortFake_Find(const AlxSerialPort* me)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS; i++)
	{
		if (alxSerialPortFake_slot[i].used && (alxSerialPortFake_slot[i].me == me))
		{
			return &alxSerialPortFake_slot[i];
		}
	}
	exit(1);	// test infrastructure - unregistered port is a harness bug, fail fast
}
static Alx_Status AlxSerialPortFake_Forced(uint32_t op)
{
	if (alxSerialPortFake_forcedOps & op)
	{
		return (Alx_Status)alxSerialPortFake_forcedStatus;
	}
	return Alx_Ok;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxSerialPortFake_Reset(void)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS; i++)
	{
		alxSerialPortFake_slot[i].used = false;
	}
	alxSerialPortFake_forcedStatus = Alx_Ok;
	alxSerialPortFake_forcedOps = 0;
}
// A response the other side of the bus sends as soon as the code under test starts to listen: the
// bytes land in the RX FIFO at its next flush, which is where both LIN subscribes begin. One shot.
void AlxSerialPortFake_SetRxAfterFlush(const AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	AlxSerialPortFake_Slot* s = AlxSerialPortFake_Find(me);
	if (len > ALX_SERIAL_PORT_FAKE_AFTER_FLUSH_LEN)
	{
		exit(1);	// test infrastructure - a response longer than the buffer is a harness bug
	}
	memcpy(s->afterFlush, data, len);
	s->afterFlushLen = len;
	s->afterPolls = 0;
}
// The same response, late: it lands once the code has counted the RX FIFO `polls` times after its flush,
// which is how a wait that checks, waits and checks again is made to go round.
void AlxSerialPortFake_SetRxAfterPolls(const AlxSerialPort* me, const uint8_t* data, uint32_t len, uint32_t polls)
{
	AlxSerialPortFake_SetRxAfterFlush(me, data, len);
	AlxSerialPortFake_Find(me)->afterPolls = polls;
}
void AlxSerialPortFake_SetForcedStatus(int32_t status, uint32_t ops)
{
	alxSerialPortFake_forcedStatus = status;
	alxSerialPortFake_forcedOps = ops;
}

void AlxSerialPortFake_Register(const AlxSerialPort* me)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS; i++)
	{
		if (alxSerialPortFake_slot[i].used == false)
		{
			alxSerialPortFake_slot[i].me = me;
			alxSerialPortFake_slot[i].used = true;
			alxSerialPortFake_slot[i].afterFlushLen = 0;
			alxSerialPortFake_slot[i].afterPolls = 0;
			AlxFifo_Ctor(&alxSerialPortFake_slot[i].rxFifo, alxSerialPortFake_slot[i].rxBuff, sizeof(alxSerialPortFake_slot[i].rxBuff));
			AlxFifo_Ctor(&alxSerialPortFake_slot[i].txFifo, alxSerialPortFake_slot[i].txBuff, sizeof(alxSerialPortFake_slot[i].txBuff));
			return;
		}
	}
	exit(1);	// test infrastructure - slot pool exhausted, fail fast
}

void AlxSerialPortFake_Unregister(const AlxSerialPort* me)
{
	AlxSerialPortFake_Find(me)->used = false;
}

Alx_Status AlxSerialPortFake_InjectRx(const AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	return AlxFifo_Write(&AlxSerialPortFake_Find(me)->rxFifo, data, len);
}

uint32_t AlxSerialPortFake_TxRead(const AlxSerialPort* me, uint8_t* buff, uint32_t len)
{
	AlxSerialPortFake_Slot* s = AlxSerialPortFake_Find(me);
	uint32_t n = AlxFifo_GetNumOfEntries(&s->txFifo);
	if (n > len)
	{
		n = len;
	}
	if (n > 0)
	{
		AlxFifo_Read(&s->txFifo, buff, n);
	}
	return n;
}

uint32_t AlxSerialPortFake_TxNumOfEntries(const AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&AlxSerialPortFake_Find(me)->txFifo);
}


//******************************************************************************
// The faked module's own contract (mirrors the real per-MCU pass-throughs, minus
// IRQ locks - the fake is single-threaded by construction)
//******************************************************************************
#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	AlxSerialPort_Config config,
	USART_TypeDef* uart,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	AlxGlobal_BaudRate baudRate,
	uint32_t dataWidth,
	uint32_t stopBits,
	uint32_t parity,
	uint8_t* txFifoBuff,
	uint32_t txFifoBuffLen,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority irqPriority,
	AlxIoPin* do_DBG_Tx,
	AlxIoPin* do_DBG_Rx
)
{
	(void)config; (void)uart; (void)do_TX; (void)di_RX; (void)baudRate; (void)dataWidth;
	(void)stopBits; (void)parity; (void)txFifoBuff; (void)txFifoBuffLen; (void)rxFifoBuff;
	(void)rxFifoBuffLen; (void)irqPriority; (void)do_DBG_Tx; (void)do_DBG_Rx;
	AlxSerialPortFake_Register(me);
}
#endif

Alx_Status AlxSerialPort_Init(AlxSerialPort* me)
{
	(void)me;
	return AlxSerialPortFake_Forced(ALX_SERIAL_PORT_FAKE_INIT);
}

Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me)
{
	(void)me;
	return AlxSerialPortFake_Forced(ALX_SERIAL_PORT_FAKE_DEINIT);
}

Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len)
{
	if (AlxSerialPortFake_Forced(ALX_SERIAL_PORT_FAKE_READ) != Alx_Ok)
	{
		return (Alx_Status)alxSerialPortFake_forcedStatus;
	}
	return AlxFifo_Read(&AlxSerialPortFake_Find(me)->rxFifo, data, len);
}

Alx_Status AlxSerialPort_ReadStrUntil(AlxSerialPort* me, char* str, const char* delim, uint32_t len, uint32_t* lenActual)
{
	return AlxFifo_ReadStrUntil(&AlxSerialPortFake_Find(me)->rxFifo, str, delim, len, lenActual);
}

Alx_Status AlxSerialPort_ReadStrUntilAny(AlxSerialPort* me, char* str, const char* delimSet, uint32_t len, uint32_t* lenActual)
{
	return AlxFifo_ReadStrUntilAny(&AlxSerialPortFake_Find(me)->rxFifo, str, delimSet, len, lenActual);
}

Alx_Status AlxSerialPort_Write(AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	if (AlxSerialPortFake_Forced(ALX_SERIAL_PORT_FAKE_WRITE) != Alx_Ok)
	{
		return (Alx_Status)alxSerialPortFake_forcedStatus;
	}
	return AlxFifo_Write(&AlxSerialPortFake_Find(me)->txFifo, data, len);
}

Alx_Status AlxSerialPort_WriteStr(AlxSerialPort* me, const char* str)
{
	return AlxFifo_WriteStr(&AlxSerialPortFake_Find(me)->txFifo, str);
}

void AlxSerialPort_FlushTxFifo(AlxSerialPort* me)
{
	AlxFifo_Flush(&AlxSerialPortFake_Find(me)->txFifo);
}

uint32_t AlxSerialPort_GetTxFifoNumOfEntries(AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&AlxSerialPortFake_Find(me)->txFifo);
}

void AlxSerialPort_FlushRxFifo(AlxSerialPort* me)
{
	AlxSerialPortFake_Slot* s = AlxSerialPortFake_Find(me);
	AlxFifo_Flush(&s->rxFifo);
	if (s->afterFlushLen > 0 && s->afterPolls == 0)
	{
		AlxFifo_Write(&s->rxFifo, s->afterFlush, s->afterFlushLen);
		s->afterFlushLen = 0;
	}
}

uint32_t AlxSerialPort_GetRxFifoNumOfEntries(AlxSerialPort* me)
{
	AlxSerialPortFake_Slot* s = AlxSerialPortFake_Find(me);
	if (s->afterFlushLen > 0 && s->afterPolls > 0)
	{
		s->afterPolls--;
		if (s->afterPolls == 0)
		{
			AlxFifo_Write(&s->rxFifo, s->afterFlush, s->afterFlushLen);
			s->afterFlushLen = 0;
		}
	}
	return AlxFifo_GetNumOfEntries(&s->rxFifo);
}

void AlxSerialPort_IrqHandler(AlxSerialPort* me)
{
	(void)me;	// no IRQs on the host
}
