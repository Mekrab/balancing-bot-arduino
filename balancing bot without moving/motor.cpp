#include "pins.h"
#include "motor.h"
#include <Arduino.h>

#define DEBUG

void initMotors( void)
{
  pinMode(AIN1, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(PWMA_LEFT, OUTPUT);
  pinMode(PWMB_RIGHT, OUTPUT);
  pinMode(STBY, OUTPUT);
  stopMotors();
}

void stopMotors(void)
{
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(STBY, HIGH);
  analogWrite(PWMA_LEFT, 0);
  analogWrite(PWMB_RIGHT, 0);
}


void driveForward(unsigned char speed)
{
  digitalWrite(AIN1, 0);
  digitalWrite(BIN1, 0);
  analogWrite(PWMA_LEFT, speed);
  analogWrite(PWMB_RIGHT, speed);
}


void driveBackward(unsigned char speed)
{
  digitalWrite(AIN1, 1);
  digitalWrite(BIN1, 1);
  analogWrite(PWMA_LEFT, speed);
  analogWrite(PWMB_RIGHT, speed);
}


void balance(float motorSpeed){
  if(motorSpeed>0) {
    driveBackward(char(abs(motorSpeed)));
  }
  if(motorSpeed<0) {
    driveForward(char(abs(motorSpeed)));
  }
}
