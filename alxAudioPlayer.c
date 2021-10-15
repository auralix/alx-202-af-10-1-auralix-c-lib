﻿/**
  ******************************************************************************
  * @file alxAudioPlayer.c
  * @brief Auralix C Library - ALX Audio Player Module
  * @version $LastChangedRevision: 6118 $
  * @date $LastChangedDate: 2021-09-23 17:51:52 +0200 (Thu, 23 Sep 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAudioPlayer.h"


//******************************************************************************
// Private Types
//******************************************************************************
typedef enum
{
	AlxAudioPlayer_SampleType_L = 0,
	AlxAudioPlayer_SampleType_R = 1,
	AlxAudioPlayer_SampleType_Mono = 2,
} AlxAudioPlayer_SampleType;


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAudioPlayer_GetSampleIncStep_Byte(AlxAudioPlayer* me);
static float AlxAudioPlayer_GetSample(AlxAudioPlayer* me, AlxAudioPlayer_SampleType sampleType);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAudioPlayer_Ctor
(
	AlxAudioPlayer* me,
	const uint8_t* defaultTrackPtr,
	uint32_t defaultTrackLen_Byte,
	uint32_t defaultTrackStartOffset_Sample,
	AlxAudio_Encoding defaultTrackEncoding,
	bool isDefaultTrackMono
)
{
	// Parameters - Const
	me->TRACK_SAMPLE_R_OFFSET_8bit_Byte = 1;
	me->TRACK_SAMPLE_R_OFFSET_16bit_Byte = 2;
	
	// Parameters
	me->trackPtr = defaultTrackPtr;
	me->trackLen_Byte = defaultTrackLen_Byte;
	me->trackEncoding = defaultTrackEncoding;
	me->isTrackMono = isDefaultTrackMono;

	// Variables
	me->trackSampleIncStep_Byte = AlxAudioPlayer_GetSampleIncStep_Byte(me);
	me->trackSampleOffset_Byte = (me->trackSampleIncStep_Byte * defaultTrackStartOffset_Sample) % defaultTrackLen_Byte; 
	me->isPlaying = false;
	me->isLoopOn = false;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxAudioPlayer_LoadTrack(AlxAudioPlayer* me, const uint8_t* trackPtr, uint32_t len_Byte, uint32_t startOffset_Sample, AlxAudio_Encoding encoding, bool isMono)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	// Parameters
	me->trackPtr = trackPtr;
	me->trackLen_Byte = len_Byte;
	me->trackEncoding = encoding;
	me->isTrackMono = isMono;

	// Variables
	me->trackSampleIncStep_Byte = AlxAudioPlayer_GetSampleIncStep_Byte(me);
	me->trackSampleOffset_Byte = (me->trackSampleIncStep_Byte * startOffset_Sample) % len_Byte;
}
float AlxAudioPlayer_GetSampleL(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	return AlxAudioPlayer_GetSample(me, AlxAudioPlayer_SampleType_L);
}
float AlxAudioPlayer_GetSampleR(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	return AlxAudioPlayer_GetSample(me, AlxAudioPlayer_SampleType_R);
}
float AlxAudioPlayer_GetSampleMono(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	return AlxAudioPlayer_GetSample(me, AlxAudioPlayer_SampleType_Mono);
}
void AlxAudioPlayer_IncSampleOffset(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	if
	(
		(me->isLoopOn == false) &&
		(me->trackSampleOffset_Byte == (me->trackLen_Byte - me->trackSampleIncStep_Byte))	// If we came to end of track and loop is not enabled, STOP player
	)
	{
		AlxAudioPlayer_Stop(me);
	}
	else
	{
		me->trackSampleOffset_Byte = (me->trackSampleOffset_Byte + me->trackSampleIncStep_Byte) % me->trackLen_Byte;	// Increment offset, rewind if necessary
	}
}
void AlxAudioPlayer_Play(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	me->isPlaying = true;
}
void AlxAudioPlayer_Stop(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	me->trackSampleOffset_Byte = 0;	// Rewind current track to beginning
	me->isPlaying = false;
}
void AlxAudioPlayer_Pause(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	me->isPlaying = false;
}
void AlxAudioPlayer_Replay(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	AlxAudioPlayer_Stop(me);
	AlxAudioPlayer_Play(me);
}
void AlxAudioPlayer_LoopOn(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	me->isLoopOn = true;
}
void AlxAudioPlayer_LoopOff(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	me->isLoopOn = false;
}
void AlxAudioPlayer_LoopConfig(AlxAudioPlayer* me, bool isOn)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);
	
	me->isLoopOn = isOn;
}
bool AlxAudioPlayer_IsPlaying(AlxAudioPlayer* me)
{
	ALX_AUDIO_PLAYER_ASSERT(me->wasCtorCalled == true);

	return me->isPlaying;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAudioPlayer_GetSampleIncStep_Byte(AlxAudioPlayer* me)
{
	uint8_t trackSampleIncStep_Byte = 0;
	
	if
	(
		(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int8) ||
		(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Uint8)
	)
	{
		if (me->isTrackMono)
			trackSampleIncStep_Byte = 1;
		else
			trackSampleIncStep_Byte = 2;
	}
	else if
	(
		(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int16) ||
		(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Uint16) ||
		(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int16_InternalFlash)
	)
	{
		if (me->isTrackMono)
			trackSampleIncStep_Byte = 2;
		else
			trackSampleIncStep_Byte = 4;
	}
	else
	{
		ALX_AUDIO_PLAYER_ASSERT(false);	// We should never get here
	}
	
	return trackSampleIncStep_Byte;
}
float AlxAudioPlayer_GetSample(AlxAudioPlayer* me, AlxAudioPlayer_SampleType sampleType)
{
	// #1 Variables
	float sampleOut = 0.f;
	
	// #2 Player is playing
	if (me->isPlaying)
	{
		// #2.1 Encoding - Liner PCM Int 8
		if (me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int8)
		{
			// #2.1.1 Check if track is mono
			if (me->isTrackMono)
			{
				// Left
				int8_t sampleInt8L = *(me->trackPtr + me->trackSampleOffset_Byte);
				sampleOut = AlxAudio_LinerPcmInt8ToFloat(sampleInt8L); 		// If track is mono we always return left channel (no matter what type is selected)
			}
			// #2.1.2 Track is stereo
			else
			{
				if (sampleType == AlxAudioPlayer_SampleType_L)
				{
					// Left
					int8_t sampleInt8L = *(me->trackPtr + me->trackSampleOffset_Byte);
					sampleOut = AlxAudio_LinerPcmInt8ToFloat(sampleInt8L);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_R)
				{
					// Right
					int8_t sampleInt8R = *(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_8bit_Byte);
					sampleOut = AlxAudio_LinerPcmInt8ToFloat(sampleInt8R);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_Mono)
				{
					// Left
					int8_t sampleInt8L = *(me->trackPtr + me->trackSampleOffset_Byte);
					float sampleFloatL = AlxAudio_LinerPcmInt8ToFloat(sampleInt8L);
					sampleOut = sampleFloatL;
					// Right
					int8_t sampleInt8R = *(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_8bit_Byte);
					float sampleFloatR = AlxAudio_LinerPcmInt8ToFloat(sampleInt8R);
					// Mono
					sampleOut = AlxAudio_StereoToMono(sampleFloatL, sampleFloatR);
				}
				else
				{
					ALX_AUDIO_PLAYER_ASSERT(false);	// We should never get here
				}
			}
		}
		// #2.2 Encoding - Liner PCM Uint 8
		else if(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Uint8)
		{
			// #2.2.1 Check if track is mono
			if(me->isTrackMono)
			{
				// Left
				uint8_t sampleUint8L = *(me->trackPtr + me->trackSampleOffset_Byte);
				sampleOut = AlxAudio_LinerPcmUint8ToFloat(sampleUint8L);	// If track is mono we always return left channel (no matter what type is selected)
			}
			// #2.2.2 Track is stereo
			else
			{
				if (sampleType == AlxAudioPlayer_SampleType_L)
				{
					// Left
					uint8_t sampleUint8L = *(me->trackPtr + me->trackSampleOffset_Byte);
					sampleOut = AlxAudio_LinerPcmUint8ToFloat(sampleUint8L);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_R)
				{
					// Right
					uint8_t sampleUint8R = *(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_8bit_Byte);
					sampleOut = AlxAudio_LinerPcmUint8ToFloat(sampleUint8R);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_Mono)
				{
					// Left
					uint8_t sampleUint8L = *(me->trackPtr + me->trackSampleOffset_Byte);
					float sampleFloatL = AlxAudio_LinerPcmUint8ToFloat(sampleUint8L);
					// Right
					uint8_t sampleUint8R = *(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_8bit_Byte);
					float sampleFloatR = AlxAudio_LinerPcmUint8ToFloat(sampleUint8R);
					// Mono
					sampleOut = AlxAudio_StereoToMono(sampleFloatL, sampleFloatR);
				}
				else
				{
					ALX_AUDIO_PLAYER_ASSERT(false); 	// We should never get here
				}
			}
		}
		// #2.3 Encoding - Liner PCM Int 16
		else if (me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int16)
		{
			// TODO
			ALX_AUDIO_PLAYER_ASSERT(false);
		}
		// #2.4 Encoding - Liner PCM Uint 16
		else if(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Uint16)
		{
			// TODO
			ALX_AUDIO_PLAYER_ASSERT(false);
		}
		// #2.5 Encoding - Liner PCM Int 16
		else if(me->trackEncoding == AlxAudio_Encoding_LinerPcm_Int16_InternalFlash)
		{
			// #2.3.1 Check if track is mono
			if (me->isTrackMono)
			{
				int16_t sampleInt16L = *((volatile int16_t *)(me->trackPtr + me->trackSampleOffset_Byte));
				float sampleFloatL = AlxAudio_LinerPcmInt16ToFloat(sampleInt16L);
				sampleOut = sampleFloatL;	// If track is mono we always return left channel (no matter what type is selected)
			}
			// #2.3.2 Track is stereo
			else
			{
				if (sampleType == AlxAudioPlayer_SampleType_L)
				{
					// Left
					int16_t sampleInt16L = *((volatile int16_t *)(me->trackPtr + me->trackSampleOffset_Byte));
					sampleOut = AlxAudio_LinerPcmInt16ToFloat(sampleInt16L);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_R)
				{			
					// Right
					int16_t sampleInt16R = *((volatile int16_t *)(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_16bit_Byte));
					sampleOut = AlxAudio_LinerPcmInt16ToFloat(sampleInt16R);
				}
				else if (sampleType == AlxAudioPlayer_SampleType_Mono)
				{
					// Left
					int16_t sampleInt16L = *((volatile int16_t *)(me->trackPtr + me->trackSampleOffset_Byte));
					float sampleFloatL = AlxAudio_LinerPcmInt16ToFloat(sampleInt16L);			
					// Right
					int16_t sampleInt16R = *((volatile int16_t *)(me->trackPtr + me->trackSampleOffset_Byte + me->TRACK_SAMPLE_R_OFFSET_16bit_Byte));
					float sampleFloatR = AlxAudio_LinerPcmInt16ToFloat(sampleInt16R);
					// Mono
					sampleOut = AlxAudio_StereoToMono(sampleFloatL, sampleFloatR);
				}
				else
				{
					ALX_AUDIO_PLAYER_ASSERT(false);	// We should never get here
				}
			}
		}
		// #2.5 Encoding - Error
		else
		{
			ALX_AUDIO_PLAYER_ASSERT(false);	// We should never get here
		}
	}
	// #3 Player is not playing
	else
	{
		sampleOut = 0.f;	// We will set sample = 0, because player is not playing
	}
	
	// #4 Return sample
	return sampleOut;
}
