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
  * @brief						The low leg's share of the input
  * @param[in]	vin_V			Voltage across both legs
  * @param[in]	resHigh_kOhm	High leg resistance
  * @param[in]	resLow_kOhm		Low leg resistance
  * @return						Voltage across the low leg, in V
  */
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm)
{
	return (vin_V * resLow_kOhm) / (resHigh_kOhm + resLow_kOhm);
}

/**
  * @brief						The input that produced a given low leg voltage
  * @param[in]	vout_V			Voltage across the low leg
  * @param[in]	resHigh_kOhm	High leg resistance
  * @param[in]	resLow_kOhm		Low leg resistance
  * @return						Voltage across both legs, in V
  */
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm)
{
	return ((resHigh_kOhm + resLow_kOhm) * vout_V) / resLow_kOhm;
}

/**
  * @brief						The high leg a pair of voltages implies
  * @param[in]	vin_V			Voltage across both legs
  * @param[in]	vout_V			Voltage across the low leg
  * @param[in]	resLow_kOhm		Low leg resistance
  * @return						High leg resistance, in kOhm
  */
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm)
{
	return (resLow_kOhm * (vin_V - vout_V)) / vout_V;
}

/**
  * @brief						The low leg a pair of voltages implies
  * @param[in]	vin_V			Voltage across both legs
  * @param[in]	vout_V			Voltage across the low leg
  * @param[in]	resHigh_kOhm	High leg resistance
  * @return						Low leg resistance, in kOhm
  */
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm)
{
	return (resHigh_kOhm * vout_V) / (vin_V - vout_V);
}

/**
  * @brief						Current through the low leg, from the voltage across it
  * @param[in]	vout_V			Voltage across the low leg
  * @param[in]	resLow_kOhm		Low leg resistance
  * @return						Current in mA - V / kOhm IS mA, so nothing is scaled here or by the caller; a caller wanting uA multiplies by 1000, which on a float is exact. See the unit note in alxVdiv.h
  */
float AlxVdiv_GetCurrent_mA(float vout_V, float resLow_kOhm)
{
	return vout_V / resLow_kOhm;
}

/**
  * @brief						The low leg's share of the input
  * @param[in]	vin_mV			Voltage across both legs
  * @param[in]	resHigh_ohm		High leg resistance
  * @param[in]	resLow_ohm		Low leg resistance
  * @return						Voltage across the low leg, in mV
  */
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return (vin_mV * resLow_ohm) / (resHigh_ohm + resLow_ohm);
}

/**
  * @brief						The input that produced a given low leg voltage
  * @param[in]	vout_mV			Voltage across the low leg
  * @param[in]	resHigh_ohm		High leg resistance
  * @param[in]	resLow_ohm		Low leg resistance
  * @return						Voltage across both legs, in mV
  */
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return ((resHigh_ohm + resLow_ohm) * vout_mV) / resLow_ohm;
}

/**
  * @brief						The high leg a pair of voltages implies
  * @param[in]	vin_mV			Voltage across both legs
  * @param[in]	vout_mV			Voltage across the low leg
  * @param[in]	resLow_ohm		Low leg resistance
  * @return						High leg resistance, in ohm
  */
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm)
{
	return (resLow_ohm * (vin_mV - vout_mV)) / vout_mV;
}

/**
  * @brief						The low leg a pair of voltages implies
  * @param[in]	vin_mV			Voltage across both legs
  * @param[in]	vout_mV			Voltage across the low leg
  * @param[in]	resHigh_ohm		High leg resistance
  * @return						Low leg resistance, in ohm
  */
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm)
{
	return (resHigh_ohm * vout_mV) / (vin_mV - vout_mV);
}

/**
  * @brief						Current through the low leg, from the voltage across it
  * @param[in]	vout_uV			Voltage across the low leg, in MICROvolts
  * @param[in]	resLow_ohm		Low leg resistance
  * @return						Current in uA - the finer input unit is deliberate: mV / ohm would yield whole milliamps and throw the fraction away
  */
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm)
{
	return vout_uV / resLow_ohm;
}


#endif	// #if defined(ALX_C_LIB)
