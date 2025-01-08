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
import logging
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
		# Log
		logging.debug(
			f"ENTER: "
			f"targetName {targetName} "
			f"fwDir {fwDir} "
			f"addrAppHexStr {addrAppHexStr} "
			f"addrSignedHexStr {addrSignedHexStr} "
		)

		# Parameters
		self.__alxJlink = alxJlink
		self.__targetName = targetName
		self.__fwDir = fwDir
		self.__addrAppHexStr = addrAppHexStr
		self.__addrSignedHexStr = addrSignedHexStr

		# Log
		logging.debug("EXIT")


	#-------------------------------------------------------------------------------
	# Public Functions
	#-------------------------------------------------------------------------------
	def Reset(self):
		logging.debug("ENTER")
		self.__alxJlink.Reset(self.__targetName)
		logging.debug("EXIT")

	def Erase(self):
		logging.debug("ENTER")
		self.__alxJlink.ResetErase(self.__targetName)
		logging.debug("EXIT")

	def ProgramApp(self):
		# Log
		logging.debug("ENTER")

		# Set FW path
		fwAppPath = next(
			f for f in pathlib.Path(self.__fwDir).glob("*.bin")
			if not f.name.endswith("_Signed.bin") and not f.name.endswith("_NoBoot.bin")
		)
		fwAppPathStr = str(fwAppPath)

		# Program
		logging.debug(f"DO: alxJlink.ResetProgramVerifyReset() targetName {self.__targetName} fwAppPathStr {fwAppPathStr} addrAppHexStr {self.__addrAppHexStr}")
		self.__alxJlink.ResetProgramVerifyReset(self.__targetName, fwAppPathStr, self.__addrAppHexStr)
		logging.debug("DONE: alxJlink.ResetProgramVerifyReset()")

		# Log
		logging.debug("EXIT")

	def ProgramSigned(self):
		# Log
		logging.debug("ENTER")

		# Set FW path
		fwSignedPath = next(pathlib.Path(self.__fwDir).glob("*_Signed.bin"))
		fwSignedPathStr = str(fwSignedPath)

		# Program
		logging.debug(f"DO: alxJlink.ResetProgramVerifyReset() targetName {self.__targetName} fwSignedPathStr {fwSignedPathStr} addrSignedHexStr {self.__addrSignedHexStr}")
		self.__alxJlink.ResetProgramVerifyReset(self.__targetName, fwSignedPathStr, self.__addrSignedHexStr)
		logging.debug("DONE: alxJlink.ResetProgramVerifyReset()")

		# Log
		logging.debug("EXIT")
