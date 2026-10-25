/**
  ******************************************************************************
  * @file		cmsisHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - CMSIS Core
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the CMSIS core of a Cortex-M as far as the alx code uses it -
  * the register qualifiers, NVIC, SCB and SysTick, the system clock - written
  * from that use, so the configuration matrix compiles every alx line on every
  * platform (Test/config/matrix.toml). Nothing here is the vendor's and nothing
  * here runs.
  *
  * The registers are plain volatile: the CMSIS qualifiers (__IO and the rest)
  * are names a host's system headers use too, and the host rows include this
  * ahead of every header. A device fake defines its IRQn_Type and ALX_SHIM_IRQN_TYPE before including
  * this, as a CMSIS device header defines it before core_cm<x>.h; the host rows
  * include this alone (the platform's -include), and get the core exceptions.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_CMSIS_CMSISHOSTSHIM_H
#define ALX_SHIM_CMSIS_CMSISHOSTSHIM_H

#include <stdint.h>
#include "cmsis_version.h"
#include "cmsis_gcc.h"


//******************************************************************************
// Interrupt numbers
//******************************************************************************
#if !defined(ALX_SHIM_IRQN_TYPE)
typedef enum
{
	NonMaskableInt_IRQn = -14,
	HardFault_IRQn = -13,
	SVCall_IRQn = -5,
	PendSV_IRQn = -2,
	SysTick_IRQn = -1
} IRQn_Type;
#define ALX_SHIM_IRQN_TYPE
#endif


//******************************************************************************
// NVIC, SCB, SysTick
//******************************************************************************
typedef struct
{
	volatile uint32_t ISER[8U];
	uint32_t RESERVED0[24U];
	volatile uint32_t ICER[8U];
	uint32_t RESERVED1[24U];
	volatile uint32_t ISPR[8U];
	uint32_t RESERVED2[24U];
	volatile uint32_t ICPR[8U];
	uint32_t RESERVED3[24U];
	volatile uint32_t IABR[8U];
	uint32_t RESERVED4[56U];
	volatile uint8_t IP[240U];
} NVIC_Type;

typedef struct
{
	volatile const uint32_t CPUID;
	volatile uint32_t ICSR;
	volatile uint32_t VTOR;
	volatile uint32_t AIRCR;
	volatile uint32_t SCR;
	volatile uint32_t CCR;
} SCB_Type;

typedef struct
{
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile const uint32_t CALIB;
} SysTick_Type;

extern NVIC_Type alxShimCmsis_Nvic;
extern SCB_Type alxShimCmsis_Scb;
extern SysTick_Type alxShimCmsis_SysTick;
#define NVIC (&alxShimCmsis_Nvic)
#define SCB (&alxShimCmsis_Scb)
#define SysTick (&alxShimCmsis_SysTick)

void NVIC_EnableIRQ(IRQn_Type IRQn);
void NVIC_DisableIRQ(IRQn_Type IRQn);
void NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
void NVIC_SystemReset(void);
uint32_t SysTick_Config(uint32_t ticks);


//******************************************************************************
// System
//******************************************************************************
extern uint32_t SystemCoreClock;
void SystemCoreClockUpdate(void);

#endif	// ALX_SHIM_CMSIS_CMSISHOSTSHIM_H
