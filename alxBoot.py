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


"""
Auralix C Library - ALX Bootloader Script

Signs and packages firmware binaries and emits boot metadata. Uses
``alxBuild_GENERATED.h`` for versioning, extracts the application payload,
signs with MCUboot ``imgtool.py``, writes a manifest, and generates
``alxBootMetadata_GENERATED.h``.
Intended for VisualGDB post-build steps or manual CLI use.
"""


#*******************************************************************************
# Imports
#*******************************************************************************
import pathlib
import sys
import subprocess
import json
import hashlib


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsTargetPath: str, imgSlotLenHexStr: str, bootLenHexStr: str) -> None:
	"""Sign and package a VisualGDB firmware binary and emit boot metadata.

	Reads ``alxBuild_GENERATED.h`` in the CWD, slices the application region
	from ``<vsTargetPath>.bin`` (after the bootloader + ``0x0200`` header and
	before the ``0x0028`` trailer), writes ``_Raw.bin``, signs to ``_Signed.bin``
	with MCUboot, writes a manifest, and generates
	``alxBootMetadata_GENERATED.h``.

	Args:
		vsTargetPath: Absolute path to the VisualGDB target; the source binary
			is resolved as ``Path(vsTargetPath).with_suffix(".bin")``.
		imgSlotLenHexStr: Image slot length as a hex string
			(e.g., ``"0x00120000"``).
		bootLenHexStr: Bootloader length as a hex string
			(e.g., ``"0x00020000"``).

	Returns:
		None

	Raises:
		FileNotFoundError: If ``alxBuild_GENERATED.h`` or required binaries are missing.
		ValueError: If ``imgSlotLenHexStr`` or ``bootLenHexStr`` are not valid hex.
		OSError: On file I/O or subprocess errors.

	Side Effects:
		Creates ``_Raw.bin``, ``_Signed.bin``, ``_Manifest.json``, and
		``alxBootMetadata_GENERATED.h``; invokes ``imgtool.py``; prints progress.
	"""


	#-------------------------------------------------------------------------------
	# Print
	#-------------------------------------------------------------------------------
	print("")
	print(f"alxBoot.py - START: vsTargetPath {vsTargetPath} imgSlotLenHexStr {imgSlotLenHexStr} bootLenHexStr {bootLenHexStr}")


	#-------------------------------------------------------------------------------
	# Read alxBuild_GENERATED.h
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Read alxBuild_GENERATED.h")

	# Read build file
	inFilePath = pathlib.Path("alxBuild_GENERATED.h")
	inFileText = inFilePath.read_text()
	inFileLines = inFileText.splitlines()

	# Parse build file
	date = inFileLines[5][23:]
	hashShort = inFileLines[8][30:37]
	fwVerMajor = inFileLines[11][31:]
	fwVerMinor = inFileLines[12][31:]
	fwVerPatch = inFileLines[13][31:]

	# Print
	print("DONE: Read alxBuild_GENERATED.h")


	#-------------------------------------------------------------------------------
	# Read Source .bin
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Read Source .bin")

	# Set lengths
	bootLen = int(bootLenHexStr, 16)
	headerLenStr = r'0x0200'	# 512 bytes
	trailerLenStr =  r'0x0028'	# 40 bytes
	headerLen = int(headerLenStr, 16)
	trailerLen = int(trailerLenStr, 16)

	# Read source bin
	binSrcPath = pathlib.Path(vsTargetPath).with_suffix('.bin')
	binData = binSrcPath.read_bytes()
	binLen = len(binData)

	# Extract application data
	appStartOffset = bootLen + headerLen
	appEndOffset = binLen - trailerLen
	appData = binData[appStartOffset:appEndOffset]
	appDataLen = len(appData)

	# Print
	print("DONE: Read Source .bin")


	#-------------------------------------------------------------------------------
	# Generate _Raw.bin & _Signed.bin
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Generate _Raw.bin & _Signed.bin")

	# Write extracted application data to raw bin
	binRawPath = binSrcPath.with_name(binSrcPath.stem + '_Raw' + binSrcPath.suffix)
	binRawPath.write_bytes(appData)

	# Set signed bin path
	binSignedPath = binSrcPath.with_name(binSrcPath.stem + '_Signed' + binSrcPath.suffix)

	# Set imgtool path
	imgtoolPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "Sub" / "mcuboot" / "scripts" / "imgtool.py"

	# Run imgtool
	cmd = (r"python {imgtoolPath} sign"
		r" --header-size {headerLenStr}"
		r" --pad-header"
		r" --slot-size {imgSlotLenHexStr}"
		r" --version {fwVerMajor}.{fwVerMinor}.{fwVerPatch}+{date}"
		r" --pad"
		r" {binPathIn}"
		r" {binPathOut}").format(
		imgtoolPath=imgtoolPath,
		headerLenStr=headerLenStr,
		imgSlotLenHexStr=imgSlotLenHexStr,
		fwVerMajor=fwVerMajor,
		fwVerMinor=fwVerMinor,
		fwVerPatch=fwVerPatch,
		date=date,
		binPathIn=binRawPath,
		binPathOut=binSignedPath
	)
	print("imgtool.py - cmd:" + cmd)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print imgtool
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)

	# Print
	print("DONE: Generate _Raw.bin & _Signed.bin")


	#-------------------------------------------------------------------------------
	# Generate _Manifest.json
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Generate _Manifest.json")

	# Read signed bin
	binSignedData = binSignedPath.read_bytes()

	# Prepare manifest file variables
	manifestVer = int(date)
	manifestSize = len(binSignedData)
	manifestHashObj = hashlib.sha256(binSignedData)
	manifestHashStr = manifestHashObj.hexdigest().upper()
	manifestPath = binSrcPath.with_name(binSrcPath.stem + '_Manifest.json')

	# Prepare manifest file text
	manifestDict = {
		"ver": manifestVer,
		"size": manifestSize,
		"hash": manifestHashStr,
		"url": "/api/v1/NEMO/fw-update/get-bin"
	}
	manifestJsonObj = json.dumps(manifestDict, indent=4)

	# Write manifest file text
	manifestPath.write_text(manifestJsonObj)

	# Print
	print("DONE: Generate _Manifest.json")


	#-------------------------------------------------------------------------------
	# Generate alxBootMetadata_GENERATED.h
	#-------------------------------------------------------------------------------

	# Print
	print("DO: Generate alxBootMetadata_GENERATED.h")

	# Extract signed bin header & trailer
	binSignedHeader = binSignedData[:headerLen]
	binSignedTrailerStartOffset = headerLen + appDataLen
	binSignedTrailerEndOffset = binSignedTrailerStartOffset + trailerLen
	binSignedTrailer = binSignedData[binSignedTrailerStartOffset:binSignedTrailerEndOffset]

	# Prepare signed bin header & trailer variables
	binSignedHeaderArr = ", ".join(f"0x{byte:02X}" for byte in binSignedHeader)
	binSignedTrailerArr = ", ".join(f"0x{byte:02X}" for byte in binSignedTrailer)
	binSignedHeaderTrailerPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "Sub" / "alx-202-af-10-1-auralix-c-lib" / "alxBootMetadata_GENERATED.h"

	# Prepare signed bin header & trailer file text
	binSignedHeaderTrailerText = """#ifndef ALX_BOOT_METADATA_GENERATED_H
#define ALX_BOOT_METADATA_GENERATED_H


#if defined(ALX_BUILD_CONFIG_DEBUG)
static const unsigned char app_header[{headerLenStr}] __attribute__((section(".app_header"), used)) = {{{binSignedHeaderArr}}};
static const unsigned char app_trailer[{trailerLenStr}] __attribute__((section(".app_trailer"), used)) = {{{binSignedTrailerArr}}};
#endif


#endif	// ALX_BOOT_METADATA_GENERATED_H
""".format(headerLenStr=headerLenStr, binSignedHeaderArr=binSignedHeaderArr, trailerLenStr=trailerLenStr, binSignedTrailerArr=binSignedTrailerArr)

	# Write signed bin header & trailer file text
	binSignedHeaderTrailerPath.write_text(binSignedHeaderTrailerText)

	# Print
	print("DONE: Generate alxBootMetadata_GENERATED.h")


	#-------------------------------------------------------------------------------
	# Print
	#-------------------------------------------------------------------------------
	print("alxBoot.py - FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	vsTargetPath = sys.argv[1]
	imgSlotLenHexStr = sys.argv[2]
	bootLenHexStr = sys.argv[3]

	# Script
	Script(vsTargetPath, imgSlotLenHexStr, bootLenHexStr)
