﻿/**
  ******************************************************************************
  * @file alxTrace_Printf.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_PRINTF_H
#define ALX_TRACE_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_PC) || defined(ALX_MBED)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Dummy
	bool dummy;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_PRINTF_H
