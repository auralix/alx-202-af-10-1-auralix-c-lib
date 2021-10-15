/**
  ******************************************************************************
  * @file alxAudioPlayer.h
  * @brief Auralix C Library - ALX Audio Player Module
  * @version $LastChangedRevision: 5999 $
  * @date $LastChangedDate: 2021-09-10 17:46:05 +0200 (Fri, 10 Sep 2021) $
  ******************************************************************************
  */

#ifndef ALX_AUDIO_PLAYER_H
#define ALX_AUDIO_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxAudio.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_PLAYER_FILE "alxAudioPlayer"

// Assert //
#if defined(_ALX_AUDIO_PLAYER_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_AUDIO_PLAYER_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AUDIO_PLAYER_FILE, expr)
#elif defined(_ALX_AUDIO_PLAYER_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_AUDIO_PLAYER_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AUDIO_PLAYER_FILE, expr)
#elif defined(_ALX_AUDIO_PLAYER_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_AUDIO_PLAYER_ASSERT(expr) ALX_ASSERT_RST(ALX_AUDIO_PLAYER_FILE, expr)
#else
	#define ALX_AUDIO_PLAYER_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_AUDIO_PLAYER_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_AUDIO_PLAYER_TRACE(...) ALX_TRACE_STD(ALX_AUDIO_PLAYER_FILE, __VA_ARGS__)
#else
	#define ALX_AUDIO_PLAYER_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_AUDIO_PLAYER_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_AUDIO_PLAYER_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_AUDIO_PLAYER_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	uint8_t TRACK_SAMPLE_R_OFFSET_8bit_Byte;	// Offset of the beginning of right channel sample relative to the beginning of left channel sample
	uint8_t TRACK_SAMPLE_R_OFFSET_16bit_Byte;
	
	// Parameters
	const uint8_t* trackPtr;
	uint32_t trackLen_Byte;				// Track length in bytes
	AlxAudio_Encoding trackEncoding;
	bool isTrackMono;					// If false = stereo

	// Variables
	uint8_t trackSampleIncStep_Byte;	// For how many bytes we will increment offset to get new left channel sample
	uint32_t trackSampleOffset_Byte;	// Hold offset of the current left channel sample of track
	bool isPlaying;
	bool isLoopOn;

	// Info
	bool wasCtorCalled;
} AlxAudioPlayer;


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
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxAudioPlayer_LoadTrack(AlxAudioPlayer* me, const uint8_t* trackPtr, uint32_t len_Byte, uint32_t startOffset_Sample, AlxAudio_Encoding encoding, bool isMono);
float AlxAudioPlayer_GetSampleL(AlxAudioPlayer* me);
float AlxAudioPlayer_GetSampleR(AlxAudioPlayer* me);
float AlxAudioPlayer_GetSampleMono(AlxAudioPlayer* me);
void AlxAudioPlayer_IncSampleOffset(AlxAudioPlayer* me);
void AlxAudioPlayer_Play(AlxAudioPlayer* me);
void AlxAudioPlayer_Stop(AlxAudioPlayer* me);
void AlxAudioPlayer_Pause(AlxAudioPlayer* me);
void AlxAudioPlayer_Replay(AlxAudioPlayer* me);
void AlxAudioPlayer_LoopOn(AlxAudioPlayer* me);
void AlxAudioPlayer_LoopOff(AlxAudioPlayer* me);
void AlxAudioPlayer_LoopConfig(AlxAudioPlayer* me, bool isOn);
bool AlxAudioPlayer_IsPlaying(AlxAudioPlayer* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_AUDIO_PLAYER_H
