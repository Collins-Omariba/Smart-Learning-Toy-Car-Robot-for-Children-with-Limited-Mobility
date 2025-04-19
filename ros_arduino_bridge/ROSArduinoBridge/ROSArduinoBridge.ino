/*********************************************************************
 *  ROSArduinoBridge
 
    A set of simple serial commands to control a differential drive
    robot and receive back sensor and odometry data. Default 
    configuration assumes use of an Arduino Mega + Pololu motor
    controller shield + Robogaia Mega Encoder shield.  Edit the
    readEncoder() and setMotorSpeed() wrapper functions if using 
    different motor controller or encoder method.

    Created for the Pi Robot Project: http://www.pirobot.org
    and the Home Brew Robotics Club (HBRC): http://hbrobotics.org
    
    Authors: Patrick Goebel, James Nugen

    Inspired and modeled after the ArbotiX driver by Michael Ferguson
    
    Software License Agreement (BSD License)

    Copyright (c) 2012, Patrick Goebel.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials provided
       with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#define USE_BASE      // Enable the base controller code
//#undef USE_BASE     // Disable the base controller code

/* Define the motor controller and encoder library you are using */
#ifdef USE_BASE
   /* The Pololu VNH5019 dual motor driver shield */
   //#define POLOLU_VNH5019

   /* The Pololu MC33926 dual motor driver shield */
   //#define POLOLU_MC33926

   /* The RoboGaia encoder shield */
   //#define ROBOGAIA
   
   /* Encoders directly attached to Arduino board */
   #define ARDUINO_ENC_COUNTER

   /* L298 Motor driver*/
   #define L298_MOTOR_DRIVER
#endif

// #define USE_SERVOS  // Enable use of PWM servos as defined in servos.h
#undef USE_SERVOS     // Disable use of PWM servos

/* Serial port baud rate */
#define BAUDRATE     57600
// #define BAUDRATE     9600 //for debugging

/* Maximum PWM signal */
#define MAX_PWM   56

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* Include definition of serial commands */
#include "commands.h"

/* Sensor functions */
#include "sensors.h"

/* Include servo support if required */
#ifdef USE_SERVOS
   #include <Servo.h>
   #include "servos.h"
#endif

#ifdef USE_BASE
  /* Motor driver function definitions */
  #include "motor_driver.h"

  /* Encoder driver function definitions */
  #include "encoder_driver.h"

  /* PID parameters and functions */
  #include "diff_controller.h"

  /* Run the PID loop at 30 times per second */
  #define PID_RATE           30     // Hz

  /* Convert the rate into an interval */
  const int PID_INTERVAL = 1000 / PID_RATE;
  
  /* Track the next time we make a PID calculation */
  unsigned long nextPID = PID_INTERVAL;

  /* Stop the robot if it hasn't received a movement command
   in this number of milliseconds */
  #define AUTO_STOP_INTERVAL 5000
  long lastMotorCommand = AUTO_STOP_INTERVAL;
#endif

/* Variable initialization */

// A pair of varibles to help parse serial commands (thanks Fergs)
int arg = 0;
int index = 0;

// Variable to hold an input character
char chr;

// Variable to hold the current single-character command
char cmd;

// Character arrays to hold the first and second arguments
char argv1[16];
char argv2[16];

// The arguments converted to integers
long arg1;
long arg2;



// bluetooth related
#define DEFAULT_SPEED 44 
unsigned long lastBluetoothCommandTime = 0;
bool bluetoothOverrideActive = false;
const unsigned long bluetoothOverrideDuration = 400;


// Define pins for the front ultrasonic sensor
#define FRONT_TRIG_PIN 49
#define FRONT_ECHO_PIN 48

// Define pins for the back ultrasonic sensor
#define BACK_TRIG_PIN 53
#define BACK_ECHO_PIN 52

// For wheels stopping
int currentLeftSpeed = 0;
int currentRightSpeed = 0;



// Function to measure distance using an ultrasonic sensor
long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // distance in cm
  return distance;
}


/* Clear the current command parameters */
void resetCommand() {
  cmd = NULL;
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  arg1 = 0;
  arg2 = 0;
  arg = 0;
  index = 0;
}

