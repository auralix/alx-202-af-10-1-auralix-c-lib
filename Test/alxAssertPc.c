/**
  ******************************************************************************
  * @file		alxAssertPc.c
  * @brief		Auralix C Library - PC assert action (test parity with production)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The product ships with asserts ENABLED in RST mode (a failed assert resets
  * the MCU - a hard, unmissable stop). To test the code AS SHIPPED, the PC
  * test builds enable the same asserts and this file provides the PC action:
  * print the location and abort() the process, so a genuinely failed library
  * assert turns the pytest run RED with a diagnosable message - the host
  * analogue of the device reset. These strong definitions override the weak
  * no-op AlxAssert_Rst in alxAssert.c.
  *
  * WHY THIS MATTERS (finding, ALX-1514): several modules emit real side
  * effects INSIDE asserts (e.g. alxCli.c wraps every AlxSerialPort_WriteStr
  * response in ALX_CLI_ASSERT(... == Alx_Ok)). With asserts compiled out the
  * expression is never evaluated and the CLI is SILENT - so a PC test with
  * asserts off does not exercise the shipped behavior at all.
  ******************************************************************************
  **/

#include "alxAssert.h"

#include <stdio.h>
#include <stdlib.h>

void AlxAssert_Rst(const char* file, uint32_t line, const char* fun)
{
	fprintf(stderr, "ALX ASSERT FAILED (RST): %s:%u in %s\n", file, (unsigned)line, fun);
	fflush(stderr);
	abort();
}

void AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun)
{
	fprintf(stderr, "ALX ASSERT FAILED (BKPT): %s:%u in %s\n", file, (unsigned)line, fun);
	fflush(stderr);
	abort();
}

void AlxAssert_Trace(const char* file, uint32_t line, const char* fun)
{
	fprintf(stderr, "ALX ASSERT FAILED (TRACE): %s:%u in %s\n", file, (unsigned)line, fun);
	fflush(stderr);
	abort();
}
