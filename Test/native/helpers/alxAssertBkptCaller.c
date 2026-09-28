/**
  ******************************************************************************
  * @file		alxAssertBkptCaller.c
  * @brief		Auralix C Library - ALX Assert Module - the BKPT call sites
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The third macro form, in a translation unit of its own so that it can be
  * linked into ONE of the group's two DLLs and not the other.
  *
  * AlxAssert_Bkpt's weak default in alxAssert.c runs ALX_BKPT(), which on this
  * host is __debugbreak(): calling it in process does not return and takes the
  * test runner with it. So these callers are safe only where a strong
  * definition has displaced that default - the override DLL, where
  * alxAssertPc.c records and returns - and they are deliberately absent from
  * the weak-defaults DLL, where a test could otherwise reach them by name.
  *
  * That leaves the weak Bkpt body itself untested by any DLL, which is a
  * property of what it does rather than an omission: it can only be observed
  * out of process, from a built .exe whose exit code is the evidence (the
  * SANITIZE lane's alxFifoSanSmoke.exe is the in-repo precedent for building
  * one).
  *
  * These callers also close a gap in the suite that has nothing to do with weak
  * symbols: ALX_ASSERT_BKPT is one of the three forms the library offers and no
  * test has ever compiled it. Every group derives ALX_<MODULE>_ASSERT_RST_
  * ENABLE, so the RST form is the only one the suite has seen.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAssert.h"


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_ASSERT_BKPT_CALLER_FILE "alxAssertBkptCaller.c"

static uint32_t alxAssertBkptCaller_line;
static bool alxAssertBkptCaller_reached;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
void AlxAssertTest_DriveBkpt(bool expr);
uint32_t AlxAssertTest_LineBkpt(void);
const char* AlxAssertTest_FileBkpt(void);
bool AlxAssertTest_ReachedAfterBkpt(void);
bool AlxAssertTest_CallBkptDirect(void);


//******************************************************************************
// Functions
//******************************************************************************
void AlxAssertTest_DriveBkpt(bool expr)
{
	alxAssertBkptCaller_reached = false;
	alxAssertBkptCaller_line = (uint32_t)__LINE__ + 1;	// the macro is the NEXT line
	ALX_ASSERT_BKPT(ALX_ASSERT_BKPT_CALLER_FILE, expr);
	alxAssertBkptCaller_reached = true;
}

uint32_t AlxAssertTest_LineBkpt(void)
{
	return alxAssertBkptCaller_line;
}

const char* AlxAssertTest_FileBkpt(void)
{
	return ALX_ASSERT_BKPT_CALLER_FILE;
}

bool AlxAssertTest_ReachedAfterBkpt(void)
{
	// True only because a strong definition displaced the weak default. The library's own body
	// runs ALX_BKPT() and never gets here, which is why this getter exists in this file alone.
	return alxAssertBkptCaller_reached;
}

bool AlxAssertTest_CallBkptDirect(void)
{
	alxAssertBkptCaller_reached = false;
	AlxAssert_Bkpt(ALX_ASSERT_BKPT_CALLER_FILE, 3, "AlxAssertTest_CallBkptDirect");
	alxAssertBkptCaller_reached = true;
	return alxAssertBkptCaller_reached;
}
