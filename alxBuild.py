#*******************************************************************************
# @file alxBuild.py
# @brief Auralix Python Library
# @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
#*******************************************************************************


#*******************************************************************************
# Imports
#*******************************************************************************
import pathlib
import datetime
import subprocess
import sys


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsGdbSolutionDir):
	# Print START
	print("")
	print("alxBuild.py - Script START")

	# Set gitDir
	gitDir = pathlib.Path(vsGdbSolutionDir).parent
	print("alxBuild.py - gitDir: " + str(gitDir))

	# Set build date
	date = datetime.datetime.now().strftime("%y%m%d%H%M")
	print("alxBuild.py - buildDate: " + date)

	# Set build hash from GIT hash
	cmdHash = "git --git-dir=" + str(gitDir) + "\.git rev-parse HEAD"
	cmdHashCompletedObj = subprocess.run(cmdHash, capture_output=True)
	_hash = cmdHashCompletedObj.stdout.decode().rstrip('\n')
	print("alxBuild.py - buildHash: " + _hash)

	# Try to set FW version from GIT tag
	try:
		# Get
		cmdTag = "git --git-dir=" + str(gitDir) + "\.git tag --points-at HEAD"
		cmdTagCompletedObj = subprocess.run(cmdTag, capture_output=True)
		tag = cmdTagCompletedObj.stdout.decode().rstrip('\n')
		print("alxBuild.py - buildTag: " + tag)

		# Split
		tagList = tag.split('.')
		fwVerMajorStr = tagList[0]
		fwVerMinorStr = tagList[1]
		fwVerPatchStr = tagList[2]

		# Check
		fwVerMajor = int(fwVerMajorStr)
		fwVerMinor = int(fwVerMinorStr)
		fwVerPatch = int(fwVerPatchStr)
		if (fwVerMajor > 255) or (fwVerMinor > 255) or (fwVerPatch > 255):
			raise
	except:
		print("alxBuild.py - fwVer: No valid GIT tag, FW version could not be set")
		fwVerMajorStr = "0"
		fwVerMinorStr = "0"
		fwVerPatchStr = "0"
	print("alxBuild.py - fwVerMajor: " + fwVerMajorStr)
	print("alxBuild.py - fwVerMinor: " + fwVerMinorStr)
	print("alxBuild.py - fwVerPatch: " + fwVerPatchStr)

	# Prepare output file text
	outFileText = """#ifndef ALX_BUILD_GENERATED_H
#define ALX_BUILD_GENERATED_H


#define ALX_BUILD_NAME "VisualGDB Local"
#define ALX_BUILD_DATE {date}
#define ALX_BUILD_NUM 0
#define ALX_BUILD_HASH "{_hash}"
#define ALX_BUILD_REV 0
#define ALX_BUILD_FW_VER_MAJOR {fwVerMajorStr}
#define ALX_BUILD_FW_VER_MINOR {fwVerMinorStr}
#define ALX_BUILD_FW_VER_PATCH {fwVerPatchStr}


#endif	// ALX_BUILD_GENERATED_H
""".format(date=date, _hash=_hash, fwVerMajorStr=fwVerMajorStr, fwVerMinorStr=fwVerMinorStr, fwVerPatchStr=fwVerPatchStr)

	# Write output file text
	outFilePath = pathlib.Path("alxBuild_GENERATED.h")
	outFilePath.write_text(outFileText)
	print("alxBuild.py - Generated: alxBuild_GENERATED.h")

	# Print
	print("alxBuild.py - Script FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param
	vsGdbSolutionDir = sys.argv[1]

	# Script
	Script(vsGdbSolutionDir)
