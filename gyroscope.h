#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "time.h"
#include "i2c.h"
#include "board.h"

#define GYRO_ADDRESS                      0x6B
#define GYRO_SENSITIVITY_250DPS           0.00875F    // Roughly 22/256 for fixed point match
#define GYRO_SENSITIVITY_500DPS           0.0175F     // Roughly 45/256
#define GYRO_SENSITIVITY_2000DPS          0.070F      // Roughly 18/256
#define SENSORS_DPS_TO_RADS               0.017453293F          /**< Degrees/s to rad/s multiplier */

#define GYRO_REGISTER_CTRL_REG1    0x20
#define GYRO_REGISTER_CTRL_REG2    0x21 
#define GYRO_REGISTER_CTRL_REG3    0x22 
#define GYRO_REGISTER_CTRL_REG4    0x23 
#define GYRO_REGISTER_CTRL_REG5    0x24 

#define GYRO_REGISTER_OUT_X_L      0x28 
#define GYRO_REGISTER_OUT_X_H      0x29 
#define GYRO_REGISTER_OUT_Y_L      0x2A 
#define GYRO_REGISTER_OUT_Y_H      0x2B 
#define GYRO_REGISTER_OUT_Z_L      0x2C 
#define GYRO_REGISTER_OUT_Z_H      0x2D 

typedef enum
{
  GYRO_RANGE_250DPS  = 250,
  GYRO_RANGE_500DPS  = 500,
  GYRO_RANGE_2000DPS = 2000
} gyroRange_t;

gyroAccelXYZ  getGyroscopeData();

#endif
