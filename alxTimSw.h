/**
  ******************************************************************************
  * @file alxTimSw.h
  * @brief Auralix C Library - ALX Timer SW Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_TIM_SW_H
#define ALX_TIM_SW_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	uint64_t ticksStart_ns;
	bool isRunning;

	//Info
	bool wasCtorCalled;
} AlxTimSw;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTimSw_Ctor(AlxTimSw* me, bool start);


//******************************************************************************
// Functions
//******************************************************************************
void AlxTimSw_Start(AlxTimSw* me);
void AlxTimSw_Stop(AlxTimSw* me);
bool AlxTimSw_IsRunning(AlxTimSw* me);
uint64_t AlxTimSw_Get_ns(AlxTimSw* me);
uint64_t AlxTimSw_Get_us(AlxTimSw* me);
uint64_t AlxTimSw_Get_ms(AlxTimSw* me);
uint64_t AlxTimSw_Get_sec(AlxTimSw* me);
uint64_t AlxTimSw_Get_min(AlxTimSw* me);
uint64_t AlxTimSw_Get_hr(AlxTimSw* me);
bool AlxTimSw_IsTimeout_ns(AlxTimSw* me, uint64_t timeout_ns);
bool AlxTimSw_IsTimeout_us(AlxTimSw* me, uint64_t timeout_us);
bool AlxTimSw_IsTimeout_ms(AlxTimSw* me, uint64_t timeout_ms);
bool AlxTimSw_IsTimeout_sec(AlxTimSw* me, uint64_t timeout_sec);
bool AlxTimSw_IsTimeout_min(AlxTimSw* me, uint64_t timeout_min);
bool AlxTimSw_IsTimeout_hr(AlxTimSw* me, uint64_t timeout_hr);


#ifdef __cplusplus
}
#endif

#endif // ALX_TIM_SW_H
