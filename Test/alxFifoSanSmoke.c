/**
  ******************************************************************************
  * @file		alxFifoSanSmoke.c
  * @brief		Auralix C Library - ALX FIFO Module - ASan+UBSan smoke runner
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Native exe built with /fsanitize=address. Exercises the memory-risky paths
  * of AlxFifo_ReadStrUntil/ReadStrUntilAny with EXACTLY-sized heap buffers
  * (both the FIFO buffer and the str destination), at every rotation of the
  * circular buffer - any out-of-bounds read/write of even one byte trips an
  * ASan redzone. Exit 0 = clean.
  ******************************************************************************
  **/

#include "alxFifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIFO_LEN 8

static AlxFifo* NewRotated(uint8_t** fifoBuffOut, uint32_t rot)
{
	AlxFifo* me = (AlxFifo*)malloc(sizeof(AlxFifo));
	uint8_t* fifoBuff = (uint8_t*)malloc(FIFO_LEN);
	if ((me == NULL) || (fifoBuff == NULL))
	{
		// Test infrastructure - fail fast on OOM
		exit(1);
	}
	AlxFifo_Ctor(me, fifoBuff, FIFO_LEN);
	for (uint32_t r = 0; r < rot; r++)
	{
		uint8_t d = 0;
		AlxFifo_Write(me, (const uint8_t*)"x", 1);
		AlxFifo_Read(me, &d, 1);
	}
	*fifoBuffOut = fifoBuff;
	return me;
}

static void RunAny(uint32_t rot, uint32_t len, const char* input, uint32_t inputLen)
{
	uint8_t* fifoBuff = NULL;
	AlxFifo* me = NewRotated(&fifoBuff, rot);
	AlxFifo_Write(me, (const uint8_t*)input, inputLen);

	// Destination of exactly len bytes - any overflow hits an ASan redzone; fail fast on OOM (test infrastructure)
	char* str = (char*)malloc(len);
	if (str == NULL) { exit(1); }
	uint32_t lenActual = 0;
	while (AlxFifo_ReadStrUntilAny(me, str, "\r\n", len, &lenActual) == Alx_Ok) {}
	free(str);
	free(fifoBuff);
	free(me);
}

static void RunSeq(uint32_t rot, uint32_t len, const char* input, uint32_t inputLen)
{
	uint8_t* fifoBuff = NULL;
	AlxFifo* me = NewRotated(&fifoBuff, rot);
	AlxFifo_Write(me, (const uint8_t*)input, inputLen);

	// Destination of exactly len bytes - any overflow hits an ASan redzone; fail fast on OOM (test infrastructure)
	char* str = (char*)malloc(len);
	if (str == NULL) { exit(1); }
	uint32_t lenActual = 0;
	while (AlxFifo_ReadStrUntil(me, str, "\r\n", len, &lenActual) == Alx_Ok) {}
	free(str);
	free(fifoBuff);
	free(me);
}

int main(void)
{
	for (uint32_t rot = 0; rot < FIFO_LEN; rot++)
	{
		for (uint32_t len = 1; len <= FIFO_LEN + 2; len++)
		{
			// Char-set reader: basic, CRLF pair, exactly-full line with terminator, flood (full, no terminator),
			// too-long candidate + survivor (partial write), embedded NUL data byte
			RunAny(rot, len, "get\r", 4);
			RunAny(rot, len, "get\r\n", 5);
			RunAny(rot, len, "abcdefg\r", 8);
			RunAny(rot, len, "XXXXXXXX", 8);
			RunAny(rot, len, "abcde\rok\r", 8);
			RunAny(rot, len, "a\x00b\r", 4);

			// Sequence reader: basic, false start, full with partial delim at end, delim split across the wrap (rot 7)
			RunSeq(rot, len, "get\r\n", 5);
			RunSeq(rot, len, "a\rb\r\n", 5);
			RunSeq(rot, len, "AAAAAAA\r", 8);
			RunSeq(rot, len, "ab\r\n", 4);
		}
	}
	// Exercise the remaining exported functions (WriteStr, Flush, Rewind, partial R/W)
	for (uint32_t rot = 0; rot < FIFO_LEN; rot++)
	{
		uint8_t* fifoBuff = NULL;
		AlxFifo* me = NewRotated(&fifoBuff, rot);
		AlxFifo_WriteStr(me, "abc");
		uint8_t rd[FIFO_LEN];
		AlxFifo_Read(me, rd, 2);
		AlxFifo_Rewind(me, 2);
		AlxFifo_Read(me, rd, 3);

		// Partial write into the remaining space, then a read from the flushed (empty) fifo
		AlxFifo_WriteStr(me, "toolongstring");
		AlxFifo_Flush(me);
		AlxFifo_Read(me, rd, 1);
		free(fifoBuff);
		free(me);
	}
	printf("ASAN SMOKE CLEAN\n");
	return 0;
}
