/* Functions and type-defs for PID control.

   Taken mostly from Mike Ferguson's ArbotiX code which lives at:
   
   http://vanadium-ros-pkg.googlecode.com/svn/trunk/arbotix/
*/

/* PID setpoint info For a Motor */
typedef struct {
  double TargetTicksPerFrame;    // target speed in ticks per frame
  long Encoder;                  // encoder count
  long PrevEnc;                  // last encoder count

  /*
  * Using previous input (PrevInput) instead of PrevError to avoid derivative kick,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  */
  int PrevInput;                // last input
  //int PrevErr;                   // last error

  /*
  * Using integrated term (ITerm) instead of integrated error (Ierror),
  * to allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  //int Ierror;
  int ITerm;                    //integrated term

  long output;                    // last motor setting
}
SetPointInfo;

SetPointInfo leftPID, rightPID;

/* PID Parameters */
// int Kp = 20;
// int Kd = 12;
// int Ki = 0;
// int Ko = 50;
/* PID Parameters */
int Kp = 170;
int Kd = 80;
int Ki = 0;
int Ko = 50;


unsigned char moving = 0; // is the base in motion?

/*
* Initialize PID variables to zero to prevent startup spikes
* when turning PID on to start moving
* In particular, assign both Encoder and PrevEnc the current encoder value
* See http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
* Note that the assumption here is that PID is only turned on
* when going from stop to moving, that's why we can init everything on zero.
*/
void resetPID(){
   leftPID.TargetTicksPerFrame = 0.0;
   leftPID.Encoder = readEncoder(LEFT);
   leftPID.PrevEnc = leftPID.Encoder;
   leftPID.output = 0;
   leftPID.PrevInput = 0;
   leftPID.ITerm = 0;

   rightPID.TargetTicksPerFrame = 0.0;
   rightPID.Encoder = readEncoder(RIGHT);
   rightPID.PrevEnc = rightPID.Encoder;
   rightPID.output = 0;
   rightPID.PrevInput = 0;
   rightPID.ITerm = 0;
}

/* PID routine to compute the next motor commands */
void doPID(SetPointInfo * p) {
  long Perror;
  long output;
  int input;

  //Perror = p->TargetTicksPerFrame - (p->Encoder - p->PrevEnc);
  input = p->Encoder - p->PrevEnc;
  Perror = p->TargetTicksPerFrame - input;


  /*
  * Avoid derivative kick and allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  //output = (Kp * Perror + Kd * (Perror - p->PrevErr) + Ki * p->Ierror) / Ko;
  // p->PrevErr = Perror;
  output = (Kp * Perror - Kd * (input - p->PrevInput) + p->ITerm) / Ko;
  p->PrevEnc = p->Encoder;

  output += p->output;
  // Accumulate Integral error *or* Limit output.
  // Stop accumulating when output saturates
  if (output >= 255)
    output = 255;
  else if (output <= -255)
    output = -255;
  else
  /*
  * allow turning changes, see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
    p->ITerm += Ki * Perror;

  p->output = output;
  p->PrevInput = input;
}

/* Read the encoder values and call the PID routine */
void updatePID() {
  /* Read the encoders */
  leftPID.Encoder = readEncoder(LEFT);
  rightPID.Encoder = readEncoder(RIGHT);
  
  /* If we're not moving there is nothing more to do */
  if (!moving){
    /*
    * Reset PIDs once, to prevent startup spikes,
    * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
    * PrevInput is considered a good proxy to detect
    * whether reset has already happened
    */
    if (leftPID.PrevInput != 0 || rightPID.PrevInput != 0) resetPID();
    return;
  }

  /* Compute PID update for each motor */
  doPID(&rightPID);
  doPID(&leftPID);

  /* Set the motor speeds accordingly */
  setMotorSpeeds(leftPID.output, rightPID.output);
}


// /* PID setpoint info For a Motor */
// typedef struct {
//   double TargetTicksPerFrame;    // target speed in ticks per frame
//   long Encoder;                  // current encoder count
//   long PrevEnc;                  // last encoder count
//   int PrevInput;                 // last input (used for derivative term)
//   int ITerm;                     // integrated term
//   long output;                   // last motor output value
// } SetPointInfo;

// SetPointInfo leftPID, rightPID;

// /* PID Parameters */
// int Kp = 20;
// int Kd = 12;
// int Ki = 0;   // You may need to raise this if you want integral action
// int Ko = 50;


// const long MAX_ITERM = 500;   // Maximum allowed value for the integrator

// unsigned char moving = 0; // flag indicating if the base is in motion
// unsigned char raw_moving = 0; // flag indicating if the base is in motion with raw pwm


// /*
//  * Reset the PID variables to prevent startup spikes.
//  * Note: When going from stop to motion, it’s useful to initialize
//  * the integrator and previous input values.
//  */
// void resetPID(){
//    leftPID.TargetTicksPerFrame = 0.0;
//    leftPID.Encoder = readEncoder(LEFT);
//    leftPID.PrevEnc = leftPID.Encoder;
//    leftPID.output = 0;
//    leftPID.PrevInput = 0;
//    leftPID.ITerm = 0;

//    rightPID.TargetTicksPerFrame = 0.0;
//    rightPID.Encoder = readEncoder(RIGHT);
//    rightPID.PrevEnc = rightPID.Encoder;
//    rightPID.output = 0;
//    rightPID.PrevInput = 0;
//    rightPID.ITerm = 0;
// }

// /*
//  * PID routine to compute the next motor commands with anti-windup.
//  */
// void doPID(SetPointInfo * p) {
//   long Perror;
//   long output;
//   int input;

//   // Calculate change in encoder value since last update
//   input = p->Encoder - p->PrevEnc;
//   // Compute error between target and actual ticks per frame
//   Perror = p->TargetTicksPerFrame - input;

//   // Compute the proportional term
//   long Pterm = Kp * Perror;
//   // Compute the derivative term (using change in input to avoid derivative kick)
//   long Dterm = -Kd * (input - p->PrevInput);

//   // Update the integral term with anti-windup clamping:
//   p->ITerm += Ki * Perror;
//   if (p->ITerm > MAX_ITERM) p->ITerm = MAX_ITERM;
//   if (p->ITerm < -MAX_ITERM) p->ITerm = -MAX_ITERM;

//   // Combine PID terms and scale by Ko
//   output = (Pterm + Dterm + p->ITerm) / Ko;

//   // Optionally add the previous output for smoothness
//   output += p->output;

//   // Clamp the final output to valid PWM range
//   if (output > 255) {
//       output = 255;
//   } else if (output < -255) {
//       output = -255;
//   }

//   // Update stored values for next iteration
//   p->output = output;
//   p->PrevEnc = p->Encoder;
//   p->PrevInput = input;
// }

// /*
//  * Read the encoder values and update the PID controllers.
//  */
// void updatePID() {
//   // Read current encoder counts
//   leftPID.Encoder = readEncoder(LEFT);
//   rightPID.Encoder = readEncoder(RIGHT);
  
//   // If the robot is not moving, reset PID to avoid spikes.
//   if (!moving) {
//     if (leftPID.PrevInput != 0 || rightPID.PrevInput != 0) {
//       resetPID();
//     }
//     return;
//   }

//   // Compute PID output for each motor
//   doPID(&leftPID);
//   doPID(&rightPID);

//   // Set the motor speeds based on PID output.
//   setMotorSpeeds(leftPID.output, rightPID.output);
// }
