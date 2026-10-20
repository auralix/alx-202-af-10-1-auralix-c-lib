/**
  ******************************************************************************
  * @file		alxAssertAnalyzer.h
  * @brief		Auralix C Library - what clang's static analyser has to be told about the assert handlers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * The statement Test/config/alxAssert.cfg makes to cppcheck, made to clang-tidy's analyser: alxAssert.h
  * declares the handlers as plain void, so the analyser follows the else branch of a failed assertion
  * back out and reports the code after it as reached with the asserted condition FALSE - an index the
  * assertion bounded read out of bounds, a pointer it checked dereferenced as null. Every analysed matrix
  * row is read with this file included first (-include, Test/harness/host/matrix.py), and the
  * declarations below add analyzer_noreturn - an attribute only the analyser reads, code generation
  * ignores it - to the declarations alxAssert.h makes after them.
  *
  * Rst and Bkpt only, as in the cfg: a reset does not come back and a breakpoint stops in a debugger,
  * but AlxAssert_Trace writes a line and returns, so the code after a failed TRACE assertion stays
  * analysed. No library source is changed by it.
  ******************************************************************************
  **/

#ifndef ALX_ASSERT_ANALYZER_H
#define ALX_ASSERT_ANALYZER_H

#include <stdint.h>

void AlxAssert_Rst(const char* file, uint32_t line, const char* fun) __attribute__((analyzer_noreturn));
void AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun) __attribute__((analyzer_noreturn));

#endif	// #ifndef ALX_ASSERT_ANALYZER_H
