/**
  ******************************************************************************
  * @file alxVdiv.h
  * @brief Auralix C Library - ALX Voltage Divider Module
  * @version $LastChangedRevision: 5338 $
  * @date $LastChangedDate: 2021-06-07 11:51:21 +0200 (Mon, 07 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_V_DIV_H
#define ALX_V_DIV_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_V_DIV_FILE "alxVdiv"

// Assert //
#if defined(_ALX_V_DIV_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_BKPT(ALX_V_DIV_FILE, expr)
#elif defined(_ALX_V_DIV_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_TRACE(ALX_V_DIV_FILE, expr)
#elif defined(_ALX_V_DIV_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_RST(ALX_V_DIV_FILE, expr)
#else
	#define ALX_V_DIV_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_V_DIV_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_V_DIV_TRACE(...) ALX_TRACE_STD(ALX_V_DIV_FILE, __VA_ARGS__)
#else
	#define ALX_V_DIV_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_V_DIV_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_V_DIV_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_V_DIV_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************

//******************************************************************************
// Constructor
//******************************************************************************

//******************************************************************************
// Functions
//******************************************************************************
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm);
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm);
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm);
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm);


#ifdef __cplusplus
}
#endif

#endif // ALX_V_DIV_H
