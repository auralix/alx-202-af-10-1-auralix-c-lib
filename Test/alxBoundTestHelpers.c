/**
  ******************************************************************************
  * @file		alxBoundTestHelpers.c
  * @brief		Auralix C Library - ALX Bound Module - PC test helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Status-enum getters for the pytest suite (enum values are never hardcoded
  * in Python). AlxBound functions operate on caller scalars/buffers - no
  * opaque handle is needed, tests call the public API directly via ctypes.
  ******************************************************************************
  **/

#include "alxGlobal.h"

int32_t AlxBoundTest_Status_Ok(void);
int32_t AlxBoundTest_Status_ErrMin(void);
int32_t AlxBoundTest_Status_ErrMax(void);
int32_t AlxBoundTest_Status_ErrLen(void);

int32_t AlxBoundTest_Status_Ok(void)     { return (int32_t)Alx_Ok; }
int32_t AlxBoundTest_Status_ErrMin(void) { return (int32_t)AlxBound_ErrMin; }
int32_t AlxBoundTest_Status_ErrMax(void) { return (int32_t)AlxBound_ErrMax; }
int32_t AlxBoundTest_Status_ErrLen(void) { return (int32_t)AlxBound_ErrLen; }
