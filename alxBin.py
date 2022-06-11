#*******************************************************************************
# @file alxBin.py
# @brief Auralix Python Library
# @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
#*******************************************************************************


#*******************************************************************************
# Imports
#*******************************************************************************
import pathlib
import shutil
import sys


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsGdbSolutionDir):
	# Read input file
	inFilePath = pathlib.Path("alxBuild_GENERATED.h")
	inFileText = inFilePath.read_text()
	inFileLines = inFileText.splitlines()

	# Parse input file
	date = inFileLines[5][23:]
	_hash = inFileLines[7][24:64]
	hashShort = _hash[0:7]
	fwVerMajor = inFileLines[9][31:]
	fwVerMinor = inFileLines[10][31:]
	fwVerPatch = inFileLines[11][31:]

	# Set source bin variables
	binSrcName = pathlib.Path(vsGdbSolutionDir).stem + ".bin"
	binSrcDir = pathlib.Path(vsGdbSolutionDir, "VisualGDB", "Debug")	# Relative path: VisualGDB\Debug
	binSrcPath = binSrcDir / binSrcName

	# Create clean directory for destination bin
	fwArtfAndName = pathlib.Path(vsGdbSolutionDir).parent.stem
	binDstDirName = date + "_" + fwArtfAndName + "_" + fwVerMajor + "_" + fwVerMinor + "_" + fwVerPatch + "_" + hashShort
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
	vsGdbSolutionDir = sys.argv[1]

	# Script
	Script(vsGdbSolutionDir)
