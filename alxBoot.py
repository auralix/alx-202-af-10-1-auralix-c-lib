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
def Script(vsTargetPath, imgSlotSize, bootSize):
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

	# Convert boot size from hex to int
	bootSize = int(bootSize, 16)
	headerSize = 0x200  # 512 bytes
	trailerSize = 0x28  # 40 bytes

	# Set source bin variables
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix('.bin')

	# Define paths for temporary and final files
	appTmpPath = binSrcPath.with_name('app_data.bin')
	signedAppPath = binSrcPath.with_name('app_data_signed.bin')
	finalOutputPath = binSrcPath  # This will be the final output with the original name
	rawFilePath = binSrcPath.with_name(binSrcPath.stem + '_raw.bin')

	# Check if the raw file already exists and delete it if it does
	if rawFilePath.exists():
		rawFilePath.unlink()

	# Rename the original file to raw file
	binSrcPath.rename(rawFilePath)

	# Read the original (now raw) file
	originalData = rawFilePath.read_bytes()

	# Calculate application data offset and size
	appStartOffset = bootSize + headerSize
	appData = originalData[appStartOffset:]

	# Write the extracted application data to a temporary file
	appTmpPath.write_bytes(appData)

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
		binSrcPathIn=appTmpPath,
		binSrcPathOut=signedAppPath
	)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True)

	# Read the signed application data
	signedAppData = signedAppPath.read_bytes()

	# Create the final combined file
	finalData = (
		originalData[:bootSize] +
		signedAppData
	)
	finalOutputPath.write_bytes(finalData)

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
	bootSize = sys.argv[3]

	# Script
	Script(vsTargetPath, imgSlotSize, bootSize)