/* Run a command.  Commands are defined in commands.h */
int runCommand() {
  int i = 0;
  char *p = argv1;
  char *str;
  int pid_args[4];
  arg1 = atoi(argv1);
  arg2 = atoi(argv2);
  
  switch(cmd) {
  case GET_BAUDRATE:
    Serial.println(BAUDRATE);
    break;
  case ANALOG_READ:
    Serial.println(analogRead(arg1));
    break;
  case DIGITAL_READ:
    Serial.println(digitalRead(arg1));
    break;
  case ANALOG_WRITE:
    analogWrite(arg1, arg2);
    Serial.println("OK"); 
    break;
  case DIGITAL_WRITE:
    if (arg2 == 0) digitalWrite(arg1, LOW);
    else if (arg2 == 1) digitalWrite(arg1, HIGH);
    Serial.println("OK"); 
    break;
  case PIN_MODE:
    if (arg2 == 0) pinMode(arg1, INPUT);
    else if (arg2 == 1) pinMode(arg1, OUTPUT);
    Serial.println("OK");
    break;
  case PING:
    Serial.println(Ping(arg1));
    break;
  case DANCE:
    randomEarWiggle();
    break;
#ifdef USE_SERVOS
  case SERVO_WRITE:
    servos[arg1].setTargetPosition(arg2);
    Serial.println("OK");
    break;
  case SERVO_READ:
    Serial.println(servos[arg1].getServo().read());
    break;

#endif
    
#ifdef USE_BASE
  case READ_ENCODERS:
    Serial.print(readEncoder(LEFT));
    Serial.print(" ");
    Serial.println(readEncoder(RIGHT));
    break;
   case RESET_ENCODERS:
    resetEncoders();
    resetPID();
    Serial.println("OK");
    break;

  case MOTOR_SPEEDS:
    /* Reset the auto stop timer */
    lastMotorCommand = millis();

    // Check for forward motion: both speeds positive
    if (arg1 > 0 && arg2 > 0) {
      long frontDistance = readUltrasonic(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
      if (frontDistance < 10) {
        setMotorSpeeds(0, 0);
        moving = 0;
        Serial.println("Obstacle Ahead: Cannot move forward!");
        break;
      }
    }
    // Check for backward motion: both speeds negative
    if (arg1 < 0 && arg2 < 0) {
      long backDistance = readUltrasonic(BACK_TRIG_PIN, BACK_ECHO_PIN);
      if (backDistance < 10) {
        setMotorSpeeds(0, 0);
        moving = 0;
        Serial.println("Obstacle Behind: Cannot move backward!");
        break;
      }
    }
    
    if (arg1 == 0 && arg2 == 0) {
      setMotorSpeeds(0, 0);
      resetPID();
      moving = 0;
    }
    else {
      moving = 1;
      // Store the current speeds
      currentLeftSpeed = arg1;
      currentRightSpeed = arg2;
      // Serial.println("Motr speeds"); 
      // Serial.print(currentLeftSpeed);
      // Serial.print(currentRightSpeed);
    }
    leftPID.TargetTicksPerFrame = arg1;
    rightPID.TargetTicksPerFrame = arg2;
    Serial.println("OK"); 
    break;

  case MOTOR_RAW_PWM:
    /* Reset the auto stop timer */
    lastMotorCommand = millis();
    resetPID();
    moving = 0; // Temporarily disable the PID
    raw_moving = 1; 
    setMotorSpeeds(arg1, arg2);

    // Check for forward motion: both speeds positive
    if (arg1 > 0 && arg2 > 0) {
      long frontDistance = readUltrasonic(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
      if (frontDistance < 10) {
        setMotorSpeeds(0, 0);  // Stop the motors
        moving = 0;
        Serial.println("Obstacle Ahead: Cannot move forward!");
        break;
      }
    }
    // Check for backward motion: both speeds negative
    if (arg1 < 0 && arg2 < 0) {
      long backDistance = readUltrasonic(BACK_TRIG_PIN, BACK_ECHO_PIN);
      if (backDistance < 10) {
        setMotorSpeeds(0, 0);  // Stop the motors
        moving = 0;
        Serial.println("Obstacle Behind: Cannot move backward!");
        break;
      }
    }

    // Store the raw PWM values as current speeds
    currentLeftSpeed = arg1;
    currentRightSpeed = arg2;
    Serial.println("OK"); 
    break;
  case UPDATE_PID:
    while ((str = strtok_r(p, ":", &p)) != '\0') {
       pid_args[i] = atoi(str);
       i++;
    }
    Kp = pid_args[0];
    Kd = pid_args[1];
    Ki = pid_args[2];
    Ko = pid_args[3];
    Serial.println("OK");
    break;
#endif

  case READ_ULTRASONIC: {
    long frontDistance = readUltrasonic(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
    long backDistance = readUltrasonic(BACK_TRIG_PIN, BACK_ECHO_PIN);
    // Print distances separated by a space:
    Serial.print(frontDistance);
    Serial.print(" ");
    Serial.println(backDistance);
    break;
  }

  case TEST: {
    Serial.print("test");
    break;
  }

  default:
    Serial.println("Invalid Command");
    break;
  }
}



/* Setup function--runs once at startup. */
void setup() {
  Serial.begin(BAUDRATE);        // USB serial
  Serial1.begin(9600);       // Bluetooth serial
    // Initialize ultrasonic sensor pins
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(BACK_TRIG_PIN, OUTPUT);
  pinMode(BACK_ECHO_PIN, INPUT);



// Initialize the motor controller if used */
#ifdef USE_BASE
  #ifdef ARDUINO_ENC_COUNTER
    //set as inputs
    DDRD &= ~(1<<LEFT_ENC_PIN_A);
    DDRD &= ~(1<<LEFT_ENC_PIN_B);
    DDRC &= ~(1<<RIGHT_ENC_PIN_A);
    DDRC &= ~(1<<RIGHT_ENC_PIN_B);
    
    //enable pull up resistors
    PORTD |= (1<<LEFT_ENC_PIN_A);
    PORTD |= (1<<LEFT_ENC_PIN_B);
    PORTC |= (1<<RIGHT_ENC_PIN_A);
    PORTC |= (1<<RIGHT_ENC_PIN_B);
    
    // tell pin change mask to listen to left encoder pins
    PCMSK2 |= (1 << LEFT_ENC_PIN_A)|(1 << LEFT_ENC_PIN_B);
    // tell pin change mask to listen to right encoder pins
    PCMSK1 |= (1 << RIGHT_ENC_PIN_A)|(1 << RIGHT_ENC_PIN_B);
    
    // enable PCINT1 and PCINT2 interrupt in the general interrupt mask
    PCICR |= (1 << PCIE1) | (1 << PCIE2);
  #endif
  initMotorController();
  resetPID();
#endif

/* Attach servos if used */
  #ifdef USE_SERVOS
    int i;
    for (i = 0; i < N_SERVOS; i++) {
      servos[i].initServo(
          servoPins[i],
          stepDelay[i],
          servoInitPosition[i]);
    }
  #endif
}



void injectCommand(const char* commandStr) {
  // Clear any previous command info.
  resetCommand();

  // The command string is expected to be in the form: "m 100 100"
  // Set the command character.
  cmd = commandStr[0]; // e.g. 'm'

  // Move past the command and a space.
  const char* p = commandStr + 2;

  // Copy first argument into argv1.
  int i = 0;
  while (p[i] != ' ' && p[i] != '\0') {
    argv1[i] = p[i];
    i++;
  }
  argv1[i] = '\0';

  // If there is a space, copy second argument.
  if (p[i] == ' ') {
    i++;  // Skip the space.
    int j = 0;
    while (p[i] != '\0') {
      argv2[j] = p[i];
      i++; j++;
    }
    argv2[j] = '\0';
  }
  
  // Execute the command.
  runCommand();
  resetCommand();
}


void processBluetoothCommands() {
  if (Serial1.available()) {
    char btCmd = Serial1.read();
    
    switch (btCmd) {
      case '1': // forward: send "m 100 100"
        injectCommand("m 44 44");
        bluetoothOverrideActive = true;
        lastBluetoothCommandTime = millis();
        break;
        
      case '2': // backward: send "m -100 -100"
        injectCommand("m -44 -44");
        bluetoothOverrideActive = true;
        lastBluetoothCommandTime = millis();
        break;
        
      case '3': // left: send "m -100 100"
        injectCommand("m -44 44");
        bluetoothOverrideActive = true;
        lastBluetoothCommandTime = millis();
        break;
        
      case '4': // right: send "m 100 -100"
        injectCommand("m 44 -44");
        bluetoothOverrideActive = true;
        lastBluetoothCommandTime = millis();
        break;
        
      // Optionally, you can add a case for stopping the robot:
      case '0': // stop
        injectCommand("m 0 0");
        bluetoothOverrideActive = false;
        break;
        
      default:
        break;
    }
  }
}



/* Enter the main loop.  Read and parse input from the serial port
   and run any valid commands. Run a PID calculation at the target
   interval and check for auto-stop conditions.
*/
void loop() {
  // Process Bluetooth commands regardless.
  processBluetoothCommands();

  // If Bluetooth override is active, check if its duration has expired.
  if (bluetoothOverrideActive) {
    if (millis() - lastBluetoothCommandTime >= bluetoothOverrideDuration) {
      // Override period is over; allow ROS commands again.
      bluetoothOverrideActive = false;
    }
  }

  // Only process ROS (Serial) commands if Bluetooth override is NOT active.
  if (!bluetoothOverrideActive) {
  while (Serial.available() > 0) {
    
    // Read the next character
    chr = Serial.read();

    // Terminate a command with a CR
    if (chr == 13) {
      if (arg == 1) argv1[index] = NULL;
      else if (arg == 2) argv2[index] = NULL;
      runCommand();
      resetCommand();
    }
    // Use spaces to delimit parts of the command
    else if (chr == ' ') {
      // Step through the arguments
      if (arg == 0) arg = 1;
      else if (arg == 1)  {
        argv1[index] = NULL;
        arg = 2;
        index = 0;
      }
      continue;
    }
    else {
      if (arg == 0) {
        // The first arg is the single-letter command
        cmd = chr;
      }
      else if (arg == 1) {
        // Subsequent arguments can be more than one character
        argv1[index] = chr;
        index++;
      }
      else if (arg == 2) {
        argv2[index] = chr;
        index++;
      }
    }
  }
  }
  

// If we are using base control, run a PID calculation at the appropriate intervals
#ifdef USE_BASE
  if (millis() > nextPID) {
    updatePID();
    nextPID += PID_INTERVAL;
  }

  // If the robot is currently moving (either moving or raw_moving), check the ultrasonic sensors
  if (moving || raw_moving) {
    long frontDistance = readUltrasonic(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
    long backDistance = readUltrasonic(BACK_TRIG_PIN, BACK_ECHO_PIN);

    // If moving forward (both speeds positive) and an obstacle is detected in front:
    if ((currentLeftSpeed > 0 && currentRightSpeed > 0) && frontDistance < 10) {
      setMotorSpeeds(0, 0);
      moving = 0;
      raw_moving = 0; // Stop raw movement if obstacle detected
      Serial.println("Safety Stop: Obstacle Ahead!");
    }

    // If moving backward (both speeds negative) and an obstacle is detected at the back:
    if ((currentLeftSpeed < 0 && currentRightSpeed < 0) && backDistance < 10) {
      setMotorSpeeds(0, 0);
      moving = 0;
      raw_moving = 0; // Stop raw movement if obstacle detected
      Serial.println("Safety Stop: Obstacle Behind!");
    }
  }


  // Check to see if we have exceeded the auto-stop interval
  if ((millis() - lastMotorCommand) > AUTO_STOP_INTERVAL) {;
    setMotorSpeeds(0, 0);
    moving = 0;
  }
#endif

// Sweep servos
#ifdef USE_SERVOS
  int i;
  for (i = 0; i < N_SERVOS; i++) {
    servos[i].doSweep();
  }
#endif
}

