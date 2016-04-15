
#include "accelerometer.h"

gyroAccelXYZ getAccelerometerData()
{

  //Sample code has options for controlling resolution.
  uint8_t     wBuffer[ 2 ];
  uint8_t     rBuffer[ 6 ];
  int16_t rawAcceleration[3];
  gyroAccelXYZ acceleration;
  float LSM303ACCEL_MG_LSB = (0.00094F); 

  wBuffer[ 0 ] = ( LSM303_REGISTER_ACCEL_CTRL_REG1_A ); // Control register initializes all
  wBuffer[ 1 ] = 0x57;

  Chip_I2C_MasterSend( I2C0, ACC_ADDRESS, wBuffer, 2 );
  Chip_I2C_MasterCmdRead( I2C0, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80, rBuffer, 6 );

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  rawAcceleration[0] = (((uint16_t)rBuffer[0]) | (((uint16_t)rBuffer[1]) << 8)) >> 4;
  rawAcceleration[1] = (((uint16_t)rBuffer[2]) | (((uint16_t)rBuffer[3]) << 8)) >> 4;
  rawAcceleration[2] = (((uint16_t)rBuffer[4]) | (((uint16_t)rBuffer[5]) << 8)) >> 4;

  acceleration.x = ((float)rawAcceleration[0]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration.y = ((float)rawAcceleration[1]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration.z = ((float)rawAcceleration[2]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;

  return acceleration;

}
