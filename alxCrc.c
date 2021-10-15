/**
  ******************************************************************************
  * @file alxCrc.c
  * @brief Auralix C Library - ALX CRC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCrc.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCrc_Ctor
(
	AlxCrc* me,
	AlxCrc_Config config
)
{
}


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxCrc_Calc(AlxCrc* me, uint8_t* data, uint32_t len)
{
}
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc)
{
}
uint32_t AlxCrc_GetLen(AlxCrc* me)
{
}
