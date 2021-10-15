/**
  ******************************************************************************
  * @file alxVdiv.c
  * @brief Auralix C Library - ALX Voltage Divider Module
  * @version $LastChangedRevision: 5338 $
  * @date $LastChangedDate: 2021-06-07 11:51:21 +0200 (Mon, 07 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxVdiv.h"


//******************************************************************************
// Constructor
//******************************************************************************


//******************************************************************************
// Functions
//******************************************************************************
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm)
{
	return ((vin_V * resLow_kOhm) / (resHigh_kOhm + resLow_kOhm));
}
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm)
{
	return ((resHigh_kOhm + resLow_kOhm) * vout_V / resLow_kOhm);
}
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm)
{
	return (resLow_kOhm * (vin_V - vout_V) / vout_V);
}
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm)
{
	return ((resHigh_kOhm * vout_V) / (vin_V - vout_V));
}
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return ((vin_mV * resLow_ohm) / (resHigh_ohm + resLow_ohm));
}
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm)
{
	return ((resHigh_ohm + resLow_ohm) * vout_mV / resLow_ohm);
}
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm)
{
	return (resLow_ohm * (vin_mV - vout_mV) / vout_mV);
}
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm)
{
	return ((resHigh_ohm * vout_mV) / (vin_mV - vout_mV));
}
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm)
{
	return (vout_uV / resLow_ohm);
}
