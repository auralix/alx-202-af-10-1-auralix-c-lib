/**
  ******************************************************************************
  * @file		cmsis_gcc.h
  * @brief		Auralix C Library - Configuration Matrix Fake - CMSIS Core
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the CMSIS core intrinsics as far as the alx code uses them,
  * written from that use, so the configuration matrix compiles every alx line
  * on every platform (Test/config/matrix.toml). Nothing here is the vendor's
  * and nothing here runs: an intrinsic is a function that does nothing, so the
  * same header serves a Cortex-M target and the host alike.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_CMSIS_CMSIS_GCC_H
#define ALX_SHIM_CMSIS_CMSIS_GCC_H

#include <stdint.h>

static inline void __BKPT(uint32_t value) { (void)value; }
static inline void __NOP(void) {}
static inline void __ISB(void) {}
static inline void __DSB(void) {}
static inline void __DMB(void) {}
static inline void __enable_irq(void) {}
static inline void __disable_irq(void) {}
static inline uint32_t __get_PRIMASK(void) { return 0U; }
static inline void __set_PRIMASK(uint32_t priMask) { (void)priMask; }
static inline void __set_MSP(uint32_t topOfMainStack) { (void)topOfMainStack; }
static inline void __set_CONTROL(uint32_t control) { (void)control; }

#endif	// ALX_SHIM_CMSIS_CMSIS_GCC_H
