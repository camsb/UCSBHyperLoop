//Hyperloop Peripheral Management System
//Kevin Kha

//#define ARDUINO
#define LPC


//Includes and Libraries
#include "math.h"

#ifdef ARDUINO //ARDUINO LIBRARIES BELOW
#include "Arduino.h"
#include "Wire.h"

#endif //ARDUINO

#ifdef LPC //LPC LIBRARIES BELOW
#include "stdlib.h"
#include "initialization.h"
#include "i2c.h"
#include "timer.h"
#include "ranging.h"

//I2C Bus Select

#endif //LPC


#ifndef I2CPERIPHS_H_
#define I2CPERIPHS_H_

//Number of Boards
#define NUM_HUBS 3
#define NUM_HUB_PORTS 4
#define NUM_HEMS 4
#define NUM_MAGLEV_BMS 2
#define NUM_ELECTRONICS_BMS 1
#define NUM_18V5_BMS 2

//Thermistor Data
#define REFERENCE_RESISTANCE 5100 //[ohms]
#define THERMISTOR_BETA 3380
#define THERMISTOR_OFFSET -2.126

//Ammeter Data
#define AMMETER_10A_SENSITIVITY 264
#define AMMETER_50A_SENSITIVITY 40
#define AMMETER_150A_SENSITIVITY 8.8		//[mV/A] for the 150amp version of the sensor

#define NUM_SHORTIR 2

//Tachometer Data
#define TACHOMETER_TICKS 1	// Number of reflective strips on the motor.

//Safety:
#define HEMS_MAX_TEMP 60      //Too hot
#define HEMS_MIN_TEMP 5       //More to detect disconnects than for "too cold"
#define HEMS_MAX_CURRENT 50   //

#define BATT_MAX_TEMP 60      //Too hot
#define BATT_MIN_TEMP 5

//Averaging:
#define TACHOMETER_AVG_WEIGHT 0.2 //Out of 1 (value = (old_value * AVG_WEIGHT + (1 - AVG_WEIGHT) * new_value) Set to 0 if you don't want exponential averaging.
#define THERMISTOR_AVG_WEIGHT 0.4 //Out of 1 (value = (old_value * AVG_WEIGHT + (1 - AVG_WEIGHT) * new_value)

#define MAX12BITVAL 4095.0

typedef struct {
  uint8_t identity;
  
  //I2C Parameters
  uint8_t bus;                    //Which I2C bus
  uint8_t ADC_device_address[1];   //ADC LTC2309 - Thermistors, Ammeter
  uint8_t DAC_device_address[1];   //DAC MCP4725 - Throttle
  uint8_t IOX_device_address[2];   //IOX MCP23017 - Tachometer    {MAGLEV, NAVIGATION}
  uint8_t BC_RESET_pin;
  uint8_t PWR_RESET_pin;
  uint8_t dI2C_READY_pin;

  //Data Storage
  float DAC_diagnostic;
  int temperatures[4];
  float short_data[NUM_SHORTIR];
  uint8_t amps;
  uint16_t rpm[2];

  //Helper Data
  float timestamp;
  uint16_t tachometer_counter[2];

  //Control
  float throttle_voltage;
  uint8_t bc_reset;
  uint8_t pwr_reset;

  //Safety
  uint8_t alarm;
} HEMS;

HEMS* initialize_HEMS(uint8_t identity);  //See below for I2C DIP addressing
uint8_t update_HEMS(HEMS* engine);
int calculate_temperature(uint16_t therm_adc_val);
float runtime();

/*HEMS I2C Parameters
  Device Addressing (7-bit addressing):
  ADC LTC2309: 0 ...						          -0??10??-	  //Tri-state inputs A0 and A1, however we'll not use float (don't need that many addresses)
  IOX MCP23017: 0 1 0 0 A2 A1 A0			    -0100???-   //Three two-state inputs A0, A1, and A2.
  DAC MCP4725: 1 1 0 0 A2(0) A1(1) A0		  -110001?-   //Three two-state inputs, A0, A1, and A2, BUT A2 and A1 are internal hardware pins; they are set when manufactured.
       		   _________ADC LTC2309 (Thermistors, Ammeter)
       		   ||_____DAC MCP4725 (Throttle)
      		   |||____IOX MCP23017 (Tachometer 0 & 1); Both share 2/3 address bits, with the third bit hardwired to 0 (for tach0) and 1 (for tach1)
     		     vvvvv
  I2C_DIP: 0b?????XXX   //X = don't cares; can be anything. They're not connected.
*/


