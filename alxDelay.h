/**
  ******************************************************************************
  * @file alxDelay.h
  * @brief Auralix C Library - ALX Delay Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_DELAY_H
#define ALX_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Functions
//******************************************************************************
void AlxDelay_ns(uint64_t delay_ns);
void AlxDelay_us(uint64_t delay_us);
void AlxDelay_ms(uint64_t delay_ms);
void AlxDelay_sec( uint64_t delay_sec);
void AlxDelay_min(uint64_t delay_min);
void AlxDelay_hr(uint64_t delay_hr);


#ifdef __cplusplus
}
#endif

#endif // ALX_TIM_H
