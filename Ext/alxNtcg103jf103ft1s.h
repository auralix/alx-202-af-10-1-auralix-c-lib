/**
  ******************************************************************************
  * @file alxNtcg103jf103ft1s.h
  * @brief Auralix C Library - ALX RTD NTCG103JF103FT1S Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_NTCG103JF103FT1S_H
#define ALX_NTCG103JF103FT1S_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_NTCG103JF103FT1S_NUM_OF_POINTS 191


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	float xPointRes_kOhm[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	float yPointTemp_degC[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	uint32_t numOfPoints;
} AlxNtcg103jf103ft1s;


//******************************************************************************
// Variables
//******************************************************************************
extern const AlxNtcg103jf103ft1s alxNtcg103jf103ft1s;


#ifdef __cplusplus
}
#endif

#endif // ALX_NTCG103JF103FT1S_H
