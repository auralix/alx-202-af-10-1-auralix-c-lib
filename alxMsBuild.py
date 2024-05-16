#*******************************************************************************
# @file			alxMsBuild.py
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
import shutil
import sys
import subprocess


#*******************************************************************************
# Script
#*******************************************************************************
def Script():
	cmd = r'call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" && msbuild "C:\AuralixGitHub\alx-202-af-12-nucleo-f429zi-c-fw\ALX-202-AF-12\ALX-202-AF-12.sln" /p:Configuration=FwUp'
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print the output and errors
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param

	# Script
	Script()
