#ifndef BATTERY_H_
#define BATTERY_H_

#include "i2c.h"
#include "board.h"
#include "sensor_data.h"

#define BATT_ADDRESS    0x08
#define ADC_14_BIT      .000382f  // 382 uV
#define ADC_16_BIT      .00153f   // 1.532 mV

#define SYS_STAT        0x00
#define SYS_CTRL1       0x04
#define SYS_CTRL1       0x05
#define PROTECT1        0x06
#define PROTECT2        0x07
#define PROTECT3        0x08
#define OV_TRIP         0x09
#define UV_TRIP         0x0A
#define CC_CFG          0x0B
#define VC1_HI          0x0C
#define VC1_LO          0x0D
#define VC2_HI          0x0E
#define VC2_LO          0x0F
#define VC3_HI          0x10
#define VC3_LO          0x11
#define VC4_HI          0x12
#define VC4_LO          0x13
#define VC5_HI          0x14
#define VC5_LO          0x15
#define BAT_HI          0x2A
#define BAT_LO          0x2B
#define TS1_HI          0x2C
#define TS2_HI          0x2D

// Each of the 5 battery cells and the sum of them all
typedef struct {
  float AC1;
  float AC2;
  float AC3;
  float AC4;
  float AC5;
  
  float BATSUM;
} batteries;

batteries battery;

#endif /* BATTERY_H_ */
