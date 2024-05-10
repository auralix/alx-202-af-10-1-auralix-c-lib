#*******************************************************************************
# @file			alxBoot.py
# @brief		Auralix C Library - ALX Bootloader Script
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
import shutil
import sys
import subprocess


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsTargetPath, imgSlotSize):
	# Print START
	print("")
	print("alxBoot.py - Script START")

	# Read input file
	inFilePath = pathlib.Path("alxBuild_GENERATED.h")
	inFileText = inFilePath.read_text()
	inFileLines = inFileText.splitlines()

	# Parse input file
	date = inFileLines[5][23:]
	hashShort = inFileLines[8][30:37]
	fwVerMajor = inFileLines[10][31:]
	fwVerMinor = inFileLines[11][31:]
	fwVerPatch = inFileLines[12][31:]

	# Set source bin variables
	binSrcName = pathlib.Path(vsTargetPath).stem + ".bin"
	binSrcDir = pathlib.Path(vsTargetPath).parent
	binSrcPath = binSrcDir / binSrcName

	# Set imgtool variables
	imgtoolPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "Sub" / "mcuboot" / "scripts" / "imgtool.py"

	# Run cmd
	cmd = (r"python {imgtoolPath} sign"
		r" --header-size 0x200"
		r" --pad-header"
		r" --slot-size {imgSlotSize}"
		r" --version {fwVerMajor}.{fwVerMinor}.{fwVerPatch}+{date}"
		r" {binSrcPathIn}"
		r" {binSrcPathOut}").format(
		imgtoolPath=imgtoolPath,
		imgSlotSize=imgSlotSize,
		fwVerMajor=fwVerMajor,
		fwVerMinor=fwVerMinor,
		fwVerPatch=fwVerPatch,
		date=date,
		binSrcPathIn=binSrcPath,
		binSrcPathOut=binSrcPath
	)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True)

	# Print
	print("alxBoot.py - Script FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param
	vsTargetPath = sys.argv[1]
	imgSlotSize = sys.argv[2]

	# Script
	Script(vsTargetPath, imgSlotSize)
