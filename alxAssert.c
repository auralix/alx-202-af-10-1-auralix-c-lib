/**
  ******************************************************************************
  * @file alxAssert.c
  * @brief Auralix C Library - ALX Assert Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAssert.h"


//******************************************************************************
// Functions
//******************************************************************************
void ALX_WEAK AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun)
{
	(void)file;
	(void)line;
	(void)fun;

	ALX_BKPT;
}
void ALX_WEAK AlxAssert_Trace(const char* file, uint32_t line, const char* fun)
{
	AlxTrace_WriteStd(&alxTrace, file, line, fun, "ASSERT");
}
void ALX_WEAK AlxAssert_Rst(const char* file, uint32_t line, const char* fun)
{
	(void)file;
	(void)line;
	(void)fun;
}