typedef struct{   //Designed for 3x 6S batteries; 
  uint8_t identity;

  //I2C Parameters
  uint8_t bus;                //Only one allowed per bus, since addresses are hard-wired.
  
  //Data Storage
  float battery_voltage[3];   //From left to right on the board.
  float cell_voltages[3][6];
  int temperatures[3][2];
  uint8_t amps;               //No onboard ammeter; relies on data from HEMS or other.

  //Controls
  uint8_t relay_pin;
  uint8_t relay_active_low;       //Active Low

  float timestamp;
  uint8_t alarm;
} Maglev_BMS;

Maglev_BMS* initialize_Maglev_BMS(uint8_t identity);
uint8_t update_Maglev_BMS(Maglev_BMS* bms);

/*ADC LTC2309
   Max I2C Clock Frequency: 400kHz
  I2C Protocol:
	1. Master Write: Device Address (with Write Bit)
	2. Master Write: DIN (Input Data Word)
	3. Master Write: Device Address (with Read Bit)
	4. Master Read: 2 Bytes
  Input Data Word (6-bit)
  S/D | O/S | S1 | S0 | UNI | SLP | X | X
  S/D = Single-Ended/_Differential (We'll always use Single-Ended here, so = 1)
  O/S = Odd/_Sign (Used to select which channel)
  S1 = Channel Select Bit 1 (Also used to select which channel)
  S2 = Channel Select Bit 0 (Also used to select which channel)
  UNI = Unipolar/_Bipolar (We'll always use Unipolar, so = 1)
  SLP = Sleep Mode (We won't put this into sleep mode, so = 0)
*/
#define ADC_CONFIG 0x88		//0b1???10XX

// Single-Ended Channel Configuration
#define LTC2309_CHN_0	0x80
#define LTC2309_CHN_1	0xC0
#define LTC2309_CHN_2	0x90
#define LTC2309_CHN_3	0xD0
#define LTC2309_CHN_4	0xA0
#define LTC2309_CHN_5	0xE0
#define LTC2309_CHN_6	0xB0
#define LTC2309_CHN_7	0xF0

//ADC Associated Functions:
uint16_t ADC_read(uint8_t i2c_bus, uint8_t ADC_address, uint8_t ADC_channel);



/*DAC MCP4725
   I2C Frequencies: 100kHz, 400kHz, 3.4MHz
  The MCP4725 is a 12-bit single-channel DAC controlled via I2C. It has one external address bit (so only 2 devices can be used on the same bus.)
  I2C Protocol (Fast Mode; check Datasheet for details):
	1. Write Device Address
	2. 4-bit Configuration: 0000
	3. 12-bit Output Data:
  DAC Configuration (4-bit)
  C2 | C1 | PD1 | PD0
  C2 = 0	Command mode bit 2 (0, 0) = Fast Command Mode
  C1 = 0	Command mode bit 1 (0, 0) = Fast Command Mode
  PD1 = 0 Pulldown Resistor (we don't want any)
  PD0 = 0 Pulldown Resistor (we don't want any)
  Since the configuration is all 0s, writing to the DAC is simply writing the 12-bit output we want.
  There are other modes that allow for greater configuration (like writing to the EEPROM so that the DAC remembers the output after power-off.)
*/

#define DAC_CONFIG 0x00 //0b00000000

#define DAC_MIN 0.00
#define DAC_MAX 5.00

//DAC Associated Functions:
void DAC_write(uint8_t i2c_bus, uint8_t DAC_address, uint16_t output_voltage);



/*IOX MCP23017
   I2C Frequencies: 100kHz, 400kHz, 1.7MHz
  The MCP23017 is a 16-bit IO Expander controlled via I2C; the device is split into two 8-bit ports.
  The device is configured by a shared configuration register, IOCON, which consists of a byte. The settings are as follows:
  IOX Configuration Byte (8-bit):
  BANK | MIRROR | SEQOP | DISSLW | HAEN | ODR | INTPOL | NC 				POR (Power-On Reset) : 00000000
  BANK = Sets the addressing of the registers of each port. (We want them to alternate, so = 0)
  MIRROR = Sets whether the Interrupt outputs are OR'd together. (We want this active for the BMS, so = 1) Does not affect tachometer
  SEQOP = Sets whether addresses increment. (We want to operate in Byte mode, not Sequential, so = 1)
  DISSLW = Sets slew rate function
  HAEN = Don't care; controls hardware addressing on the 23S17
  ODR = ?
  INTPOL = ?
*/
#define IOX_CONFIG 0x42//0b01110010

//Banked Addresses (IOCON.BANK = 0) This is also the default register mapping.
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14

#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

//IOX Associated Functions:
void IOX_setup(uint8_t i2c_bus, uint8_t IOX_address);
uint16_t IOX_read(uint8_t i2c_bus, uint8_t IOX_address);

#endif //I2CPERIPHS_H
