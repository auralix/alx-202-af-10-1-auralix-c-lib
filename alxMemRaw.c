/**
  ******************************************************************************
  * @file alxMemRaw.c
  * @brief Auralix C Library - ALX Memory Raw Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMemRaw.h"


//******************************************************************************
// Constructor
//******************************************************************************
ALX_WEAK void AlxMemRaw_Ctor
(
	AlxMemRaw* me
)
{
	(void)me;
}


//******************************************************************************
// Functions
//******************************************************************************
ALX_WEAK Alx_Status AlxMemRaw_Init(AlxMemRaw* me)
{
	(void)me;
}
ALX_WEAK Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me);
ALX_WEAK Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms);
ALX_WEAK Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms);
