//Hyperloop Hover Engine Monitoring System
//Kevin Kha

#include "HEMS.h"

// Global variables.
const uint8_t ADC_Address[4] = {0x8, 0xA, 0x1A, 0x28};
const uint8_t IOX_Address[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
const uint8_t DAC_Address[2] = {0x62, 0x63};
//To select the channel, we can OR ADC_CONFIG with channel selection bits.
const uint8_t ADC_CHANNEL_SELECT[8] = {
	LTC2309_CHN_0 | ADC_CONFIG,
	LTC2309_CHN_1 | ADC_CONFIG,
	LTC2309_CHN_2 | ADC_CONFIG,
	LTC2309_CHN_3 | ADC_CONFIG,
	LTC2309_CHN_4 | ADC_CONFIG,
	LTC2309_CHN_5 | ADC_CONFIG,
	LTC2309_CHN_6 | ADC_CONFIG,
	LTC2309_CHN_7 | ADC_CONFIG};

void initialize_HEMS(){
	#ifdef ARDUINO
	Wire.begin();

	#else //LPC code
	i2cInit(I2C0, SPEED_400KHZ);
	#endif //ARDUINO
	//return IOX_value;
}

uint16_t ADC_read(uint8_t ADCaddress, uint8_t ADCchannel){
	#ifdef ARDUINO
	Wire.beginTransmission(ADC_address);
    Wire.write(ADC_CHANNEL_SELECT[ADC_channel]);
    Wire.endTransmission(false);
    Wire.requestFrom(ADC_address, 2, true);
    uint16_t ADC_value = (Wire.read() << 4) | (Wire.read() >> 4);

	#else //LPC code
    DEBUGOUT("DAC_write\n");
    uint16_t ADC_value;
    Chip_I2C_MasterCmdRead(I2C0, ADCaddress, ADC_CHANNEL_SELECT[ADCchannel], &ADC_value, 2);
	#endif
	return ADC_value;
}


void DAC_write(uint8_t DACaddress, uint16_t output_voltage){
	#ifdef ARDUINO
	Wire.beginTransmission(DACaddress);
    Wire.write(DAC_CONFIG | (output_voltage >> 8));
    Wire.write(output_voltage % 1024);
    Wire.endTransmission(true);

    #else //LPC code
    DEBUGOUT("DAC_write\n");
    Chip_I2C_MasterSend(I2C0, DACaddress, &output_voltage, 2);
    #endif //ARDUINO
}
