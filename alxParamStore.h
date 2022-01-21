/**
  ******************************************************************************
  * @file alxParamStore.h
  * @brief Auralix C Library - ALX Parameter Store Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_PARAM_STORE_H
#define ALX_PARAM_STORE_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxParamGroup.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_STORE_FILE "alxParamStore"

// Assert //
#if defined(_ALX_PARAM_STORE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_STORE_FILE, expr)
#elif defined(_ALX_PARAM_STORE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_STORE_FILE, expr)
#elif defined(_ALX_PARAM_STORE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_STORE_FILE, expr)
#else
	#define ALX_PARAM_STORE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_STORE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_STORE_TRACE(...) ALX_TRACE_STD(ALX_PARAM_STORE_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_STORE_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_PARAM_STORE_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PARAM_STORE_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PARAM_STORE_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxParamStore_St_Init = 0,
	AlxParamStore_St_Err = 1,
	AlxParamStore_St_CheckingGroup1 = 2,
	AlxParamStore_St_CheckingGroup2 = 3,
	AlxParamStore_St_CheckingGroup3 = 4,
	AlxParamStore_St_CheckingGroup4 = 5,
	AlxParamStore_St_WritingGroup1 = 6,
	AlxParamStore_St_WritingGroup2 = 7,
	AlxParamStore_St_WritingGroup3 = 8,
	AlxParamStore_St_WritingGroup4 = 9
} AlxParamStore_St;

typedef struct
{
	// Parameters
	AlxParamGroup** paramGroups;
	uint8_t numOfParamGroups;

	// Variables
	bool isParamGroupInitErr;

	// SM
	AlxParamStore_St st;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxParamStore;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamStore_Ctor
(
	AlxParamStore* me,
	AlxParamGroup** paramGroups,
	uint32_t numOfParamGroups
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamStore_Init(AlxParamStore* me, Alx_Status* status, uint8_t numOfParamGroups);
void AlxParamStore_Handle(AlxParamStore* me);
bool AlxParamStore_IsErr(AlxParamStore* me);

#ifdef __cplusplus
}
#endif

#endif // ALX_PARAM_STORE_H
