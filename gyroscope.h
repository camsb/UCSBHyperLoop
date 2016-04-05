#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "time.h"
#include "i2c.h"
#include "stdint.h"

#define GYRO_ADDRESS     0x6B
#define GYRO_SENSITIVITY_250DPS  (0.00875F)    // Roughly 22/256 for fixed point match
#define GYRO_SENSITIVITY_500DPS  (0.0175F)     // Roughly 45/256
#define GYRO_SENSITIVITY_2000DPS (0.070F)      // Roughly 18/256
#define SENSORS_DPS_TO_RADS               (0.017453293F)          /**< Degrees/s to rad/s multiplier */

float*    getRotAcceleration();
float*    calculateRotAcc( uint8_t* uncalcRotAcc );

typedef enum
{                                             // DEFAULT    TYPE
  GYRO_REGISTER_WHO_AM_I            = 0x0F,   // 11010100   r
  GYRO_REGISTER_CTRL_REG1           = 0x20,   // 00000111   rw
  GYRO_REGISTER_CTRL_REG2           = 0x21,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG3           = 0x22,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG4           = 0x23,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG5           = 0x24,   // 00000000   rw
  GYRO_REGISTER_REFERENCE           = 0x25,   // 00000000   rw
  GYRO_REGISTER_OUT_TEMP            = 0x26,   //            r
  GYRO_REGISTER_STATUS_REG          = 0x27,   //            r
  GYRO_REGISTER_OUT_X_L             = 0x28,   //            r
  GYRO_REGISTER_OUT_X_H             = 0x29,   //            r
  GYRO_REGISTER_OUT_Y_L             = 0x2A,   //            r
  GYRO_REGISTER_OUT_Y_H             = 0x2B,   //            r
  GYRO_REGISTER_OUT_Z_L             = 0x2C,   //            r
  GYRO_REGISTER_OUT_Z_H             = 0x2D,   //            r
  GYRO_REGISTER_FIFO_CTRL_REG       = 0x2E,   // 00000000   rw
  GYRO_REGISTER_FIFO_SRC_REG        = 0x2F,   //            r
  GYRO_REGISTER_INT1_CFG            = 0x30,   // 00000000   rw
  GYRO_REGISTER_INT1_SRC            = 0x31,   //            r
  GYRO_REGISTER_TSH_XH              = 0x32,   // 00000000   rw
  GYRO_REGISTER_TSH_XL              = 0x33,   // 00000000   rw
  GYRO_REGISTER_TSH_YH              = 0x34,   // 00000000   rw
  GYRO_REGISTER_TSH_YL              = 0x35,   // 00000000   rw
  GYRO_REGISTER_TSH_ZH              = 0x36,   // 00000000   rw
  GYRO_REGISTER_TSH_ZL              = 0x37,   // 00000000   rw
  GYRO_REGISTER_INT1_DURATION       = 0x38    // 00000000   rw
} gyroRegisters_t;

typedef enum
{
  GYRO_RANGE_250DPS  = 250,
  GYRO_RANGE_500DPS  = 500,
  GYRO_RANGE_2000DPS = 2000
} gyroRange_t;

#endif /* GYROSCOPE_H_ */
