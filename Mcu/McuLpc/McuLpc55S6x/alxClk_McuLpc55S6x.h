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
	/*AlxClk_Config_McuLpc55S6x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default = 0,
	AlxClk_Config_McuLpc55S6x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz = 1,
	AlxClk_Config_McuLpc55S6x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz = 2,
	AlxClk_Config_McuLpc55S6x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz = 3,
	AlxClk_Config_McuLpc55S6x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz = 4,
	AlxClk_Config_McuLpc55S6x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz = 5,
	AlxClk_Config_McuLpc55S6x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5 = 6,
	AlxClk_Config_McuLpc55S6x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz = 7,
	AlxClk_Config_McuLpc55S6x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5 = 8,*/

	// To so vsi main Clk
	AlxClk_Config_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default = 0,
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_FroOsc_96MHz = 1,	// MF: Hf mean High Speed
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_FroOsc_1MHz = 2,
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_ExtOsc_ = 3,
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_Pll0 = 4,
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_Pll1 = 5,
	AlxClk_Config_McuLpc55S6x_SysClk_xxxHz_RtcOsc_32kHz = 6,
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
