#ifndef MPU6050_H_MOTOR_CONTROL
#define MPU6050_H_MOTOR_CONTROL
#include <stdint.h>             // int16_t

//*******************************Structure calls********************************************//
struct SensorData
{
  int16_t x;
  int16_t y;
  int16_t z;
  int16_t temperature;
  int16_t gX;
  int16_t gY;
  int16_t gZ;
};

struct AngleData
{
  double x;
  double y;
  double z;
};

struct BiasData
{
  int16_t aXB;
  int16_t aYB;
  int16_t aZB;
  int16_t gXB;
  int16_t gYB;
  int16_t gZB;
};

//********************************Function calls********************************************//

void setupMPU6050(void);
void readGyro(SensorData* gyro);
void readAccelerometer(SensorData* accel);
void meanGyroscope(SensorData* meanData, int32_t numSamples);
void meanAccelerometer(SensorData* meanAccel, int32_t numSamples);
void calibrateGyro(SensorData* zeroBiasOffset, int32_t numSamples);
void calibrateAccelerometer(SensorData* zeroBiasOffset, int32_t numSamples);

#endif 
