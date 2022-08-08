/**
  ******************************************************************************
  * @file		alxAudioPlayer.h
  * @brief		Auralix C Library - ALX Audio Player Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef ALX_AUDIO_PLAYER_H
#define ALX_AUDIO_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxAudio.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AUDIO_PLAYER_FILE "alxAudioPlayer.h"

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

/**
  * @brief
  * @param[in,out] me
  * @param[in] defaultTrackPtr
  * @param[in] defaultTrackLen_Byte
  * @param[in] defaultTrackStartOffset_Sample
  * @param[in] defaultTrackEncoding
  * @param[in] isDefaultTrackMono
  */
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

/**
  * @brief
  * @param[in,out] me
  * @param[in] trackPtr
  * @param[in] len_Byte
  * @param[in] startOffset_Sample
  * @param[in] encoding
  * @param[in] isMono
  */
void AlxAudioPlayer_LoadTrack(AlxAudioPlayer* me, const uint8_t* trackPtr, uint32_t len_Byte, uint32_t startOffset_Sample, AlxAudio_Encoding encoding, bool isMono);

/**
  * @brief
  * @param[in,out] me
  */
float AlxAudioPlayer_GetSampleL(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
float AlxAudioPlayer_GetSampleR(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
float AlxAudioPlayer_GetSampleMono(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_IncSampleOffset(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_Play(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_Stop(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_Pause(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_Replay(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_LoopOn(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxAudioPlayer_LoopOff(AlxAudioPlayer* me);

/**
  * @brief
  * @param[in,out] me
  * @param[in] isOn
  */
void AlxAudioPlayer_LoopConfig(AlxAudioPlayer* me, bool isOn);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxAudioPlayer_IsPlaying(AlxAudioPlayer* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_AUDIO_PLAYER_H
