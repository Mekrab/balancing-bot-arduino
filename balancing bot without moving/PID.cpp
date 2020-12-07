#include "PID.h"
#include <Arduino.h> 
 
//#define DEBUG
#define TO_PYTHON
//*************************************************PID Function************************************************//
float PID(float wantedAngle, float currentAngle){
  float error = 0;                                 // Error rate  used in PID 

  float kp = 60;                                   // kp, ki, kd ratios 
  float kd = 0.1;
  float ki = 140;
  
  float currentT = 0.0;                            // Set clocks 
  static float lastT = 0.0;
  
  float dT = 0.0;                                  // Delta time 
  float pd = 0.0;                                  // pd, pi, NO PID NEEDED 
  float pi = 0.0;
  float derivative = 0.0;
  static float intergral = 0;
  static float prevAngle = 90;
  
  currentT = (millis()/1000.0);                    // Set clocks 
  dT = currentT - lastT;
  lastT = currentT;
  
  #ifdef DEBUG
  Serial.print("current time: ");
  Serial.print(currentT);
  Serial.print("\t");
  #endif

  #ifdef TO_PYTHON
  Serial.print(currentAngle);
  Serial.print(",");
  #endif

  #ifdef DEBUG
  Serial.print("dT: ");
  Serial.print(dT);
  Serial.print(" \t");
  #endif

  error = wantedAngle - currentAngle;                // Error rate calculation

  #ifdef DEBUG
  Serial.print("error: ");
  Serial.print(error);
  Serial.print("\t");
  #endif

  #ifdef TO_PYTHON
  Serial.print(error);
  Serial.print(",");
  #endif

  //P  Proportional which produces a correction which is proportional to the deviation from set-point
  //error*kp  
  //I   Intergral which produces a correction based on how long the measured variable has deviated from the set-point off full error
  intergral = intergral + error*dT; 
  //D  Differential which produces a correction based upon the speed with which the deviation is going away from the set-point the rate error
  derivative = (currentAngle - prevAngle)/dT;

  //updated angle 
  prevAngle = currentAngle;

  //calculate PID value
  //pid = (error*kp) + (intergral*ki) + (derivative*kd);
  pd = (error*kp) + (derivative*kd);
  pi = (error*kp) + (intergral*ki);

  #ifdef DEBUG
  Serial.print("pd:");
  Serial.print(pd);
  Serial.print("\t");
  #endif

  #ifdef DEBUG
  Serial.print("pi:");
  Serial.print(pi);
  Serial.print("\t");
  #endif
  
  #ifdef TO_PYTHON
  Serial.print(pd);
  Serial.print(",");
  #endif
  //PID   -200<pd>200 on speed. Did not use 255 because bot freaks out and spins out of controll at high speeds 
  if ( pd > 255 ){
    pd = 255;
  }
  else if (pd < -255){
    pd = -255;
  }

  return pd; 
}
