/**
  ******************************************************************************
  * @file		alxVdiv.c
  * @brief		Auralix C Library - ALX Voltage Divider Module
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
// Includes
//******************************************************************************
#include "alxVdiv.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] vin_V
  * @param[in] resHigh_kOhm
  * @param[in] resLow_kOhm
  * @return
  */
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm)
{
	return ((vin_V * resLow_kOhm) / (resHigh_kOhm + resLow_kOhm));
}

/**
  * @brief
  * @param[in] vout_V
  * @param[in] resHigh_kOhm
  * @param[in] resLow_kOhm
  * @return
  */
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm)
{
	return ((resHigh_kOhm + resLow_kOhm) * vout_V / resLow_kOhm);
}

/**
  * @brief
  * @param[in] vin_V
  * @param[in] vout_V
  * @param[in] resLow_kOhm
  * @return
  */
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm)
{
	return (resLow_kOhm * (vin_V - vout_V) / vout_V);
}

/**
  * @brief
  * @param[in] vin_V
  * @param[in] vout_V
  * @param[in] resHigh_kOhm
  * @return
  */
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm)
{
	return ((resHigh_kOhm * vout_V) / (vin_V - vout_V));
}

/**
  * @brief
  * @param[in] vin_mV
  * @param[in] resHigh_ohm
  * @param[in] resLow_ohm
  * @return
  */
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return ((vin_mV * resLow_ohm) / (resHigh_ohm + resLow_ohm));
}

/**
  * @brief
  * @param[in] vout_mV
  * @param[in] resHigh_ohm
  * @param[in] resLow_ohm
  * @return
  */
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return ((resHigh_ohm + resLow_ohm) * vout_mV / resLow_ohm);
}

/**
  * @brief
  * @param[in] vin_mV
  * @param[in] vout_mV
  * @param[in] resLow_ohm
  * @return
  */
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm)
{
	return (resLow_ohm * (vin_mV - vout_mV) / vout_mV);
}

/**
  * @brief
  * @param[in] vin_mV
  * @param[in] vout_mV
  * @param[in] resHigh_ohm
  * @return
  */
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm)
{
	return ((resHigh_ohm * vout_mV) / (vin_mV - vout_mV));
}

/**
  * @brief
  * @param[in] vout_uV
  * @param[in] resLow_ohm
  * @return
  */
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm)
{
	return (vout_uV / resLow_ohm);
}


#endif	// #if defined(ALX_C_LIB)
