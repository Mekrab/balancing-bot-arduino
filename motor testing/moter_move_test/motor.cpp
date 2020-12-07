#include "pins.h"
#include "motor.h"
#include <Arduino.h>


void initMotors( void)
{
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(PWMA_LEFT_PIN, OUTPUT);
  pinMode(PWMB_RIGHT_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);
  digitalWrite(AIN1_PIN, 0);
  digitalWrite(BIN1_PIN, 0);
  analogWrite(PWMA_LEFT_PIN, 25);
  analogWrite(PWMB_RIGHT_PIN, 25);
  digitalWrite(AIN1_PIN, 1);
  digitalWrite(BIN1_PIN, 1);
  analogWrite(PWMA_LEFT_PIN, 25);
  analogWrite(PWMB_RIGHT_PIN, 25);
  stopMotors();
}

void stopMotors(void)
{
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(BIN1_PIN, LOW);
  digitalWrite(STBY_PIN, HIGH);
  analogWrite(PWMA_LEFT_PIN, 0);
  analogWrite(PWMB_RIGHT_PIN, 0);
}


void driveForward(unsigned char speed)
{
  digitalWrite(AIN1_PIN, 0);
  digitalWrite(BIN1_PIN, 0);
  analogWrite(PWMA_LEFT_PIN, speed);
  analogWrite(PWMB_RIGHT_PIN, speed);
}


void driveBackward(unsigned char speed)
{
  digitalWrite(AIN1_PIN, 1);
  digitalWrite(BIN1_PIN, 1);
  analogWrite(PWMA_LEFT_PIN, speed);
  analogWrite(PWMB_RIGHT_PIN, speed);
}
