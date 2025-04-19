#!/usr/bin/env python3
# /home/fyp213/wifi_alert/offline_check.py

import socket
import time
import os

def is_connected():
    try:
        socket.create_connection(("8.8.8.8", 53), timeout=3)
        return True
    except OSError:
        return False

def play_offline_sound():
    os.system("aplay /home/fyp213/wifi_alert/jarvis.wav")

# Delay before starting (allow network to initialize after boot)
time.sleep(10)

# Keeps track of last known connection status
was_connected = True

while True:
    connected = is_connected()
    
    if not connected and was_connected:
        # Only play sound once per disconnect event
        play_offline_sound()
    
    # Update the state
    was_connected = connected

    # Wait 15 seconds before checking again
    time.sleep(15)

