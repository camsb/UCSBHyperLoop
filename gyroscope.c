
#include "gyroscope.h"

gyroAccelXYZ calculateRotAcc( int16_t* uncalcRotAcceleration )
{
  gyroAccelXYZ rotAcceleration;

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  rotAcceleration.x = (uncalcRotAcceleration[0] | (uncalcRotAcceleration[1] << 8));
  rotAcceleration.y = (uncalcRotAcceleration[2] | (uncalcRotAcceleration[3] << 8));
  rotAcceleration.z = (uncalcRotAcceleration[4] | (uncalcRotAcceleration[5] << 8));

  /* NOTE: If using 500 or 2000 DPS, make sure to change this */
  rotAcceleration.x = rotAcceleration.x * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration.y = rotAcceleration.y * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration.z = rotAcceleration.z * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;

  return rotAcceleration;
}

gyroAccelXYZ getGyroscopeData()
{
  //Sample code has options for controlling resolution.

  uint8_t     wBuffer[ 2 ];
  uint8_t     rBuffer[ 6 ];

  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG1; // Control register initializes all
  wBuffer[ 1 ] = 0x00;
  Chip_I2C_MasterSend( i2cDev, GYRO_ADDRESS, wBuffer, 2 );

  delay( 1 );
  wBuffer[ 1 ] = 0x0F;
  Chip_I2C_MasterSend( i2cDev, GYRO_ADDRESS, wBuffer, 2 );

  // adjustments for resolution
  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG4; // Control register initializes all
  wBuffer[ 1 ] = 0x00; // range 250  DPS
//  wBuffer[ 1 ] = 0x10; // range 500  DPS
//  wBuffer[ 1 ] = 0x20; // range 2000 DPS
  Chip_I2C_MasterSend( i2cDev, GYRO_ADDRESS, wBuffer, 2 );

  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG5; // Control register initializes all
  wBuffer[ 1 ] = 0x80; // range 250  DPS

  Chip_I2C_MasterSend( i2cDev, GYRO_ADDRESS, wBuffer, 2 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_X_L, rBuffer, 6);

  return calculateRotAcc( (int16_t *) rBuffer);
}
