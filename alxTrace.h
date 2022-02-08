/**
  ******************************************************************************
  * @file alxTrace.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5720 $
  * @date $LastChangedDate: 2021-08-02 23:21:08 +0200 (Mon, 02 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_H
#define ALX_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"

// AlxMcu //
#if (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)) && (!defined(ALX_MBED))
#include "alxTrace_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxTrace_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxTrace_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxTrace_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxTrace_McuLpc55S6x.h"

#elif defined(ALX_PC) || defined(ALX_MBED)
#include "alxTrace_Printf.h"

#else
typedef struct { bool dummy; } AlxTrace;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TRACE_STR(str) do								{ AlxTrace_WriteStr(&alxTrace, str); } while(false)
#define ALX_TRACE_FORMAT(...) do							{ AlxTrace_WriteFormat(&alxTrace, __VA_ARGS__); } while(false)
#define ALX_TRACE_STD(file, ...) do							{ AlxTrace_WriteStd(&alxTrace, file, __LINE__, __func__, __VA_ARGS__); } while(false)
#define ALX_TRACE_SM(smLevel, smName, stName, acName) do	{ AlxTrace_WriteSm(&alxTrace, smLevel, smName, stName, acName); } while(false)


//******************************************************************************
// Variables
//******************************************************************************
extern AlxTrace alxTrace;


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me);
Alx_Status AlxTrace_DeInit(AlxTrace* me);
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str);


//******************************************************************************
// Common Functions
//******************************************************************************
void AlxTrace_WriteFormat(AlxTrace* me, const char* format, ...);
void AlxTrace_WriteStd(AlxTrace* me, const char* file, uint32_t line, const char* fun, const char* format, ...);
void AlxTrace_WriteSm(AlxTrace* me, uint8_t smLevel, const char* smName, const char* stName, const char* acName);
void AlxTrace_GetSmLevelStr(uint32_t smLevel, char* smLevelStr);


#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_H
