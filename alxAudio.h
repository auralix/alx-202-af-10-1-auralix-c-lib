/**
  ******************************************************************************
  * @file alxAudio.h
  * @brief Auralix C Library - ALX Audio Module
  * @version $LastChangedRevision: 6065 $
  * @date $LastChangedDate: 2021-09-17 17:35:23 +0200 (Fri, 17 Sep 2021) $
  ******************************************************************************
  */

#ifndef ALX_AUDIO_H
#define ALX_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_FILE "alxAudio"

// Assert //
#if defined(_ALX_AUDIO_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_FILE, expr)
#elif defined(_ALX_AUDIO_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_FILE, expr)
#elif defined(_ALX_AUDIO_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_AUDIO_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_FILE, expr)
#else
	#define ALX_AUDIO_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_AUDIO_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_AUDIO_TRACE(...) ALX_TRACE_STD(ALX_AUDIO_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_AUDIO_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_AUDIO_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_AUDIO_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxAudio_Encoding_LinerPcm_Int8   = 0,
	AlxAudio_Encoding_LinerPcm_Uint8  = 1,
	AlxAudio_Encoding_LinerPcm_Int16  = 2,
	AlxAudio_Encoding_LinerPcm_Uint16 = 3,
	AlxAudio_Encoding_LinerPcm_Int16_InternalFlash  = 4
} AlxAudio_Encoding;


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudio_LinerPcmInt8ToFloat(int8_t inSample);
float AlxAudio_LinerPcmUint8ToFloat(uint8_t inSample);
float AlxAudio_LinerPcmInt16ToFloat(int16_t inSample);
float AlxAudio_LinerPcmUint16ToFloat(uint16_t inSample);
int8_t AlxAudio_FloatToLinerPcmInt8(float inSample);
int16_t AlxAudio_FloatToLinerPcmInt16(float inSample);
float AlxAudio_StereoToMono(float sampleL, float sampleR);


#ifdef __cplusplus
}
#endif

#endif // ALX_AUDIO_GLOBAL_H
