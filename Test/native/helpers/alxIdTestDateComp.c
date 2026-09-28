/**
  ******************************************************************************
  * @file		alxIdTestDateComp.c
  * @brief		Auralix C Library - ALX ID Module - PC Unit Test Compile Stamp
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Three accessors that would belong in alxIdTestHelpers.c, in a file of their
  * own for one reason: any translation unit that so much as MENTIONS
  * ALX_BUILD_DATE_COMP cannot be compiled under this suite's gate.
  *
  * The macro sums the digits of __DATE__ and __TIME__ into a YYMMDDhhmm number
  * using plain int arithmetic, and a number of that size does not fit in an
  * int: clang folds the constant and reports "overflow in expression; result is
  * -1'694'967'296 with type 'int' [-Winteger-overflow]", which -Werror makes an
  * error. It is the library's macro in somebody else's module and read-only to
  * this suite, so the file that uses it is in the group's CLOSURE (compiled
  * with -w) and the rest of the helpers stay under the full warning set where
  * this suite's own code belongs. arm-gcc is silent about the same expression,
  * which is why it has never been noticed on the target.
  *
  * UBSan sees it too, and at RUNTIME rather than at compile time - clang does
  * not fold the sum away at -O0, so the SANITIZE lane reported "signed integer
  * overflow: 2000000000 + 600000000 cannot be represented in type 'int'" here.
  * That is the same defect from a second tool and it is a good finding, but
  * -fno-sanitize-recover turns it into an abort that takes the whole test
  * runner down, which would leave the suite unable to say anything about this
  * module at all. So the one accessor that evaluates the macro switches the
  * check off for itself and the defect is carried by a strict xfail in
  * test_alxId.py (P481) instead, where it is named, explained and counted -
  * which is louder than a crash, not quieter.
  *
  * The value is what the constructor is handed as fwBuildDateComp by a product
  * that does not run a build job, and __DATE__ / __TIME__ are what it is built
  * from - so a test can check the module's date arithmetic against the compiler
  * rather than against a number somebody typed.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxBuild.h"


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
uint32_t AlxIdTest_BuildDateComp(void);
const char* AlxIdTest_CompDate(void);
const char* AlxIdTest_CompTime(void);


//******************************************************************************
// Functions
//******************************************************************************
__attribute__((no_sanitize("undefined")))	// the macro's own overflow - see the file header and P481
uint32_t AlxIdTest_BuildDateComp(void)
{
	// Read back as a uint32_t rather than as the int the macro computes in, because the int is
	// NEGATIVE: the wraparound is the whole finding, and the unsigned reinterpretation is what
	// lets a test see it as a number above INT_MAX instead of as a value it cannot name.
	return (uint32_t)ALX_BUILD_DATE_COMP;
}
const char* AlxIdTest_CompDate(void)
{
	// "Sep 11 2026" - the string the macro takes the date digits out of.
	return __DATE__;
}
const char* AlxIdTest_CompTime(void)
{
	// "07:44:19" - and the time digits.
	return __TIME__;
}
