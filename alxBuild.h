/**
  ******************************************************************************
  * @file alxBuild.h
  * @brief Auralix C Library - ALX Build Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_BUILD_H
#define ALX_BUILD_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Preprocesor
//******************************************************************************

// __DATE__ string: "Jul 27 2012"
//					 01234567890
#define ALX_BUILD_COMP_YEAR_CH0		(__DATE__[7])
#define ALX_BUILD_COMP_YEAR_CH1		(__DATE__[8])
#define ALX_BUILD_COMP_YEAR_CH2		(__DATE__[9])
#define ALX_BUILD_COMP_YEAR_CH3		(__DATE__[10])
#define ALX_BUILD_COMP_MONTH_IS_JAN	(__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define ALX_BUILD_COMP_MONTH_IS_FEB	(__DATE__[0] == 'F')
#define ALX_BUILD_COMP_MONTH_IS_MAR	(__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define ALX_BUILD_COMP_MONTH_IS_APR	(__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define ALX_BUILD_COMP_MONTH_IS_MAY	(__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define ALX_BUILD_COMP_MONTH_IS_JUN	(__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define ALX_BUILD_COMP_MONTH_IS_JUL	(__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define ALX_BUILD_COMP_MONTH_IS_AUG	(__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define ALX_BUILD_COMP_MONTH_IS_SEP	(__DATE__[0] == 'S')
#define ALX_BUILD_COMP_MONTH_IS_OCT	(__DATE__[0] == 'O')
#define ALX_BUILD_COMP_MONTH_IS_NOV	(__DATE__[0] == 'N')
#define ALX_BUILD_COMP_MONTH_IS_DEC	(__DATE__[0] == 'D')
#define ALX_BUILD_COMP_MONTH_CH0	((ALX_BUILD_COMP_MONTH_IS_OCT || ALX_BUILD_COMP_MONTH_IS_NOV || ALX_BUILD_COMP_MONTH_IS_DEC) ? '1' : '0')
#define ALX_BUILD_COMP_MONTH_CH1	((ALX_BUILD_COMP_MONTH_IS_JAN) ? '1' : \
									 (ALX_BUILD_COMP_MONTH_IS_FEB) ? '2' : \
									 (ALX_BUILD_COMP_MONTH_IS_MAR) ? '3' : \
									 (ALX_BUILD_COMP_MONTH_IS_APR) ? '4' : \
									 (ALX_BUILD_COMP_MONTH_IS_MAY) ? '5' : \
									 (ALX_BUILD_COMP_MONTH_IS_JUN) ? '6' : \
									 (ALX_BUILD_COMP_MONTH_IS_JUL) ? '7' : \
									 (ALX_BUILD_COMP_MONTH_IS_AUG) ? '8' : \
									 (ALX_BUILD_COMP_MONTH_IS_SEP) ? '9' : \
									 (ALX_BUILD_COMP_MONTH_IS_OCT) ? '0' : \
									 (ALX_BUILD_COMP_MONTH_IS_NOV) ? '1' : \
									 (ALX_BUILD_COMP_MONTH_IS_DEC) ? '2' : \
																	 '?')
#define ALX_BUILD_COMP_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define ALX_BUILD_COMP_DAY_CH1  (__DATE__[5])

// __TIME__ string: "21:06:19"
//					 01234567
#define ALX_BUILD_COMP_HOUR_CH0 (__TIME__[0])
#define ALX_BUILD_COMP_HOUR_CH1 (__TIME__[1])
#define ALX_BUILD_COMP_MIN_CH0  (__TIME__[3])
#define ALX_BUILD_COMP_MIN_CH1  (__TIME__[4])
#define ALX_BUILD_COMP_SEC_CH0  (__TIME__[6])
#define ALX_BUILD_COMP_SEC_CH1  (__TIME__[7])

// Compiler Date
#define ALX_BUILD_DATE_COMP		((ALX_BUILD_COMP_YEAR_CH2  - '0') * 1000000000 + \
								 (ALX_BUILD_COMP_YEAR_CH3  - '0') * 100000000 + \
								 (ALX_BUILD_COMP_MONTH_CH0 - '0') * 10000000 + \
								 (ALX_BUILD_COMP_MONTH_CH1 - '0') * 1000000 + \
								 (ALX_BUILD_COMP_DAY_CH0   - '0') * 100000 + \
								 (ALX_BUILD_COMP_DAY_CH1   - '0') * 10000 + \
								 (ALX_BUILD_COMP_HOUR_CH0  - '0') * 1000 + \
								 (ALX_BUILD_COMP_HOUR_CH1  - '0') * 100 + \
								 (ALX_BUILD_COMP_MIN_CH0   - '0') * 10 + \
								 (ALX_BUILD_COMP_MIN_CH1   - '0') * 1)

// Build
#define ALX_BUILD_NAME "Engineer"
#define ALX_BUILD_DATE 0
#define ALX_BUILD_NUM 0
#define ALX_BUILD_HASH 0
#define ALX_BUILD_REV 0


#ifdef __cplusplus
}
#endif

#endif // ALX_BUILD_H
