#*******************************************************************************
# @file			alxBuild.py
# @brief		Auralix C Library - ALX Build Script
# @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
import datetime
import subprocess
import sys


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsSolDir):
	# Print
	print("")
	print("alxBuild.py - START")

	# Set gitDir
	gitDir = pathlib.Path(vsSolDir).parent
	print("alxBuild.py - gitDir: " + str(gitDir))

	# Set build date
	date = datetime.datetime.now().strftime("%y%m%d%H%M")
	print("alxBuild.py - buildDate: " + date)

	# Set build hash from GIT hash
	cmdHash = "git --git-dir=" + str(gitDir) + "\.git rev-parse HEAD"
	cmdHashCompletedObj = subprocess.run(cmdHash, capture_output=True)
	if cmdHashCompletedObj.returncode == 0:
		_hash = cmdHashCompletedObj.stdout.decode().rstrip('\n')
		hashShort = _hash[0:7]
	else:
		print("alxBuild.py - Not a GIT repo, hash could not be set")
		_hash = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
		hashShort = "abcdefa"
	print("alxBuild.py - buildHash: " + _hash)
	print("alxBuild.py - buildHashShort: " + hashShort)

	# Try to set FW version from GIT tag
	try:
		# Get
		cmdTag = "git --git-dir=" + str(gitDir) + "\.git tag --points-at HEAD"
		cmdTagCompletedObj = subprocess.run(cmdTag, capture_output=True)
		tag = cmdTagCompletedObj.stdout.decode().rstrip('\n')
		tagOnlyNum = tag.lstrip('v')

		# Split
		tagList = tagOnlyNum.split('.')
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
		tag ="v0.0.0"
		fwVerMajorStr = "0"
		fwVerMinorStr = "0"
		fwVerPatchStr = "0"

	# Print
	print("alxBuild.py - buildTag: " + tag)
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
#define ALX_BUILD_HASH_SHORT "{hashShort}"
#define ALX_BUILD_REV 0
#define ALX_BUILD_FW_VER_MAJOR {fwVerMajorStr}
#define ALX_BUILD_FW_VER_MINOR {fwVerMinorStr}
#define ALX_BUILD_FW_VER_PATCH {fwVerPatchStr}


#endif	// ALX_BUILD_GENERATED_H
""".format(date=date, _hash=_hash, hashShort=hashShort, fwVerMajorStr=fwVerMajorStr, fwVerMinorStr=fwVerMinorStr, fwVerPatchStr=fwVerPatchStr)

	# Write output file text
	outFilePath = pathlib.Path("alxBuild_GENERATED.h")
	outFilePath.write_text(outFileText)
	print("alxBuild.py - Generated: alxBuild_GENERATED.h")

	# Print
	print("alxBuild.py - FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	vsSolDir = sys.argv[1]

	# Script
	Script(vsSolDir)
