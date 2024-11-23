#*******************************************************************************
# @file			alxSerialPortLogger.py
# @brief		Auralix C Library - ALX Serial Port Logger Script
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
from typing import final
import serial


#*******************************************************************************
# Script
#*******************************************************************************
def Script(port, baudRate, logDir):
	serialPort = None
	try:
		#-------------------------------------------------------------------------------
		# START
		#-------------------------------------------------------------------------------
		print("")
		print(f"alxSerialPortLogger.py - START: port {port} baudRate {baudRate} logDir {logDir}")


		#-------------------------------------------------------------------------------
		# Init
		#-------------------------------------------------------------------------------
		serialPort = serial.Serial(port=port, baudrate=baudRate)


		#-------------------------------------------------------------------------------
		# Loop
		#-------------------------------------------------------------------------------
		while True:
			line = serialPort.read_until(b'\r\n')
			print(line.decode(), end="")

	except Exception as e:
		#-------------------------------------------------------------------------------
		# EXCEPTION
		#-------------------------------------------------------------------------------
		print(f"alxSerialPortLogger.py - EXCEPTION: {e}")
		print("")
		sys.exit(1)

	finally:
		#-------------------------------------------------------------------------------
		# FINISH
		#-------------------------------------------------------------------------------
		print("alxSerialPortLogger.py - FINISH")
		print("")
		if serialPort and serialPort.is_open:
			serialPort.close()


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare
	port = sys.argv[1]
	baudRate = sys.argv[2]
	logDir = sys.argv[3]

	# Script
	Script(port, baudRate, logDir)
