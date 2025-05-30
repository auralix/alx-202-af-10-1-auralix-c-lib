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
import json
import hashlib
import base64


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsTargetPath, imgSlotLenHexStr, bootLenHexStr):
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

	# Set srec_cat path
	srecPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "Tools" / "srec_cat"

	# If singing server is configured read settings and use it to generate the signature
	signserverSettingsPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "signserver.json"

	# Set SHA256 bin path
	binSHA256Path = binSrcPath.with_name(binSrcPath.stem + '_SHA256.bin')
	if binSHA256Path.exists():
		binSHA256Path.unlink()

	# Set external signature path
	extSigPath = binSrcPath.with_name(binSrcPath.stem + '_ExtSig.base64')
	if extSigPath.exists():
		extSigPath.unlink()

	# Set fake pubkey path
	binSHA256Path = binSrcPath.with_name(binSrcPath.stem + '_SHA256.bin')
	if binSHA256Path.exists():
		binSHA256Path.unlink()

	with open(signserverSettingsPath) as json_file:
		signserverSettings = json.load(json_file)

		if len(signserverSettings["worker_name"]) > 0:
			# Get digest
			cmd = (r"python {imgtoolPath} sign"
				r" --header-size {headerLenStr}"
				r" --pad-header"
				r" --slot-size {imgSlotLenHexStr}"
				r" --version {fwVerMajor}.{fwVerMinor}.{fwVerPatch}+{date}"
				r" {binPathIn}"
				r" {sha256PathOut}"
				r" --vector-to-sign digest").format(
				imgtoolPath=imgtoolPath,
				headerLenStr=headerLenStr,
				imgSlotLenHexStr=imgSlotLenHexStr,
				fwVerMajor=fwVerMajor,
				fwVerMinor=fwVerMinor,
				fwVerPatch=fwVerPatch,
				date=date,
				binPathIn=binRawPath,
				sha256PathOut=binSHA256Path
			)
			enc_pubPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "enc_pub.pem"
			if enc_pubPath.is_file():
				cmd = cmd + (r" --encrypt {encKey}").format(encKey=enc_pubPath)

			print("imgtool.py - cmd:" + cmd)
			cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

			# Print imgtool
			print(cmdCompletedObj.stdout)
			print(cmdCompletedObj.stderr, file=sys.stderr)

			if len(signserverSettings["certificate_thumbprint"]) > 0:
				cert_cmd = (r"--cert CurrentUser\\MY\\{certificate_thumbprint}").format(
					certificate_thumbprint=signserverSettings["certificate_thumbprint"]
				)
			else:
				cert_cmd = ""

			cmd = (r"curl {cert_cmd} -F workerName={worker}"
				" -F \"file=@{SHA256}\""
				" {server_url}"
				" -o \"{signature}\"").format(
				cert_cmd=cert_cmd,
				worker=signserverSettings["worker_name"],
				server_url=signserverSettings["server_url"],
				SHA256=binSHA256Path,
				signature=extSigPath
			)


			print("curl - cmd:" + cmd)
			cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

			# Print imgtool
			print(cmdCompletedObj.stdout)
			print(cmdCompletedObj.stderr, file=sys.stderr)

			# Convert signature is returned as binary; convert it to base64
			binSig = extSigPath.read_bytes()
			encoded = base64.b64encode(binSig)
			extSigPath.write_bytes(encoded)

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

	# If sig_key exists sign the FW and if enc_key exists also encrypt the FW
	sig_keyPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "sig_key.pem"
	if sig_keyPath.is_file():
		cmd = cmd + (r" -k {sigKey}").format(sigKey=sig_keyPath)
		enc_pubPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "enc_pub.pem"
		if enc_pubPath.is_file():
			cmd = cmd + (r" --encrypt {encKey}").format(encKey=enc_pubPath)
	elif extSigPath.is_file(): # look for external signature
		# Generate fake pubkey for mcuboot (real pubkey is in secure element)
		sig_pubPath = binSrcPath.with_name("fake_pubkey.pem")
		sig_pubPath.write_bytes(b"-----BEGIN PUBLIC KEY-----\r\n"
			b"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAESgQNrEUInMsjK2OQJmBe5AMcQfL+\r\n"
			b"5eooj27ulcKzf0iASF2Top9ypskjTv3LigXzcBbUlktWpBIA766l0cp/Xw==\r\n"
			b"-----END PUBLIC KEY-----\r\n"
		)
		cmd = cmd + (r" --fix-sig {extSignature}"
			r" --fix-sig-pubkey {sigPubKey}").format(
				extSignature=extSigPath,
				sigPubKey=sig_pubPath)
		enc_pubPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / "enc_pub.pem"
		if enc_pubPath.is_file():
			cmd = cmd + (r" --encrypt {encKey}").format(encKey=enc_pubPath)

	print("imgtool.py - cmd:" + cmd)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print imgtool
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)


	# Set bootloader path
	bootloaderPath = pathlib.Path(vsTargetPath).parent.parent.parent / pathlib.Path(vsTargetPath).stem / "SignServer" / signserverSettings["bootloader_binary"]

	# Set combined hex paths
	combinedPath = binSrcPath.with_name(binSrcPath.stem + '_Unsigned_BL.hex')
	binUnsignedPath = binSrcPath.with_name(binSrcPath.stem + '_Unsigned' + binSrcPath.suffix)

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
		binPathOut=binUnsignedPath
	)

	print("unsigned image: " + cmd)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print imgtool
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)

	cmd = (r"{srecPath}"
		r" {bootloaderPath} -binary -offset 0x8000000"
		r" {imagePath} -binary -offset 0x8020000"
		r" -o {combinedPath} -Intel").format(
		srecPath=srecPath,
		bootloaderPath=bootloaderPath,
		imagePath=binUnsignedPath,
		combinedPath=combinedPath
	)

	print("srec-cmd: " + cmd)
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
