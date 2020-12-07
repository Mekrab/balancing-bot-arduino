#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"
#include "helper.h"
#include "pins.h"
#include "motor.h"
#include "PID.h"

//#define DEBUG                 // Debug for info 
//#define CALIBRATE             // Calibrat of intital offset values 
#define GOGOGOGO                // Run program 
#define INT_SAMPLE_MS  10       // Sample rate of 10 ms  
#define TO_PYTHON               // Push Values to Python for plotting 

//*****************************************Setup Serial I2C & mpu read*****************************************//
void setup(void)
{
  Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200);
  delay(1000);
  setupMPU6050();
  #ifdef DEGUG
    Serial.println(F("\nsetup complete\n"));
  #endif
  delay(100);
}
//************************************************Main Function************************************************//
int main(void)
{ 
  SensorData offset, accel,gyroRate;                  // Set sensor variables 
  AngleData gyroPosition = {0.0, 0.0, 0.0};           // Set angle variables 
  AngleData deltaPosition;                            // Set var for position change 
  unsigned long startTime, elapsedTime;               // Set clocks 
  double roll, pitch;                                 // Roll rotation about x, pitch rotation about y
  int sampleCount = 0;                                // Count for DEBUG use & if you want to send count to python.
  double roll_Filt = 0.0;                             // Filtered roll
  double pitch_Filt = 0.0;                            // Filtered pitch
  double gyroX_Filt = 0.0;                            // Filtered gyro X position 
  double gyroY_Filt = 0.0;                            // Filtered gyro Y position
  double gyroZ_Filt = 0.0;                            // Filtered gyro Y position  
  float currentAngle;                                 // Angle of robot at given sample
  float wantedAngle = 90;                             //test angles as robot seems to like to lean forward 
                                                      //90.00 89.00 88.00 87.00 86.00 85.00 84.00 83.00 82.00 81.00 80.00;               
  float motorSpeed = 0.0; 
   
  init();                                             // initialize timers, pulse width modulation, + hardware
  initMotors();                                       // Initialize Motors via header file 
  setup();                                            // Loop setup for setupMPU6050(); & Clock & Baud rate  

  #ifdef CALIBRATE
  calibrateAccelerometer(&offset, 1000);              //Calibrate Accelerometer w/ offset 
  calibrateGyro(&offset, 1000);                       //Calibrate Gyro w/ offset 
  #endif

  #ifdef GOGOGOGO                                     // Hardcode offsets 
  offset.x = -416;
  offset.y = 737;
  offset.z = 2235;
  offset.gX = 206;
  offset.gY = -185;
  offset.gZ = 113; 
  #endif
  
  startTime = millis();
  
  while(1)
  {
    if( (elapsedTime = (millis() - startTime)) >= INT_SAMPLE_MS)
    {
      readAccelerometer(&accel);
      readGyro(&gyroRate);
      startTime = millis(); 
      ++sampleCount;

      accel.x += offset.x;                                                   // apply accel  += offset at given position 
      accel.y += offset.y;
      accel.z += offset.z;
      gyroRate.x += offset.gX;                                                // apply gyro rate += offset at given position 
      gyroRate.y += offset.gY;
      gyroRate.z += offset.gZ;  
//1        
      pitch = atan2(accel.y, accel.z) * 180.0/PI;                             //Freescale equations pitch                       
      roll = atan(-accel.x / sqrt(pow(accel.y,2) + pow(accel.z,2))) * 180.0/PI; // Scale from -pi to +pi       
//2
      roll_Filt = 0.94 * roll_Filt + 0.06 * roll;                             // low pass filter roll
      pitch_Filt = 0.94 * pitch_Filt + 0.06 * pitch;                          // low pass filter pitch
//3
      deltaPosition.x = (double)gyroRate.x / 131.0 * elapsedTime / 1000.0;    // Scale to calculate change in overall position x y x
      deltaPosition.y = (double)gyroRate.y / 131.0 * elapsedTime / 1000.0;
      deltaPosition.z = (double)gyroRate.z / 131.0 * elapsedTime / 1000.0;
//4
      gyroPosition.x += deltaPosition.x;                                      // angle position + delta position to give overall angle gained                                       
      gyroPosition.y += deltaPosition.y;                                             
      gyroPosition.z += deltaPosition.z;
//5                                                                              
      gyroY_Filt = (0.94*gyroY_Filt) + (0.94*(deltaPosition.y));// Using a high pass filter we can filter the output as the change of the input       
      gyroX_Filt = (0.94*gyroX_Filt) + (0.94*(deltaPosition.x));
      gyroZ_Filt = (0.94*gyroZ_Filt) + (0.94*(deltaPosition.z));

//***************************** PLEASE READ *****************************//
// This is where it all came together for me and why the code works.
// We need to take the pitch angle and add the gyro angle AND add the 90 degrees to actually gain the current working angle.
// We first get the pitch and roll from the offset position and scale it into the radian from (1). 
// We then apply a low pass filter (2) that passes signals with a frequency lower than a selected cutoff frequency.
// After we have that we can then apply the change in positinon in radians to the time (3) putting in an easy commutable method [RAD/SEC]
// We can then apply this to an acutal angle position rate (4) for the final high pass filter (5) to allow high frequencies to get through while filtering or cutting low frequencies.

      currentAngle = (gyroX_Filt + pitch_Filt + 90);                         // See above ^^ 

      motorSpeed = PID(wantedAngle, currentAngle);                           //PID control variable to set moter speed for balance  
      
      balance(motorSpeed);                                                   //balance robot by taking moter speed off PID to drive forward or backward and keep stability in check   
        
      #ifdef TO_PYTHON
        Serial.print(motorSpeed);
        Serial.println("");
      #endif  

      //**************DEBUG calibrations**************//
       #ifdef DEBUG
         Serial.println(F("\nAcelerometer calibration complete\n"));
         printOffsetValues(&offset);
        delay(1000);
      #endif
      #ifdef DEBUG
        Serial.print("Angl: ");
        Serial.print(currentAngle);
        Serial.print("\t");
      #endif
      #ifdef DEBUG
        Serial.print("Motor Output at current angle: ");
        Serial.println(motorSpeed);
      #endif               
 

    }
  }

   
  return 0;
  
}
