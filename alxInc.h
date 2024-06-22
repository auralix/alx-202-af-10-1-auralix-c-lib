/**
  ******************************************************************************
  * @file		alxInc.h
  * @brief		Auralix C Library - ALX Include File
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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


//------------------------------------------------------------------------------
// ALX - EXT
//------------------------------------------------------------------------------
#include "alxAdau1961.h"
#include "alxAds8678.h"
#include "alxAds114s08.h"
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


//------------------------------------------------------------------------------
// ALX - MCU
//------------------------------------------------------------------------------
#include "alxAdc.h"
#include "alxCan.h"
#include "alxClk.h"
#include "alxDac.h"
#include "alxI2c.h"
#include "alxI2s.h"
#include "alxIoPin.h"
#include "alxIoPinIrq.h"
#include "alxMmc.h"
#include "alxPwm.h"
#include "alxRtc.h"
#include "alxSerialPort.h"
#include "alxSpi.h"
#include "alxTrace.h"
#include "alxWdt.h"


//------------------------------------------------------------------------------
// ALX
//------------------------------------------------------------------------------
#include "alxAssert.h"
#include "alxAudio.h"
#include "alxAudioPlayer.h"
#include "alxAudioVol.h"
#include "alxAvg.h"
#include "alxBool.h"
#include "alxBoot.h"
#include "alxBound.h"
#include "alxBuild.h"
#include "alxCanParser.h"
#include "alxCli.h"
#include "alxCrc.h"
#include "alxDelay.h"
#include "alxFifo.h"
#include "alxFiltGlitchBool.h"
#include "alxFiltGlitchUint32.h"
#include "alxFs.h"
#include "alxFtoa.h"
#include "alxGlobal.h"
#include "alxHys1.h"
#include "alxHys2.h"
#include "alxId.h"
#include "alxInc.h"
#include "alxInterpLin.h"
#include "alxLfsConfig.h"
#include "alxLin.h"
#include "alxLinFun.h"
#include "alxLogger.h"
#include "alxMemRaw.h"
#include "alxMemSafe.h"
#include "alxNet.h"
#include "alxOsCriticalSection.h"
#include "alxOsDelay.h"
#include "alxOsDelayUntil.h"
#include "alxOsEventFlagGroup.h"
#include "alxOsKernel.h"
#include "alxOsMutex.h"
#include "alxOsThread.h"
#include "alxParamGroup.h"
#include "alxParamItem.h"
#include "alxParamKvStore.h"
#include "alxParamMgmt.h"
#include "alxParamStore.h"
#include "alxPwr.h"
#include "alxRange.h"
#include "alxRotSw.h"
#include "alxRtc_Global.h"
#include "alxSd.h"
#include "alxSocket.h"
#include "alxTempSensRtdVdiv.h"
#include "alxTick.h"
#include "alxTimSw.h"
#include "alxVdiv.h"


#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_INC_H
