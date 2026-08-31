/**
  ******************************************************************************
  * @file		alxFifoAsanSmoke.c
  * @brief		Auralix C Library - ALX FIFO Module - AddressSanitizer smoke runner
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
	char* str = (char*)malloc(len);	// exactly len bytes - overflow = ASan redzone hit
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
	char* str = (char*)malloc(len);
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
			RunAny(rot, len, "get\r", 4);				// basic
			RunAny(rot, len, "get\r\n", 5);				// CRLF pair
			RunAny(rot, len, "abcdefg\r", 8);			// exactly-full line with terminator
			RunAny(rot, len, "XXXXXXXX", 8);			// flood: full, no terminator
			RunAny(rot, len, "abcde\rok\r", 8);			// too-long candidate + survivor (partial write)
			RunAny(rot, len, "a\x00b\r", 4);			// embedded NUL data byte
			RunSeq(rot, len, "get\r\n", 5);				// sequence basic
			RunSeq(rot, len, "a\rb\r\n", 5);			// false start
			RunSeq(rot, len, "AAAAAAA\r", 8);			// full, partial delim at end
			RunSeq(rot, len, "ab\r\n", 4);				// delim split across wrap (rot 7)
		}
	}
	printf("ASAN SMOKE CLEAN\n");
	return 0;
}
