/**
  ******************************************************************************
  * @file		alxTracePc.c
  * @brief		Auralix C Library - ALX Trace - PC Unit Test Recorder
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * What a library trace does on a PC. On a target it is formatted twice and
  * pushed out of a UART; here it is COUNTED, by level, and returns.
  *
  * This file SUPPLIES rather than displaces. Every enabled ALX_TRACE_<LEVEL>
  * macro expands to AlxTrace_WriteLevel(&alxTrace, ...), and both of those
  * symbols live in Mcu/alxTrace.c, which NO test group compiles - measured, a
  * group that switches its traces on fails to link with exactly two undefined
  * symbols and no others. So there is no weak default to beat here, unlike
  * alxAssertPc.c: this file is simply the only definition in the image.
  *
  * Mcu/alxTrace.c is not the alternative. It needs AlxTrace_WriteStr, which has
  * no ALX_PC body anywhere in the library - only six MCU ones - plus AlxTick,
  * AlxIrq and AlxRtc behind its date-time callback. A stub also makes the call
  * COUNTABLE, which is the whole point: what a variant changes is whether the
  * call happens at all.
  *
  * THE ARGUMENTS ARE DELIBERATELY NOT RENDERED. (void)format, no vsnprintf.
  * Two reasons, and the second is the one that bites. The count and the level
  * are what a configuration test asks about, and rendering adds nothing to
  * them. And the build runs -Wformat=2 under a blanket -Werror, so a vsnprintf
  * of a non-literal format is a hard error; the usual remedy, a printf format
  * attribute, would mean re-declaring a function Mcu/alxTrace.h already
  * declares, which -Wredundant-decls rejects in turn. If you are here to "fix"
  * this file by rendering the text, that is why it does not.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_TRACE_PC_TEXT_LEN 256
#define ALX_TRACE_PC_LEVEL_NUM (ALX_TRACE_LEVEL_VRB + 1)

// The library's trace instance. On a target this lives in Mcu/alxTrace.c; here it only has to
// exist, because every trace macro takes its address and nothing below looks inside it.
AlxTrace alxTrace;

static uint32_t alxTracePc_count;
static uint32_t alxTracePc_countAtLevel[ALX_TRACE_PC_LEVEL_NUM];
static uint8_t alxTracePc_lastLevel;
static uint32_t alxTracePc_lastLine;
static char alxTracePc_lastFile[ALX_TRACE_PC_TEXT_LEN];
static char alxTracePc_lastFun[ALX_TRACE_PC_TEXT_LEN];


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxTracePc_Reset(void);
uint32_t AlxTracePc_Count(void);
uint32_t AlxTracePc_CountAtLevel(uint8_t level);
uint8_t AlxTracePc_LastLevel(void);
uint32_t AlxTracePc_LastLine(void);
const char* AlxTracePc_LastFile(void);
const char* AlxTracePc_LastFun(void);
uint8_t AlxTracePc_LevelConfigured(void);


//******************************************************************************
// The recorder's own controls
//******************************************************************************
void AlxTracePc_Reset(void)
{
	alxTracePc_count = 0;
	memset(alxTracePc_countAtLevel, 0, sizeof(alxTracePc_countAtLevel));
	alxTracePc_lastLevel = ALX_TRACE_LEVEL_OFF;
	alxTracePc_lastLine = 0;
	alxTracePc_lastFile[0] = '\0';
	alxTracePc_lastFun[0] = '\0';
}

uint32_t AlxTracePc_Count(void)
{
	return alxTracePc_count;
}

uint32_t AlxTracePc_CountAtLevel(uint8_t level)
{
	// Out of range answers zero rather than asserting: a test asking "how many at DBG" in a build
	// where DBG cannot occur is a legitimate question with the answer 0.
	if (level >= ALX_TRACE_PC_LEVEL_NUM)
	{
		return 0;
	}
	return alxTracePc_countAtLevel[level];
}

uint8_t AlxTracePc_LastLevel(void)
{
	return alxTracePc_lastLevel;
}

uint32_t AlxTracePc_LastLine(void)
{
	return alxTracePc_lastLine;
}

const char* AlxTracePc_LastFile(void)
{
	// The caller's ALX_<MODULE>_FILE constant - a .h, for every module in the library.
	return alxTracePc_lastFile;
}

const char* AlxTracePc_LastFun(void)
{
	// The caller's __func__, which comes from the .c the site is in. Kept apart from the file for
	// the reason alxAssertPc.c keeps them apart: they are two claims, not one.
	return alxTracePc_lastFun;
}

uint8_t AlxTracePc_LevelConfigured(void)
{
	// What ALX_TRACE_LEVEL was at compile time, read back at run time. This is how a test says
	// which variant's binary it is actually holding, rather than trusting the name it asked for.
	return (uint8_t)ALX_TRACE_LEVEL;
}


//******************************************************************************
// The library's trace sink
//******************************************************************************
void AlxTrace_WriteLevel(AlxTrace* me, uint8_t level, const char* file, uint32_t line,
	const char* fun, const char* format, ...)
{
	(void)me;
	(void)format;	// not rendered, on purpose - see the file header

	alxTracePc_lastLevel = level;
	alxTracePc_lastLine = line;
	snprintf(alxTracePc_lastFile, sizeof(alxTracePc_lastFile), "%s", file);
	snprintf(alxTracePc_lastFun, sizeof(alxTracePc_lastFun), "%s", fun);

	if (level < ALX_TRACE_PC_LEVEL_NUM)
	{
		alxTracePc_countAtLevel[level]++;
	}
	alxTracePc_count++;

	// Set ALX_TRACE_ECHO to watch them go by while working out where one comes from.
	if (getenv("ALX_TRACE_ECHO") != NULL)
	{
		fprintf(stderr, "ALX TRACE (%u): %s:%u in %s\n", (unsigned)level, file, (unsigned)line, fun);
		fflush(stderr);
	}
}
