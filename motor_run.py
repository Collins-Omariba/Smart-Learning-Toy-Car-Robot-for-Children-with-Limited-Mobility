#!/usr/bin/env python3
import serial
import time

def main():
    try:
        ser = serial.Serial('/dev/ttyACM0', 57600, timeout=1)
    except serial.SerialException as e:
        print("Could not open serial port:", e)
        return

    # Allow extra time for the Arduino to reset upon opening the serial connection.
    print("Waiting for Arduino to reset...")
    time.sleep(2) 

    # Clear any data in the input buffer.
    ser.reset_input_buffer()

    # Prepare the command using CR+LF as terminator.
    command = "m 100 100\r\n"
    print(f"Sending command: {command.strip()}")
    ser.write(command.encode('utf-8'))
    ser.flush()  # Ensure data is sent immediately

    # Optional: wait for and print any response from the Arduino.
    time.sleep(1)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8', errors='replace').strip()
        if response:
            print("Response:", response)

    # Close the serial connection.
    ser.close()
    print("Serial connection closed.")

if __name__ == '__main__':
    main()