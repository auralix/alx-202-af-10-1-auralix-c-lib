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
def Script(vsTargetPath, fwName, copyFwUpNoBoot2Enable, binRawBinSignedManifestGenEnable, bootHdrFileGenEnable, bootHdrFileLenHexStr):
	#-------------------------------------------------------------------------------
	# Print
	#-------------------------------------------------------------------------------
	print("")
	print(f"alxBin.py - START: vsTargetPath {vsTargetPath} fwName {fwName} copyFwUpNoBoot2Enable {copyFwUpNoBoot2Enable} binRawBinSignedManifestGenEnable {binRawBinSignedManifestGenEnable} bootHdrFileGenEnable {bootHdrFileGenEnable} bootHdrFileLenHexStr {bootHdrFileLenHexStr}")


	#-------------------------------------------------------------------------------
	# Read alxBuild_GENERATED.h
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Read alxBuild_GENERATED.h")

	# Read
	buildFilePath = pathlib.Path("alxBuild_GENERATED.h")
	buildFileText = buildFilePath.read_text()
	buildFileLines = buildFileText.splitlines()

	# Parse
	date = buildFileLines[5][23:]
	hashShort = buildFileLines[8][30:37]
	fwVerMajor = buildFileLines[11][31:]
	fwVerMinor = buildFileLines[12][31:]
	fwVerPatch = buildFileLines[13][31:]

	# Print
	print("DONE: Read alxBuild_GENERATED.h")


	#-------------------------------------------------------------------------------
	# Generate .bin Destination Directory & .bin
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Generate .bin Destination Directory & .bin")

	# Set binSrc variables
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix(".bin")
	binSrcDir = binSrcPath.parent
	binSrcDirName = binSrcDir.name

	# Set fwArtf
	fwArtf = binSrcPath.stem

	# Set binDst variables
	binDstDirName = date + "_" + fwArtf + "_" + fwName + "_" + "V" + fwVerMajor + "-" + fwVerMinor + "-" + fwVerPatch + "_" + hashShort
	binDstDir = binSrcDir / binDstDirName
	if binSrcDirName == 'NoBoot' or binSrcDirName == 'NoBoot2':
		binDstName = binDstDirName + "_NoBoot.bin"
	else:
		binDstName = binDstDirName + ".bin"
	binDstPath = binDstDir / binDstName

	# Create clean directory for binDst
	shutil.rmtree(binDstDir, ignore_errors=True)
	pathlib.Path(binDstDir).mkdir(parents=True, exist_ok=True)

	# Copy binSrc to binDst directory & rename it
	shutil.copy2(binSrcPath, binDstPath)

	# Set binDstArtf variables
	binDstArtfDir = binSrcDir.parent.parent.parent / 'Artf'

	# Create clean directory for binDstArtf
	shutil.rmtree(binDstArtfDir, ignore_errors=True)
	pathlib.Path(binDstArtfDir).mkdir(parents=True, exist_ok=True)

	# Copy binDst to binDstArtf directory
	shutil.copy2(binDstPath, binDstArtfDir)

	# Print
	print("Generated: " + binDstName)
	print("DONE: Generate .bin Destination Directory & .bin")


	#-------------------------------------------------------------------------------
	# Copy FwUp & NoBoot2 Files to .bin Destination Directory
	#-------------------------------------------------------------------------------
	if copyFwUpNoBoot2Enable == "True":
		# Print
		print("DO: Copy FwUp & NoBoot2 Files to .bin Destination Directory")

		# Set fwUpSrc & noBoot2Src source variables
		fwUpSrcDir = binSrcDir.parent / 'FwUp' / binDstDirName
		noBoot2SrcDir = binSrcDir.parent / 'NoBoot2' / binDstDirName
		fwUpBinSignedSrcName = binDstDirName + '_Signed.bin'
		fwUpManifestSrcName = binDstDirName + '_Manifest.json'
		fwUpHexCombinedSrcName = binDstDirName + '_Unsigned_BL.hex'
		noBoot2BinSrcName = binDstDirName + '_NoBoot.bin'
		fwUpBinSignedSrcPath = fwUpSrcDir / fwUpBinSignedSrcName
		fwUpManifestSrcPath = fwUpSrcDir / fwUpManifestSrcName
		fwUpHexCombinedSrcPath = fwUpSrcDir / fwUpHexCombinedSrcName
		noBoot2BinSrcPath = noBoot2SrcDir / noBoot2BinSrcName

		# Copy fwUpSrc & noBoot2Src to binDst directory
		shutil.copy2(fwUpBinSignedSrcPath, binDstDir)
		shutil.copy2(fwUpManifestSrcPath, binDstDir)
		shutil.copy2(fwUpHexCombinedSrcPath, binDstDir)
		shutil.copy2(noBoot2BinSrcPath, binDstDir)

		# Copy fwUpSrc & noBoot2Src to binDstArtf directory
		shutil.copy2(fwUpBinSignedSrcPath, binDstArtfDir)
		shutil.copy2(fwUpManifestSrcPath, binDstArtfDir)
		shutil.copy2(fwUpHexCombinedSrcPath, binDstArtfDir)
		shutil.copy2(noBoot2BinSrcPath, binDstArtfDir)

		# Print
		print("Added: " + fwUpBinSignedSrcName)
		print("Added: " + fwUpManifestSrcName)
		print("Added: " + noBoot2BinSrcName)
		print("DONE: Copy FwUp & NoBoot2 Files to .bin Destination Directory")


	#-------------------------------------------------------------------------------
	# Generate _Raw.bin & _Signed.bin & _Manifest.json
	#-------------------------------------------------------------------------------
	if binRawBinSignedManifestGenEnable == "True":
		# Print
		print("DO: Generate _Raw.bin & _Signed.bin & _Manifest.json")

		# Set source variables
		binRawSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Raw' + binSrcPath.suffix)
		binUnsignedSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Unsigned' + binSrcPath.suffix)
		binSignedSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Signed' + binSrcPath.suffix)
		manifestSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Manifest.json')
		hexCombinedSrcPath = binSrcPath.with_name(binSrcPath.stem + '_Unsigned_BL.hex')

		# Set destination variables
		binRawDstName = binDstDirName + "_Raw.bin"
		binUnsignedDstName = binDstDirName + "_Unsigned.bin"
		binSignedDstName = binDstDirName + "_Signed.bin"
		manifestDstName = binDstDirName + "_Manifest.json"
		hexCombinedDstName = binDstDirName + "_Unsigned_BL.hex"

		# Copy source files to binDst directory & rename it
		shutil.copy2(binRawSrcPath, binDstDir / binRawDstName)
		shutil.copy2(binUnsignedSrcPath, binDstDir / binUnsignedDstName)
		shutil.copy2(binSignedSrcPath, binDstDir / binSignedDstName)
		shutil.copy2(manifestSrcPath, binDstDir / manifestDstName)
		shutil.copy2(hexCombinedSrcPath, binDstDir / hexCombinedDstName)

		# Print
		print("Generated: " + binRawDstName)
		print("Generated: " + binUnsignedDstName)
		print("Generated: " + binSignedDstName)
		print("Generated: " + manifestDstName)
		print("Generated: " + hexCombinedDstName)
		print("DONE: Generate _Raw.bin & _Signed.bin & _Manifest.json")


	#-------------------------------------------------------------------------------
	# Generate Bootloader Header File
	#-------------------------------------------------------------------------------
	if bootHdrFileGenEnable == "True":
		# Print
		print("DO: Generate Bootloader Header File")

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
		print("DONE: Generate Bootloader Header File")


	#-------------------------------------------------------------------------------
	# Print
	#-------------------------------------------------------------------------------
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
		copyFwUpNoBoot2Enable = sys.argv[3]
	else:
		copyFwUpNoBoot2Enable = "False"
	if len(sys.argv) > 4:
		binRawBinSignedManifestGenEnable = sys.argv[4]
	else:
		binRawBinSignedManifestGenEnable = "False"
	if len(sys.argv) > 5:
		bootHdrFileGenEnable = sys.argv[5]
		bootHdrFileLenHexStr = sys.argv[6]
	else:
		bootHdrFileGenEnable = "False"
		bootHdrFileLenHexStr = "0x00000000"

	# Script
	Script(vsTargetPath, fwName, copyFwUpNoBoot2Enable, binRawBinSignedManifestGenEnable, bootHdrFileGenEnable, bootHdrFileLenHexStr)
