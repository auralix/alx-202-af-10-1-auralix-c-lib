/**
  ******************************************************************************
  * @file		alxMemRawTestHelpers.c
  * @brief		Auralix C Library - ALX Memory Raw Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle construction and one caller per public function, linked into
  * BOTH DLLs of the group so the two images differ only in whether a strong
  * implementation is present. Nothing here is an implementation of AlxMemRaw -
  * that is alxMemRawTestOverride.c, and it is linked into one DLL only.
  *
  * Not to be confused with alxMemRawFake.c, which is a complete strong
  * replacement the MemSafe group links INSTEAD of alxMemRaw.c. This group is
  * the opposite arrangement: the library's own module IS compiled and linked,
  * which no lane in this suite had ever done before, and the point is what its
  * five weak defaults do when nothing displaces them. The exported names here
  * are AlxMemRawTest_* so the two can never be read as the same surface.
  *
  * The handle is poisoned with 0xFF before the constructor runs, the trick
  * alxFifoTestHelpers.c established: a constructor that forgets a field then
  * leaves garbage rather than the accidental zeros a fresh heap block would
  * hand it. It matters here more than usual - AlxMemRaw declares wasCtorCalled
  * and isInit, the two fields alxMemSafe.c and alxCrc.c both set and check, and
  * the weak constructor writes neither.
  *
  * The data buffer is the helper's own and is exposed byte by byte, because the
  * question a raw-memory read has to answer is not only what it returned but
  * whether it wrote anything at all: fill it with a recognisable pattern, read,
  * and see whether the pattern survived.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMemRaw.h"

#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_MEM_RAW_TEST_BUFF_LEN 64
#define ALX_MEM_RAW_TEST_POISON 0xFF

static uint8_t alxMemRawTest_buff[ALX_MEM_RAW_TEST_BUFF_LEN];


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxMemRaw* AlxMemRawTest_New(void);
AlxMemRaw* AlxMemRawTest_NewNoCtor(void);
void AlxMemRawTest_Delete(AlxMemRaw* me);
uint8_t AlxMemRawTest_WasCtorCalled(AlxMemRaw* me);
uint8_t AlxMemRawTest_IsInit(AlxMemRaw* me);
uint32_t AlxMemRawTest_SizeOf(void);
uint8_t AlxMemRawTest_PoisonByte(void);
int32_t AlxMemRawTest_Init(AlxMemRaw* me);
int32_t AlxMemRawTest_DeInit(AlxMemRaw* me);
int32_t AlxMemRawTest_Read(AlxMemRaw* me, uint32_t addr, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms);
int32_t AlxMemRawTest_Write(AlxMemRaw* me, uint32_t addr, uint32_t len, bool checkWithReadEnable,
	uint8_t numOfTries, uint16_t timeout_ms);
void AlxMemRawTest_BuffFill(uint8_t value);
uint8_t AlxMemRawTest_BuffPeek(uint32_t index);
void AlxMemRawTest_BuffPoke(uint32_t index, uint8_t value);
uint32_t AlxMemRawTest_BuffLen(void);
int32_t AlxMemRawTest_Status_Ok(void);
int32_t AlxMemRawTest_Status_Err(void);
int32_t AlxMemRawTest_Status_ErrNumOfTries(void);


//******************************************************************************
// Construction
//******************************************************************************
AlxMemRaw* AlxMemRawTest_New(void)
{
	AlxMemRaw* me = AlxMemRawTest_NewNoCtor();
	AlxMemRaw_Ctor(me);
	return me;
}

AlxMemRaw* AlxMemRawTest_NewNoCtor(void)
{
	AlxMemRaw* me = (AlxMemRaw*)malloc(sizeof(AlxMemRaw));
	if (me == NULL)
	{
		exit(1);	// test infrastructure - fail fast on OOM
	}
	memset(me, ALX_MEM_RAW_TEST_POISON, sizeof(*me));	// poison the STRUCT - see the file header
	return me;
}

void AlxMemRawTest_Delete(AlxMemRaw* me)
{
	free(me);
}


//******************************************************************************
// What the constructor left behind
//******************************************************************************
// Both fields are read as BYTES and never loaded as bool, and that is required rather than
// fastidious: the struct arrives poisoned with 0xFF, and 255 is not a value a C bool may hold. A
// plain `return me->wasCtorCalled;` is undefined behaviour on an object the constructor did not
// write, and the SANITIZE lane says so - -fsanitize=bool reports "load of value 255, which is not
// a valid value for type '_Bool'" and -fno-sanitize-recover turns that into an abort. The byte is
// what the test wants anyway: the question is what the constructor left in the field, not what
// that would mean if it were a bool.
uint8_t AlxMemRawTest_WasCtorCalled(AlxMemRaw* me)
{
	uint8_t byte = 0;
	memcpy(&byte, &me->wasCtorCalled, sizeof(byte));
	return byte;
}

uint8_t AlxMemRawTest_IsInit(AlxMemRaw* me)
{
	uint8_t byte = 0;
	memcpy(&byte, &me->isInit, sizeof(byte));
	return byte;
}

uint32_t AlxMemRawTest_SizeOf(void)
{
	return (uint32_t)sizeof(AlxMemRaw);
}

uint8_t AlxMemRawTest_PoisonByte(void)
{
	return (uint8_t)ALX_MEM_RAW_TEST_POISON;
}


//******************************************************************************
// One caller per public function
//******************************************************************************
int32_t AlxMemRawTest_Init(AlxMemRaw* me)
{
	Alx_Status status = AlxMemRaw_Init(me);
	return (int32_t)status;
}

int32_t AlxMemRawTest_DeInit(AlxMemRaw* me)
{
	Alx_Status status = AlxMemRaw_DeInit(me);
	return (int32_t)status;
}

int32_t AlxMemRawTest_Read(AlxMemRaw* me, uint32_t addr, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	Alx_Status status = AlxMemRaw_Read(me, addr, alxMemRawTest_buff, len, numOfTries, timeout_ms);
	return (int32_t)status;
}

int32_t AlxMemRawTest_Write(AlxMemRaw* me, uint32_t addr, uint32_t len, bool checkWithReadEnable,
	uint8_t numOfTries, uint16_t timeout_ms)
{
	Alx_Status status = AlxMemRaw_Write(me, addr, alxMemRawTest_buff, len, checkWithReadEnable,
		numOfTries, timeout_ms);
	return (int32_t)status;
}


//******************************************************************************
// The data buffer a read is supposed to fill
//******************************************************************************
void AlxMemRawTest_BuffFill(uint8_t value)
{
	memset(alxMemRawTest_buff, value, sizeof(alxMemRawTest_buff));
}

uint8_t AlxMemRawTest_BuffPeek(uint32_t index)
{
	if (index >= sizeof(alxMemRawTest_buff))
	{
		return 0;
	}
	return alxMemRawTest_buff[index];
}

void AlxMemRawTest_BuffPoke(uint32_t index, uint8_t value)
{
	if (index < sizeof(alxMemRawTest_buff))
	{
		alxMemRawTest_buff[index] = value;
	}
}

uint32_t AlxMemRawTest_BuffLen(void)
{
	return (uint32_t)sizeof(alxMemRawTest_buff);
}


//******************************************************************************
// The status values, read out of the library's own enum
//******************************************************************************
int32_t AlxMemRawTest_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}

int32_t AlxMemRawTest_Status_Err(void)
{
	return (int32_t)Alx_Err;
}

int32_t AlxMemRawTest_Status_ErrNumOfTries(void)
{
	return (int32_t)Alx_ErrNumOfTries;
}
