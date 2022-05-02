/**
  ******************************************************************************
  * @file alxParamGroup.h
  * @brief Auralix C Library - ALX Parameter Group Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_PARAM_GROUP_H
#define ALX_PARAM_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxParamItem.h"
#include "alxMemSafe.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_GROUP_FILE "alxParamGroup"

// Assert //
#if defined(_ALX_PARAM_GROUP_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_GROUP_FILE, expr)
#elif defined(_ALX_PARAM_GROUP_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_GROUP_FILE, expr)
#elif defined(_ALX_PARAM_GROUP_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_GROUP_FILE, expr)
#else
	#define ALX_PARAM_GROUP_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_GROUP_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_GROUP_TRACE(...) ALX_TRACE_STD(ALX_PARAM_GROUP_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_GROUP_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_PARAM_GROUP_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PARAM_GROUP_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PARAM_GROUP_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxMemSafe* memSafe;

	// Parameters
	const char* name;
	uint32_t len;
	uint8_t* valBuff;
	uint8_t* valStoredBuff;
	uint8_t* valToStoreBuff;
	AlxParamItem** paramItemArr;
	uint32_t numOfParamItems;
	uint8_t initNumOfTries;

	// Variables

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxParamGroup;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamGroup_Ctor
(
	AlxParamGroup* me,
	AlxMemSafe* memSafe,
	const char* name,
	uint32_t len,
	uint8_t* valBuff,
	uint8_t* valStoredBuff,
	uint8_t* valToStoreBuff,
	AlxParamItem** paramItemArr,
	uint32_t numOfParamItems,
	uint8_t initNumOfTries
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamGroup_Init(AlxParamGroup* me);
void AlxParamGroup_Write(AlxParamGroup* me);
bool AlxParamGroup_IsWriteDone(AlxParamGroup* me);
bool AlxParamGroup_IsWriteErr(AlxParamGroup* me);
bool AlxParamGroup_IsValStoredBuffDiff(AlxParamGroup* me);
void AlxParamGroup_ValBuffToValToStoreBuff(AlxParamGroup* me);
void AlxParamGroup_ValToStoreBuffToValStoredBuff(AlxParamGroup* me);
void AlxParamGroup_ParamItemsValToValBuff(AlxParamGroup* me);

#ifdef __cplusplus
}
#endif

#endif // ALX_PARAM_GROUP_H
