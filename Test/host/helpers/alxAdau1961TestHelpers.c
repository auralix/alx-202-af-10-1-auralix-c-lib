/**
  ******************************************************************************
  * @file		alxAdau1961TestHelpers.c
  * @brief		Auralix C Library - ALX ADAU1961 Audio Codec - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its I2S interface and its I2C bus, so the
  * helper owns both next to the codec. The bus is the I2C fake, keyed by the
  * low byte of the codec's 16-bit register address; the interface is the I2S
  * fake, which counts.
  *
  * The gains are register fields, and the host compiler lays the named
  * bit-fields out differently from the target (A93), so the helper reads each
  * field back by its name - the value the driver computed - rather than as
  * the bytes that go on the wire. An enumeration field is also SIGNED on this
  * host, so a mute bit of 1 reads back as -1: the helper masks each one to
  * its width, which is the bit pattern the target holds.
  *
  * The driver's two weak RegStruct hooks assert - "Implement in APP!" - so
  * this helper is the application and keeps every default the driver set.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdau1961.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxAdau1961 adau;
	AlxI2s i2s;
	AlxI2c i2c;
} AlxAdau1961Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAdau1961Test_Obj* AlxAdau1961Test_New(void);
void AlxAdau1961Test_Delete(AlxAdau1961Test_Obj* me);
AlxAdau1961* AlxAdau1961Test_Adau(AlxAdau1961Test_Obj* me);
void AlxAdau1961Test_InDiff(AlxAdau1961Test_Obj* me, bool right, uint8_t* vol, uint8_t* unmuted, uint8_t* boost);
uint8_t AlxAdau1961Test_InAux(AlxAdau1961Test_Obj* me, bool right);
void AlxAdau1961Test_OutLine(AlxAdau1961Test_Obj* me, bool right, uint8_t* vol, uint8_t* unmuted);
int32_t AlxAdau1961Test_Status_Ok(void);
int32_t AlxAdau1961Test_Status_Err(void);

// The application hooks the driver declares in its .c and this helper defines
void AlxAdau1961_RegStruct_SetClkVal(AlxAdau1961* me);
void AlxAdau1961_RegStruct_SetNonClkVal(AlxAdau1961* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxAdau1961Test_Obj* AlxAdau1961Test_New(void)
{
	AlxAdau1961Test_Obj* me = (AlxAdau1961Test_Obj*)calloc(1, sizeof(AlxAdau1961Test_Obj));
	if (me == NULL) { return NULL; }
	AlxAdau1961_Ctor(&me->adau, &me->i2s, &me->i2c, 0x70, false, 3, 10);
	return me;
}

void AlxAdau1961Test_Delete(AlxAdau1961Test_Obj* me)
{
	free(me);
}

AlxAdau1961* AlxAdau1961Test_Adau(AlxAdau1961Test_Obj* me)		{ return &me->adau; }
int32_t AlxAdau1961Test_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxAdau1961Test_Status_Err(void)						{ return (int32_t)Alx_Err; }

void AlxAdau1961Test_InDiff(AlxAdau1961Test_Obj* me, bool right, uint8_t* vol, uint8_t* unmuted, uint8_t* boost)
{
	if (right)
	{
		*vol = me->adau.reg.R9_RightDiffInputVol.val.RDVOL;
		*unmuted = (uint8_t)me->adau.reg.R9_RightDiffInputVol.val.RDMUTE & 0x01U;
		*boost = (uint8_t)me->adau.reg.R7_RecMixerRight1.val.RDBOOST & 0x03U;
	}
	else
	{
		*vol = me->adau.reg.R8_LeftDiffInputVol.val.LDVOL;
		*unmuted = (uint8_t)me->adau.reg.R8_LeftDiffInputVol.val.LDMUTE & 0x01U;
		*boost = (uint8_t)me->adau.reg.R5_RecMixerLeft1.val.LDBOOST & 0x03U;
	}
}

uint8_t AlxAdau1961Test_InAux(AlxAdau1961Test_Obj* me, bool right)
{
	if (right)
	{
		return (uint8_t)me->adau.reg.R7_RecMixerRight1.val.MX2AUXG & 0x07U;
	}
	else
	{
		return (uint8_t)me->adau.reg.R5_RecMixerLeft1.val.MX1AUXG & 0x07U;
	}
}

void AlxAdau1961Test_OutLine(AlxAdau1961Test_Obj* me, bool right, uint8_t* vol, uint8_t* unmuted)
{
	if (right)
	{
		*vol = me->adau.reg.R32_LineOutputRightVol.val.ROUTVOL;
		*unmuted = (uint8_t)me->adau.reg.R32_LineOutputRightVol.val.ROUTM & 0x01U;
	}
	else
	{
		*vol = me->adau.reg.R31_LineOutputLeftVol.val.LOUTVOL;
		*unmuted = (uint8_t)me->adau.reg.R31_LineOutputLeftVol.val.LOUTM & 0x01U;
	}
}


//******************************************************************************
// The application hooks the driver requires
//******************************************************************************
void AlxAdau1961_RegStruct_SetClkVal(AlxAdau1961* me)
{
	(void)me;	// this application keeps every default the driver set
}
void AlxAdau1961_RegStruct_SetNonClkVal(AlxAdau1961* me)
{
	(void)me;	// this application keeps every default the driver set
}
