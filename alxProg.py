#*******************************************************************************
# @file			alxProg.py
# @brief		Auralix C Library - ALX Programmer Module
# @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
#
# @section License
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This file is part of Auralix C Library.
#
# Auralix C Library is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# Auralix C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
#*****************************************************************************


#*******************************************************************************
# Imports
#*******************************************************************************
import pathlib
import alxJlink


#*******************************************************************************
# Class - Prog
#*******************************************************************************
class Prog:
	#-------------------------------------------------------------------------------
	# Ctor
	#-------------------------------------------------------------------------------
	def __init__(
		self,
		alxJlink,
		targetName,
		fwDir,
		addrAppHexStr,
		addrSignedHexStr
	):
		#-------------------------------------------------------------------------------
		# Private Variables
		#-------------------------------------------------------------------------------

		# Parameters
		self.__alxJlink = alxJlink
		self.__targetName = targetName
		self.__fwDir = fwDir
		self.__addrAppHexStr = addrAppHexStr
		self.__addrSignedHexStr = addrSignedHexStr


	#-------------------------------------------------------------------------------
	# Public Functions
	#-------------------------------------------------------------------------------
	def Reset(self):
		self.__alxJlink.Reset(self.__targetName)

	def Erase(self):
		self.__alxJlink.ResetErase(self.__targetName)

	def ProgramApp(self):
		# Set FW path
		fwAppPath = next(
			f for f in pathlib.Path(self.__fwDir).glob("*.bin")
			if not f.name.endswith("_Signed.bin") and not f.name.endswith("_NoBoot.bin")
		)
		fwAppPathStr = str(fwAppPath)

		# Program
		self.__alxJlink.ResetProgramVerifyReset(self.__targetName, fwAppPathStr, self.__addrAppHexStr)

	def ProgramSigned(self):
		# Set FW path
		fwSignedPath = next(pathlib.Path(self.__fwDir).glob("*_Signed.bin"))
		fwSignedPathStr = str(fwSignedPath)

		# Program
		self.__alxJlink.ResetProgramVerifyReset(self.__targetName, fwSignedPathStr, self.__addrSignedHexStr)
