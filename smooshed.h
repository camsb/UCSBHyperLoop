#ifndef SMOOSHED_H_
#define SMOOSHED_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* EEPROM SLAVE data */
#define I2C_SLAVE_EEPROM_SIZE       64
#define I2C_SLAVE_EEPROM_ADDR       0x5A
#define I2C_SLAVE_IOX_ADDR          0x5B

/* Data area for slave operations */
static uint8_t seep_data[I2C_SLAVE_EEPROM_SIZE + 1];
static uint8_t buffer[2][256];
static uint8_t iox_data[2]; /* PORT0 input port, PORT1 Output port */
static I2C_XFER_T iox_xfer;

static int mode_poll;   /* Poll/Interrupt mode flag */
static I2C_ID_T i2cDev = DEFAULT_I2C; /* Currently active I2C device */


/////////////////////////////////////////////
// Main.c
void    delay( uint32_t ms );
void    i2cInit();
void    timer0DeInit();
void    timer0Init();

void      I2C0_IRQHandler(void);
void    I2C1_IRQHandler(void);
void    TIMER0_IRQHandler(void);

extern volatile uint32_t  NumOfMS;
extern uint8_t        SampleSetting;
extern volatile uint8_t   Timer0Running;
extern volatile uint32_t  Timer0Count;

typedef struct constants {
  int16_t  AC1;
  int16_t  AC2;
  int16_t  AC3;
  uint16_t AC4;
  uint16_t AC5;
  uint16_t AC6;
  int16_t  B1;
  int16_t  B2;
  int16_t  MB;
  int16_t  MC;
  int16_t  MD;

  int32_t  B5;
};

typedef enum
{                                                     // DEFAULT    TYPE
  LSM303_REGISTER_ACCEL_CTRL_REG1_A         = 0x20,   // 00000111   rw
  LSM303_REGISTER_ACCEL_CTRL_REG2_A         = 0x21,   // 00000000   rw
  LSM303_REGISTER_ACCEL_CTRL_REG3_A         = 0x22,   // 00000000   rw
  LSM303_REGISTER_ACCEL_CTRL_REG4_A         = 0x23,   // 00000000   rw
  LSM303_REGISTER_ACCEL_CTRL_REG5_A         = 0x24,   // 00000000   rw
  LSM303_REGISTER_ACCEL_CTRL_REG6_A         = 0x25,   // 00000000   rw
  LSM303_REGISTER_ACCEL_REFERENCE_A         = 0x26,   // 00000000   r
  LSM303_REGISTER_ACCEL_STATUS_REG_A        = 0x27,   // 00000000   r
  LSM303_REGISTER_ACCEL_OUT_X_L_A           = 0x28,
  LSM303_REGISTER_ACCEL_OUT_X_H_A           = 0x29,
  LSM303_REGISTER_ACCEL_OUT_Y_L_A           = 0x2A,
  LSM303_REGISTER_ACCEL_OUT_Y_H_A           = 0x2B,
  LSM303_REGISTER_ACCEL_OUT_Z_L_A           = 0x2C,
  LSM303_REGISTER_ACCEL_OUT_Z_H_A           = 0x2D,
  LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A     = 0x2E,
  LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A      = 0x2F,
  LSM303_REGISTER_ACCEL_INT1_CFG_A          = 0x30,
  LSM303_REGISTER_ACCEL_INT1_SOURCE_A       = 0x31,
  LSM303_REGISTER_ACCEL_INT1_THS_A          = 0x32,
  LSM303_REGISTER_ACCEL_INT1_DURATION_A     = 0x33,
  LSM303_REGISTER_ACCEL_INT2_CFG_A          = 0x34,
  LSM303_REGISTER_ACCEL_INT2_SOURCE_A       = 0x35,
  LSM303_REGISTER_ACCEL_INT2_THS_A          = 0x36,
  LSM303_REGISTER_ACCEL_INT2_DURATION_A     = 0x37,
  LSM303_REGISTER_ACCEL_CLICK_CFG_A         = 0x38,
  LSM303_REGISTER_ACCEL_CLICK_SRC_A         = 0x39,
  LSM303_REGISTER_ACCEL_CLICK_THS_A         = 0x3A,
  LSM303_REGISTER_ACCEL_TIME_LIMIT_A        = 0x3B,
  LSM303_REGISTER_ACCEL_TIME_LATENCY_A      = 0x3C,
  LSM303_REGISTER_ACCEL_TIME_WINDOW_A       = 0x3D
} lsm303AccelRegisters_t;

