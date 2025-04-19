/* Functions and type-defs for PID control.

   Taken mostly from Mike Ferguson's ArbotiX code which lives at:
   
   http://vanadium-ros-pkg.googlecode.com/svn/trunk/arbotix/
*/

/* PID setpoint info For a Motor */


/* PID setpoint info For a Motor */
typedef struct {
  double TargetTicksPerFrame;    // target speed in ticks per frame
  long Encoder;                  // current encoder count
  long PrevEnc;                  // last encoder count
  int PrevInput;                 // last input (used for derivative term)
  int ITerm;                     // integrated term
  long output;                   // last motor output value
} SetPointInfo;

SetPointInfo leftPID, rightPID;

/* PID Parameters */
int Kp = 20;
int Kd = 12;
int Ki = 0.5; 
int Ko = 50;


const long MAX_ITERM = 500;   // Maximum allowed value for the integrator

unsigned char moving = 0; // flag indicating if the base is in motion
unsigned char raw_moving = 0; // flag indicating if the base is in motion with raw pwm

int PWM_max = 80;

/*
 * Reset the PID variables to prevent startup spikes.
 * Note: When going from stop to motion, it’s useful to initialize
 * the integrator and previous input values.
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

/*
 * PID routine to compute the next motor commands with anti-windup.
 */
void doPID(SetPointInfo * p) {
  long Perror;
  long output;
  int input;

  // Calculate change in encoder value since last update
  input = p->Encoder - p->PrevEnc;
  // Compute error between target and actual ticks per frame
  Perror = p->TargetTicksPerFrame - input;

  // Compute the proportional term
  long Pterm = Kp * Perror;
  // Compute the derivative term (using change in input to avoid derivative kick)
  long Dterm = -Kd * (input - p->PrevInput);

  // Update the integral term with anti-windup clamping:
  p->ITerm += Ki * Perror;
  if (p->ITerm > MAX_ITERM) p->ITerm = MAX_ITERM;
  if (p->ITerm < -MAX_ITERM) p->ITerm = -MAX_ITERM;

  // Combine PID terms and scale by Ko
  output = (Pterm + Dterm + p->ITerm) / Ko;

  // Optionally add the previous output for smoothness
  output += p->output;


  // Clamp the final output to valid PWM range
  if (output > PWM_max) {
      output = PWM_max;
  } else if (output < -PWM_max) {
      output = -PWM_max;
  }

  // Update stored values for next iteration
  p->output = output;
  p->PrevEnc = p->Encoder;
  p->PrevInput = input;
}

/*
 * Read the encoder values and update the PID controllers.
 */
void updatePID() {
  // Read current encoder counts
  leftPID.Encoder = readEncoder(LEFT);
  rightPID.Encoder = readEncoder(RIGHT);
  
  // If the robot is not moving, reset PID to avoid spikes.
  if (!moving) {
    if (leftPID.PrevInput != 0 || rightPID.PrevInput != 0) {
      resetPID();
    }
    return;
  }

  // Compute PID output for each motor
  doPID(&leftPID);
  doPID(&rightPID);

  // Set the motor speeds based on PID output.
  setMotorSpeeds(leftPID.output, rightPID.output);
}
