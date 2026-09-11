/**
  ******************************************************************************
  * @file		alxConfig.h
  * @brief		Auralix C Library - PC Unit Test Configuration
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * PC-side unit test build (Test/ folder). Selects the ALX_PC platform path
  * (C11 standard headers, alxGlobal.h) and enables the C library. All module
  * asserts/traces are intentionally OFF - the PC tests verify the documented
  * function contracts, not the assert configuration.
  ******************************************************************************
  **/

#ifndef ALX_CONFIG_H
#define ALX_CONFIG_H

//******************************************************************************
// Platform
//******************************************************************************
#define ALX_PC

//******************************************************************************
// Library
//******************************************************************************
#define ALX_C_LIB

//******************************************************************************
// Trace - OFF unless a build variant says otherwise
//
// A DLL built for a named variant passes -DALX_TRACE_LEVEL=... on the command line, so the guard
// is what lets one header serve every variant. OFF stays the default because most groups test
// module contracts rather than logging - but OFF is not what a product ships, and it compiles
// every ALX_TRACE_* call away while DISCARDING its arguments, so the `debug` variant exists to
// compile them for real.
//******************************************************************************
#ifndef ALX_TRACE_LEVEL
	#define ALX_TRACE_LEVEL ALX_TRACE_LEVEL_OFF
#endif

//******************************************************************************
// CMSIS shims - on the MCU these come from core headers; on PC the test
// helpers provide recording implementations (alxCliTestHelpers.c)
//******************************************************************************
void NVIC_SystemReset(void);

#endif	// #ifndef ALX_CONFIG_H
