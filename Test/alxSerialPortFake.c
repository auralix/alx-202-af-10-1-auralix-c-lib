/**
  ******************************************************************************
  * @file		alxSerialPortFake.c
  * @brief		Auralix C Library - ALX SerialPort Module - PC link-time fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Tier-2 fake, named by the FAKED module. On PC, alxSerialPort.h falls back
  * to `typedef struct { bool dummy; } AlxSerialPort;`, so this file implements
  * the full wrapper API over two host-side AlxFifo instances per registered
  * port: rx = what the "device" receives (tests inject bytes), tx = what the
  * code under test sends (tests read responses). A small slot registry maps
  * AlxSerialPort* -> state, mirroring how the real ports own their FIFOs.
  ******************************************************************************
  **/

#include "alxSerialPort.h"
#include "alxFifo.h"

#include <stdlib.h>


//******************************************************************************
// Fake state registry
//******************************************************************************
#define ALX_SERIAL_PORT_FAKE_MAX_PORTS 2
#define ALX_SERIAL_PORT_FAKE_RX_LEN 1024
#define ALX_SERIAL_PORT_FAKE_TX_LEN 8192

typedef struct
{
	AlxSerialPort* me;
	AlxFifo rxFifo;
	AlxFifo txFifo;
	uint8_t rxBuff[ALX_SERIAL_PORT_FAKE_RX_LEN];
	uint8_t txBuff[ALX_SERIAL_PORT_FAKE_TX_LEN];
	bool used;
} AlxSerialPortFake_Slot;

static AlxSerialPortFake_Slot slots[ALX_SERIAL_PORT_FAKE_MAX_PORTS];

// Test-control API (exported to pytest via .def)
void AlxSerialPortFake_Register(AlxSerialPort* me);
void AlxSerialPortFake_Unregister(AlxSerialPort* me);
Alx_Status AlxSerialPortFake_InjectRx(AlxSerialPort* me, const uint8_t* data, uint32_t len);
uint32_t AlxSerialPortFake_TxRead(AlxSerialPort* me, uint8_t* buff, uint32_t len);
uint32_t AlxSerialPortFake_TxNumOfEntries(AlxSerialPort* me);

static AlxSerialPortFake_Slot* AlxSerialPortFake_Find(AlxSerialPort* me)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_MAX_PORTS; i++)
	{
		if (slots[i].used && (slots[i].me == me))
		{
			return &slots[i];
		}
	}

	// Test infrastructure - an unregistered port is a harness bug, fail fast
	exit(1);
}

void AlxSerialPortFake_Register(AlxSerialPort* me)
{
	for (uint32_t i = 0; i < ALX_SERIAL_PORT_FAKE_MAX_PORTS; i++)
	{
		if (slots[i].used == false)
		{
			slots[i].me = me;
			slots[i].used = true;
			AlxFifo_Ctor(&slots[i].rxFifo, slots[i].rxBuff, sizeof(slots[i].rxBuff));
			AlxFifo_Ctor(&slots[i].txFifo, slots[i].txBuff, sizeof(slots[i].txBuff));
			return;
		}
	}

	// Test infrastructure - slot pool exhausted, fail fast
	exit(1);
}

void AlxSerialPortFake_Unregister(AlxSerialPort* me)
{
	AlxSerialPortFake_Find(me)->used = false;
}

Alx_Status AlxSerialPortFake_InjectRx(AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	return AlxFifo_Write(&AlxSerialPortFake_Find(me)->rxFifo, data, len);
}

uint32_t AlxSerialPortFake_TxRead(AlxSerialPort* me, uint8_t* buff, uint32_t len)
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

uint32_t AlxSerialPortFake_TxNumOfEntries(AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&AlxSerialPortFake_Find(me)->txFifo);
}


//******************************************************************************
// Faked wrapper API (mirrors the real per-MCU pass-throughs, minus IRQ locks -
// the fake is single-threaded by construction)
//******************************************************************************
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
	// No IRQs on the host
	(void)me;
}
