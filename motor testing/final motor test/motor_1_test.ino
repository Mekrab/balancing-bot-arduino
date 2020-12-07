#include "pins.h"
#include "motor.h"
#include <Arduino.h>
#include <util/atomic.h>

const unsigned long ENCODER_SAMPLE_INTERVAL = 500UL;  // Sample time

volatile unsigned long leftEncoderCount = 0;          // Count of left Encoder
volatile unsigned long rightEncoderCount = 0;         // Count of right Encoder

// Interrupt pull up left
void leftEncoderISR(void)                             
{
  leftEncoderCount++;
}

// Interrupt pull up rigth (Both wheels)
ISR(PCINT2_vect)
{
  rightEncoderCount++;
}

ISR(PCINT0_vect)
{
  rightEncoderCount++;
}

//***************Setup to push Interrupt change and put ports for output***********// 
void mysetup(void)
{
  pinMode(LEFT_ENCODER_A_PIN, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_A_PIN), leftEncoderISR, CHANGE);
  cli();// cli to turn off interupts at fist  
  PCICR |= 0b00000100; //enables port D pin change interrupts  puts port D to change the interrupt sequence 
  PCMSK2 |= 0b00010000; 
  sei();// sei to turn on interupts at fist 
}

//*********************************** Enter Main***********************************//
int main(void)
{
  unsigned long startTime, dTEncoder;                           // Start time 
  unsigned long tempLeftEncoderCount,tempRightEncoderCount ;    // Start temp count for output on serial monitor
  unsigned char motorSpeed = 10;
  init();    
  mysetup();                                                    // Calls Interrupt setup 
  Serial.begin(115200);
  initMotors();                                                 // Start motor pull ups 
  startTime = millis();
  while(1)
  {
    if( (millis() - startTime) >=  ENCODER_SAMPLE_INTERVAL)
    {
      dTEncoder = millis() - startTime;
      startTime = millis();
      driveForward(motorSpeed);                                 // Need to start slow drive forward to start all service 
      
//**************************** Enter ATOMIC_BLOCK**********************************//      
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                      
        tempLeftEncoderCount = leftEncoderCount;                // Most important part of the code, the block keeps the count
        tempRightEncoderCount = rightEncoderCount;              // for output while keeping interupts running 
        Serial.println(tempLeftEncoderCount);
        Serial.println(tempRightEncoderCount);
      }      
    }
  }

  return 0;
}
