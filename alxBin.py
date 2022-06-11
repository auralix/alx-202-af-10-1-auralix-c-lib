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
def Script(vsSolDir, vsPrjDir):
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
	binSrcName = pathlib.Path(vsPrjDir).stem + ".bin"
	binSrcDir = pathlib.Path(vsSolDir, "VisualGDB", "Debug")	# Relative path: VisualGDB\Debug
	binSrcPath = binSrcDir / binSrcName

	# Set fwArtf & fwName
	fwArtf = pathlib.Path(vsPrjDir).stem
	vsSolArtf = pathlib.Path(vsSolDir).stem
	vsSolArtfLen = len(vsSolArtf)
	vsSolArtfName = pathlib.Path(vsSolDir).parent.stem
	fwName = vsSolArtfName[vsSolArtfLen + 1:]

	# Create clean directory for destination bin
	binDstDirName = date + "_" + fwArtf + "_" + fwName + "_" + fwVerMajor + "_" + fwVerMinor + "_" + fwVerPatch + "_" + hashShort
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
	vsSolDir = sys.argv[1]
	vsPrjDir = sys.argv[2]

	# Script
	Script(vsSolDir, vsPrjDir)
