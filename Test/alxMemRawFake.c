/**
  ******************************************************************************
  * @file		alxMemRawFake.c
  * @brief		Auralix C Library - ALX Memory Raw Module - PC link-time fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Tier-2 fake, named by the FAKED module. Replaces the ALX_WEAK stubs of
  * alxMemRaw.c (which is NOT linked into the MemSafe test DLL) with a
  * byte-addressable RAM "flash" of 2 kB (the RWWEE size of the product MCU)
  * plus test controls:
  *   - call counters per operation (Init / DeInit / Read / Write)
  *   - fail-the-n-th-call injection per operation (or fail always)
  *   - a power-loss model: the n-th upcoming Write stores only its first k
  *     bytes and reports an error, and every later Write fails until the test
  *     switches the power back on (reads keep working - the reboot reads)
  *   - an optional row-erase model: a Write first erases the 256 B row(s) it
  *     touches to 0xFF, as RWWEE row-erase + page-write does on the target
  *
  * Single instance (static): the fake ignores the AlxMemRaw* handle. The
  * memory persists across contexts until AlxMemRawFake_Reset() - that is how
  * a test models a reboot (new context over the same flash).
  ******************************************************************************
  **/

#include "alxMemRaw.h"

#include <string.h>

#define ALX_MEM_RAW_FAKE_SIZE 2048

enum
{
	ALX_MEM_RAW_FAKE_INIT = 0,
	ALX_MEM_RAW_FAKE_DEINIT = 1,
	ALX_MEM_RAW_FAKE_READ = 2,
	ALX_MEM_RAW_FAKE_WRITE = 3,
	ALX_MEM_RAW_FAKE_KINDS = 4
};

static uint8_t fakeMem[ALX_MEM_RAW_FAKE_SIZE];
static uint32_t fakeCount[ALX_MEM_RAW_FAKE_KINDS];
static uint32_t fakeFailAt[ALX_MEM_RAW_FAKE_KINDS];		// 0 = never, UINT32_MAX = always, n = the n-th call of that kind
static uint32_t fakePowerLossWrite;						// 0 = off, n = the n-th upcoming Write is cut
static uint32_t fakePowerLossBytes;						// bytes that still land before the cut
static uint32_t fakePowerLossCountdown;					// writes left before the cut
static bool fakePowerLost;								// after the cut: every Write fails until PowerOn
static bool fakeRowEraseModel;
static uint32_t fakeRowSize;
static uint32_t fakeLastNumOfTries;
static uint32_t fakeLastTimeout_ms;
static bool fakeIsInit;								// Init/DeInit state: Read/Write while not initialised fail (and are counted)
static uint32_t fakeNotInitCalls;
static bool fakeArgsSeen;							// first Read/Write fixes the expected (numOfTries, timeout_ms) ...
static uint32_t fakeArgsTries;
static uint32_t fakeArgsTimeout_ms;
static uint32_t fakeArgsMismatch;						// ... every later call with other args counts here
static uint32_t fakeLastWriteAddr;
static uint32_t fakeLastWriteLen;

// Test-control API (exported through alxMemSafeTest.def)
void AlxMemRawFake_Reset(void);
void AlxMemRawFake_Fill(uint8_t val);
void AlxMemRawFake_Peek(uint32_t addr, uint8_t* out, uint32_t len);
void AlxMemRawFake_Poke(uint32_t addr, uint8_t* in, uint32_t len);
void AlxMemRawFake_FailAt(uint32_t kind, uint32_t nthCall);
uint32_t AlxMemRawFake_Count(uint32_t kind);
void AlxMemRawFake_PowerLossAt(uint32_t nthWriteFromNow, uint32_t bytesThatLand);
void AlxMemRawFake_PowerOn(void);
bool AlxMemRawFake_IsPowerLost(void);
void AlxMemRawFake_SetRowEraseModel(bool enable, uint32_t rowSize);
uint32_t AlxMemRawFake_Size(void);
uint32_t AlxMemRawFake_LastNumOfTries(void);
uint32_t AlxMemRawFake_LastTimeout_ms(void);
uint32_t AlxMemRawFake_LastWriteAddr(void);
uint32_t AlxMemRawFake_LastWriteLen(void);
uint32_t AlxMemRawFake_ArgsMismatchCount(void);
uint32_t AlxMemRawFake_NotInitCallCount(void);

