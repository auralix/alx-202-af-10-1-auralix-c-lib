/**
  ******************************************************************************
  * @file alxAssert.h
  * @brief Auralix C Library - ALX Assert Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_ASSERT_H
#define ALX_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ASSERT_BKPT(file, expr) if (expr) {} else { AlxAssert_Bkpt(file, __LINE__, __func__); }
#define ALX_ASSERT_TRACE(file, expr) if (expr) {} else { AlxAssert_Trace(file, __LINE__, __func__); }
#define ALX_ASSERT_RST(file, expr) if (expr) {} else { AlxAssert_Rst(file, __LINE__, __func__); }


//******************************************************************************
// Functions
//******************************************************************************
void AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun);
void AlxAssert_Trace(const char* file, uint32_t line, const char* fun);
void AlxAssert_Rst(const char* file, uint32_t line, const char* fun);


#ifdef __cplusplus
}
#endif

#endif // ALX_ASSERT_H
