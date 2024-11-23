#*******************************************************************************
# @file			alxMonitor.py
# @brief		Auralix C Library - ALX Monitor Script
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
import sys
import shutil
import subprocess
import alxJlink
import time


#*******************************************************************************
# Script
#*******************************************************************************
def Script(progPath, targetName, fwDir, termExePath, termPort, termBaudRate, logDir):
	try:
		#-------------------------------------------------------------------------------
		# Print - START
		#-------------------------------------------------------------------------------
		print("")
		print(f"alxMonitor.py - START: progPath {progPath} targetName {targetName} fwDir {fwDir} termExePath {termExePath} termPort {termPort} termBaudRate {termBaudRate} logDir {logDir}")


		#-------------------------------------------------------------------------------
		# Init TeraTerm
		#-------------------------------------------------------------------------------

		# Set logPath
		fwAppPath = next(
			f for f in pathlib.Path(fwDir).glob("*.bin")
			if not f.name.endswith("_Signed.bin") and not f.name.endswith("_NoBoot.bin")
		)
		logFolderDir =  pathlib.Path(logDir) / fwAppPath.stem
		logPath = logFolderDir / f"{fwAppPath.stem}.log"

		# Create clean directory for logPath
		shutil.rmtree(logFolderDir, ignore_errors=True)
		pathlib.Path(logFolderDir).mkdir(parents=True, exist_ok=True)

		# Kill existing TeraTerm process
		argsKill = [
			"taskkill",
			"/IM",
			"ttermpro.exe",
			"/F"
		]
		print("DO: Kill existing TeraTerm process: subprocess.run()", argsKill)
		resultKill = subprocess.run(argsKill, capture_output=True, text=True)
		print(f"stdout: {resultKill.stdout.strip()} stderr: {resultKill.stderr.strip()}")
		print("DONE: Kill existing TeraTerm process")

		# Start new TeraTerm process
		argsStart = [
			termExePath,
			f"/C={termPort}",
			f"/BAUD={termBaudRate}",
			f"/L={logPath}"
		]
		DETACHED_PROCESS = 0x00000008
		print("DO: Start new TeraTerm process: subprocess.Popen()", argsStart)
		resultStart = subprocess.Popen(argsStart, creationflags=DETACHED_PROCESS)
		print("DONE: Start new TeraTerm process")

		# Check if new TeraTerm process is running
		argsCheck = [
			"tasklist",
			"/FI",
			f"PID eq {resultStart.pid}"
		]
		print("DO: Check if new TeraTerm process is running: subprocess.run()", argsCheck)
		resultCheck = subprocess.run(argsCheck, capture_output=True, text=True, check=True)
		print("DONE: Check if new TeraTerm process is running")
		time.sleep(20)


		#-------------------------------------------------------------------------------
		# Reset FW
		#-------------------------------------------------------------------------------
		print(f"DO: Reset FW")
		alxJlinkObj = alxJlink.Jlink(progPath)
		alxJlinkObj.Reset(targetName)
		print("DONE: Reset FW")

	except Exception as e:
		#-------------------------------------------------------------------------------
		# Print - EXCEPTION
		#-------------------------------------------------------------------------------
		print(f"alxMonitor.py - EXCEPTION: {e}")
		print("")
		sys.exit(1)

	finally:
		#-------------------------------------------------------------------------------
		# FINISH
		#-------------------------------------------------------------------------------
		print("alxMonitor.py - FINISH")
		print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	progPath = sys.argv[1]
	targetName = sys.argv[2]
	fwDir = sys.argv[3]
	termExePath = sys.argv[4]
	termPort = sys.argv[5]
	termBaudRate = sys.argv[6]
	logDir = sys.argv[7]

	# Script
	Script(progPath, targetName, fwDir, termExePath, termPort, termBaudRate, logDir)
