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
