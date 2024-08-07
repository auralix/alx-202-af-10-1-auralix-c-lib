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
def Script(vsTargetPath, fwName, binRawAndBinSignedCopyEnable, bootHdrFileGenEnable, bootHdrFileLenHexStr):
	# Print
	print("")
	print("alxBin.py - START")

	# Read input file
	inFilePath = pathlib.Path("alxBuild_GENERATED.h")
	inFileText = inFilePath.read_text()
	inFileLines = inFileText.splitlines()

	# Parse input file
	date = inFileLines[5][23:]
	hashShort = inFileLines[8][30:37]
	fwVerMajor = inFileLines[11][31:]
	fwVerMinor = inFileLines[12][31:]
	fwVerPatch = inFileLines[13][31:]

	# Set source bin variables
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix(".bin")
	binSrcDir = binSrcPath.parent

	# Set fwArtf
	fwArtf = binSrcPath.stem

	# Set destination bin variables
	binDstDirName = date + "_" + fwArtf + "_" + fwName + "_" + "V" + fwVerMajor + "-" + fwVerMinor + "-" + fwVerPatch + "_" + hashShort
	binDstDir = binSrcDir / binDstDirName
	binDstName = binDstDirName + ".bin"

	# Create clean directory for destination bin
	shutil.rmtree(binDstDir, ignore_errors=True)
	pathlib.Path(binDstDir).mkdir(parents=True, exist_ok=True)

	# Copy source bin to destination bin directory & rename it to destination bin
	shutil.copy2(binSrcPath, binDstDir / binDstName)

	# Print
	print("Generated: " + binDstName)

	# If bin _Raw & _Signed copy enabled
	if binRawAndBinSignedCopyEnable == "True":
		# Set source bin variables
		binRawSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Raw' + binSrcPath.suffix)
		binSignedSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Signed' + binSrcPath.suffix)

		# Set destination bin variables
		binRawDstName = binDstDirName + "_Raw.bin"
		binSignedDstName = binDstDirName + "_Signed.bin"

		# Copy source bin to destination bin directory & rename it to destination bin
		shutil.copy2(binRawSrcPath, binDstDir / binRawDstName)
		shutil.copy2(binSignedSrcPath, binDstDir / binSignedDstName)

		# Print
		print("Generated: " + binRawDstName)
		print("Generated: " + binSignedDstName)

	# If bootloader header file generation enabled
	if bootHdrFileGenEnable == "True":
		# Read bin
		binData = binSrcPath.read_bytes()
		binLen = len(binData)

		# Set bootHdr variables
		bootHdrDstName = binDstDirName + ".h"
		bootHdrLen = int(bootHdrFileLenHexStr, 16)

		bootHdrData = binData + bytes([0xFF] * (bootHdrLen - binLen))
		bootHdrArr = ", ".join(f"0x{byte:02X}" for byte in bootHdrData)

		bootHdrDataFF = bytes([0xFF] * bootHdrLen)
		bootHdrArrFF = ", ".join(f"0x{byte:02X}" for byte in bootHdrDataFF)

		# Prepare bootloader header file text
		bootHdrText = """#ifndef ALX_BOOT_GENERATED_H
#define ALX_BOOT_GENERATED_H


// {binDstName}
#if defined(ALX_BUILD_CONFIG_DEBUG)
static const unsigned char boot[{bootHdrFileLenHexStr}] __attribute__((section(".boot"), used)) = {{{bootHdrArr}}};
#endif
#if defined(ALX_BUILD_CONFIG_FW_UP)
static const unsigned char boot[{bootHdrFileLenHexStr}] __attribute__((section(".boot"), used)) = {{{bootHdrArrFF}}};
#endif


#endif	// ALX_BOOT_GENERATED_H
""".format(binDstName=binDstName, bootHdrFileLenHexStr=bootHdrFileLenHexStr, bootHdrArr=bootHdrArr, bootHdrArrFF=bootHdrArrFF)

		# Write bootloader header file text
		bootHdrSrcPath = binSrcDir / "alxBoot_GENERATED.h"
		bootHdrSrcPath.write_text(bootHdrText)
		bootHdrDstPath = binDstDir / bootHdrDstName
		bootHdrDstPath.write_text(bootHdrText)

		# Print
		print("Generated: " + bootHdrDstName)

	# Print
	print("alxBin.py - FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	vsTargetPath = sys.argv[1]
	fwName = sys.argv[2]
	if len(sys.argv) > 3:
		binRawAndBinSignedCopyEnable = sys.argv[3]
	else:
		binRawAndBinSignedCopyEnable = "False"
	if len(sys.argv) > 4:
		bootHdrFileGenEnable = sys.argv[4]
		bootHdrFileLenHexStr = sys.argv[5]
	else:
		bootHdrFileGenEnable = "False"
		bootHdrFileLenHexStr = "0x00000000"

	# Script
	Script(vsTargetPath, fwName, binRawAndBinSignedCopyEnable, bootHdrFileGenEnable, bootHdrFileLenHexStr)
