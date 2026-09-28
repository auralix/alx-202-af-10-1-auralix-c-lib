/**
  ******************************************************************************
  * @file		alxMemRawTestOverride.c
  * @brief		Auralix C Library - ALX Memory Raw Module - a product's override
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * What a product supplies. alxMemRaw.c declares its whole public API ALX_WEAK
  * and implements none of it, so every product that stores anything outside
  * flash writes this file for its own device; this one stores into RAM and
  * records what it was handed.
  *
  * Linked into the override DLL only, and alxMemRaw.c is linked there too, so
  * the two definitions of each symbol are in the same image and the strong one
  * has to win. That became possible on this host only recently: alxGlobal.h
  * emits __attribute__((weak)) for clang on Windows since this branch, and
  * before that lld-link reported a duplicate symbol. Test/alxIna228TestHelpers.c
  * and Test/alxPi4ioeTestHelpers.c are the other two overrides in the suite.
  *
  * FOUR of the five symbols are overridden and AlxMemRaw_DeInit deliberately is
  * NOT, so one image shows that displacement is PER SYMBOL: four calls land in
  * this file and the fifth still lands in the library's asserting stub. A
  * product that implements what it needs and leaves the rest is the ordinary
  * case, and the ordinary case is what leaves a stub reachable in a shipped
  * build.
  *
  * The constructor here sets both fields the header declares - wasCtorCalled
  * true, isInit false - which is the convention alxMemSafe.c and alxCrc.c
  * follow. Nothing told it to: no library function reads either field for this
  * module, and the weak constructor writes neither.
  *
  * numOfTries and timeout_ms are RECORDED and not interpreted, and neither is
  * checkWithReadEnable. That is not laziness in the override - the header
  * defines no semantics for any of the three, so there is nothing to implement
  * faithfully, and what a test can pin is that the caller's values arrive
  * unmodified rather than replaced by a library default.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMemRaw.h"

#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_MEM_RAW_OVERRIDE_SIZE 256

enum
{
	ALX_MEM_RAW_OVERRIDE_CTOR = 0,
	ALX_MEM_RAW_OVERRIDE_INIT = 1,
	ALX_MEM_RAW_OVERRIDE_READ = 2,
	ALX_MEM_RAW_OVERRIDE_WRITE = 3,
	ALX_MEM_RAW_OVERRIDE_KINDS = 4
};

static uint8_t alxMemRawOverride_mem[ALX_MEM_RAW_OVERRIDE_SIZE];
static uint32_t alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_KINDS];
static uint32_t alxMemRawOverride_lastAddr;
static uint32_t alxMemRawOverride_lastLen;
static uint32_t alxMemRawOverride_lastNumOfTries;
static uint32_t alxMemRawOverride_lastTimeout_ms;
static bool alxMemRawOverride_lastCheckWithReadEnable;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
void AlxMemRawOverride_Reset(void);
uint32_t AlxMemRawOverride_CtorCount(void);
uint32_t AlxMemRawOverride_InitCount(void);
uint32_t AlxMemRawOverride_ReadCount(void);
uint32_t AlxMemRawOverride_WriteCount(void);
uint32_t AlxMemRawOverride_LastAddr(void);
uint32_t AlxMemRawOverride_LastLen(void);
uint32_t AlxMemRawOverride_LastNumOfTries(void);
uint32_t AlxMemRawOverride_LastTimeout_ms(void);
bool AlxMemRawOverride_LastCheckWithReadEnable(void);
uint8_t AlxMemRawOverride_Peek(uint32_t addr);
void AlxMemRawOverride_Poke(uint32_t addr, uint8_t value);
uint32_t AlxMemRawOverride_Size(void);


//******************************************************************************
// The override's own controls
//******************************************************************************
void AlxMemRawOverride_Reset(void)
{
	memset(alxMemRawOverride_mem, 0xFF, sizeof(alxMemRawOverride_mem));	// blank, as an erased device reads
	memset(alxMemRawOverride_count, 0, sizeof(alxMemRawOverride_count));
	alxMemRawOverride_lastAddr = 0;
	alxMemRawOverride_lastLen = 0;
	alxMemRawOverride_lastNumOfTries = 0;
	alxMemRawOverride_lastTimeout_ms = 0;
	alxMemRawOverride_lastCheckWithReadEnable = false;
}

uint32_t AlxMemRawOverride_CtorCount(void)
{
	return alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_CTOR];
}

uint32_t AlxMemRawOverride_InitCount(void)
{
	return alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_INIT];
}

uint32_t AlxMemRawOverride_ReadCount(void)
{
	return alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_READ];
}

uint32_t AlxMemRawOverride_WriteCount(void)
{
	return alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_WRITE];
}

uint32_t AlxMemRawOverride_LastAddr(void)
{
	return alxMemRawOverride_lastAddr;
}

uint32_t AlxMemRawOverride_LastLen(void)
{
	return alxMemRawOverride_lastLen;
}

uint32_t AlxMemRawOverride_LastNumOfTries(void)
{
	return alxMemRawOverride_lastNumOfTries;
}

uint32_t AlxMemRawOverride_LastTimeout_ms(void)
{
	return alxMemRawOverride_lastTimeout_ms;
}

bool AlxMemRawOverride_LastCheckWithReadEnable(void)
{
	return alxMemRawOverride_lastCheckWithReadEnable;
}

uint8_t AlxMemRawOverride_Peek(uint32_t addr)
{
	if (addr >= sizeof(alxMemRawOverride_mem))
	{
		return 0;
	}
	return alxMemRawOverride_mem[addr];
}

void AlxMemRawOverride_Poke(uint32_t addr, uint8_t value)
{
	if (addr < sizeof(alxMemRawOverride_mem))
	{
		alxMemRawOverride_mem[addr] = value;
	}
}

uint32_t AlxMemRawOverride_Size(void)
{
	return (uint32_t)sizeof(alxMemRawOverride_mem);
}


//******************************************************************************
// The library's weak symbols, displaced - AlxMemRaw_DeInit on purpose is not
//******************************************************************************
void AlxMemRaw_Ctor(AlxMemRaw* me)
{
	alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_CTOR]++;
	me->wasCtorCalled = true;	// the convention alxMemSafe.c and alxCrc.c set and check
	me->isInit = false;
}

Alx_Status AlxMemRaw_Init(AlxMemRaw* me)
{
	alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_INIT]++;
	me->isInit = true;
	return Alx_Ok;
}

Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries,
	uint16_t timeout_ms)
{
	(void)me;

	alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_READ]++;
	alxMemRawOverride_lastAddr = addr;
	alxMemRawOverride_lastLen = len;
	alxMemRawOverride_lastNumOfTries = numOfTries;
	alxMemRawOverride_lastTimeout_ms = timeout_ms;

	if ((addr + len) > sizeof(alxMemRawOverride_mem))
	{
		return Alx_Err;
	}
	memcpy(data, &alxMemRawOverride_mem[addr], len);
	return Alx_Ok;
}

Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable,
	uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;

	alxMemRawOverride_count[ALX_MEM_RAW_OVERRIDE_WRITE]++;
	alxMemRawOverride_lastAddr = addr;
	alxMemRawOverride_lastLen = len;
	alxMemRawOverride_lastCheckWithReadEnable = checkWithReadEnable;
	alxMemRawOverride_lastNumOfTries = numOfTries;
	alxMemRawOverride_lastTimeout_ms = timeout_ms;

	if ((addr + len) > sizeof(alxMemRawOverride_mem))
	{
		return Alx_Err;
	}
	memcpy(&alxMemRawOverride_mem[addr], data, len);
	return Alx_Ok;
}