static bool AlxMemRawFake_Hit(uint32_t kind)
{
	fakeCount[kind]++;
	if (fakeFailAt[kind] == UINT32_MAX)
	{
		return true;
	}
	return (fakeFailAt[kind] != 0) && (fakeCount[kind] == fakeFailAt[kind]);
}

static void AlxMemRawFake_NoteArgs(uint32_t numOfTries, uint32_t timeout_ms)
{
	if (fakeArgsSeen == false)
	{
		fakeArgsSeen = true;
		fakeArgsTries = numOfTries;
		fakeArgsTimeout_ms = timeout_ms;
	}
	else if ((numOfTries != fakeArgsTries) || (timeout_ms != fakeArgsTimeout_ms))
	{
		fakeArgsMismatch++;
	}
}

static bool AlxMemRawFake_InRange(uint32_t addr, uint32_t len)
{
	return (addr <= ALX_MEM_RAW_FAKE_SIZE) && (len <= (ALX_MEM_RAW_FAKE_SIZE - addr));
}

void AlxMemRawFake_Reset(void)
{
	memset(fakeMem, 0xFF, sizeof(fakeMem));
	memset(fakeCount, 0, sizeof(fakeCount));
	memset(fakeFailAt, 0, sizeof(fakeFailAt));
	fakePowerLossWrite = 0;
	fakePowerLossBytes = 0;
	fakePowerLossCountdown = 0;
	fakePowerLost = false;
	fakeRowEraseModel = false;
	fakeRowSize = 256;
	fakeLastNumOfTries = 0;
	fakeLastTimeout_ms = 0;
	fakeIsInit = false;
	fakeNotInitCalls = 0;
	fakeArgsSeen = false;
	fakeArgsTries = 0;
	fakeArgsTimeout_ms = 0;
	fakeArgsMismatch = 0;
	fakeLastWriteAddr = 0;
	fakeLastWriteLen = 0;
}

void AlxMemRawFake_Fill(uint8_t val)
{
	memset(fakeMem, val, sizeof(fakeMem));
}

void AlxMemRawFake_Peek(uint32_t addr, uint8_t* out, uint32_t len)
{
	if (AlxMemRawFake_InRange(addr, len))
	{
		memcpy(out, &fakeMem[addr], len);
	}
}

void AlxMemRawFake_Poke(uint32_t addr, uint8_t* in, uint32_t len)
{
	if (AlxMemRawFake_InRange(addr, len))
	{
		memcpy(&fakeMem[addr], in, len);
	}
}

void AlxMemRawFake_FailAt(uint32_t kind, uint32_t nthCall)
{
	if (kind < ALX_MEM_RAW_FAKE_KINDS)
	{
		fakeFailAt[kind] = nthCall;
	}
}

uint32_t AlxMemRawFake_Count(uint32_t kind)
{
	if (kind < ALX_MEM_RAW_FAKE_KINDS)
	{
		return fakeCount[kind];
	}
	return 0;
}

void AlxMemRawFake_PowerLossAt(uint32_t nthWriteFromNow, uint32_t bytesThatLand)
{
	fakePowerLossWrite = nthWriteFromNow;
	fakePowerLossCountdown = nthWriteFromNow;
	fakePowerLossBytes = bytesThatLand;
}

void AlxMemRawFake_PowerOn(void)
{
	fakePowerLost = false;
	fakePowerLossWrite = 0;
	fakePowerLossCountdown = 0;
}

bool AlxMemRawFake_IsPowerLost(void)
{
	return fakePowerLost;
}

void AlxMemRawFake_SetRowEraseModel(bool enable, uint32_t rowSize)
{
	fakeRowEraseModel = enable;
	if (rowSize != 0)
	{
		fakeRowSize = rowSize;
	}
}

uint32_t AlxMemRawFake_Size(void)
{
	return ALX_MEM_RAW_FAKE_SIZE;
}

