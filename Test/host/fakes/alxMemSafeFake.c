/**
  ******************************************************************************
  * @file		alxMemSafeFake.c
  * @brief		Auralix C Library - ALX Memory Safe Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the safe memory, named after the module it fakes, for a
  * consumer that stores a block through it. It is RAM: what was written comes
  * back, so a parameter group that stores its values and loads them again
  * behaves as it does on a board. The real module keeps two CRC-checked copies
  * in a raw memory; the library's own suite tests that over the raw memory
  * fake, and this fake answers what a caller of the module sees.
  *
  * A read answers WHICH copy it used, not Alx_Ok, because the caller switches
  * on that. Memory never written reads as both copies bad, which is what a
  * board with a blank memory reports and what makes a group write its
  * defaults. A write completes before the call returns, so it is always done
  * and never in error.
  *
  * The memory outlives the object, as the chip outlives the firmware: it is
  * one block for the whole fake, not one per handle, and only a reset clears it.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMemSafe.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_MEM_SAFE_FAKE_LEN 8192

static uint8_t alxMemSafeFake_mem[ALX_MEM_SAFE_FAKE_LEN];
static uint32_t alxMemSafeFake_len;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxMemSafeFake_Reset(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxMemSafeFake_Reset(void)
{
	memset(alxMemSafeFake_mem, 0, sizeof(alxMemSafeFake_mem));
	alxMemSafeFake_len = 0;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxMemSafe_Ctor
(
	AlxMemSafe* me,
	AlxMemRaw* memRaw,
	AlxCrc* crc,
	uint32_t copyAddrA,
	uint32_t copyAddrB,
	uint32_t copyLen,
	bool nonBlockingEnable,
	uint8_t memSafeReadWriteNumOfTries,
	uint8_t memRawReadWriteNumOfTries,
	uint16_t memRawReadWriteTimeout_ms,
	uint8_t* buff1,
	uint32_t buff1Len,
	uint8_t* buff2,
	uint32_t buff2Len
)
{
	(void)me; (void)memRaw; (void)crc; (void)copyAddrA; (void)copyAddrB; (void)copyLen;
	(void)nonBlockingEnable; (void)memSafeReadWriteNumOfTries; (void)memRawReadWriteNumOfTries;
	(void)memRawReadWriteTimeout_ms; (void)buff1; (void)buff1Len; (void)buff2; (void)buff2Len;
}

Alx_Status AlxMemSafe_Write(AlxMemSafe* me, uint8_t* data, uint32_t len)
{
	(void)me;
	if (len > ALX_MEM_SAFE_FAKE_LEN) { return Alx_Err; }
	memcpy(alxMemSafeFake_mem, data, len);
	alxMemSafeFake_len = len;
	return Alx_Ok;
}

Alx_Status AlxMemSafe_Read(AlxMemSafe* me, uint8_t* data, uint32_t len)
{
	(void)me;
	if (alxMemSafeFake_len == 0 || len > alxMemSafeFake_len) { return AlxSafe_BothCopyErr_OrigErr; }
	memcpy(data, alxMemSafeFake_mem, len);
	return AlxSafe_BothCopyOkCrcSame_OrigDontCare_UseCopyA;
}

bool AlxMemSafe_IsWriteDone(AlxMemSafe* me)
{
	(void)me;
	return true;
}

bool AlxMemSafe_IsWriteErr(AlxMemSafe* me)
{
	(void)me;
	return false;
}
