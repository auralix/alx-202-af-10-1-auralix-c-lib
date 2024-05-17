#*******************************************************************************
# @file			alxBin.py
# @brief		Auralix C Library - ALX .bin Script
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


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsTargetPath, fwName):
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
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix(".bin")
	binSrcDir = binSrcPath.parent
	binSrcName = binSrcPath.name

	# Set fwArtf
	fwArtf = binSrcPath.stem

	# Create clean directory for destination bin
	binDstDirName = date + "_" + fwArtf + "_" + fwName + "_" + "V" + fwVerMajor + "-" + fwVerMinor + "-" + fwVerPatch + "_" + hashShort
	binDstDir = binSrcDir / binDstDirName
	shutil.rmtree(binDstDir, ignore_errors=True)
	pathlib.Path(binDstDir).mkdir(parents=True, exist_ok=True)

	# Copy source bin to destination bin directory
	shutil.copy(binSrcPath, binDstDir)

	# Rename source bin to destination bin
	binDstPath = binDstDir / binSrcName
	binDstPath = binDstPath.rename(binDstDir / (binDstDirName + ".bin"))

	# Print
	print("")
	print("alxBin.py - Generated:")
	print(binDstPath.name)
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param
	vsTargetPath = sys.argv[1]
	fwName = sys.argv[2]

	# Script
	Script(vsTargetPath, fwName)