uint32_t AlxMemRawFake_LastNumOfTries(void)
{
	return fakeLastNumOfTries;
}

uint32_t AlxMemRawFake_LastTimeout_ms(void)
{
	return fakeLastTimeout_ms;
}

uint32_t AlxMemRawFake_LastWriteAddr(void)
{
	return fakeLastWriteAddr;
}

uint32_t AlxMemRawFake_LastWriteLen(void)
{
	return fakeLastWriteLen;
}

uint32_t AlxMemRawFake_ArgsMismatchCount(void)
{
	return fakeArgsMismatch;
}

uint32_t AlxMemRawFake_NotInitCallCount(void)
{
	return fakeNotInitCalls;
}


//******************************************************************************
// AlxMemRaw API - the faked module
//******************************************************************************
void AlxMemRaw_Ctor(AlxMemRaw* me)
{
	me->wasCtorCalled = true;
	me->isInit = false;
}

Alx_Status AlxMemRaw_Init(AlxMemRaw* me)
{
	if (AlxMemRawFake_Hit(ALX_MEM_RAW_FAKE_INIT))
	{
		return Alx_Err;
	}
	me->isInit = true;
	fakeIsInit = true;
	return Alx_Ok;
}

Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me)
{
	if (AlxMemRawFake_Hit(ALX_MEM_RAW_FAKE_DEINIT))
	{
		return Alx_Err;
	}
	me->isInit = false;
	fakeIsInit = false;
	return Alx_Ok;
}

Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	fakeLastNumOfTries = numOfTries;
	fakeLastTimeout_ms = timeout_ms;
	AlxMemRawFake_NoteArgs(numOfTries, timeout_ms);
	if (fakeIsInit == false)
	{
		fakeNotInitCalls++;
		return Alx_Err;
	}
	if (AlxMemRawFake_InRange(addr, len) == false)
	{
		return Alx_Err;
	}
	if (AlxMemRawFake_Hit(ALX_MEM_RAW_FAKE_READ))
	{
		return Alx_Err;
	}
	memcpy(data, &fakeMem[addr], len);
	return Alx_Ok;
}

Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	fakeLastNumOfTries = numOfTries;
	fakeLastTimeout_ms = timeout_ms;
	fakeLastWriteAddr = addr;
	fakeLastWriteLen = len;
	AlxMemRawFake_NoteArgs(numOfTries, timeout_ms);
	if (fakeIsInit == false)
	{
		fakeNotInitCalls++;
		return Alx_Err;
	}
	if (AlxMemRawFake_InRange(addr, len) == false)
	{
		return Alx_Err;
	}
	if (AlxMemRawFake_Hit(ALX_MEM_RAW_FAKE_WRITE))
	{
		return Alx_Err;
	}
	if (fakePowerLost)
	{
		return Alx_Err;		// power is off - nothing lands until AlxMemRawFake_PowerOn()
	}

	// Row-erase model: the target erases the whole row before programming a page
	if (fakeRowEraseModel && (len > 0))
	{
		uint32_t firstRow = addr - (addr % fakeRowSize);
		uint32_t lastByte = addr + len - 1;
		uint32_t lastRow = lastByte - (lastByte % fakeRowSize);
		uint32_t eraseLen = (lastRow - firstRow) + fakeRowSize;
		if (AlxMemRawFake_InRange(firstRow, eraseLen))
		{
			memset(&fakeMem[firstRow], 0xFF, eraseLen);
		}
	}

	// Power-loss model: the n-th write from the arming point is cut after k bytes
	if (fakePowerLossWrite != 0)
	{
		fakePowerLossCountdown--;
		if (fakePowerLossCountdown == 0)
		{
			uint32_t k = fakePowerLossBytes;
			if (k > len)
			{
				k = len;
			}
			memcpy(&fakeMem[addr], data, k);
			fakePowerLossWrite = 0;
			fakePowerLost = true;
			return Alx_Err;
		}
	}

	memcpy(&fakeMem[addr], data, len);
	if (checkWithReadEnable && (memcmp(&fakeMem[addr], data, len) != 0))
	{
		return Alx_Err;
	}
	return Alx_Ok;
}
