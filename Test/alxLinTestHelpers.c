/**
  ******************************************************************************
  * @file		alxLinTestHelpers.c
  * @brief		Auralix C Library - ALX LIN - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The bus master's side of a LIN transaction, over the library's own serial
  * port fake - so what a test reads back is the BYTES the master put on the
  * wire, which is the only place the protected identifier and the checksum
  * exist.
  *
  * AlxLin_Master_Publish takes its frame BY VALUE, so the helper builds it from
  * scalars: a test names an identifier, some data and which checksum, and never
  * has to lay out an AlxLin_Frame across the ctypes boundary.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxLin.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxLin lin;
	AlxSerialPort port;
	AlxIoPin dbgPin;
} AlxLinTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
void AlxSerialPortFake_Register(AlxSerialPort* me);
void AlxSerialPortFake_Unregister(AlxSerialPort* me);

AlxLinTest_Obj* AlxLinTest_New(void);
void AlxLinTest_Delete(AlxLinTest_Obj* me);
AlxLin* AlxLinTest_Lin(AlxLinTest_Obj* me);
AlxSerialPort* AlxLinTest_Port(AlxLinTest_Obj* me);
int32_t AlxLinTest_Publish(AlxLinTest_Obj* me, uint8_t id, const uint8_t* data, uint8_t len, bool enhanced);
int32_t AlxLinTest_Subscribe(AlxLinTest_Obj* me, uint8_t id, uint8_t dataLen);
void AlxLinTest_RxFlush(AlxLinTest_Obj* me);
void AlxLinTest_RxByte(AlxLinTest_Obj* me, uint8_t data);

void AlxLinTest_TraceProbe(void);
void AlxLinTest_TraceReset(void);
uint32_t AlxLinTest_TraceArgEvalsWrn(void);
uint32_t AlxLinTest_TraceArgEvalsDbg(void);
uint32_t AlxLinTest_TraceArgEvalsVrb(void);
void AlxLinTest_BumpWrnDirect(void);
void AlxLinTest_BumpDbgDirect(void);
void AlxLinTest_BumpVrbDirect(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxLinTest_Obj* AlxLinTest_New(void)
{
	AlxLinTest_Obj* me = (AlxLinTest_Obj*)calloc(1, sizeof(AlxLinTest_Obj));
	if (me == NULL) { return NULL; }

	AlxSerialPortFake_Register(&me->port);

	// breakSyncOffset and the response timeout belong to the slave-facing half, which the publish
	// path does not reach; they are given ordinary values rather than zero so nothing reads as
	// deliberately degenerate.
	AlxLin_Ctor(&me->lin, &me->port, 1, 100, &me->dbgPin);
	return me;
}

void AlxLinTest_Delete(AlxLinTest_Obj* me)
{
	if (me == NULL) { return; }
	AlxSerialPortFake_Unregister(&me->port);
	free(me);
}

AlxLin* AlxLinTest_Lin(AlxLinTest_Obj* me)
{
	return &me->lin;
}

AlxSerialPort* AlxLinTest_Port(AlxLinTest_Obj* me)
{
	return &me->port;
}

int32_t AlxLinTest_Publish(AlxLinTest_Obj* me, uint8_t id, const uint8_t* data, uint8_t len, bool enhanced)
{
	AlxLin_Frame frame;
	memset(&frame, 0, sizeof(frame));

	frame.id = id;
	frame.dataLen = len;
	if (len > ALX_LIN_FRAME_DATA_LEN_MAX) { return -1; }
	memcpy(frame.data, data, len);
	frame.enhancedChecksumEnable = enhanced;

	Alx_Status status = AlxLin_Master_Publish(&me->lin, frame);
	return (int32_t)status;
}


//******************************************************************************
// The three-level trace probe
//******************************************************************************
// What a build variant changes about a trace is not how it is formatted - it is whether the call
// site exists. Two independent gates decide that, and a probe is the only way to see them apart:
//
//     gate A   ALX_LIN_TRACE_ENABLE          alxLin.h:74 - the module's own switch
//     gate B   ALX_TRACE_LEVEL >= <level>    Mcu/alxTrace.h:93-127 - the global threshold
//
// Either one closing replaces the macro with do{} while(false) taking NO parameters, so the
// arguments are deleted by the preprocessor and never evaluated. These counters are how a test
// sees that: the argument is a call that increments one, so a counter that does not move is an
// expression that was not compiled.
//
// alxLin.c's own sites prove the same thing on real library code, but only for whichever levels
// that module happens to use. The probe carries one site at each of the three the module declares,
// which is what makes the two gates separable in a single image.
static uint32_t alxLinTest_traceArgEvalsWrn;
static uint32_t alxLinTest_traceArgEvalsDbg;
static uint32_t alxLinTest_traceArgEvalsVrb;

static uint8_t AlxLinTest_BumpWrn(uint8_t value)
{
	alxLinTest_traceArgEvalsWrn++;
	return value;
}

static uint8_t AlxLinTest_BumpDbg(uint8_t value)
{
	alxLinTest_traceArgEvalsDbg++;
	return value;
}

static uint8_t AlxLinTest_BumpVrb(uint8_t value)
{
	alxLinTest_traceArgEvalsVrb++;
	return value;
}

void AlxLinTest_TraceProbe(void)
{
	ALX_LIN_TRACE_WRN("probe %02X", AlxLinTest_BumpWrn(1));
	ALX_LIN_TRACE_DBG("probe %02X", AlxLinTest_BumpDbg(2));
	ALX_LIN_TRACE_VRB("probe %02X", AlxLinTest_BumpVrb(3));
}

void AlxLinTest_TraceReset(void)
{
	alxLinTest_traceArgEvalsWrn = 0;
	alxLinTest_traceArgEvalsDbg = 0;
	alxLinTest_traceArgEvalsVrb = 0;
}

uint32_t AlxLinTest_TraceArgEvalsWrn(void)
{
	return alxLinTest_traceArgEvalsWrn;
}

uint32_t AlxLinTest_TraceArgEvalsDbg(void)
{
	return alxLinTest_traceArgEvalsDbg;
}

uint32_t AlxLinTest_TraceArgEvalsVrb(void)
{
	return alxLinTest_traceArgEvalsVrb;
}

// The controls. A counter that stays at zero is only evidence if the same counter can be shown to
// move - otherwise a probe that was never linked in reads exactly like a trace that was elided.
// These three call the same bumps from OUTSIDE any trace macro, so they move in every variant.
void AlxLinTest_BumpWrnDirect(void)
{
	AlxLinTest_BumpWrn(1);
}

void AlxLinTest_BumpDbgDirect(void)
{
	AlxLinTest_BumpDbg(2);
}

void AlxLinTest_BumpVrbDirect(void)
{
	AlxLinTest_BumpVrb(3);
}


//******************************************************************************
// Drivers for alxLin.c's own trace sites
//******************************************************************************
// One ordinary call each, no fault injected, one site per level:
//
//     WRN   alxLin.c:426   AlxLinTest_Subscribe - a master asks for a response nobody sent, so
//                          the received length cannot match the expected one
//     DBG   alxLin.c:836   AlxLinTest_RxByte with a protected id whose parity is wrong
//     VRB   alxLin.c:875   AlxLinTest_RxByte with a protected id whose parity is RIGHT, for an
//                          identifier no slave frame config claims
//
// The two RX ones need a SLAVE, which is what an object that was never Master_Init'd already is -
// AlxLin_Ctor leaves isMaster false. Neither AlxLin_RxBuff_Flush nor AlxLin_RxBuff_Handle asserts
// anything, so a bare constructed object is a legitimate caller.
int32_t AlxLinTest_Subscribe(AlxLinTest_Obj* me, uint8_t id, uint8_t dataLen)
{
	AlxLin_Frame frame;
	memset(&frame, 0, sizeof(frame));

	frame.id = id;
	frame.dataLen = dataLen;

	// 0 ms: alxOsDelayFake makes the wait a no-op anyway, and a real one would only slow the suite.
	Alx_Status status = AlxLin_Master_Subscribe(&me->lin, &frame, 0);
	return (int32_t)status;
}

void AlxLinTest_RxFlush(AlxLinTest_Obj* me)
{
	AlxLin_RxBuff_Flush(&me->lin);
}

void AlxLinTest_RxByte(AlxLinTest_Obj* me, uint8_t data)
{
	AlxLin_RxBuff_Handle(&me->lin, data);
}
