#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

void initMotors( void);
void stopMotors(void);
void driveForward(unsigned char speed);
void driveBackward(unsigned char speed);
void balance(float motorSpeed);

#endif 
