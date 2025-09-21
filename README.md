# Toy Car Robot Code
For the mobile app code check out https://github.com/Collins-Omariba/SIMPLE-ROBOT-CONTROL-ANDROID-APP.git
#### The main code in this project is in the `wyoming-satellite` folder and is a modified version of https://github.com/rhasspy/wyoming-satellite. Read below for more information on the modifications made to the code. Refer to the original repository for more information on the original code.

CHECK `standalone-bot` (The currently default and main version) branch for the code of the standalone bot also available in `main` branch. This is able to operate without the server

CHECK `server+bot` branch for the code of the server + bot. This is unable to operate without the server and is not being updated anymore. It is only here for reference and to show the original code.


# Installation Instructions

- Follow [2-Microphone Setup Tutorial](wyoming-satellite/docs/tutorial_2mic.md) except instead of cloning the `https://github.com/rhasspy/wyoming-satellite.git` repo, clone this repo and work inside the [wyoming-satellite](wyoming-satellite) folder , also there is no need to clone wyoming-openwakeword from `https://github.com/rhasspy/wyoming-openwakeword.git` since it is already included in this repo. For these two just install the system dependencies as described in [2-Microphone Setup Tutorial](wyoming-satellite/docs/tutorial_2mic.md).

⚠️ **Warning:** For `standalone-bot` do not setup the led service and remember to change the various paths and usernames accordingly. 

- Setup piper in the following folders as shown

```
           "/home/fyp213/piper_voices/en_US-amy-medium.onnx"
           "/home/fyp213/piper_voices/en_US-amy-medium.onnx.json"
```

- Replace `fyp213` with your username in the above paths and in the relevant code in WakeStreamingSatellite class.

- The systemd service files used are in the [systemd service files ](<systemd service files>) folder.

- create a .env file in the `wyoming-satellite` folder and add the following lines to it:

```
GEMINI_API_KEY=your_gemini_api_key
```
- Check out the [requirements.txt](wyoming-satellite/requirements.txt) for the required packages if you encounter any issues related to new features added to wyoming satellite. Install the packages after activating the virtual environment in the `wyoming-satellite` folder.

- The code in the [ros_arduino_bridge](ros_arduino_bridge) folder is to be compiled and uploaded to an arduino mega connected to the pi in order for it to receive commands from the pi and control the motors.


 
# WakeStreamingSatellite Documentation for Smart Toy Car Robot

The `WakeStreamingSatellite` class, originally part of the Wyoming Satellite system, has been extensively revised to operate as a standalone component within a smart toy car robot. Previously dependent on an external server for audio processing and response generation, the modified version now performs all tasks locally, integrating wake word detection, speech-to-text, text-to-speech, and AI-driven responses. It also incorporates hardware features like LED indicators and motor control to enhance interactivity. These changes make the toy self-contained, responsive, and suitable for educational interactions with children.

---

## Key Modifications

The original `WakeStreamingSatellite` class relied heavily on server-based processing. The following **14 modifications** eliminate this dependency and adapt the class for the smart toy car:

### 1. Removed Server Dependency  
- **Original**: Audio processing, wake word detection, and response generation were handled by an external server.  
- **Modification**: All operations are now performed locally, making the system standalone.

---

### 2. Audio Capture and Processing
- **Original**: Audio was streamed to a server for processing.
- **Modification**: Audio is captured locally and processed using the Whisper `tiny.en` model.
- **Note**: The audio is saved as a WAV file before transcription.

---

### 3. Local Speech-to-Text (STT)  
- **Original**: Audio was sent to a server for transcription.  
- **Modification**: The Whisper `tiny.en` model transcribes audio locally.

---

### 4. Local Text-to-Speech (TTS)  
- **Original**: TTS was server-generated.  
- **Modification**: The piper tts library converts text into audio locally.

---

### 5. Integration with Gemini Model  
- **Original**: Response generation was server-based.  
- **Modification**: The Gemini model is queryed locally using an API key stored in the environment.
- **Note**: The API key is set in the environment variable

---

### 6. Age-Based Response Customization  
- **Original**: No age-specific tailoring.  
- **Modification**: Reads child’s age from `/home/fyp213/age.txt` and adjusts prompts accordingly.

---

### 7. Stanalone LED Status Indicators  
- **Original**: Visual feedback based on server status.  
- **Modification**: APA102 LEDs indicate system states:  
  - **Blue**: Idle  
  - **Green**: Listening  
  - **Red**: Processing

---

