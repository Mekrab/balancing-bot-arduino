#include "pins.h"
#include "motor.h"
#include <util/atomic.h>

//*******************Global Constants & Global Variables***************************//
const unsigned long ENCODER_SAMPLE_INTERVAL = 500UL;    // units, milliseconds
volatile unsigned long leftEncoderCount = 0;
volatile unsigned long rightEncoderCount = 0;

//********Interrupt routine to update left encoder to take voltage*****************//
void leftEncoderISR(void)
{
  leftEncoderCount++;
}

//**************************Put pin to take voltage********************************//
void voltage()
{
  analogReference(INTERNAL);
}

//**********************Put pin to Interrupt left & right motor********************//
ISR(PCINT0_vect)
  {
  rightEncoderCount++;
  }
ISR(PCINT2_vect)
  {
  rightEncoderCount++;
  }

//**********************Function mysetup change port based in int********************//  
void mysetup(void)
{
  pinMode(LEFT_ENCODER_A_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_A_PIN), leftEncoderISR, CHANGE);
  voltage();
  cli();// cli to turn off interupts at fist  
  PCICR |= 0b00000100; // Enable ports to flip interrupts during service
  PCMSK2 |= 0b00010000; 
  sei();//enable interupts  
  
}

//*********************************Enter Main**************************************//
int main(void){

  unsigned char motorSpeed = 64;                             // This number is the 25% dudty cycle that is needed for output
  unsigned long startTime, dTEncoder;                        // Start clock for passing tp python 
  unsigned long count_left[32],count_right[32],elapsed_time[32] = {0};                // Arrays for storing counts and sending over to python 
  float battery[32] = {0};
  int count[32] = {0}; 
  int x = 0;
  int y = 0;
  const char* endsigMsg = "ENDOFSIG";  
  init();                                                    // Set up pins and hardware 
  mysetup(); 
  
  Serial.begin(115200);                                      // Set up baud rate at 115200
  initMotors();                                              // Initialize Motors via header file 
  delay(3000);
 
//********************Enter while loop for main program****************************// 
  while(1)                                                   // Found while loop for state machine was best way 
  {                                                          // To switch motors back and forth between speeds 
    if(x == 0){
      driveForward(motorSpeed);                              // Dirve forward at 25 % speed 
      delay(3000);                                           // Bring the motor hardware up to speed 
      startTime = millis();
    }         
    while(x < 32){
      if( (millis() - startTime) >=  ENCODER_SAMPLE_INTERVAL)
      {      
        dTEncoder = millis() - startTime;                      // Store time into an array for output and reading in python 
        elapsed_time[x] = {dTEncoder};                     // Store time into an array for output and reading in python         
        startTime = millis();
//**************************** Enter ATOMIC_BLOCK**********************************//         
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                     // Need to block interrupts so that atomic count for output 
                                                                // does not get killed and we can pass clean output to python
//************************ Set up counts for output********************************//  
          count[x] = x; 
          //battery[x] = (analogRead(VOL_MEASURE_PIN) * 1.1 / 1024) * (7.6666);   /// Am not sending so do not need 
          count_right[x] = rightEncoderCount;
          count_left[x] = leftEncoderCount;                                       
        }  
           if(x == 30){
            for(int y = 0; y < 31; y++){
              Serial.println("COUNT");
              Serial.print('\t');
              Serial.println(count[y]);
              Serial.print('\t');
              Serial.println("LEFT");
              Serial.print('\t');
              Serial.println(count_left[y]);
              Serial.print('\t');
              Serial.println("RIGHT");
              Serial.print('\t');
              Serial.println(count_right[y]);
              Serial.print('\t');
              //Serial.print('\t');
              //Serial.print("BATTERY");
              //Serial.print(battery[y]);                             Use for future programs or debugging
              //Serial.print(",");
              //Serial.println(" ");
              //Serial.print("TIME");
              //Serial.print(elapsed_time[y]);
              //Serial.print(",");
             }  
          }
                     
           if(x == 10){
            motorSpeed = 128;                                         // This number is the 50% dudty cycle that is needed for output
           if(y==0){                                                 // We are driving forward though so it will be 128-64
              driveForward(motorSpeed); 
              delay(3000);                                            // Bring the motor hardware up to speed 
              startTime = millis();                
              }
           if(y==1){
              driveBackward(motorSpeed);
              delay(3000);                                             // Bring the motor hardware up to speed 
              startTime = millis();
            }
          }
        
           if(x == 20){
           motorSpeed = 191;                                          // This number is the 75% dudty cycle that is needed for output
           if(y==0){
              driveForward(motorSpeed); 
              delay(3000);                                            // Bring the motor hardware up to speed 
              startTime = millis();
            }
           if(y==1){
             driveBackward(motorSpeed);
             delay(3000);
             startTime = millis();
            }
          }

           if(x == 30 & y == 0){
            stopMotors();
            delay(3000);                                           // Delay is needed as when its not there the motor will switch speeds to fast, bad output 
            motorSpeed = 64;                                       // This number is the 50% dudty cycle that is needed for output 
            driveBackward(motorSpeed);                             // We are drivingbackward though so it will be 64-0
            delay(3000);                                           // Bring the motor hardware up to speed 
            startTime = millis();
            x = 0;
            y = 1;
           }

 
           if(x == 30 & y == 1){
            stopMotors();
            Serial.println(endsigMsg);
           }
          x++;
          leftEncoderCount = 0UL;
          rightEncoderCount = 0UL;  
      }      
    }   
   }  
  return 0;
}
