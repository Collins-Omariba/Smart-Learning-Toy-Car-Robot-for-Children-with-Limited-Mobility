#!/usr/bin/env python3
import serial
import time
import logging
import sys

# --- CONFIGURATION ---
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 57600
TURN_SPEED = 44
TURN_DURATION = 1.6  # seconds

# --- LOGGING SETUP ---
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        logging.info("Serial port opened successfully")
    except serial.SerialException as e:
        logging.error("Could not open serial port: %s", e)
        sys.exit(1)

    time.sleep(2)  # Wait for Arduino reset
    ser.reset_input_buffer()

    # Send turn left command
    command_turn = f"m {TURN_SPEED} -{TURN_SPEED}\r\n"
    logging.info("Turning left 90° with command: %s", command_turn.strip())
    ser.write(command_turn.encode('utf-8'))
    ser.flush()
    time.sleep(TURN_DURATION)

    # Stop motors after turn
    stop_command = "m 0 0\r\n"
    ser.write(stop_command.encode('utf-8'))
    ser.flush()
    logging.info("Stopped motors")

    ser.close()
    logging.info("Serial connection closed")

if __name__ == "__main__":
    main()
