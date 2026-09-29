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


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS 8
#define ALX_SERIAL_PORT_FAKE_RX_LEN 16384
#define ALX_SERIAL_PORT_FAKE_TX_LEN 16384

typedef struct
{
	const AlxSerialPort* me;
	AlxFifo rxFifo;
	AlxFifo txFifo;
	uint8_t rxBuff[ALX_SERIAL_PORT_FAKE_RX_LEN];
	uint8_t txBuff[ALX_SERIAL_PORT_FAKE_TX_LEN];
	bool used;
} AlxSerialPortFake_Slot;

static AlxSerialPortFake_Slot alxSerialPortFake_slot[ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS];


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxSerialPortFake_Reset(void);
void AlxSerialPortFake_Register(const AlxSerialPort* me);
void AlxSerialPortFake_Unregister(const AlxSerialPort* me);
Alx_Status AlxSerialPortFake_InjectRx(const AlxSerialPort* me, const uint8_t* data, uint32_t len);
uint32_t AlxSerialPortFake_TxRead(const AlxSerialPort* me, uint8_t* buff, uint32_t len);
uint32_t AlxSerialPortFake_TxNumOfEntries(const AlxSerialPort* me);


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


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxSerialPortFake_Reset(void)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS; i++)
	{
		alxSerialPortFake_slot[i].used = false;
	}
}

void AlxSerialPortFake_Register(const AlxSerialPort* me)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_NUM_OF_PORTS; i++)
	{
		if (alxSerialPortFake_slot[i].used == false)
		{
			alxSerialPortFake_slot[i].me = me;
			alxSerialPortFake_slot[i].used = true;
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
	return Alx_Ok;
}

Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len)
{
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
	AlxFifo_Flush(&AlxSerialPortFake_Find(me)->rxFifo);
}

uint32_t AlxSerialPort_GetRxFifoNumOfEntries(AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&AlxSerialPortFake_Find(me)->rxFifo);
}

void AlxSerialPort_IrqHandler(AlxSerialPort* me)
{
	(void)me;	// no IRQs on the host
}
