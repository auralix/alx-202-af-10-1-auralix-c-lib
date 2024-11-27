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
import time
import alxJlink


#*******************************************************************************
# Script
#*******************************************************************************
def Script(progPath, targetName, fwDir, addrAppHexStr, addrSignedHexStr, termPort, termBaudRate, logDir):
	try:
		#-------------------------------------------------------------------------------
		# Print - START
		#-------------------------------------------------------------------------------
		print("")
		print(f"alxMonitor.py - START: progPath {progPath} targetName {targetName} fwDir {fwDir} termPort {termPort} termBaudRate {termBaudRate} logDir {logDir}")


		#-------------------------------------------------------------------------------
		# Init Dir & Path
		#-------------------------------------------------------------------------------
		alxJlinkObj = alxJlink.Jlink(progPath)

		# Print
		print("DO: Init Dir & Path")

		# Set logFolderDir & pidPath
		fwAppPath = next(
			f for f in pathlib.Path(fwDir).glob("*.bin")
			if not f.name.endswith("_Signed.bin") and not f.name.endswith("_NoBoot.bin")
		)
		fwAppPathStr = str(fwAppPath)
		logFolderDir =  pathlib.Path(logDir) / fwAppPath.stem
		pidPath = pathlib.Path(logDir) / "alxSerialPortLogger.pid"
		logPath = logFolderDir / f"{logFolderDir.name}.log"

		# Create clean directory for logPath
		shutil.rmtree(logFolderDir, ignore_errors=True)
		pathlib.Path(logFolderDir).mkdir(parents=True, exist_ok=True)

		# Print
		print(f"DONE: Init Dir & Path: logFolderDir {logFolderDir} pidPath {pidPath}")


		#-------------------------------------------------------------------------------
		# Kill Existing alxSerialPortLogger Process
		#-------------------------------------------------------------------------------
		if pidPath.exists():
			try:
				# Read PID file
				pid = int(pidPath.read_text().strip())

				# Kill Process
				print(f"DO: Kill existing alxSerialPortLogger process: subprocess.run() pid {pid}")
				resultKill = subprocess.run(
					["taskkill", "/PID", str(pid), "/F"],
					capture_output=True,
					text=True,
					check=True
				)
				print(f"stdout: {resultKill.stdout.strip()} stderr: {resultKill.stderr.strip()}")
				print(f"DONE: Kill existing alxSerialPortLogger process")
			except subprocess.CalledProcessError as e:
				print(f"FAIL: Failed to kill process: PID {pid}: {e}")
			except ValueError:
				print(f"FAIL: Invalid PID: pidPath {pidPath}")
			finally:
				# Delete PID file
				pidPath.unlink(missing_ok=True)


		#-------------------------------------------------------------------------------
		# Erase FW
		#-------------------------------------------------------------------------------
		print(f"DO: Erase FW")
		alxJlinkObj.ResetErase(targetName)
		print("DONE: Erase FW")


		#-------------------------------------------------------------------------------
		# Start New alxSerialPortLogger Process
		#-------------------------------------------------------------------------------

		# Prepare
		loggerScriptPath = pathlib.Path(__file__).parent / "alxSerialPortLogger.py"
		argsStart = [
			sys.executable,
			str(loggerScriptPath),
			termPort,
			termBaudRate,
			str(logFolderDir)
		]
		DETACHED_PROCESS = 0x00000008

		# Start
		print("DO: Start new alxSerialPortLogger process: subprocess.Popen()", argsStart)
		resultStart = subprocess.Popen(
			argsStart,
			creationflags=DETACHED_PROCESS
		)
		print(f"DONE: Start new alxSerialPortLogger process: PID {resultStart.pid}")

		# Save new PID
		pidPath.write_text(str(resultStart.pid))


		#-------------------------------------------------------------------------------
		# Check if alxSerialPortLogger Process is Running
		#-------------------------------------------------------------------------------
		time.sleep(2)
		print("DO: Check if alxSerialPortLogger process is running")
		logText = logPath.read_text().strip()
		if logText.endswith("alxSerialPortLogger.py - FINISH"):
			print("FAIL: alxSerialPortLogger NOT running")
			raise
		print("DONE: Check if alxSerialPortLogger process is running")


		#-------------------------------------------------------------------------------
		# Program FW APP
		#-------------------------------------------------------------------------------
		print(f"DO: Program FW APP: ResetProgramVerifyReset() fwAppPathStr {fwAppPathStr}")
		alxJlinkObj.ResetProgramVerifyReset(targetName, fwAppPathStr, addrAppHexStr)
		print("DONE: Program FW APP")


		#-------------------------------------------------------------------------------
		# Program FW Signed
		#-------------------------------------------------------------------------------
		if addrSignedHexStr != "":
			# Set FW path
			fwSignedPath = next(pathlib.Path(fwDir).glob("*_Signed.bin"))
			fwSignedPathStr = str(fwSignedPath)

			# Program
			print(f"DO: Program FW Signed: ResetProgramVerifyReset() fwSignedPathStr {fwSignedPathStr}")
			alxJlinkObj.ResetProgramVerifyReset(targetName, fwSignedPathStr, addrSignedHexStr)
			print("DONE: Program FW Signed")

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
	termPort = sys.argv[4]
	termBaudRate = sys.argv[5]
	logDir = sys.argv[6]

	# Script
	Script(progPath, targetName, fwDir, termPort, termBaudRate, logDir)
