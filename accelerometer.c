/*
 * Acceleration.c
 */

#include "accelerometer.h"

float* calculateAcceleration( uint8_t* uncalcAcceleration )
{
  static int16_t concAcceleration[3];
  static float acceleration[3];
  static float LSM303ACCEL_MG_LSB = (0.00094F);   // 1, 2, 4 or 12 mg per lsb
  //acceleration = uncalcAcceleration;

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  concAcceleration[0] = (int16_t)(uncalcAcceleration[0] | (uncalcAcceleration[1] << 8)) >> 4;
  concAcceleration[1] = (int16_t)(uncalcAcceleration[2] | (uncalcAcceleration[3] << 8)) >> 4;
  concAcceleration[2] = (int16_t)(uncalcAcceleration[4] | (uncalcAcceleration[5] << 8)) >> 4;

  acceleration[0] = concAcceleration[0] * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration[1] = concAcceleration[1] * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
  acceleration[2] = concAcceleration[2] * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;

  DEBUGOUT( "Calculated Acceleration:\n" );
  DEBUGOUT( "X: %f, Y: %f, Z: %f\n",
      acceleration[0], acceleration[1], acceleration[2]);

  return acceleration;
}

float* getAcceleration()
{
  //Sample code has options for controlling resolution.

  uint8_t     wBuffer[ 2 ];
  static uint8_t  rBuffer[ 6 ];
  float*      calcAcceleration;

  wBuffer[ 0 ] = ( LSM303_REGISTER_ACCEL_CTRL_REG1_A ); // Control register initializes all
  wBuffer[ 1 ] = 0x57;

  Chip_I2C_MasterSend( i2cDev, ACC_ADDRESS, wBuffer, 2 );
  Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80, rBuffer, 6 );

  calcAcceleration = calculateAcceleration( (uint8_t *)rBuffer);
  return (float *)calcAcceleration;//calculateAcceleration( c, uncalcAcceleration);
}
