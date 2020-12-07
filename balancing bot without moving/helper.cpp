#include "helper.h"
#include <Arduino.h>          // Serial
void printOffsetValues(const SensorData* offset)
{

  Serial.print("Accelerometer offset values, x: ");
  Serial.print(offset->x);
  Serial.print(", y: ");
  Serial.print(offset->y);
  Serial.print(", z: ");
  Serial.println(offset->z);
  Serial.print("Gyroscope offset values, x: ");
  Serial.print(offset->gX);
  Serial.print(", y: ");
  Serial.print(offset->gY);
  Serial.print(", z: ");
  Serial.println(offset->gZ);
}
