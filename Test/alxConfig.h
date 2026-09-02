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
// Trace - OFF for PC unit tests (module contracts, not logging, are under test)
//******************************************************************************
#define ALX_TRACE_LEVEL ALX_TRACE_LEVEL_OFF

//******************************************************************************
// CMSIS shims - on the MCU these come from core headers; on PC the test
// helpers provide recording implementations (alxCliTestHelpers.c)
//******************************************************************************
void NVIC_SystemReset(void);

#endif	// #ifndef ALX_CONFIG_H
