/**
  ******************************************************************************
  * @file		alxRtc_Global.c
  * @brief		Auralix C Library - ALX RTC Global Module
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
#include "alxRtc_Global.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Defines
//******************************************************************************
#define ALX_RTC_LEAP_YEAR(yr)		((((yr) % 4 == 0) && ((yr) % 100 != 0)) || ((yr) % 400 == 0))
#define ALX_RTC_DAYS_IN_YEAR(yr)	(ALX_RTC_LEAP_YEAR(yr) ? 366 : 365)


//******************************************************************************
// Private Variables
//******************************************************************************
static uint8_t AlxRtc_MonthDaysTable[2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	// Not leap year
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	// Leap year
};


//******************************************************************************
// Functions
//******************************************************************************
uint64_t AlxRtc_MsUsNsToNs(uint64_t rtcTick_ns, uint16_t ms, uint16_t us, uint16_t ns)
{
	ALX_RTC_GLOBAL_ASSERT(ms <= 999);
	ALX_RTC_GLOBAL_ASSERT(us <= 999);
	ALX_RTC_GLOBAL_ASSERT(ns <= 999);

	uint64_t _ns = (uint64_t)ms * 1000000ull + (uint64_t)us * 1000ull + (uint64_t)ns;
	uint64_t _subSec = _ns / rtcTick_ns;
	return _subSec;
}
uint64_t AlxRtc_MsUsToNs(uint64_t rtcTick_ns, uint16_t ms, uint16_t us)
{
	return AlxRtc_MsUsNsToNs(rtcTick_ns, ms, us, 0);	// Round to lower integer
}
uint64_t AlxRtc_MsToNs(uint64_t rtcTick_ns, uint16_t ms)
{
	return AlxRtc_MsUsNsToNs(rtcTick_ns, ms, 0, 0);	// Round to lower integer
}
float AlxRtc_MsUsNsToSecFract(uint16_t ms, uint16_t us, uint16_t ns)
{
	ALX_RTC_GLOBAL_ASSERT(ms <= 999);
	ALX_RTC_GLOBAL_ASSERT(us <= 999);
	ALX_RTC_GLOBAL_ASSERT(ns <= 999);

	uint64_t _ns = (uint64_t)ms * 1000000ull + (uint64_t)us * 1000ull + (uint64_t)ns;
	float secFract = _ns * 0.000000001;

	ALX_RTC_GLOBAL_ASSERT((0.f <= secFract) && (secFract <= 0.999999999f));

	return secFract;
}
void AlxRtc_NsToMsUsNs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms, uint16_t* us, uint16_t* ns)
{
	ALX_RTC_GLOBAL_ASSERT((rtcTick_ns * in_ns) <= 999999999);

	// Prepare variables
	uint64_t temp = rtcTick_ns * in_ns;

	// Set ns
	uint64_t _ns = temp % 1000;

	// Go to us
	temp = temp / 1000;
	// Set us
	uint64_t _us = temp % 1000;

	// Go to ms
	temp = temp / 1000;
	// Set ms
	uint64_t _ms = temp % 1000;

	// Assert
	ALX_RTC_GLOBAL_ASSERT(_ms <= 999);
	ALX_RTC_GLOBAL_ASSERT(_us <= 999);
	ALX_RTC_GLOBAL_ASSERT(_ns <= 999);

	// Return
	*ms = (uint16_t)_ms;
	*us = (uint16_t)_us;
	*ns = (uint16_t)_ns;
}
void AlxRtc_NsToMsUs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms, uint16_t* us)
{
	uint16_t ns = 0;	// Round to lower integer
	AlxRtc_NsToMsUsNs(rtcTick_ns, in_ns, ms, us, &ns);
}
void AlxRtc_NsToMs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms)
{
	uint16_t ns = 0;	// Round to lower integer
	uint16_t us = 0;	// Round to lower integer
	AlxRtc_NsToMsUsNs(rtcTick_ns, in_ns, ms, &us, &ns);
}
void AlxRtc_SecFractToMsUsNs(float secFract, uint16_t* ms, uint16_t* us, uint16_t* ns)
{
	ALX_RTC_GLOBAL_ASSERT((0.f <= secFract) && (secFract < 1.f));

	// Prepare variables
	float tempFloat_ns = secFract * 1000000000.f;		// Convert to ns - float
	uint64_t tempUint64_ns = (uint64_t)tempFloat_ns;	// Convert to ns - uint64 -> Round to lower integer

	// Set ns
	uint64_t _ns = tempUint64_ns % 1000;

	// Go to us
	tempUint64_ns = tempUint64_ns / 1000;
	// Set us
	uint64_t _us = tempUint64_ns % 1000;

	// Go to ms
	tempUint64_ns = tempUint64_ns / 1000;
	// Set ms
	uint64_t _ms = tempUint64_ns % 1000;

	// Assert
	ALX_RTC_GLOBAL_ASSERT(_ms <= 999);
	ALX_RTC_GLOBAL_ASSERT(_us <= 999);
	ALX_RTC_GLOBAL_ASSERT(_ns <= 999);

	// Return
	*ms = (uint16_t)_ms;
	*us = (uint16_t)_us;
	*ns = (uint16_t)_ns;
}
AlxRtc_DateTime AlxRtc_UnixTimeNsToDateTime(uint64_t unixTime_ns)
{
	ALX_RTC_GLOBAL_ASSERT(946684800000000000 <= unixTime_ns);	// Must be larger than: Date and time (GMT): Saturday, January 1, 2000 0:00:00

	// Prepare variables
	uint64_t unixTime = unixTime_ns;

	AlxRtc_DateTime dateTime;
	dateTime.yr = 0;
	dateTime.mo = 0;
	dateTime.day = 0;
	dateTime.weekDay = 0;
	dateTime.hr = 0;
	dateTime.min = 0;
	dateTime.sec = 0;
	dateTime.ms = 0;
	dateTime.us = 0;
	dateTime.ns = 0;

	// Set ns
	dateTime.ns = unixTime % 1000;

	// Go to us
	unixTime = unixTime / 1000;
	// Set us
	dateTime.us = unixTime % 1000;

	// Go to ms
	unixTime = unixTime / 1000;
	// Set ms
	dateTime.ms = unixTime % 1000;

	// Go to sec
	unixTime = unixTime / 1000;
	// Set sec
	dateTime.sec = unixTime % 60;

	// Go to min
	unixTime = unixTime / 60;
	// Set Min
	dateTime.min = unixTime % 60;

	// Go to hr
	unixTime = unixTime / 60;
	// Set hr
	dateTime.hr = unixTime % 24;

	// Go to days
	unixTime = unixTime / 24;

	// Set weekDay
	dateTime.weekDay = (unixTime + 3) % 7 + 1;

	// Set yr
	uint16_t year = 1970u;
	while (1)
	{
		if(ALX_RTC_LEAP_YEAR(year))
		{
			if (unixTime >= 366ull)
			{
				unixTime -= 366ull;
			}
			else
			{
				break;
			}
		}
		else if (unixTime >= 365ull)
		{
			unixTime -= 365ull;
		}
		else
		{
			break;
		}

		year++;
	}
	dateTime.yr = (uint8_t)(year - 2000u);	// Set yr in xx format

	// Set mo
	for (dateTime.mo = 0; dateTime.mo < 12; dateTime.mo++)
	{
		if (ALX_RTC_LEAP_YEAR(year))
		{
			if (unixTime >= (uint64_t)AlxRtc_MonthDaysTable[1][dateTime.mo])
			{
				unixTime -= (uint64_t)AlxRtc_MonthDaysTable[1][dateTime.mo];
			}
			else
			{
				break;
			}
		}
		else if (unixTime >= (uint64_t)AlxRtc_MonthDaysTable[0][dateTime.mo])
		{
			unixTime -= (uint64_t)AlxRtc_MonthDaysTable[0][dateTime.mo];
		}
		else
		{
			break;
		}
	}
	dateTime.mo++;	// Month starts with 1

	// Set day
	dateTime.day = unixTime + 1;

	// Return
	return dateTime;
}
AlxRtc_DateTime AlxRtc_UnixTimeUsToDateTime(uint64_t unixTime_us)
{
	return AlxRtc_UnixTimeNsToDateTime(unixTime_us * 1000);
}
AlxRtc_DateTime AlxRtc_UnixTimeMsToDateTime(uint64_t unixTime_ms)
{
	return AlxRtc_UnixTimeNsToDateTime(unixTime_ms * 1000000);
}
AlxRtc_DateTime AlxRtc_UnixTimeSecToDateTime(uint64_t unixTime_sec)
{
	return AlxRtc_UnixTimeNsToDateTime(unixTime_sec * 1000000000);
}
uint64_t AlxRtc_DateTimeToUnixTimeNs(AlxRtc_DateTime dateTime)
{
	ALX_RTC_GLOBAL_ASSERT(dateTime.yr <= 99);
	ALX_RTC_GLOBAL_ASSERT((1 <= dateTime.mo) && (dateTime.mo <= 12));
	ALX_RTC_GLOBAL_ASSERT((1 <= dateTime.day) && (dateTime.day <= 31));
	ALX_RTC_GLOBAL_ASSERT(dateTime.hr <= 23);
	ALX_RTC_GLOBAL_ASSERT(dateTime.min <= 59);
	ALX_RTC_GLOBAL_ASSERT(dateTime.sec <= 59);
	ALX_RTC_GLOBAL_ASSERT(dateTime.ms <= 999);
	ALX_RTC_GLOBAL_ASSERT(dateTime.us <= 999);
	ALX_RTC_GLOBAL_ASSERT(dateTime.ns <= 999);

	// #1 Prepare variables
	uint64_t currentYr = 0;
	uint64_t daysSinceEpoch = 0;
	uint64_t nsSinceEpoch = 0;

	// #2 Set Year
	currentYr = (uint64_t)((uint64_t)dateTime.yr + 2000ull);

	// #2 Set Day
	// Past years
	for (uint64_t yr = 1970ull; yr < currentYr; yr++)
	{
		daysSinceEpoch = daysSinceEpoch + (uint64_t)ALX_RTC_DAYS_IN_YEAR(yr);
	}
	// Current year
	for (uint64_t mo = 1ull; mo < (uint64_t)dateTime.mo; mo++)
	{
		daysSinceEpoch = daysSinceEpoch + (uint64_t)AlxRtc_MonthDaysTable[ALX_RTC_LEAP_YEAR(currentYr)][mo - 1ull];
	}
	daysSinceEpoch = daysSinceEpoch + (uint64_t)dateTime.day - 1ull;	// Day start with 1, so we substract 1

	// #3 Set us
	nsSinceEpoch = daysSinceEpoch								* 86400000000000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.hr		* 3600000000000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.min	* 60000000000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.sec	* 1000000000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.ms		* 1000000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.us		* 1000ull;
	nsSinceEpoch = nsSinceEpoch		+ (uint64_t)dateTime.ns;

	return nsSinceEpoch;
}
uint64_t AlxRtc_DateTimeToUnixTimeUs(AlxRtc_DateTime dateTime)
{
	return AlxRtc_DateTimeToUnixTimeNs(dateTime) / 1000ull;
}
uint64_t AlxRtc_DateTimeToUnixTimeMs(AlxRtc_DateTime dateTime)
{
	return AlxRtc_DateTimeToUnixTimeNs(dateTime) / 1000000ull;
}
uint64_t AlxRtc_DateTimeToUnixTimeSec(AlxRtc_DateTime dateTime)
{
	return AlxRtc_DateTimeToUnixTimeNs(dateTime) / 1000000000ull;
}


#endif	// #if defined(ALX_C_LIB)
