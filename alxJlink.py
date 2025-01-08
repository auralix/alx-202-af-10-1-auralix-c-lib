#*******************************************************************************
# @file			alxJlink.py
# @brief		Auralix C Library - ALX J-Link Script
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
import subprocess
import pathlib
import logging
import tempfile


#*******************************************************************************
# Class - Jlink
#*******************************************************************************
class Jlink:
	#-------------------------------------------------------------------------------
	# Ctor
	#-------------------------------------------------------------------------------
	def __init__(
		self,
		exePath
	):
		# Log
		logging.debug(f"ENTER: exePath {exePath}")

		# Parameters
		self.__exePath = exePath;

		# Log
		logging.debug("EXIT")


	#-------------------------------------------------------------------------------
	# Public Functions
	#-------------------------------------------------------------------------------
	def Reset(self, targetName):
		# Log
		logging.debug(f"ENTER: targetName {targetName}")

		# Prepare Script
		scriptText = """r
q"""
		# Script:
		# r			- Reset
		# q			- Quit
		with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".jlink") as script:
			script.write(scriptText)
			scriptPath = pathlib.Path(script.name)

		# Prepare & Execute Process
		self.PrepareExecuteProcess(targetName, scriptPath)

		# Log
		logging.debug("EXIT")

	def ResetErase(self, targetName):
		# Log
		logging.debug(f"ENTER: targetName {targetName}")

		# Prepare Script
		scriptText = """r
h
n
erase
q"""
		# Script:
		# r			- Reset
		# erase		- Erase
		# q			- Quit
		with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".jlink") as script:
			script.write(scriptText)
			scriptPath = pathlib.Path(script.name)

		# Prepare & Execute Process
		self.PrepareExecuteProcess(targetName, scriptPath)

		# Log
		logging.debug("EXIT")

	def ResetProgramVerifyReset(self, targetName, fwPath, addrHexStr):
		# Log
		logging.debug(
			f"ENTER: "
			f"targetName {targetName} "
			f"fwPath {fwPath} "
			f"addrHexStr {addrHexStr} "
		)

		# Prepare Script
		scriptText = f"""
r
h
loadbin {fwPath} {addrHexStr}
verifybin {fwPath} {addrHexStr}s
r
q"""
		# Script:
		# r			- Reset
		# h			- Halt
		# loadbin	- Load .bin at addrHexStr
		# verifybin	- Verify .bin at addrHexStr
		# r			- Reset
		# q			- Quit
		with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".jlink") as script:
			script.write(scriptText)
			scriptPath = pathlib.Path(script.name)

		# Prepare & Execute Process
		self.PrepareExecuteProcess(targetName, scriptPath)

		# Log
		logging.debug("EXIT")

	def ResetEraseProgramVerifyReset(self, targetName, fwPath, addrHexStr):
		# Log
		logging.debug(
			f"ENTER: "
			f"targetName {targetName} "
			f"fwPath {fwPath} "
			f"addrHexStr {addrHexStr} "
		)

		# Prepare Script
		scriptText = f"""
r
h
erase
loadbin {fwPath} {addrHexStr}
verifybin {fwPath} {addrHexStr}s
r
q"""
		# Script:
		# r			- Reset
		# h			- Halt
		# erase		- Erase
		# loadbin	- Load .bin at addrHexStr
		# verifybin	- Verify .bin at addrHexStr
		# r			- Reset
		# q			- Quit
		with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".jlink") as script:
			script.write(scriptText)
			scriptPath = pathlib.Path(script.name)

		# Prepare & Execute Process
		self.PrepareExecuteProcess(targetName, scriptPath)

		# Log
		logging.debug("EXIT")


	#-------------------------------------------------------------------------------
	# Private Functions
	#-------------------------------------------------------------------------------
	def PrepareExecuteProcess(self, targetName, scriptPath):
		# Log
		logging.debug("ENTER")

		# Prepare Process
		scriptPathStr = str(scriptPath)
		args = [
			self.__exePath,
			"-device", targetName,
			"-if", "SWD",
			"-speed", "4000",
			"-CommanderScript", scriptPathStr
		]

		# Execute Process
		logging.debug(f"DO: subprocess.run() args {args}")
		try:
			completedProcessObj = subprocess.run(args, capture_output=True, check=True, timeout=60)
			logging.debug(completedProcessObj.stdout.decode())
		finally:
			scriptPath.unlink()
		logging.debug("DONE: subprocess.run()")

		# Log
		logging.debug("EXIT")
