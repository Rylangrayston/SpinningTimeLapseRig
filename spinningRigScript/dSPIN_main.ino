
int cameraProcesingDelay = 2000 ;// Nathan change the larger number on this line this number 1000 = 1 second 
int microSteps = 128;


int moterStopDelay = 250;  // delay this long after every motor move before taking a picture
int exposureTime = 20;

int shutterPin = 8;
int shutterLight = 9;

int stepsPerRevolution = 200 * microSteps; 
int revolutions = 1; // how many revolutions to take pictures for

int picsPerRevolution = 96;

int stepsPerPicture = stepsPerRevolution / picsPerRevolution;

int totalPicsToTake = (revolutions * stepsPerRevolution)/ stepsPerPicture;

int picCount = 0 ;












//dSPIN_main.ino - Contains the setup() and loop() functions.

float testSpeed = 10;

void setup() 
{
  pinMode(shutterPin, OUTPUT);
  pinMode(shutterLight, OUTPUT);
  
  // Standard serial port initialization for debugging.
  Serial.begin(9600);
  
  // dSPIN_init() is implemented in the dSPIN_support.ino file. It includes
  //  all the necessary port setup and SPI setup to allow the Arduino to
  //  control the dSPIN chip and relies entirely upon the pin redefinitions
  //  in dSPIN_example.ino
  dSPIN_init();
  
  // The following function calls are for this demo application- you will
  //  need to adjust them for your particular application, and you may need
  //  to configure additional registers.
  
  // First, let's set the step mode register:
  //   - dSPIN_SYNC_EN controls whether the BUSY/SYNC pin reflects the step
  //     frequency or the BUSY status of the chip. We want it to be the BUSY
  //     status.
  //   - dSPIN_STEP_SEL_x is the microstepping rate- we'll go full step.
  //   - dSPIN_SYNC_SEL_x is the ratio of (micro)steps to toggles on the
  //     BUSY/SYNC pin (when that pin is used for SYNC). Make it 1:1, despite
  //     not using that pin.
  dSPIN_SetParam(dSPIN_STEP_MODE, !dSPIN_SYNC_EN | dSPIN_STEP_SEL_1_128 | dSPIN_SYNC_SEL_1);
  // Configure the MAX_SPEED register- this is the maximum number of (micro)steps per
  //  second allowed. You'll want to mess around with your desired application to see
  //  how far you can push it before the motor starts to slip. The ACTUAL parameter
  //  passed to this function is in steps/tick; MaxSpdCalc() will convert a number of
  //  steps/s into an appropriate value for this function. Note that for any move or
  //  goto type function where no speed is specified, this value will be used.
  dSPIN_SetParam(dSPIN_MAX_SPEED, MaxSpdCalc(50));
  // Configure the FS_SPD register- this is the speed at which the driver ceases
  //  microstepping and goes to full stepping. FSCalc() converts a value in steps/s
  //  to a value suitable for this register; to disable full-step switching, you
  //  can pass 0x3FF to this register.
  dSPIN_SetParam(dSPIN_FS_SPD, FSCalc(100000));
  // Configure the acceleration rate, in steps/tick/tick. There is also a DEC register;
  //  both of them have a function (AccCalc() and DecCalc() respectively) that convert
  //  from steps/s/s into the appropriate value for the register. Writing ACC to 0xfff
  //  sets the acceleration and deceleration to 'infinite' (or as near as the driver can
  //  manage). If ACC is set to 0xfff, DEC is ignored. To get infinite deceleration
  //  without infinite acceleration, only hard stop will work.
  dSPIN_SetParam(dSPIN_ACC, 0x009);
  dSPIN_SetParam(dSPIN_DEC, 0x009);
  // Configure the overcurrent detection threshold. The constants for this are defined
  //  in the dSPIN_example.ino file.
  dSPIN_SetParam(dSPIN_OCD_TH, dSPIN_OCD_TH_6000mA);
  // Set up the CONFIG register as follows:
  //  PWM frequency divisor = 1
  //  PWM frequency multiplier = 2 (62.5kHz PWM frequency)
  //  Slew rate is 290V/us
  //  Do NOT shut down bridges on overcurrent
  //  Disable motor voltage compensation
  //  Hard stop on switch low
  //  16MHz internal oscillator, nothing on output
  dSPIN_SetParam(dSPIN_CONFIG, 
                   dSPIN_CONFIG_PWM_DIV_1 | dSPIN_CONFIG_PWM_MUL_2 | dSPIN_CONFIG_SR_290V_us
                 | dSPIN_CONFIG_OC_SD_DISABLE | dSPIN_CONFIG_VS_COMP_DISABLE 
                 | dSPIN_CONFIG_SW_HARD_STOP | dSPIN_CONFIG_INT_16MHZ);
  // Configure the RUN KVAL. This defines the duty cycle of the PWM of the bridges
  //  during running. 0xFF means that they are essentially NOT PWMed during run; this
  //  MAY result in more power being dissipated than you actually need for the task.
  //  Setting this value too low may result in failure to turn.
  //  There are ACC, DEC, and HOLD KVAL registers as well; you may need to play with
  //  those values to get acceptable performance for a given application.
  dSPIN_SetParam(dSPIN_KVAL_RUN, 0xff);
  // Calling GetStatus() clears the UVLO bit in the status register, which is set by
  //  default on power-up. The driver may not run without that bit cleared by this
  //  read operation.
  dSPIN_GetStatus();
  
  dSPIN_Move(FWD, 200*128);
}

// Continually turn one revolution forward, then back again, stopping in between each turn.
void loop()
{
  
  
  // 200 steps is one revolution on a 1.8 deg/step motor.
  digitalWrite(shutterPin, HIGH);  //  will execute the next motion with
  digitalWrite(shutterLight, HIGH);
  delay(exposureTime);
   digitalWrite(shutterPin, LOW);  //  will execute the next motion with
  digitalWrite(shutterLight, LOW);
  delay(cameraProcesingDelay);
  
  picCount += 1;
  if (picCount > totalPicsToTake){
    
    
  dSPIN_Move(FWD, stepsPerPicture);
  while (digitalRead(dSPIN_BUSYN) == LOW);
  dSPIN_SoftStop();                         // Inserting a soft stop between
  delay(moterStopDelay);     
    
   dSPIN_SetParam(dSPIN_KVAL_HOLD, 0x00);
   while(true){
   delay(100);
   digitalWrite(shutterLight,LOW);
   delay(100);
   digitalWrite(shutterLight,HIGH);
   
   } 
  }
  
  
  
  dSPIN_Move(FWD, stepsPerPicture);
  while (digitalRead(dSPIN_BUSYN) == LOW);
  dSPIN_SoftStop();                         // Inserting a soft stop between
  delay(moterStopDelay); 
                                            //  BUSYN pin will be low. 
                                            
                                            


  
  
}
