#ifndef BATTERY_H_
#define BATTERY_H_

#include "i2c.h"
#include "board.h"
#include "sensor_data.h"

#define BATT_NVIC			GPIO_IRQn
#define BATT_IRQ_HANDLER	GPIO_IRQHandler

#define BATT_ADDRESS    0x08
#define BATT_I2C		I2C0
#define BATT_GPIO_PORT	2
#define BATT_GPIO_PIN	11
#define MASK_6_BIT		0x3F  		// 0b 0011 1111
#define MASK_8_BIT		0xFF  		// 0b 1111 1111
#define ADC_EN_BIT		0x10		// 0b 0001 0000
#define ADC_GAIN_MIN    365			// in uV
#define ADC_GAIN1_MASK	0x0C		// 0b 0000 1100
#define ADC_GAIN2_MASK	0xE0		// 0b 1110 0000
#define mV_TO_VOLTS		.001		// 1/1000
#define uV_TO_VOLTS		.000001		// 1/1000000
#define VOLTAGE_MAX		4.5			// Over Voltage Threshold
#define VOLTAGE_MIN		3.1			// Under Voltage Threshold

#define SYS_STAT        0x00
#define SYS_CTRL1       0x04
#define SYS_CTRL2       0x05
#define PROTECT1        0x06
#define PROTECT2        0x07
#define PROTECT3        0x08
#define OV_TRIP         0x09
#define UV_TRIP         0x0A
#define CC_CFG          0x0B
#define VC1_HI          0x0C
//#define VC1_LO          0x0D	// These are not used because we can
//#define VC2_HI          0x0E	// read 10 registers in one command
//#define VC2_LO          0x0F	// because they are sequential reads.
//#define VC3_HI          0x10
//#define VC3_LO          0x11	// May need to use if we need to specifically
//#define VC4_HI          0x12	// use for balancing batteries.
//#define VC4_LO          0x13
//#define VC5_HI          0x14
//#define VC5_LO          0x15
#define BAT_HI          0x2A
//#define BAT_LO          0x2B	// again not used, sequential reads.
#define TS1_HI          0x2C
#define TS1_LO	        0x2D
#define CC_HI			0x32
#define CC_LO			0x33
#define ADCGAIN1		0x50
#define ADCOFFSET		0x51
#define ADCGAIN2		0x59

typedef struct {
  float VC1;
  float VC2;
  float VC3;
  float VC4;
  float VC5;
  
  float GAIN;
  float OFFSET;
  float BATSUM;
} batteries;

batteries battery; // global, may want to store in sensor_data.h
uint8_t batteryFlag;

// void 	BATT_IRQ_HANDLER(void);
void  	batteryInit();
float 	convertBits14or16( uint8_t msb, uint8_t lsb, uint8_t mask );
void  	getBatteryData();
float 	getGain();
float 	getOffset();
uint8_t getVoltageThreshold( float v );

#endif /* BATTERY_H_ */
