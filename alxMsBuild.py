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
import sys
import subprocess


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsSolPath):
	# Print
	print("")
	print("alxMsBuild.py - START")

	# Run
	cmd = (r'call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" &&'
		r' msbuild "{vsSolPath}" /p:Configuration=FwUp').format(vsSolPath=vsSolPath)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Print
	print(cmdCompletedObj.stdout)
	print(cmdCompletedObj.stderr, file=sys.stderr)

	# Print
	print("alxMsBuild.py - FINISH")
	print("")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	vsSolPath = sys.argv[1]

	# Script
	Script(vsSolPath)