### 8. Motor Control Integration  
- **Original**: No physical interaction.  
- **Modification**: Executes motor scripts in `motor_run_files/` based on voice commands.

---

### 9. Audio Buffer Management  
- **Original**: Audio streamed to the server.  
- **Modification**: A local buffer collects up to 6 seconds of audio for processing by the whisper tiny.en model locally.

---

### 10. Streaming State Management  
- **Original**: Server-controlled streaming.  
- **Modification**: Local timer limits capture to 6 seconds before processing begins.

---

### 11. Error Handling and Logging  
- **Original**: Minimal error handling.  
- **Modification**: Enhanced error handling and logging via `CUSTOM_LOGGER`.

---

### 12. Configuration and Settings  
- **Original**: Server-oriented configuration.  
- **Modification**: Updated for local file paths and resources.

---

### 13. Integration with Flask Web App  
- **Original**: No external data integration.  
- **Modification**: Reads age file updated by Flask web app( `webapp/flask_app.py` ) for dynamic interaction. Also has a user manual, a restart robot button, a power off button and a view logs button.

---

### 14. Simulated Wake Word Detection for Follow-Up  
- **Original**: No conversational follow-ups.  
- **Modification**: Simulated wake detection allows natural dialogue continuation for questions.

### 15. Wifi Connection Alert
- **Original**: No feedback on connection status.
- **Modification**: Provides feedback if no wifi connection is available.

### 16. Song playing(offline content)
- **Original**: No offline content playback.
- **Modification**: Plays a song when the robot receives a specific voice command.

---

## Component Overview

### LED Control (APA102)  
- Manages a strip of three LEDs:  
  - **Blue**: Idle  
  - **Green**: Listening  
  - **Red**: Processing  
- Controlled via `set_led_color` method.

### Whisper Model  
- Local `tiny.en` model transcribes 6-second audio clips.

### Gemini Client  
- Uses environment-based API key for generating responses.

### Motor Control  
- Runs Python scripts which send serial command based on movement commands to an arduino running the `ros_arduino_bridge` code .

### Text-to-Speech (Piper TTS)  
- Converts text to audio for response playback.

### Audio Buffer  
- Stores audio from the 6-second window for local processing.

### Error Handling
- Uses `CUSTOM_LOGGER` for logging errors and system states.


---

## Workflow

### 1. Idle State  
- **LED**: Blue  
- Waiting for the wake word (e.g., "hey jarvis").

### 2. Wake Word Detection  
- LED turns **Green**.  
- Audio buffer starts collecting input (max 6 seconds).

### 3. Audio Capture and Transcription  
- LED turns **Red**.  
- Audio saved as WAV and transcribed using Whisper.

### 4. Command Recognition and Execution  
- **Movement**: Triggers motor scripts if command matches.  
- **Other Inputs**: Sent to Gemini for a response.

### 5. Age-Based Response Generation  
- Reads age from `/home/fyp213/age.txt`:  
  - **Ages ≤7**: Simple, friendly tone.  
  - **Ages >7**: Clear, informative content.

### 6. Response Playback and Interaction  
- Response played using piper tts.  
- LED resets to **Blue**.  
- If a question, simulates wake detection for child’s reply.

### 7. Return to Idle  
- System resets, LED remains **Blue**.

---

## LED Status Colors

The LEDs provide intuitive visual feedback:

- **Blue**: Idle / Ready  
- **Green**: Listening  
- **Red**: Processing

---

## How It Differs from the Original

| Feature | Original | Modified |
|--------|----------|----------|
| Server Dependency | Required | Removed (fully local) |
| Age Customization | None | Tailored via Flask integration |
| LED Feedback | Server dependent | Local |
| Motor Control | Absent | Voice-controlled motion |
| Audio Management | Streaming to server | Local buffer + timer |
| Follow-Ups | Not supported | Simulated wake detection |
| Error Handling | Minimal | Enhanced with logging |
| Wifi connection alert | None | Provides feedback if no connection |
| Configuration | Server-oriented | Local file paths |
| TTS | Server-based| Local quering of piper TTS |
| STT | Server-based | Local Whisper model |
| Songs(Offline content) | None | Offline content playback (songs) |

---

## Benefits

These changes make the toy:

- **Fully standalone**  
- **Age-aware and educational**  
- **Engaging through lights and motion**  
- **Naturally conversational**
- **Can give feedback if there is no wifi connection**
- **Has offline playback content(songs) i.e. Can play songs**
- **Has a local web app for user interface**

- **Have a great user interface via the flask web app**


