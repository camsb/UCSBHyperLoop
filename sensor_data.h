#ifndef SENSOR_DATA_H_
#define SENSOR_DATA_H_

#include "stdint.h"

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
  int32_t temp;
  uint32_t pressure;

} sensor;

uint8_t collectDataFlag;
sensor sensorData;

#endif
