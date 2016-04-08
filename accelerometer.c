/*
 * Acceleration.c
 */

#include "accelerometer.h"

gyroAccelXYZ calculateAcceleration( int16_t* uncalcAcceleration )
{

  gyroAccelXYZ acceleration;
  float LSM303ACCEL_MG_LSB = (0.00094F); 

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  acceleration.x = (uncalcAcceleration[0] | (uncalcAcceleration[1] << 8)) >> 4;
  acceleration.y = (uncalcAcceleration[2] | (uncalcAcceleration[3] << 8)) >> 4;
  acceleration.z = (uncalcAcceleration[4] | (uncalcAcceleration[5] << 8)) >> 4;

  acceleration.x = acceleration.x * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration.y = acceleration.y * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration.z = acceleration.z * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;

  return acceleration;
}

gyroAccelXYZ getAccelerometerData()
{
  //Sample code has options for controlling resolution.
  uint8_t     wBuffer[ 2 ];
  uint8_t     rBuffer[ 6 ];

  wBuffer[ 0 ] = ( LSM303_REGISTER_ACCEL_CTRL_REG1_A ); // Control register initializes all
  wBuffer[ 1 ] = 0x57;

  Chip_I2C_MasterSend( i2cDev, ACC_ADDRESS, wBuffer, 2 );
  Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80, rBuffer, 6 );

  return calculateAcceleration( (int16_t *)rBuffer );
}
