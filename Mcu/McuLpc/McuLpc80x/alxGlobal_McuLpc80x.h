/**
  ******************************************************************************
  * @file alxGlobal_McuLpc80x.h
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_GLOBAL_MCU_LPC80X_H
#define ALX_GLOBAL_MCU_LPC80X_H

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
#if defined(ALX_LPC80X)


//******************************************************************************
// Includes
//******************************************************************************
#include "fsl_acomp.h"
#include "fsl_adc.h"
#include "fsl_capt.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_crc.h"
#include "fsl_ctimer.h"
#include "fsl_dac.h"
//#include "fsl_dma.h"						// MF: Doesn't work on Lpc804
#include "fsl_gpio.h"
#include "fsl_i2c.h"
#include "fsl_iap.h"
//#include "fsl_inputmux.h"					// MF: Doesn't work on Lpc804
//#include "fsl_inputmux_connections.h"		// MF: Doesn't work on Lpc804
#include "fsl_iocon.h"
#include "fsl_mrt.h"
#include "fsl_pint.h"
#include "fsl_power.h"
#include "fsl_reset.h"
//#include "fsl_sctimer.h"					// MF: Doesn't work on Lpc804
#include "fsl_spi.h"
#include "fsl_swm.h"
#include "fsl_swm_connections.h"
#include "fsl_syscon.h"
#include "fsl_syscon_connections.h"
#include "fsl_usart.h"
#include "fsl_wkt.h"
#include "fsl_wwdt.h"
#include "rom_api.h"	// MF: We need this include to directly set FRO Freq -> "LPC_PWRD_API->set_fro_frequency((uint32_t)freq);" This is for optimization purpose


//******************************************************************************
// IoPin Functions
//******************************************************************************
uint8_t AlxPROTECTED_IoPin_GetIoconPortPinIndex(uint8_t pin, uint8_t port);
void AlxPROTECTED_IoPin_SetIoconMode(uint8_t pin, uint8_t port, uint32_t mode);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_GLOBAL_MCU_LPC80X_H
