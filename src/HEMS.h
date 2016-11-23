//Hyperloop Hover Engine Monitoring System
//Kevin Kha
//#define ARDUINO
#ifdef ARDUINO
#include "Arduino.h"
#include "Wire.h"

#else //LPC Libraries below
#include "i2c.h"
#endif //ARDUINO

#include "math.h"

#ifndef HEMS_H_
#define HEMS_H_

#define NUM_THERMISTORS 4
#define REFERENCE_RESISTANCE 5100 //ohms
#define AMMETER_SENSITIVITY 26.4 //mV/A
#define AMMETER_CONVERSION 1/AMMETER_SENSITIVITY
#define AMMETER_VCC 3.3

//ADC Channel Assignments
#define COILS_FRONT 0
#define COILS_BACK 1
#define INTERIOR_WALL_RIGHT 2
#define INTERIOR_WALL_LEFT 3
#define AMMETER 4

//Global Constants:
#define TACHOMETER_AVG_WEIGHT 40 //Out of 100 (value = (old_value * AVG_WEIGHT + (100 - AVG_WEIGHT) * new_value)/100 Set to 0 if you don't want exponential averaging.
#define THERMISTOR_AVG_WEIGHT 40 //Out of 100 (value = (old_value * AVG_WEIGHT + (100 - AVG_WEIGHT) * new_value)/100
#define TACHOMETER_HANDLER_OVERHEAD_US 0 //approximate overhead time for

#define THERMISTOR_BETA 3380
#define THERMISTOR_OFFSET -2.126

void initialize_HEMS();


/*I2C Parameters
Device Addressing (7-bit addressing):
ADC LTC2309: 0 ...						-0??10??-	//Tri-state Inputs A0 and A1, however we'll not use float (don't need that many addresses)
IO MCP23017: 0 1 0 0 A2 A1 A0			-0100???-
DAC MCP4725: 1 1 0 0 A2(0) A1(1) A0		-110001?-   //A2 and A1 are internal hardware pins; they are set when manufactured.
		   _________ADC LTC2309 (Thermistors, Ammeter)
		   ||  _____IO MCP23017 (Tachometer)
		   ||  |||__DAC MCP4725 (Throttle)
		   vv  vvvv
I2C_DIP: 0b??XX????   //X = don't cares; can be anything. They're not connected.*/



typedef struct{
	uint8_t I2C_DIP;
	//ADC LTC2309 -
	uint8_t ADC_device_0;
	//IOX MCP23017 - Tachometer
	uint8_t IOX_device_0;
	//DAC MCP4725 - Throttle
	uint8_t DAC_device_0;
} Engine_Control_System;





/*ADC LTC2309
I2C Protocol:
	1. Master Write: Device Address (with Write Bit)
	2. Master Write: DIN (Input Data Word)
	3. Master Write: Device Address (with Read Bit)
	4. Master Read
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
uint16_t ADC_read(uint8_t ADCaddress, uint8_t ADCchannel);



/*IOX MCP23017
The MCP23017 is a 16-bit IO Expander controlled via I2C; the device is split into two 8-bit ports.
The device is configured by a shared configuration register, IOCON, which consists of a byte. The settings are as follows:
IOX Configuration Byte (8-bit):
BANK | MIRROR | SEQOP | DISSLW | HAEN | ODR | INTPOL | NC
BANK = Sets the addressing of the registers of each port. (We'll want them to be sequential, so = 1)
MIRROR = Sets whether the Interrupt outputs are OR'd together. (We want this active for the BMS, so = 1) Does not affect tachometer
SEQOP = Sets whether addresses increment. (We only ever want to read the GPIO which is taken care of by BANK, so = 1)
*/
#define IOX_CONFIG 0x62//0b1100010

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
void IOX_setup(uint8_t IOX_address);
uint16_t IOX_read(uint8_t IOX_address);



/*DAC MCP4725
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

//DAC Associated Functions:
void DAC_write(uint8_t DACaddress, uint16_t output_voltage);



#endif //HEMS_H
