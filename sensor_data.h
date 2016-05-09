#ifndef SENSOR_DATA_H_
#define SENSOR_DATA_H_

#include "board.h"

void    collectData();
void    TIMER1_IRQHandler(void);
void    gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);

typedef struct{

  float x;
  float y;
  float z;

} gyroAccelXYZ;

typedef struct{

  float gyroX;
  float gyroY;
  float gyroZ;
  float roll;
  float pitch;
  float yaw;
  float posX;
  float posY;
  float posZ;
  float velX;
  float velY;
  float velZ;
  float accelX;
  float accelY;
  float accelZ;
  float longRangingJ22;
  float longRangingJ25;
  float longRangingJ30;
  float longRangingJ31;
  float shortRangingJ34;
  float shortRangingJ35;
  float shortRangingJ36;
  float shortRangingJ37;
  float temp;
  float pressure;
  float power;

} sensor;

uint8_t collectDataFlag;
sensor sensorData;

#endif
