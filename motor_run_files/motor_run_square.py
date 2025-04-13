#!/usr/bin/env python3
import serial
import time
import logging
import sys

# -----------------------------------------------------------------------------
# CONFIGURATION
# -----------------------------------------------------------------------------
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE    = 57600
TIMEOUT      = 1

STRAIGHT_SPEED = 44
TURN_SPEED     = 44
SIDES          = 4 

# List of durations for straight movements (seconds per side)
STRAIGHT_TIMES = [4.0, 4.0, 4.0, 4.0]

# List of durations for each 90° turn (seconds per turn)
TURN_TIMES     = [3.0, 3.0, 3.0, 3.0]

# -----------------------------------------------------------------------------
# LOGGING SETUP
# -----------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

# -----------------------------------------------------------------------------
# UTILITY FUNCTIONS
# -----------------------------------------------------------------------------
def send_command(ser: serial.Serial, cmd: str, wait_response: bool = True):
    logging.info(">> %s", cmd.strip())
    ser.write(cmd.encode('utf-8'))
    ser.flush()
    if wait_response:
        time.sleep(0.1)
        while ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='replace').strip()
            if line:
                logging.info("<< %s", line)

# -----------------------------------------------------------------------------
# MAIN
# -----------------------------------------------------------------------------
def main():
    # 1) Open serial port
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
        logging.info("Serial port opened: %s @ %d baud", SERIAL_PORT, BAUD_RATE)
    except serial.SerialException as e:
        logging.error("Could not open serial port: %s", e)
        sys.exit(1)

    # 2) Wait for Arduino reset
    time.sleep(2)
    ser.reset_input_buffer()

    # 3) Loop through each side
    for i in range(SIDES):
        logging.info("=== Side %d ===", i + 1)

        # --- Forward Movement ---
        cmd_straight = f"m {STRAIGHT_SPEED} {STRAIGHT_SPEED}\r\n"
        send_command(ser, cmd_straight, wait_response=False)
        time.sleep(STRAIGHT_TIMES[i])

        # --- Stop Before Turn ---
        send_command(ser, "m 0 0\r\n", wait_response=False)
        time.sleep(0.2)

        # --- 90° Turn (in-place) ---
        cmd_turn = f"m -{TURN_SPEED} {TURN_SPEED}\r\n"
        send_command(ser, cmd_turn, wait_response=False)
        time.sleep(TURN_TIMES[i])

        # --- Stop After Turn ---
        send_command(ser, "m 0 0\r\n", wait_response=False)
        time.sleep(0.2)

    # 4) Close serial connection
    ser.close()
    logging.info("Done. Serial port closed.")

if __name__ == "__main__":
    main()
