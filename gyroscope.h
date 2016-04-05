#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#define GYRO_ADDRESS     0x6B
#define GYRO_SENSITIVITY_250DPS  (0.00875F)    // Roughly 22/256 for fixed point match
#define GYRO_SENSITIVITY_500DPS  (0.0175F)     // Roughly 45/256
#define GYRO_SENSITIVITY_2000DPS (0.070F)      // Roughly 18/256
#define SENSORS_DPS_TO_RADS               (0.017453293F)          /**< Degrees/s to rad/s multiplier */

#endif /* GYROSCOPE_H_ */
