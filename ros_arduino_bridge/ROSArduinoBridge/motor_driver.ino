/***************************************************************
   Motor driver definitions
   
   Add a "#elif defined" block to this file to include support
   for a particular motor driver.  Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   *************************************************************/

#ifdef USE_BASE
   
#ifdef POLOLU_VNH5019
  /* Include the Pololu library */
  #include "DualVNH5019MotorShield.h"

  /* Create the motor driver object */
  DualVNH5019MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
#elif defined POLOLU_MC33926
  /* Include the Pololu library */
  #include "DualMC33926MotorShield.h"

  /* Create the motor driver object */
  DualMC33926MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
#elif defined L298_MOTOR_DRIVER

  #include <Servo.h>
  Servo servo1;
  Servo servo2;
  //

  void initMotorController() {
    digitalWrite(RIGHT_MOTOR_ENABLE, HIGH);
    digitalWrite(LEFT_MOTOR_ENABLE, HIGH);
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    //

  }
  
  void setMotorSpeed(int i, int spd) {
    unsigned char reverse = 0;
  
    if (spd < 0)
    {
      spd = -spd;
      reverse = 1;
    }
    if (spd > 255)
      spd = 255;
    
    if (i == LEFT) { 
      if      (reverse == 0) { analogWrite(LEFT_MOTOR_FORWARD, spd); analogWrite(LEFT_MOTOR_BACKWARD, 0); }
      else if (reverse == 1) { analogWrite(LEFT_MOTOR_BACKWARD, spd); analogWrite(LEFT_MOTOR_FORWARD, 0); }
    }
    else /*if (i == RIGHT) //no need for condition*/ {
      if      (reverse == 0) { analogWrite(RIGHT_MOTOR_FORWARD, spd); analogWrite(RIGHT_MOTOR_BACKWARD, 0); }
      else if (reverse == 1) { analogWrite(RIGHT_MOTOR_BACKWARD, spd); analogWrite(RIGHT_MOTOR_FORWARD, 0); }
    }
  }
  
  void moveServosSmoothly(int startAngle, int endAngle, int stepDelay) {
    if (startAngle < endAngle) {
        for (int pos = startAngle; pos <= endAngle; pos++) {
            servo1.write(pos);
            servo2.write(180 - pos);  // Moves opposite
            delay(stepDelay);
        }
    } else {
        for (int pos = startAngle; pos >= endAngle; pos--) {
            servo1.write(pos);
            servo2.write(180 - pos);  // Moves opposite
            delay(stepDelay);
        }
    }
}

  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);

    // Manual servo movement
    static int currentAngle = 90; // Neutral starting position

    if (leftSpeed > 0 && rightSpeed > 0) {
        // Move in one direction when both speeds are positive
        moveServosSmoothly(currentAngle, 180, 10);
        currentAngle = 180;
    } else if (leftSpeed < 0 && rightSpeed < 0) {
        // Move in the opposite direction when both speeds are negative
        moveServosSmoothly(currentAngle, 0, 10);
        currentAngle = 0;
    } else {
        // Reset to neutral when motors stop or have mixed directions
        moveServosSmoothly(currentAngle, 90, 10);
        currentAngle = 90;
    }


  }
#else
  #error A motor driver must be selected!
#endif

#endif
