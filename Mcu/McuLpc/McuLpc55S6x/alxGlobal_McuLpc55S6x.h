/**
  ******************************************************************************
  * @file alxGlobal_McuLpc55S6x.h
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_GLOBAL_MCU_LPC55S6X_H
#define ALX_GLOBAL_MCU_LPC55S6X_H

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
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Includes
//******************************************************************************
#include "fsl_anactrl.h"
#include "fsl_casper.h"
#include "fsl_clock.h"
#include "fsl_cmp.h"
#include "fsl_common.h"
#include "fsl_common_arm.h"
#include "fsl_crc.h"
#include "fsl_ctimer.h"
#include "fsl_dma.h"
#include "fsl_flexcomm.h"
#include "fsl_fro_calib.h"
#include "fsl_gint.h"
#include "fsl_gpio.h"
#include "fsl_hashcrypt.h"
#include "fsl_i2c.h"
#include "fsl_i2c_dma.h"
//#include "fsl_i2c_freertos.h"
#include "fsl_i2s.h"
#include "fsl_i2s_dma.h"
#include "fsl_iap.h"
#include "fsl_iap_ffr.h"
#include "fsl_iap_kbp.h"
#include "fsl_iap_skboot_authenticate.h"
#include "fsl_inputmux.h"
#include "fsl_inputmux_connections.h"
#include "fsl_iocon.h"
#include "fsl_lpadc.h"
#include "fsl_mailbox.h"
#include "fsl_mrt.h"
#include "fsl_ostimer.h"
#include "fsl_pint.h"
#include "fsl_plu.h"
#include "fsl_power.h"
//#include "fsl_prince.h"	// MF: error: redeclaration of enumerator 'kSECURE_FALSE'
#include "fsl_puf.h"
#include "fsl_reset.h"
#include "fsl_rng.h"
#include "fsl_rtc.h"
#include "fsl_sctimer.h"
#include "fsl_sdif.h"
#include "fsl_spi.h"
#include "fsl_spi_dma.h"
//#include "fsl_spi_freertos.h"
#include "fsl_sysctl.h"
#include "fsl_usart.h"
#include "fsl_usart_dma.h"
//#include "fsl_usart_freertos.h"
#include "fsl_utick.h"
#include "fsl_wwdt.h"


//******************************************************************************
// IoPin Functions
//******************************************************************************
//uint8_t AlxPROTECTED_IoPin_GetIoconPortPinIndex(uint8_t pin, uint8_t port);
//void AlxPROTECTED_IoPin_SetIoconMode(uint8_t pin, uint8_t port, uint32_t mode);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_GLOBAL_MCU_LPC55S6X_H
