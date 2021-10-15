/**
  ******************************************************************************
  * @file alxAudio.c
  * @brief Auralix C Library - ALX Audio Module
  * @version $LastChangedRevision: 5176 $
  * @date $LastChangedDate: 2021-05-19 21:10:16 +0200 (Wed, 19 May 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAudio.h"


//******************************************************************************
// Functions
//******************************************************************************
float AlxAudio_LinerPcmInt8ToFloat(int8_t inSample)
{
	float outSample = (float)inSample / 128.f;
	return outSample;
}
float AlxAudio_LinerPcmUint8ToFloat(uint8_t inSample)
{
	uint8_t sampleUint8 = inSample - (uint8_t)128;	// Converts decimal to 2's complement
	float outSample = (float)((int8_t)sampleUint8 / 128.f);
	return outSample;
}
float AlxAudio_LinerPcmInt16ToFloat(int16_t inSample)
{
	float outSample = (float)(inSample / 32768.f);
	return outSample;
}
float AlxAudio_LinerPcmUint16ToFloat(uint16_t inSample)
{
	uint16_t dummy = inSample - 32768U;	// Converts decimal to 2's complement
	float outSample = (float)((int16_t)dummy / 32768.f);
	return outSample;
}
int8_t AlxAudio_FloatToLinerPcmInt8(float inSample)
{
	ALX_AUDIO_ASSERT((-1.f <= inSample) && (inSample <= 1.f));
	
	int8_t outSample = (int8_t)(inSample * 127.f);
	return outSample;
}
int16_t AlxAudio_FloatToLinerPcmInt16(float inSample)
{
	ALX_AUDIO_ASSERT((-1.f <= inSample) && (inSample <= 1.f));
	
	int16_t outSample = (int16_t)(inSample * 32767.f);
	return outSample;
}
float AlxAudio_StereoToMono(float sampleL, float sampleR)
{
	ALX_AUDIO_ASSERT((-1.f <= sampleL) && (sampleL <= 1.f));
	ALX_AUDIO_ASSERT((-1.f <= sampleR) && (sampleR <= 1.f));
	
	float outSample = (sampleL + sampleR) / 2.f;
	return outSample;
}
