/**
  ******************************************************************************
  * @file		alxFtoa.h
  * @brief		Auralix C Library - alxFtoa Module - Copied from: https://github.com/antongus/stm32tpl/blob/master/ftoa.h
  * @copyright	Copyright (C) 2022 Auralix d.o.o. All rights reserved.
  ******************************************************************************
  */

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef ALX_FTOA_H
#define ALX_FTOA_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Functions
//******************************************************************************
char* AlxFtoa(double f, char* buf, int precision);


#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FTOA_H
