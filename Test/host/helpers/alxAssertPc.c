/**
  ******************************************************************************
  * @file		alxAssertPc.c
  * @brief		Auralix C Library - ALX Assert - PC Unit Test Handler
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * What a library assertion does on a PC. On a target it breakpoints, traces or
  * resets the MCU; here it RECORDS and returns.
  *
  * It used to abort(), which was honest and useless: abort inside a DLL takes
  * the whole test runner with it, so the only safe way to keep a suite green
  * was to never reach an assertion - and the cheapest way to never reach one is
  * to not compile them in at all, which is what most groups were doing. That
  * was found on 11.09 in the device repository by the mutation lane, where
  * mutants that INVERTED an assertion's condition survived the whole suite
  * because the expression had been compiled away.
  *
  * Recording instead means a group can build its module the way the product
  * ships it - assertions on - and the fixture can fail any test that leaves one
  * behind. Execution continuing past a broken invariant is not faithful; it is
  * the only option that leaves something to report.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxAssert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_ASSERT_PC_TEXT_LEN 256

static uint32_t alxAssertPc_count;
static char alxAssertPc_first[ALX_ASSERT_PC_TEXT_LEN];


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxAssertPc_Reset(void);
uint32_t AlxAssertPc_Count(void);
const char* AlxAssertPc_First(void);


//******************************************************************************
// Private functions
//******************************************************************************
static void AlxAssertPc_Record(const char* kind, const char* file, uint32_t line, const char* fun)
{
	if (alxAssertPc_count == 0)
	{
		snprintf(alxAssertPc_first, sizeof(alxAssertPc_first), "%s %s:%u in %s",
			kind, file, (unsigned)line, fun);
	}
	alxAssertPc_count++;

	// Set ALX_ASSERT_ECHO to watch them go by while working out where one comes from.
	if (getenv("ALX_ASSERT_ECHO") != NULL)
	{
		fprintf(stderr, "ALX ASSERT (%s): %s:%u in %s\n", kind, file, (unsigned)line, fun);
		fflush(stderr);
	}
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxAssertPc_Reset(void)
{
	alxAssertPc_count = 0;
	alxAssertPc_first[0] = '\0';
}

uint32_t AlxAssertPc_Count(void)
{
	return alxAssertPc_count;
}

const char* AlxAssertPc_First(void)
{
	// Where the module first said its own invariant was broken - file, line and function.
	return alxAssertPc_first;
}


//******************************************************************************
// The library's handlers
//******************************************************************************
void AlxAssert_Rst(const char* file, uint32_t line, const char* fun)
{
	AlxAssertPc_Record("RST", file, line, fun);
}

void AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun)
{
	AlxAssertPc_Record("BKPT", file, line, fun);
}

void AlxAssert_Trace(const char* file, uint32_t line, const char* fun)
{
	AlxAssertPc_Record("TRACE", file, line, fun);
}
