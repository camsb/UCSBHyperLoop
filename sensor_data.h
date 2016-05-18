#ifndef SENSOR_DATA_H_
#define SENSOR_DATA_H_

#include "board.h"

void 	collectCalibrationData(I2C_ID_T id);
void    collectData(I2C_ID_T id);
void    TIMER1_IRQHandler(void);
void    gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);

typedef struct{

  float x;
  float y;
  float z;

} XYZ;

typedef struct{

  uint32_t dataPrintFlag;

  float gyroX;
  float gyroY;
  float gyroZ;

  float longRangingJ22;
  float longRangingJ25;
  float longRangingJ30;
  float longRangingJ31;

  float shortRangingJ34;
  float shortRangingJ35;
  float shortRangingJ36;
  float shortRangingJ37;

  float initialAccelX;
  float initialAccelY;
  float initialAccelZ;

  float accelX;
  float accelY;
  float accelZ;

  float velocityX;
  float velocityY;
  float velocityZ;

  float positionX;
  float positionY;
  float positionZ;

  float temp1;
  float temp2;
  float pressure;

} sensor;

uint8_t collectDataFlag;
sensor sensorData;

#endif