typedef enum
{                                             // DEFAULT    TYPE
  GYRO_REGISTER_WHO_AM_I            = 0x0F,   // 11010100   r
  GYRO_REGISTER_CTRL_REG1           = 0x20,   // 00000111   rw
  GYRO_REGISTER_CTRL_REG2           = 0x21,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG3           = 0x22,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG4           = 0x23,   // 00000000   rw
  GYRO_REGISTER_CTRL_REG5           = 0x24,   // 00000000   rw
  GYRO_REGISTER_REFERENCE           = 0x25,   // 00000000   rw
  GYRO_REGISTER_OUT_TEMP            = 0x26,   //            r
  GYRO_REGISTER_STATUS_REG          = 0x27,   //            r
  GYRO_REGISTER_OUT_X_L             = 0x28,   //            r
  GYRO_REGISTER_OUT_X_H             = 0x29,   //            r
  GYRO_REGISTER_OUT_Y_L             = 0x2A,   //            r
  GYRO_REGISTER_OUT_Y_H             = 0x2B,   //            r
  GYRO_REGISTER_OUT_Z_L             = 0x2C,   //            r
  GYRO_REGISTER_OUT_Z_H             = 0x2D,   //            r
  GYRO_REGISTER_FIFO_CTRL_REG       = 0x2E,   // 00000000   rw
  GYRO_REGISTER_FIFO_SRC_REG        = 0x2F,   //            r
  GYRO_REGISTER_INT1_CFG            = 0x30,   // 00000000   rw
  GYRO_REGISTER_INT1_SRC            = 0x31,   //            r
  GYRO_REGISTER_TSH_XH              = 0x32,   // 00000000   rw
  GYRO_REGISTER_TSH_XL              = 0x33,   // 00000000   rw
  GYRO_REGISTER_TSH_YH              = 0x34,   // 00000000   rw
  GYRO_REGISTER_TSH_YL              = 0x35,   // 00000000   rw
  GYRO_REGISTER_TSH_ZH              = 0x36,   // 00000000   rw
  GYRO_REGISTER_TSH_ZL              = 0x37,   // 00000000   rw
  GYRO_REGISTER_INT1_DURATION       = 0x38    // 00000000   rw
} gyroRegisters_t;

typedef enum
{
  GYRO_RANGE_250DPS  = 250,
  GYRO_RANGE_500DPS  = 500,
  GYRO_RANGE_2000DPS = 2000
} gyroRange_t;

/////////////////////////////////////////////
// Acceleration.c
float*    calculateAcceleration( uint8_t* uncalcAcceleration );
float*    getAcceleration();

/////////////////////////////////////////////
// Pressure.c

uint32_t  calculatePressure( struct constants *c, uint32_t uncalcPres );
uint32_t  calculateTemperature( struct constants *c, uint32_t uncalcTemp );
//uint32_t  getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len); // Original declaration.
uint32_t  getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len, uint8_t periph );
uint32_t  getTemperature( struct constants *c );
uint32_t  getPressure( struct constants *c );
void    initCalibrationData( struct constants *c );
void    printConstants( struct constants *c );
uint16_t  readRegs( uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
//void    setCalibrationData( struct constants *c );

/////////////////////////////////////////////
// Gyro.c
float*    getRotAcceleration();
float*    calculateRotAcc( uint8_t* uncalcRotAcc );

#endif /* SMOOSHED_H_ */
