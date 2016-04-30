#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include "sensor_data.h"

#define ACC_ADDRESS      0x19
#define MAG_ADDRESS      0x1E
#define SENSORS_GRAVITY_STANDARD        (9.80665F)  /**< Earth's gravity in m/s^2 */

#define LSM303_REGISTER_ACCEL_CTRL_REG1_A          0x20
#define LSM303_REGISTER_ACCEL_CTRL_REG2_A          0x21
#define LSM303_REGISTER_ACCEL_CTRL_REG3_A          0x22
#define LSM303_REGISTER_ACCEL_CTRL_REG4_A          0x23
#define LSM303_REGISTER_ACCEL_CTRL_REG5_A          0x24
#define LSM303_REGISTER_ACCEL_CTRL_REG6_A          0x25

#define LSM303_REGISTER_ACCEL_OUT_X_L_A            0x28
#define LSM303_REGISTER_ACCEL_OUT_X_H_A            0x29
#define LSM303_REGISTER_ACCEL_OUT_Y_L_A            0x2A
#define LSM303_REGISTER_ACCEL_OUT_Y_H_A            0x2B
#define LSM303_REGISTER_ACCEL_OUT_Z_L_A            0x2C
#define LSM303_REGISTER_ACCEL_OUT_Z_H_A            0x2D

gyroAccelXYZ getAccelerometerData();

#endif /* ACCELEROMETER_H_ */
