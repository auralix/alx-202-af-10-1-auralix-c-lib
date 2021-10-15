/**
  ******************************************************************************
  * @file alxParamMgmt.h
  * @brief Auralix C Library - ALX Parameter Management Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_PARAM_MGMT_H
#define ALX_PARAM_MGMT_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxParamItem.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_MGMT_FILE "alxParamMgmt.h"

// Assert //
#if defined(_ALX_PARAM_MGMT_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_MGMT_FILE, expr)
#elif defined(_ALX_PARAM_MGMT_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_MGMT_FILE, expr)
#elif defined(_ALX_PARAM_MGMT_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_MGMT_FILE, expr)
#else
	#define ALX_PARAM_MGMT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_MGMT_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_MGMT_TRACE(...) ALX_TRACE_STD(ALX_PARAM_MGMT_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_MGMT_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_PARAM_MGMT_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PARAM_MGMT_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PARAM_MGMT_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxParamItem** paramItemArr;
	
	// Parameters
	uint32_t numOfParamItems;

	// Variables

	// Info
	bool wasCtorCalled;
} AlxParamMgmt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem** paramItemArr,
	uint8_t numOfParamItems
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxParamMgmt_GetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);
void AlxParamMgmt_GetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
void AlxParamMgmt_SetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);
void AlxParamMgmt_SetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
void AlxParamMgmt_SetToDefByName(AlxParamMgmt* me, const char* name, uint32_t len);
void AlxParamMgmt_SetToDefById(AlxParamMgmt* me, uint32_t id, uint32_t len);
void AlxParamMgmt_SetToDefGroup(AlxParamMgmt* me, uint8_t groupId);
void AlxParamMgmt_SetToDefAll(AlxParamMgmt* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_PARAM_MGMT_H
