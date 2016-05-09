#ifndef SENSOR_DATA_H_
#define SENSOR_DATA_H_

#include "stdint.h"

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

// typedef struct stringDataPair{

//   int val;
//   char * name;

// }stringDataPair;

// typedef struct sensorPosition{

  // stringDataPair gyroX;
  // stringDataPair gyroY;
  // stringDataPair gyroZ;
  // stringDataPair accelX;
  // stringDataPair accelY;
  // stringDataPair accelZ;
  // stringDataPair temp;
  // stringDataPair pressure;

// }sensorData;

sensor sensorData;

#endif
