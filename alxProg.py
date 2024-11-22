#*******************************************************************************
# @file			alxProg.py
# @brief		Auralix C Library - ALX Program Script
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
import alxJlink


#*******************************************************************************
# Script
#*******************************************************************************
def Script(progPath, targetName, fwDir, addrAppHexStr, addrSignedHexStr):
	try:
		#-------------------------------------------------------------------------------
		# Print
		#-------------------------------------------------------------------------------
		print("")
		print(f"alxProg.py - START: progPath {progPath} targetName {targetName} fwDir {fwDir} addrAppHexStr {addrAppHexStr} addrSignedHexStr {addrSignedHexStr}")


		#-------------------------------------------------------------------------------
		# Program FW APP
		#-------------------------------------------------------------------------------

		# Set FW path
		fwAppPath = next(
			f for f in pathlib.Path(fwDir).glob("*.bin")
			if not f.name.endswith("_Signed.bin") and not f.name.endswith("_NoBoot.bin")
		)
		fwAppPathStr = str(fwAppPath)

		# Program
		print(f"DO: Program FW APP: ResetEraseProgramVerifyReset() fwAppPathStr {fwAppPathStr}")
		alxJlinkObj = alxJlink.Jlink(progPath)
		alxJlinkObj.ResetEraseProgramVerifyReset(targetName, fwAppPathStr, addrAppHexStr)
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
			alxJlinkObj = alxJlink.Jlink(progPath)
			alxJlinkObj.ResetProgramVerifyReset(targetName, fwSignedPathStr, addrSignedHexStr)
			print("DONE: Program FW Signed")


		#-------------------------------------------------------------------------------
		# Print
		#-------------------------------------------------------------------------------
		print("alxProg.py - FINISH")
		print("")
	except Exception as e:
		print(f"alxProg.py - EXCEPTION: {e}")
		print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	progPath = sys.argv[1]
	targetName = sys.argv[2]
	fwDir = sys.argv[3]
	addrAppHexStr = sys.argv[4]
	if len(sys.argv) > 5:
		addrSignedHexStr = sys.argv[5]
	else:
		addrSignedHexStr = ""

	# Script
	Script(progPath, targetName, fwDir, addrAppHexStr, addrSignedHexStr)
