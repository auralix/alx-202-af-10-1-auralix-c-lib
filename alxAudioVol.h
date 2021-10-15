/**
  ******************************************************************************
  * @file alxAudioVol.h
  * @brief Auralix C Library - ALX Audio Volume Module
  * @version $LastChangedRevision: 5176 $
  * @date $LastChangedDate: 2021-05-19 21:10:16 +0200 (Wed, 19 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_AUDIO_VOL_H
#define ALX_AUDIO_VOL_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxLinFun.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_VOL_FILE "alxAudioVol"

// Assert //
#if defined(_ALX_AUDIO_VOL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_VOL_FILE, expr)
#elif defined(_ALX_AUDIO_VOL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_VOL_FILE, expr)
#elif defined(_ALX_AUDIO_VOL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_AUDIO_VOL_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_VOL_FILE, expr)
#else
	#define ALX_AUDIO_VOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_AUDIO_VOL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_AUDIO_VOL_TRACE(...) ALX_TRACE_STD(ALX_AUDIO_VOL_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_VOL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_AUDIO_VOL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_AUDIO_VOL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_AUDIO_VOL_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float volMin_dB;
	float volMax_dB;
	
	// Objects - Internal
	AlxLinFun linFun_x_pct_y_dB;
	
	// Variables
	float pctMax;
	float volFactor;	// Can be negative value for attenuation
} AlxAudioVol;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAudioVol_Ctor
(
	AlxAudioVol* me,
	float volMin_dB,
	float volMax_dB
);


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudioVol_Process(AlxAudioVol* me, float inSample);
void AlxAudioVol_Set_pct(AlxAudioVol* me, float vol_pct);
void AlxAudioVol_Set_dB(AlxAudioVol* me, float vol_dB);


#ifdef __cplusplus
}
#endif

#endif // ALX_AUDIO_VOL_H
