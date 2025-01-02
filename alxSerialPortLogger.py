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
import serial
import logging
import logging.handlers


#*******************************************************************************
# Script
#*******************************************************************************
def Script(port, baudRate, logDir):
	serialPort = None
	try:
		#-------------------------------------------------------------------------------
		# Init Logging
		#-------------------------------------------------------------------------------
		logDirPath = pathlib.Path(logDir)
		logPath = logDirPath / f"{logDirPath.name}.log"
		timedRotatingHandler = logging.handlers.TimedRotatingFileHandler(
			logPath,
			when='midnight',
			backupCount=90
		)
		logging.basicConfig(
			format = '%(asctime)s.%(msecs)03d [%(levelname)s] %(message)s',
			datefmt = '%Y-%m-%d %H:%M:%S',
			level=logging.DEBUG,
			handlers = [
				timedRotatingHandler
			]
		)


		#-------------------------------------------------------------------------------
		# START
		#-------------------------------------------------------------------------------
		logging.debug(f"alxSerialPortLogger.py - START: port {port} baudRate {baudRate} logDir {logDir}")


		#-------------------------------------------------------------------------------
		# Init Serial Port
		#-------------------------------------------------------------------------------
		serialPort = serial.Serial(port=port, baudrate=baudRate)


		#-------------------------------------------------------------------------------
		# Loop
		#-------------------------------------------------------------------------------
		while True:
			line = serialPort.read_until(b'\r\n')
			print(line.decode(), end="")
			logging.info(line.decode(errors='ignore').strip())

	except Exception as e:
		#-------------------------------------------------------------------------------
		# EXCEPTION
		#-------------------------------------------------------------------------------
		logging.fatal(f"alxSerialPortLogger.py - EXCEPTION: {e}")
		sys.exit(1)

	finally:
		#-------------------------------------------------------------------------------
		# FINISH
		#-------------------------------------------------------------------------------
		logging.debug("alxSerialPortLogger.py - FINISH")
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
