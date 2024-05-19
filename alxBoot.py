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
import sys
import subprocess


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsTargetPath, imgSlotSize, bootLenHexStr):
	# Print
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

	# Set lenghts
	bootLen = int(bootLenHexStr, 16)
	headerLen = 0x200  # 512 bytes
	trailerLen = 0x28  # 40 bytes

	# Read source bin
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix('.bin')
	binData = binSrcPath.read_bytes()
	binLen = len(binData)

	# Extract application data
	appStartOffset = bootLen + headerLen
	appEndOffset = binLen - trailerLen
	appData = binData[appStartOffset:appEndOffset]
	appDataLen = len(appData)

	# Write extracted application data to raw bin
	binRawPath = binSrcPath.with_name(binSrcPath.stem + '_Raw' + binSrcPath.suffix)
	binRawPath.write_bytes(appData)

	# Set signed bin path
	binSignedPath = binSrcPath.with_name(binSrcPath.stem + '_Signed' + binSrcPath.suffix)

	# Set imgtool path
	imgtoolPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "Sub" / "mcuboot" / "scripts" / "imgtool.py"

	# Run imgtool
	cmd = (r"python {imgtoolPath} sign"
		r" --header-size 0x200"
		r" --pad-header"
		r" --slot-size {imgSlotSize}"
		r" --version {fwVerMajor}.{fwVerMinor}.{fwVerPatch}+{date}"
		r" --pad"
		r" {binPathIn}"
		r" {binPathOut}").format(
		imgtoolPath=imgtoolPath,
		imgSlotSize=imgSlotSize,
		fwVerMajor=fwVerMajor,
		fwVerMinor=fwVerMinor,
		fwVerPatch=fwVerPatch,
		date=date,
		binPathIn=binRawPath,
		binPathOut=binSignedPath
	)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print imgtool
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)

	# Read signed bin
	binSignedData = binSignedPath.read_bytes()

	# Extract signed bin header & trailer
	binSignedHeader = binSignedData[:headerLen]
	binSignedTrailerStartOffset = headerLen + appDataLen
	binSignedTrailerEndOffset = binSignedTrailerStartOffset + trailerLen
	binSignedTrailer = binSignedData[binSignedTrailerStartOffset:binSignedTrailerEndOffset]

	# Prepare signed bin header & trailer variables
	binSignedHeaderArr = ", ".join(f"0x{byte:02X}" for byte in binSignedHeader)
	binSignedTrailerArr = ", ".join(f"0x{byte:02X}" for byte in binSignedTrailer)
	binSignedHeaderTrailerPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "Sub" / "alx-202-af-10-1-auralix-c-lib" / "alxBoot2_GENERATED.h"

	# Prepare signed bin header & trailer file text
	binSignedHeaderTrailerText = """#ifndef ALX_BOOT2_GENERATED_H
#define ALX_BOOT2_GENERATED_H


#if defined(ALX_BUILD_CONFIG_DEBUG)
static const unsigned char header[{headerLen}] __attribute__((section(".header"), used)) = {{{binSignedHeaderArr}}};
static const unsigned char trailer[{trailerLen}] __attribute__((section(".trailer"), used)) = {{{binSignedTrailerArr}}};
#endif
#if defined(ALX_BUILD_CONFIG_FW_UP)
static const unsigned char header[{headerLen}] __attribute__((section(".header"), used)) = {{0xBB, 0xBB, 0xBB, 0xBB}};
static const unsigned char trailer[{trailerLen}] __attribute__((section(".trailer"), used)) = {{0xCC, 0xCC, 0xCC, 0xCC}};
#endif


#endif	// ALX_BOOT2_GENERATED_H
""".format(headerLen=headerLen, binSignedHeaderArr=binSignedHeaderArr, trailerLen=trailerLen, binSignedTrailerArr=binSignedTrailerArr)

	# Write signed bin header & trailer file text
	binSignedHeaderTrailerPath.write_text(binSignedHeaderTrailerText)

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
