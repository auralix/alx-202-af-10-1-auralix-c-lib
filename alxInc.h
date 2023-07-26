/**
  ******************************************************************************
  * @file		alxInc.h
  * @brief		Auralix C Library - ALX Include File
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
#ifndef ALX_INC_H
#define ALX_INC_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc.h"
#include "alxAssert.h"
#include "alxAudio.h"
#include "alxAudioPlayer.h"
#include "alxAudioVol.h"
#include "alxAvg.h"
#include "alxBool.h"
#include "alxBound.h"
#include "alxBuild.h"
#include "alxCan.h"
#include "alxCanParser.h"
#include "alxClk.h"
#include "alxCrc.h"
#include "alxDac.h"
#include "alxDelay.h"
#include "alxFifo.h"
#include "alxFiltGlitchBool.h"
#include "alxFiltGlitchUint32.h"
#include "alxFtoa.h"
#include "alxGlobal.h"
#include "alxHys1.h"
#include "alxHys2.h"
#include "alxI2c.h"
#include "alxI2s.h"
#include "alxId.h"
#include "alxInc.h"
#include "alxInterpLin.h"
#include "alxIoPin.h"
#include "alxIoPinIrq.h"
#include "alxLin.h"
#include "alxLinFun.h"
#include "alxMemRaw.h"
#include "alxMemSafe.h"
#include "alxOsCriticalSection.h"
#include "alxOsDelay.h"
#include "alxOsDelayUntil.h"
#include "alxOsEventFlagGroup.h"
#include "alxOsKernel.h"
#include "alxOsMutex.h"
#include "alxOsThread.h"
#include "alxParamGroup.h"
#include "alxParamItem.h"
#include "alxParamMgmt.h"
#include "alxParamStore.h"
#include "alxPwm.h"
#include "alxPwr.h"
#include "alxRange.h"
#include "alxRtc.h"
#include "alxRtc_Global.h"
#include "alxSd.h"
#include "alxSerialPort.h"
#include "alxSpi.h"
#include "alxTempSensRtdVdiv.h"
#include "alxTick.h"
#include "alxTimSw.h"
#include "alxTrace.h"
#include "alxVdiv.h"
#include "alxWdt.h"

#include "alxAdau1961.h"
#include "alxAds8678.h"
#include "alxAdxl355.h"
#include "alxBq24600.h"
#include "alxBts724g.h"
#include "alxIna228.h"
#include "alxMcp73831.h"
#include "alxNtcg103jf103ft1s.h"
#include "alxPcal6416a.h"
#include "alxTmp1075.h"
#include "alxTpa3255.h"
#include "alxVeml6040.h"
#include "alxVnh7040.h"


#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_INC_H
