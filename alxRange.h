/**
  ******************************************************************************
  * @file alxRange.h
  * @brief Auralix C Library - ALX Range Module
  * @version $LastChangedRevision: 5338 $
  * @date $LastChangedDate: 2021-06-07 11:51:21 +0200 (Mon, 07 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_RANGE_H
#define ALX_RANGE_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_RANGE_FILE "alxRange"

// Assert //
#if defined(_ALX_RANGE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RANGE_FILE, expr)
#elif defined(_ALX_RANGE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RANGE_FILE, expr)
#elif defined(_ALX_RANGE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_RST(ALX_RANGE_FILE, expr)
#else
	#define ALX_RANGE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_RANGE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_RANGE_TRACE(...) ALX_TRACE_STD(ALX_RANGE_FILE, __VA_ARGS__)
#else
	#define ALX_RANGE_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_RANGE_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_RANGE_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_RANGE_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRange_CheckUint8		(uint8_t val, uint8_t valMin, uint8_t valMax	);
Alx_Status AlxRange_CheckUint16		(uint16_t val, uint16_t valMin, uint16_t valMax	);
Alx_Status AlxRange_CheckUint32		(uint32_t val, uint32_t valMin, uint32_t valMax	);
Alx_Status AlxRange_CheckUint64		(uint64_t val, uint64_t valMin, uint64_t valMax	);
Alx_Status AlxRange_CheckInt8		(int8_t val, int8_t valMin, int8_t valMax		);
Alx_Status AlxRange_CheckInt16		(int16_t val, int16_t valMin, int16_t valMax	);
Alx_Status AlxRange_CheckInt32		(int32_t val, int32_t valMin, int32_t valMax	);
Alx_Status AlxRange_CheckInt64		(int64_t val, int64_t valMin, int64_t valMax	);
Alx_Status AlxRange_CheckFloat		(float val, float valMin, float valMax			);
Alx_Status AlxRange_CheckDouble		(double val, double valMin, double valMax		);
//Alx_Status AlxRange_CheckArr		(void* val, uint32_t valLenMax					);	// TO DO
Alx_Status AlxRange_CheckStr		(char* val, uint32_t valLenMax					);


#ifdef __cplusplus
}
#endif

#endif // ALX_RANGE_H
