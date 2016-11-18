#ifndef BATTERY_H_
#define BATTERY_H_

#include "i2c.h"
#include "board.h"
#include "sensor_data.h"

////////////////////////////////////////////////////////////
// defined macros
#define ADC_GAIN_MIN    	365			// in uV
#define ADC_GAIN1_MASK		0x0C		// 0b 0000 1100
#define ADC_GAIN2_MASK		0xE0		// 0b 1110 0000
#define BATT_ADDRESS    	0x08
#define BATT_GPIO_PIN		11
#define BATT_GPIO_PORT		2
#define BATT_IRQ_HANDLER	GPIO_IRQHandler
#define BATT_I2C			I2C0
#define BATT_NVIC			GPIO_IRQn
#define DELAY_OCD			0x0			// 0x0 = 8ms, 0x1 = 20ms, 0x2 = 40ms, 0x3 = 80ms, 0x4 = 160ms, 0x5 = 320ms, 0x6 = 640ms, 0x7 = 1280ms
#define DELAY_OV			0x0			// 0x0 = 1s, 0x1 = 2s, 0x2 = 4s, 0x3 = 8s
#define DELAY_UV			0x0			// 0x0 = 1s, 0x1 = 4s, 0x2 = 8s, 0x3 = 16s
#define DELAY_SCD			0x0			// 0x0 = 70us, 0x1 = 100us, 0x2 = 200us, 0x4 = 400us
#define MASK_6_BIT			0x3F  		// 0b 0011 1111
#define mV_TO_VOLTS			.001		// 1/1000
#define uV_TO_VOLTS			.000001		// 1/1000000
#define VOLTAGE_MAX			4.5			// Over Voltage Limit
#define VOLTAGE_MIN			3.1			// Under Voltage Limit

//////////////////////////////////////////////////////////////
// Alert macros used for interrupts
#define ALERT_OCD			0x01
#define ALERT_SCD			0x02
#define ALERT_OV			0x04
#define ALERT_UV			0x08
#define OVRD_ALERT			0x10
#define DEVICE_XREADY		0x20
#define ALERT_CC			0x80

//////////////////////////////////////////////////////////////
// Bit masks for register values
// located in REG_SYS_CTRL1
//#define SHUT_B				0x01
//#define SHUT_A				0x02
//#define TEMP_SEL			0x08
#define ADC_EN				0x10
#define LOAD_PRESENT		0x80

// located in REG_SYS_CTRL2
#define CHG_ON				0x01
#define DSG_ON				0x02
#define CC_ONESHOT			0x20
#define CC_EN				0x40
//#define DELAY_DIS			0x80

////////////////////////////////////////////////////////////
// Registers macros
// only the ones that are uncommented are used at the moment.
#define REG_SYS_STAT        0x00
#define REG_SYS_CTRL1       0x04
#define REG_SYS_CTRL2       0x05
#define REG_PROTECT1        0x06
#define REG_PROTECT2        0x07
#define REG_PROTECT3        0x08
#define REG_OV_TRIP         0x09
#define REG_UV_TRIP         0x0A
//#define REG_CC_CFG          0x0B
#define REG_VC1_HI          0x0C
//#define REG_VC1_LO          0x0D	// These are not used because we can
//#define REG_VC2_HI          0x0E	// read 10 registers in one command
//#define REG_VC2_LO          0x0F	// because they are sequential reads.
//#define REG_VC3_HI          0x10
//#define REG_VC3_LO          0x11	// May need to use if we need to specifically
//#define REG_VC4_HI          0x12	// use for balancing batteries.
//#define REG_VC4_LO          0x13
//#define REG_VC5_HI          0x14
//#define REG_VC5_LO          0x15
//#define REG_BAT_HI          0x2A
//#define REG_BAT_LO          0x2B	// again not used, sequential reads.
//#define REG_TS1_HI          0x2C
//#define REG_TS1_LO	      0x2D
//#define REG_CC_HI			  0x32
//#define REG_CC_LO			  0x33
#define REG_ADCGAIN1		0x50
#define REG_ADCOFFSET		0x51
#define REG_ADCGAIN2		0x59

////////////////////////////////////////////////////////////
// struct definitions
typedef struct {
  float VC1;
  float VC2;
  float VC3;
  float VC4;
  float VC5;
  
  float GAIN;
  float OFFSET;
} batteries;

////////////////////////////////////////////////////////////
// globals defined
batteries battery;
uint8_t batteryFlag;

////////////////////////////////////////////////////////////
// Function declarations
float   ADCConversion14Bit( uint8_t msb, uint8_t lsb );
void 	batteryInit();
void    disableMask( uint8_t mask, uint8_t reg );
void    enableMask( uint8_t mask, uint8_t reg );
void 	getBatteryData();
float 	getGain();
float 	getOffset();
uint8_t getVoltageLimit( float v );
void    printRegValues();
void 	processBatteryInterrupt();
void 	resetRegister( uint8_t reg );
void 	setProtectDelayTimes();
void 	setVoltageLimit( float limit, uint8_t reg );

uint32_t    getBMSDataValue( I2C_ID_T id, uint8_t * writeBuf, uint8_t * readBuf, uint8_t recvLen, uint8_t sendLen, uint8_t periph );
uint16_t    readBMSRegs( I2C_ID_T id, uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
void        testBMS();

#endif /* BATTERY_H_ */
