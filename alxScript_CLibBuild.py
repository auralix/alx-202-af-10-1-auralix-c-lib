#*******************************************************************************
# @file alxScript_CLibBuild.py
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
def Script(projectDir):
	# Print START
	print("")
	print("alxCLibBuild.py - START")

	# Set gitDir
	gitDir = pathlib.Path(projectDir).parent
	print("alxCLibBuild.py - gitDir: " + str(gitDir))

	# Set build date
	buildDate = datetime.datetime.now().strftime("%y%m%d%H%M")
	print("alxCLibBuild.py - buildDate: " + buildDate)

	# Set build hash from GIT hash
	cmdBuildHash = "git --git-dir=" + str(gitDir) + "\.git rev-parse HEAD"
	cmdBuildHashCompletedObj = subprocess.run(cmdBuildHash, capture_output=True)
	buildHash = cmdBuildHashCompletedObj.stdout.decode().rstrip('\n')
	print("alxCLibBuild.py - buildHash: " + buildHash)

	# Try to set FW version from GIT tag
	try:
		# Get
		cmdBuildTag = "git --git-dir=" + str(gitDir) + "\.git tag --points-at HEAD"
		cmdBuildTagCompletedObj = subprocess.run(cmdBuildTag, capture_output=True)
		buildTag = cmdBuildTagCompletedObj.stdout.decode().rstrip('\n')
		print("alxCLibBuild.py - buildTag: " + buildTag)

		# Split
		buildTagList = buildTag.split('.')
		fwVerMajorStr = buildTagList[0]
		fwVerMinorStr = buildTagList[1]
		fwVerPatchStr = buildTagList[2]

		# Check
		fwVerMajor = int(fwVerMajorStr)
		fwVerMinor = int(fwVerMinorStr)
		fwVerPatch = int(fwVerPatchStr)
		if (fwVerMajor > 255) or (fwVerMinor > 255) or (fwVerPatch > 255):
			raise
	except:
		print("alxCLibBuild.py - fwVer: valid GIT tag not available, FW version could not be set")
		fwVerMajorStr = "0"
		fwVerMinorStr = "0"
		fwVerPatchStr = "0"
	print("alxCLibBuild.py - fwVerMajorStr: " + fwVerMajorStr)
	print("alxCLibBuild.py - fwVerMinorStr: " + fwVerMinorStr)
	print("alxCLibBuild.py - fwVerPatchStr: " + fwVerPatchStr)

	# Prepare output file text
	outputFileText = """#ifndef ALX_BUILD_GENERATED_H
#define ALX_BUILD_GENERATED_H


#define ALX_BUILD_NAME "Engineer"
#define ALX_BUILD_DATE {buildDate}
#define ALX_BUILD_NUM 0
#define ALX_BUILD_HASH {buildHash}
#define ALX_BUILD_REV 0
#define ALX_BUILD_FW_VER_MAJOR {fwVerMajorStr}
#define ALX_BUILD_FW_VER_MINOR {fwVerMinorStr}
#define ALX_BUILD_FW_VER_PATCH {fwVerPatchStr}


#endif	// ALX_BUILD_GENERATED_H
""".format(buildDate=buildDate, buildHash=buildHash, fwVerMajorStr=fwVerMajorStr, fwVerMinorStr=fwVerMinorStr, fwVerPatchStr=fwVerPatchStr)

	# Write output file text
	outputFilePath = pathlib.Path("alxBuild_GENERATED.h")
	outputFilePath.write_text(outputFileText)
	print("alxCLibBuild.py - Generated: alxBuild_GENERATED.h")

	# Print
	print("alxCLibBuild.py - DONE")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param
	projectDir = sys.argv[1]

	# Script
	Script(projectDir)
