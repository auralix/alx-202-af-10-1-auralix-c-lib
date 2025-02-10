#*******************************************************************************
# @file			alxSerialPortLogger.py
# @brief		Auralix C Library - ALX Serial Port Logger Module
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
import queue
import threading
import pathlib
import sys
import serial
import logging
import logging.handlers


#*******************************************************************************
# Class - SerialPortLogger
#*******************************************************************************
class SerialPortLogger:
	#-------------------------------------------------------------------------------
	# Ctor
	#-------------------------------------------------------------------------------
	def __init__(
		self,
		port,
		baudRate,
		logPath,
		logToConsole,
		fifoQueue = None
	):
		#-------------------------------------------------------------------------------
		# Log
		#-------------------------------------------------------------------------------
		logging.debug(
			f"ENTER: "
			f"port {port} "
			f"baudRate {baudRate} "
			f"logPath {logPath} "
			f"logToConsole {logToConsole} "
		)


		#-------------------------------------------------------------------------------
		# Parameters
		#-------------------------------------------------------------------------------
		self.__port = port
		self.__baudRate = baudRate
		self.__logPath = logPath
		self.__logToConsole = logToConsole
		self.__fifoQueue = fifoQueue
		self.__queueLoggingEnabled = threading.Event()


		#-------------------------------------------------------------------------------
		# Logging
		#-------------------------------------------------------------------------------

		# Handler
		timedRotatingHandler = logging.handlers.TimedRotatingFileHandler(
			logPath,
			when='midnight',
			backupCount=90
		)
		formatter = logging.Formatter(
			fmt='%(asctime)s.%(msecs)03d [%(levelname)s] %(message)s',
			datefmt='%Y-%m-%d %H:%M:%S'
		)
		timedRotatingHandler.setFormatter(formatter)

		# Named Logger
		self.__logger = logging.getLogger("SerialPortLogger")
		self.__logger.setLevel(logging.DEBUG)
		self.__logger.addHandler(timedRotatingHandler)
		if logToConsole:
			consoleHandler = logging.StreamHandler()
			consoleHandler.setFormatter(formatter)
			self.__logger.addHandler(consoleHandler)
		self.__logger.propagate = False


		#-------------------------------------------------------------------------------
		# Serial Port
		#-------------------------------------------------------------------------------
		self.__serialPort = serial.Serial(port=port, baudrate=baudRate, timeout=1)


		#-------------------------------------------------------------------------------
		# Stop Event
		#-------------------------------------------------------------------------------
		self.__stopEvent = threading.Event()


		#-------------------------------------------------------------------------------
		# Log
		#-------------------------------------------------------------------------------
		logging.debug("EXIT")


	#-------------------------------------------------------------------------------
	# Public Functions
	#-------------------------------------------------------------------------------
	def Run(self):
		# Log
		logging.debug("ENTER")
		self.__logger.debug(
			f"Run(): "
			f"port {self.__port} "
			f"baudRate {self.__baudRate} "
			f"logPath {self.__logPath} "
			f"logToConsole {self.__logToConsole} "
		)

		# Do
		self.__serialPort.timeout = 0.1
		buffer = ""
		try:
			while not self.__stopEvent.is_set():
				chunk = self.__serialPort.read(1024).decode(errors='ignore')
				if chunk:
					# Accumulate partial data
					buffer += chunk
					# Check if complete line
					while '\n' in buffer:
						line, buffer = buffer.split('\n', 1)
						line = line.strip()

						if '------' in line:
							logging.debug("Reset marker detected. Flushing buffer.")
							buffer = ""
							continue
						
						if line:
							self.__logger.info(line)

							# Add to the queue if logging is enabled
							if self.__queueLoggingEnabled.is_set() and self.__fifoQueue is not None:
								if self.__fifoQueue.full():
									self.__fifoQueue.get_nowait()
								self.__fifoQueue.put_nowait(line)

		except Exception as e:
			self.__logger.fatal(f"FAIL: {e}")
			logging.fatal(f"FAIL: {e}")
		finally:
			logging.debug("EXIT")
			if self.__serialPort and self.__serialPort.is_open:
				self.__serialPort.close()

	def Stop(self):
		logging.debug("ENTER")
		self.__stopEvent.set()
		logging.debug("EXIT")

	#-------------------------------------------------------------------------------
	# FIFO Queue Functions
	#-------------------------------------------------------------------------------

	def clear_queue(self, q):
		with q.mutex:
			q.queue.clear()
			q.all_tasks_done.notify_all()
			q.unfinished_tasks = 0

	def StartQueueLogging(self):
		if self.__fifoQueue is not None:
			self.clear_queue(self.__fifoQueue)
		self.__queueLoggingEnabled.set()

	def StopQueueLogging(self):
		self.__queueLoggingEnabled.clear()
		if self.__fifoQueue is not None:
			self.clear_queue(self.__fifoQueue)

	def ReadFromQueue(self):
		try:
			element = self.__fifoQueue.get_nowait()
			return element
		except queue.Empty:
			return None

	def ReadLastInputFromQueue(self):
		if self.__fifoQueue:
			with self.__fifoQueue.mutex:
				if not self.__fifoQueue.empty():
					return self.__fifoQueue.queue[-1]
		return None

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
