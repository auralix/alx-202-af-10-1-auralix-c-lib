//******************************************************************************
// @file alxParamStore.h
// @brief Auralix C Library - ALX Parameter Store Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

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
#define ALX_PARAM_STORE_FILE "alxParamStore.h"

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
	AlxParamGroup** paramGroupArr;
	uint8_t numOfParamGroups;

	// Variables
	AlxParamStore_St st;
	bool isParamGroupInitErr;

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
	AlxParamGroup** paramGroupArr,
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

#endif	// #ifndef ALX_PARAM_STORE_H
