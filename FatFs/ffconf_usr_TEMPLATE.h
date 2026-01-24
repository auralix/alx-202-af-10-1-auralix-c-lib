/**
  ******************************************************************************
  * @file		ffconf_usr.h
  * @brief		Auralix C Library - ALX FatFs Configuration File - User
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
#ifndef FFCONF_CONF_USR_H
#define FFCONF_CONF_USR_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Configuration
//******************************************************************************
#undef FF_USE_LFN
#define FF_USE_LFN 1


#ifdef __cplusplus
}
#endif

#endif	// #ifndef FFCONF_CONF_USR_H
