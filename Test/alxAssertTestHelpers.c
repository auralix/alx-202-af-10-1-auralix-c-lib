/**
  ******************************************************************************
  * @file		alxAssertTestHelpers.c
  * @brief		Auralix C Library - ALX Assert Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The call sites this group needs, plus the two symbols the library's own weak
  * AlxAssert_Trace body refers to. Linked into BOTH DLLs of the group - the one
  * that keeps alxAssert.c's weak defaults and the one that lets alxAssertPc.c
  * displace them - so the two images differ in exactly one thing, which is the
  * comparison the group exists to make.
  *
  * Why the call sites live in C at all: ALX_ASSERT_BKPT/_TRACE/_RST expand
  * __LINE__ and __func__ AT THE CALL SITE, so what a handler receives is a
  * property of the caller's translation unit and can only be produced by one.
  * Each driver records the line its own macro sits on, one line before it, so a
  * test reads the expected number out of the DLL instead of carrying a copy
  * that goes stale the moment this file is edited.
  *
  * The file constant here deliberately names THIS .c, where every module in the
  * library defines its ALX_<MODULE>_FILE as the .h. That makes this file the
  * one place in the suite where the recorded file and the recorded line come
  * from the same translation unit, which is what a correct pair looks like.
  *
  * AlxTrace_WriteLevel and the alxTrace instance are stubs, not Mcu/alxTrace.c:
  * the real one drags AlxTick and AlxRtc in behind AlxTrace_GetRtcDateTime_
  * Callback, and a stub makes the call countable, which the real one would not.
  * The count is the point - the library's weak AlxAssert_Trace calls
  * AlxTrace_WriteLevel DIRECTLY rather than through ALX_TRACE_FTL, so the
  * compile-time ALX_TRACE_LEVEL gate (ALX_TRACE_LEVEL_OFF here) does not reach
  * it.
  *
  * Nothing in this file calls AlxAssert_Bkpt. Its weak default runs ALX_BKPT(),
  * which is __debugbreak() on this host and takes the process with it, so a
  * caller for it cannot be exported from an image where the weak default is the
  * one that would run. The override DLL gets that caller from
  * alxAssertBkptCaller.c, which is linked only there.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAssert.h"

#include <stdio.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_ASSERT_TEST_FILE "alxAssertTestHelpers.c"
#define ALX_ASSERT_TEST_TEXT_LEN 256

// The library's trace instance. On a target this lives in Mcu/alxTrace.c; here it only has to exist,
// because the weak AlxAssert_Trace takes its address and the stub below never looks inside it.
AlxTrace alxTrace;

static uint32_t alxAssertTest_traceCount;
static uint8_t alxAssertTest_traceLevel;
static char alxAssertTest_traceText[ALX_ASSERT_TEST_TEXT_LEN];
static char alxAssertTest_traceFile[ALX_ASSERT_TEST_TEXT_LEN];
static char alxAssertTest_traceFun[ALX_ASSERT_TEST_TEXT_LEN];
static uint32_t alxAssertTest_traceLine;

static uint32_t alxAssertTest_sideEffects;
static uint32_t alxAssertTest_sideEffectsOff;
static bool alxAssertTest_reachedAfterRst;
static bool alxAssertTest_reachedAfterTrace;

static uint32_t alxAssertTest_lineRst;
static uint32_t alxAssertTest_lineTrace;


//******************************************************************************
// A module's own assert block, with the enable macro ABSENT
//******************************************************************************
// Copied in SHAPE from any library header - alxMemRaw.h:59-67 is the nearest - and reduced to the
// one branch that matters here. The #else body is the library's own, character for character, and
// it is what a product compiles until it defines an enable macro. Nothing defines this one, by
// design: it is the DEFAULT configuration standing beside the enabled one in a single image, so a
// test can ask what the two do with the same expression without a second build.
#if defined(ALX_ASSERT_TEST_OFF_ASSERT_RST_ENABLE)
	#define ALX_ASSERT_TEST_OFF_ASSERT(expr) ALX_ASSERT_RST(ALX_ASSERT_TEST_FILE, expr)
#else
	#define ALX_ASSERT_TEST_OFF_ASSERT(expr) do{} while (false)
#endif


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
void AlxAssertTest_Reset(void);
uint32_t AlxAssertTest_TraceCount(void);
uint8_t AlxAssertTest_TraceLevel(void);
const char* AlxAssertTest_TraceText(void);
const char* AlxAssertTest_TraceFile(void);
const char* AlxAssertTest_TraceFun(void);
uint32_t AlxAssertTest_TraceLine(void);
uint8_t AlxAssertTest_TraceLevelFtl(void);
uint8_t AlxAssertTest_TraceLevelConfigured(void);
uint8_t AlxAssertTest_TraceLevelOff(void);
const char* AlxAssertTest_File(void);
uint32_t AlxAssertTest_SideEffects(void);
uint32_t AlxAssertTest_SideEffectsOff(void);
void AlxAssertTest_DriveRst(bool expr);
void AlxAssertTest_DriveTrace(bool expr);
void AlxAssertTest_DriveRstSideEffect(void);
void AlxAssertTest_DriveOffSideEffect(void);
void AlxAssertTest_BumpOffDirect(void);
uint32_t AlxAssertTest_LineRst(void);
uint32_t AlxAssertTest_LineTrace(void);
bool AlxAssertTest_ReachedAfterRst(void);
bool AlxAssertTest_ReachedAfterTrace(void);
bool AlxAssertTest_CallRstDirect(void);
bool AlxAssertTest_CallTraceDirect(void);


//******************************************************************************
// Private functions
//******************************************************************************
static bool AlxAssertTest_Bump(bool result)
{
	// An assert expression WITH a side effect: it moves only in a build where the expression is
	// compiled at all, which is the difference between an assertion that is off and one that is absent.
	alxAssertTest_sideEffects++;
	return result;
}

static bool AlxAssertTest_BumpOff(bool result)
{
	// The same probe behind the DISABLED expansion. A counter of its own, so one image can show
	// that the two expansions do different things to the same expression.
	alxAssertTest_sideEffectsOff++;
	return result;
}


//******************************************************************************
// The trace the library's weak AlxAssert_Trace writes to
//******************************************************************************
void AlxTrace_WriteLevel(AlxTrace* me, uint8_t level, const char* file, uint32_t line, const char* fun,
	const char* format, ...)
{
	(void)me;
	(void)format;	// the arguments are not rendered: what this group measures is THAT the call happened

	alxAssertTest_traceLevel = level;
	snprintf(alxAssertTest_traceText, sizeof(alxAssertTest_traceText), "%s:%u in %s",
		file, (unsigned)line, fun);

	// The three pieces kept apart as well, because what a test has to say about them is different:
	// the file is the caller's constant, the line is the caller's __LINE__ and the function is the
	// caller's __func__ - three claims, and a parsed string would make them one.
	snprintf(alxAssertTest_traceFile, sizeof(alxAssertTest_traceFile), "%s", file);
	snprintf(alxAssertTest_traceFun, sizeof(alxAssertTest_traceFun), "%s", fun);
	alxAssertTest_traceLine = line;

	alxAssertTest_traceCount++;
}


//******************************************************************************
// The helper's own controls
//******************************************************************************
void AlxAssertTest_Reset(void)
{
	alxAssertTest_traceCount = 0;
	alxAssertTest_traceLevel = 0;
	alxAssertTest_traceText[0] = '\0';
	alxAssertTest_traceFile[0] = '\0';
	alxAssertTest_traceFun[0] = '\0';
	alxAssertTest_traceLine = 0;
	alxAssertTest_sideEffects = 0;
	alxAssertTest_sideEffectsOff = 0;
	alxAssertTest_reachedAfterRst = false;
	alxAssertTest_reachedAfterTrace = false;
}

uint32_t AlxAssertTest_TraceCount(void)
{
	return alxAssertTest_traceCount;
}

uint8_t AlxAssertTest_TraceLevel(void)
{
	return alxAssertTest_traceLevel;
}

const char* AlxAssertTest_TraceText(void)
{
	return alxAssertTest_traceText;
}

const char* AlxAssertTest_TraceFile(void)
{
	return alxAssertTest_traceFile;
}

const char* AlxAssertTest_TraceFun(void)
{
	return alxAssertTest_traceFun;
}

uint32_t AlxAssertTest_TraceLine(void)
{
	return alxAssertTest_traceLine;
}

uint8_t AlxAssertTest_TraceLevelFtl(void)
{
	return (uint8_t)ALX_TRACE_LEVEL_FTL;
}

uint8_t AlxAssertTest_TraceLevelOff(void)
{
	return (uint8_t)ALX_TRACE_LEVEL_OFF;
}

uint8_t AlxAssertTest_TraceLevelConfigured(void)
{
	// What Test/alxConfig.h sets, read out of the BUILD rather than assumed by the test: the
	// claim that assertions are not gated by it is only worth making against the real number.
	return (uint8_t)ALX_TRACE_LEVEL;
}

const char* AlxAssertTest_File(void)
{
	return ALX_ASSERT_TEST_FILE;
}

uint32_t AlxAssertTest_SideEffects(void)
{
	return alxAssertTest_sideEffects;
}

uint32_t AlxAssertTest_SideEffectsOff(void)
{
	return alxAssertTest_sideEffectsOff;
}


//******************************************************************************
// The call sites - one per macro form, the expression under the caller's control
//******************************************************************************
void AlxAssertTest_DriveRst(bool expr)
{
	alxAssertTest_reachedAfterRst = false;
	alxAssertTest_lineRst = (uint32_t)__LINE__ + 1;	// the macro is the NEXT line, and it expands __LINE__ there
	ALX_ASSERT_RST(ALX_ASSERT_TEST_FILE, expr);
	alxAssertTest_reachedAfterRst = true;
}

void AlxAssertTest_DriveTrace(bool expr)
{
	alxAssertTest_reachedAfterTrace = false;
	alxAssertTest_lineTrace = (uint32_t)__LINE__ + 1;
	ALX_ASSERT_TRACE(ALX_ASSERT_TEST_FILE, expr);
	alxAssertTest_reachedAfterTrace = true;
}

void AlxAssertTest_DriveRstSideEffect(void)
{
	ALX_ASSERT_RST(ALX_ASSERT_TEST_FILE, AlxAssertTest_Bump(true));
}

void AlxAssertTest_DriveOffSideEffect(void)
{
	// The same expression behind the DISABLED expansion. The preprocessor drops the whole argument
	// here, so the call below is not compiled at all - which is why the probe needs the direct entry
	// underneath as well, to show the counter it does not move is a counter that works.
	ALX_ASSERT_TEST_OFF_ASSERT(AlxAssertTest_BumpOff(true));
}

void AlxAssertTest_BumpOffDirect(void)
{
	AlxAssertTest_BumpOff(true);
}

uint32_t AlxAssertTest_LineRst(void)
{
	return alxAssertTest_lineRst;
}

uint32_t AlxAssertTest_LineTrace(void)
{
	return alxAssertTest_lineTrace;
}

bool AlxAssertTest_ReachedAfterRst(void)
{
	return alxAssertTest_reachedAfterRst;
}

bool AlxAssertTest_ReachedAfterTrace(void)
{
	return alxAssertTest_reachedAfterTrace;
}


//******************************************************************************
// The handlers called directly - did execution continue past one
//******************************************************************************
bool AlxAssertTest_CallRstDirect(void)
{
	alxAssertTest_reachedAfterRst = false;
	AlxAssert_Rst(ALX_ASSERT_TEST_FILE, 1, "AlxAssertTest_CallRstDirect");
	alxAssertTest_reachedAfterRst = true;
	return alxAssertTest_reachedAfterRst;
}

bool AlxAssertTest_CallTraceDirect(void)
{
	alxAssertTest_reachedAfterTrace = false;
	AlxAssert_Trace(ALX_ASSERT_TEST_FILE, 2, "AlxAssertTest_CallTraceDirect");
	alxAssertTest_reachedAfterTrace = true;
	return alxAssertTest_reachedAfterTrace;
}
