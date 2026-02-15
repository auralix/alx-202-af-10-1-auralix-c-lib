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


"""
Auralix C Library - ALX Build Script

Generates ``alxBuild_GENERATED.h`` with build metadata (build date, Git commit
hash/short hash, and firmware version from a tag pointing at ``HEAD``).
Intended for VisualGDB post-build steps or manual CLI use.
"""


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
def Script(vsSolDir: str) -> None:
	"""Generate ``alxBuild_GENERATED.h`` with build metadata.

	Infers the repository root from the parent of ``vsSolDir``, resolves the
	current Git commit (and 32-bit short form), and, if a tag points at
	``HEAD``, parses a firmware version (``vMAJOR.MINOR.PATCH``) to emit
	``#define`` macros.

	Args:
		vsSolDir: Absolute path to the VisualGDB solution directory; the Git
			directory is assumed at ``Path(vsSolDir).parent / ".git"``.

	Returns:
		None

	Raises:
		OSError: If writing the output header file fails.

	Side Effects:
		Writes ``alxBuild_GENERATED.h`` in the CWD and invokes ``git``.
	"""

	# Print
	print("")
	print("alxBuild.py - START")

	# Set gitDir
	gitDir = pathlib.Path(vsSolDir).parent
	print("gitDir: " + str(gitDir))

	# Set build date
	date = datetime.datetime.now().strftime("%y%m%d%H%M")
	print("buildDate: " + date)

	# Set build hash from GIT hash
	cmdHash = "git --git-dir=" + str(gitDir) + "\\.git rev-parse HEAD"
	cmdHashCompletedObj = subprocess.run(cmdHash, capture_output=True)
	if cmdHashCompletedObj.returncode == 0:
		_hash = cmdHashCompletedObj.stdout.decode().rstrip('\n')
	else:
		print("Not a GIT repo, hash could not be set")
		_hash = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
	hashShort = _hash[0:7]
	hashShortUint32 = int(hashShort, 16)
	hashShortUint32Str = "0x{0:08X}".format(hashShortUint32)
	print("buildHash: " + _hash)
	print("buildHashShort: " + hashShort)
	print("buildHashShortUint32: " + hashShortUint32Str)

	# Try to set FW version from GIT tag
	try:
		# Get
		cmdTag = "git --git-dir=" + str(gitDir) + "\\.git tag --points-at HEAD"
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
		print("fwVer: No valid GIT tag, FW version could not be set")
		tag ="v0.0.0"
		fwVerMajorStr = "0"
		fwVerMinorStr = "0"
		fwVerPatchStr = "0"

	# Print
	print("buildTag: " + tag)
	print("fwVerMajor: " + fwVerMajorStr)
	print("fwVerMinor: " + fwVerMinorStr)
	print("fwVerPatch: " + fwVerPatchStr)

	# Prepare output file text
	outFileText = """#ifndef ALX_BUILD_GENERATED_H
#define ALX_BUILD_GENERATED_H


#define ALX_BUILD_NAME "VisualGDB Local"
#define ALX_BUILD_DATE {date}
#define ALX_BUILD_NUM 0
#define ALX_BUILD_HASH "{_hash}"
#define ALX_BUILD_HASH_SHORT "{hashShort}"
#define ALX_BUILD_HASH_SHORT_UINT32 {hashShortUint32Str}
#define ALX_BUILD_REV 0
#define ALX_BUILD_FW_VER_MAJOR {fwVerMajorStr}
#define ALX_BUILD_FW_VER_MINOR {fwVerMinorStr}
#define ALX_BUILD_FW_VER_PATCH {fwVerPatchStr}


#endif	// ALX_BUILD_GENERATED_H
""".format(date=date, _hash=_hash, hashShort=hashShort, hashShortUint32Str=hashShortUint32Str, fwVerMajorStr=fwVerMajorStr, fwVerMinorStr=fwVerMinorStr, fwVerPatchStr=fwVerPatchStr)

	# Write output file text
	outFilePath = pathlib.Path("alxBuild_GENERATED.h")
	outFilePath.write_text(outFileText)
	print("Generated: alxBuild_GENERATED.h")

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
