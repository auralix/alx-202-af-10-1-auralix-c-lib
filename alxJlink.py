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
		exePath: str
	):
		#-------------------------------------------------------------------------------
		# Private Variables
		#-------------------------------------------------------------------------------

		# Parameters
		self.exePath: str = exePath;


	#-------------------------------------------------------------------------------
	# Public Functions
	#-------------------------------------------------------------------------------
	def Reset(self, targetName: str):
		# Prepare Script
		scriptText: str = """r
q"""
		# Script:
		# r			- Reset
		# q			- Quit
		with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".jlink") as script:
			script.write(scriptText)
			scriptPath = pathlib.Path(script.name)

		# Prepare & Execute Process
		self.PrepareExecuteProcess(targetName, scriptPath)

	def ResetErase(self, targetName: str):
		# Prepare Script
		scriptText: str = """r
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

	def ResetProgramVerifyReset(self, targetName: str, fwPath: str, addrHexStr: str):
		# Prepare Script
		scriptText: str = f"""
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

	def ResetEraseProgramVerifyReset(self, targetName: str, fwPath: str, addrHexStr: str):
		# Prepare Script
		scriptText: str = f"""
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


	#-------------------------------------------------------------------------------
	# Private Functions
	#-------------------------------------------------------------------------------
	def PrepareExecuteProcess(self, targetName: str, scriptPath: object):
		# Prepare Process
		scriptPathStr: str = str(scriptPath)
		args = [
			self.exePath,
			"-device", targetName,
			"-if", "SWD",
			"-speed", "4000",
			"-CommanderScript", scriptPathStr
		]

		# Execute Process
		print("DO: subprocess.run() ", args)
		try:
			completedProcessObj = subprocess.run(args, capture_output=True, check=True, timeout=60)
			print(completedProcessObj.stdout.decode())
		finally:
			scriptPath.unlink()
		print("DONE: subprocess.run()")
