/*
 * Gyro.c
 *
 */

#include "gyroscope.h"

float* calculateRotAcc( uint8_t* uncalcRotAcceleration )
{
  static int16_t concRotAcceleration[3];
  static float rotAcceleration[3];
  //acceleration = uncalcAcceleration;

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  concRotAcceleration[0] = (int16_t)(uncalcRotAcceleration[0] | (uncalcRotAcceleration[1] << 8));
  concRotAcceleration[1] = (int16_t)(uncalcRotAcceleration[2] | (uncalcRotAcceleration[3] << 8));
  concRotAcceleration[2] = (int16_t)(uncalcRotAcceleration[4] | (uncalcRotAcceleration[5] << 8));

  /* NOTE: If using 500 or 2000 DPS, make sure to change this */
  rotAcceleration[0] = ((float)concRotAcceleration[0]) * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration[1] = ((float)concRotAcceleration[1]) * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration[2] = ((float)concRotAcceleration[2]) * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;

  DEBUGOUT( "Calculated Rotation:\n" );
  DEBUGOUT( "X: %f, Y: %f, Z: %f\n",
      rotAcceleration[0], rotAcceleration[1], rotAcceleration[2]);

  return rotAcceleration;
}

float* getRotAcceleration()
{
  //Sample code has options for controlling resolution.

  uint8_t     wBuffer[ 3 ];
  static uint8_t  rBuffer[ 6 ];
  float*      calcRotAcceleration;


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

  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_X_L, &rBuffer[0], 1 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_X_H, &rBuffer[1], 1 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_Y_L, &rBuffer[2], 1 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_Y_H, &rBuffer[3], 1 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_Z_L, &rBuffer[4], 1 );
  Chip_I2C_MasterCmdRead( i2cDev, GYRO_ADDRESS, GYRO_REGISTER_OUT_Z_H, &rBuffer[5], 1 );

  calcRotAcceleration = calculateRotAcc( (uint8_t *)rBuffer);
  return (float *)calcRotAcceleration;//calculateAcceleration( c, uncalcAcceleration);
}
