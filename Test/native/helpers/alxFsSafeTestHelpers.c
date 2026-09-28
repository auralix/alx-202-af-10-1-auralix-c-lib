/**
  ******************************************************************************
  * @file		alxFsSafeTestHelpers.c
  * @brief		Auralix C Library - ALX Safe File Storage - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The module keeps pointers to a caller-owned file system and to THREE
  * caller-owned scratch buffers, so the helper owns all four and the object
  * together.
  *
  * It also exposes the CRC the module uses, because a test that wants to put a
  * valid copy on the "flash" before the module ever runs has to append the same
  * two bytes the module would - and a test that wants a CORRUPT copy has to be
  * able to append the wrong ones deliberately.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxFsSafe.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
#define ALX_FS_SAFE_TEST_BUFF_LEN 256

typedef struct
{
	AlxFsSafe fsSafe;
	AlxFs fs;
	uint8_t buffOrig[ALX_FS_SAFE_TEST_BUFF_LEN];
	uint8_t buffA[ALX_FS_SAFE_TEST_BUFF_LEN];
	uint8_t buffB[ALX_FS_SAFE_TEST_BUFF_LEN];
} AlxFsSafeTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxFsSafeTest_Obj* AlxFsSafeTest_New(bool useOrig);
void AlxFsSafeTest_Delete(AlxFsSafeTest_Obj* me);
AlxFsSafe* AlxFsSafeTest_FsSafe(AlxFsSafeTest_Obj* me);
uint32_t AlxFsSafeTest_CrcLen(AlxFsSafeTest_Obj* me);
uint16_t AlxFsSafeTest_Crc(AlxFsSafeTest_Obj* me, uint8_t* data, uint32_t len);


//******************************************************************************
// Functions
//******************************************************************************
AlxFsSafeTest_Obj* AlxFsSafeTest_New(bool useOrig)
{
	AlxFsSafeTest_Obj* me = (AlxFsSafeTest_Obj*)calloc(1, sizeof(AlxFsSafeTest_Obj));
	if (me == NULL) { return NULL; }

	AlxFsSafe_Ctor
	(
		&me->fsSafe,
		&me->fs,
		useOrig,
		me->buffOrig,
		me->buffA,
		me->buffB,
		ALX_FS_SAFE_TEST_BUFF_LEN
	);
	return me;
}

void AlxFsSafeTest_Delete(AlxFsSafeTest_Obj* me)
{
	free(me);
}

AlxFsSafe* AlxFsSafeTest_FsSafe(AlxFsSafeTest_Obj* me)
{
	return &me->fsSafe;
}

uint32_t AlxFsSafeTest_CrcLen(AlxFsSafeTest_Obj* me)
{
	return me->fsSafe.alxCrcLen;
}

uint16_t AlxFsSafeTest_Crc(AlxFsSafeTest_Obj* me, uint8_t* data, uint32_t len)
{
	// The module's OWN CRC over the module's OWN configuration - a test that restated the
	// polynomial would be testing its own copy of it.
	return AlxCrc_Calc(&me->fsSafe.alxCrc, data, len);
}
