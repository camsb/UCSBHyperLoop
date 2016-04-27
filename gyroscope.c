#include "gyroscope.h"
#include "time.h"
#include "i2c.h"
#include "board.h"

gyroAccelXYZ getGyroscopeData(){
  //Sample code has options for controlling resolution.

  uint8_t     wBuffer[ 2 ];
  uint8_t     rBuffer[ 6 ];
  gyroAccelXYZ rotAcceleration;

  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG1; // Control register initializes all
  wBuffer[ 1 ] = 0x00;
  Chip_I2C_MasterSend( I2C0, GYRO_ADDRESS, wBuffer, 2 );

  delay( 1 );
  wBuffer[ 1 ] = 0x0F;
  Chip_I2C_MasterSend( I2C0, GYRO_ADDRESS, wBuffer, 2 );

  // adjustments for resolution
  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG4; // Control register initializes all
  wBuffer[ 1 ] = 0x00; // range 250  DPS
//  wBuffer[ 1 ] = 0x10; // range 500  DPS
//  wBuffer[ 1 ] = 0x20; // range 2000 DPS
  Chip_I2C_MasterSend( I2C0, GYRO_ADDRESS, wBuffer, 2 );

  wBuffer[ 0 ] = GYRO_REGISTER_CTRL_REG5; // Control register initializes all
  wBuffer[ 1 ] = 0x80; // range 250  DPS

  Chip_I2C_MasterSend( I2C0, GYRO_ADDRESS, wBuffer, 2 );
  Chip_I2C_MasterCmdRead( I2C0, GYRO_ADDRESS, GYRO_REGISTER_OUT_X_L, rBuffer, 6);

  // Not sure what shifting right by 4 bits does, we may want to take this out for increased
  // accuracy
  rotAcceleration.x = (rBuffer[0] | (rBuffer[1] << 8));
  rotAcceleration.y = (rBuffer[2] | (rBuffer[3] << 8));
  rotAcceleration.z = (rBuffer[4] | (rBuffer[5] << 8));

  /* NOTE: If using 500 or 2000 DPS, make sure to change this */
  rotAcceleration.x = rotAcceleration.x * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration.y = rotAcceleration.y * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;
  rotAcceleration.z = rotAcceleration.z * GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS;

  return rotAcceleration;

}
