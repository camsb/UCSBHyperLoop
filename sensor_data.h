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
  float accelX;
  float accelY;
  float accelZ;
  int32_t temp;
  uint32_t pressure;

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
