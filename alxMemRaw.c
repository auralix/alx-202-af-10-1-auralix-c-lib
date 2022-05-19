//******************************************************************************
// @file alxMemRaw.c
// @brief Auralix C Library - ALX Memory Raw Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


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
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
