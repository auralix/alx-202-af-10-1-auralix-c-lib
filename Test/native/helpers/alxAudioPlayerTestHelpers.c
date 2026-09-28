/**
  ******************************************************************************
  * @file		alxAudioPlayerTestHelpers.c
  * @brief		Auralix C Library - ALX Audio Player - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The player keeps a POINTER to the track it plays, so the helper owns the
  * bytes: a test hands over a track as a copy and the helper keeps it alive for
  * as long as the player is. The two internal numbers a test may read are the
  * byte offset into the track and the step the player advances by, because they
  * are what "where in the track am I" means and neither is reachable through
  * the module's own API.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAudioPlayer.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
#define ALX_AUDIO_PLAYER_TEST_TRACK_MAX 256

typedef struct
{
	AlxAudioPlayer player;
	uint8_t track[ALX_AUDIO_PLAYER_TEST_TRACK_MAX];
	uint8_t other[ALX_AUDIO_PLAYER_TEST_TRACK_MAX];
} AlxAudioPlayerTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAudioPlayerTest_Obj* AlxAudioPlayerTest_New(const uint8_t* track, uint32_t len_Byte, uint32_t startOffset_Sample, int32_t encoding, bool isMono);
void AlxAudioPlayerTest_Delete(AlxAudioPlayerTest_Obj* me);
AlxAudioPlayer* AlxAudioPlayerTest_Player(AlxAudioPlayerTest_Obj* me);
void AlxAudioPlayerTest_LoadOther(AlxAudioPlayerTest_Obj* me, const uint8_t* track, uint32_t len_Byte, uint32_t startOffset_Sample, int32_t encoding, bool isMono);
uint32_t AlxAudioPlayerTest_Offset_Byte(AlxAudioPlayerTest_Obj* me);
uint32_t AlxAudioPlayerTest_Step_Byte(AlxAudioPlayerTest_Obj* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxAudioPlayerTest_Obj* AlxAudioPlayerTest_New(const uint8_t* track, uint32_t len_Byte, uint32_t startOffset_Sample, int32_t encoding, bool isMono)
{
	if (len_Byte > ALX_AUDIO_PLAYER_TEST_TRACK_MAX) { return NULL; }

	AlxAudioPlayerTest_Obj* me = (AlxAudioPlayerTest_Obj*)calloc(1, sizeof(AlxAudioPlayerTest_Obj));
	if (me == NULL) { return NULL; }

	memcpy(me->track, track, len_Byte);
	AlxAudioPlayer_Ctor(&me->player, me->track, len_Byte, startOffset_Sample, (AlxAudio_Encoding)encoding, isMono);
	return me;
}

void AlxAudioPlayerTest_Delete(AlxAudioPlayerTest_Obj* me)
{
	free(me);
}

AlxAudioPlayer* AlxAudioPlayerTest_Player(AlxAudioPlayerTest_Obj* me)
{
	return &me->player;
}

void AlxAudioPlayerTest_LoadOther(AlxAudioPlayerTest_Obj* me, const uint8_t* track, uint32_t len_Byte, uint32_t startOffset_Sample, int32_t encoding, bool isMono)
{
	if (len_Byte > ALX_AUDIO_PLAYER_TEST_TRACK_MAX) { return; }

	memcpy(me->other, track, len_Byte);
	AlxAudioPlayer_LoadTrack(&me->player, me->other, len_Byte, startOffset_Sample, (AlxAudio_Encoding)encoding, isMono);
}

uint32_t AlxAudioPlayerTest_Offset_Byte(AlxAudioPlayerTest_Obj* me)
{
	return me->player.trackSampleOffset_Byte;
}

uint32_t AlxAudioPlayerTest_Step_Byte(AlxAudioPlayerTest_Obj* me)
{
	return me->player.trackSampleIncStep_Byte;
}
