/**
  ******************************************************************************
  * @file alxClk_McuLpc55S6x.h
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_CLK_MCU_LPC55S6X_H
#define ALX_CLK_MCU_LPC55S6X_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default = 0,	// MF: OK
	AlxClk_Config_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz = 1,
	AlxClk_Config_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0 = 2,
	AlxClk_Config_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz = 3						// MF: OK
} AlxClk_Config;

typedef struct
{
	// Parameters
	AlxClk_Config config;
	AlxClk_Tick tick;

	// Variables
	uint32_t systemCoreClock;
	uint32_t coreSysClk;
	uint32_t mainClk;
	//uint32_t fro;		// MF: Don't know how we will use it
	//uint32_t froHf;	// MF: Don't know how we will use it
	//uint32_t extClk;	// MF: Don't know how we will use it
	//uint32_t fro_1m;	// MF: Don't know how we will use it
	//uint32_t pll0;	// MF: Don't know how we will use it
	//uint32_t pll1;	// MF: Don't know how we will use it
	//uint32_t rtc_32k;	// MF: Don't know how we will use it

	uint32_t coreSysClk_Ctor;
	uint32_t mainClk_Ctor;
	uint32_t fro_Ctor;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_CLK_MCU_LPC55S6X_H
