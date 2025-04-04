#!/usr/bin/env python3
import serial
import time
import logging
import sys

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def main():
    try:
        ser = serial.Serial('/dev/ttyACM0', 57600, timeout=1)
        logging.info("Serial port opened successfully")
    except serial.SerialException as e:
        logging.error("Could not open serial port: %s", e)
        sys.exit(1)

    time.sleep(2)  # Wait for Arduino reset
    ser.reset_input_buffer()

    # Command for counterclockwise circle: left motor faster than right
    command = "o 56 28\r\n"  # Left: 100, Right: 50
    logging.info("Sending circle command: %s", command.strip())
    ser.write(command.encode('utf-8'))
    ser.flush()

    # Run for 5 seconds (adjust this to complete the circle)
    time.sleep(5)

    # Stop the motors
    stop_command = "m 0 0\r\n"
    logging.info("Stopping motors: %s", stop_command.strip())
    ser.write(stop_command.encode('utf-8'))
    ser.flush()

    time.sleep(1)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8', errors='replace').strip()
        if response:
            logging.info("Arduino response: %s", response)

    ser.close()
    logging.info("Serial connection closed")

if __name__ == "__main__":
    main()

