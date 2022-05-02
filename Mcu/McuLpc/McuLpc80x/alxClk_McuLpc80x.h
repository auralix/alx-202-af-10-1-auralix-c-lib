/**
  ******************************************************************************
  * @file alxClk_McuLpc80x.h
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_CLK_MCU_LPC80X_H
#define ALX_CLK_MCU_LPC80X_H

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
#if defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_LPC80X)
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default = 0,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz = 1,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz = 2,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz = 3,
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz = 4,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz = 5,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5 = 6,
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz = 7,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5 = 8,
	#endif
	#if defined(ALX_LPC81X) || defined(ALX_LPC82X) || defined(ALX_LPC83X) || defined(ALX_LPC84X)
	// MF: NOTE - this Mcus will have the same enum
	#endif
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
	uint32_t fro;
	//uint32_t extClk;	// MF: Don't know how we will use it
	//uint32_t frg0;	// MF: Don't know how we will use it

	uint32_t coreSysClk_Ctor;
	uint32_t mainClk_Ctor;
	uint32_t fro_Ctor;
	uint32_t lpo_Ctor;

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

#endif // ALX_CLK_MCU_LPC80X_H
