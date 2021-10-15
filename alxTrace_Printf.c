/**
  ******************************************************************************
  * @file alxTrace_Printf.c
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace_Printf.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_PC) || defined(ALX_MBED)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me
)
{
	(void)me;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	(void)me;

	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	(void)me;

	return Alx_Ok;
}
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	(void)me;
	printf(str);

	return Alx_Ok;
}


#endif
