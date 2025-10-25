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


"""
Auralix C Library - ALX Bootloader Script

Builds Visual Studio solution configurations ``FwUp`` and ``NoBoot2`` by
invoking ``VsDevCmd.bat`` and ``msbuild``.
Intended for VisualGDB post-build steps or manual CLI use.
"""


#*******************************************************************************
# Imports
#*******************************************************************************
import sys
import subprocess


#*******************************************************************************
# Script
#*******************************************************************************
def Script(vsSolPath: str) -> None:
	"""Build Visual Studio solution configurations ``FwUp`` and ``NoBoot2``.

	Calls the Visual Studio developer environment batch script and invokes
	``msbuild`` for the provided solution.

	Args:
		vsSolPath: Absolute path to the Visual Studio ``.sln`` file to build.

	Returns:
		None

	Raises:
		OSError: If the developer environment or ``msbuild`` cannot be invoked.

	Side Effects:
		Invokes external tools (``VsDevCmd.bat``, ``msbuild``) and prints build
		output to stdout/stderr. Build artifacts are created in the solution’s
		output directories per configuration.
	"""


	# Print
	print("")
	print("alxMsBuild.py - START")

	# Build FwUp
	cmd = (r'call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" &&'
		r' msbuild "{vsSolPath}" /p:Configuration=FwUp /verbosity:quiet').format(vsSolPath=vsSolPath)
	cmdCompletedObj = subprocess.run(cmd, capture_output=True, text=True, shell=True)

	# Build NoBoot2
	cmd = (r'call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" &&'
		r' msbuild "{vsSolPath}" /p:Configuration=NoBoot2 /verbosity:quiet').format(vsSolPath=vsSolPath)
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
