//Hyperloop Hover Engine Monitoring System
//Kevin Kha

#include "HEMS.h"

// Global variables.
const uint8_t ADC_Address[4] = {0x8, 0xA, 0x1A, 0x28};
const uint8_t IOX_Address[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
const uint8_t DAC_Address[2] = {0x62, 0x63};

const uint8_t ADC_Address_Select[4] = {0x8, 0xA, 0x1A, 0x28};
const uint8_t DAC_Address_Select[2] = {0x62, 0x63};
const uint8_t IOX_Address_Select[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

//array to select
const I2C_ID_T I2C_ID_SELECT[] = {I2C0,I2C1,I2C2,I2C_NUM_INTERFACE};

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


HEMS* initialize_HEMS(uint8_t I2C_BUS, uint8_t I2C_DIP) {
  HEMS* engine = malloc(sizeof(HEMS));
  engine->bus = I2C_BUS;
  engine->ADC_0_device_address = ADC_Address_Select[(I2C_DIP >> 6) & 0b11];
  engine->DAC_0_device_address = DAC_Address_Select[(I2C_DIP >> 3) & 0b1];
  engine->IOX_0_device_address = IOX_Address_Select[(I2C_DIP >> 0) & 0b111];

  IOX_setup(I2C_BUS, engine->IOX_0_device_address);

  return engine;
}
uint16_t ADC_read(uint8_t bus, uint8_t ADC_address, uint8_t ADC_channel){
	#ifdef ARDUINO
	Wire.beginTransmission(ADC_address);
    Wire.write(ADC_CHANNEL_SELECT[ADC_channel]);
    Wire.endTransmission(false);
    Wire.requestFrom(ADC_address, 2, true);
    uint16_t ADC_value = (Wire.read() << 4) | (Wire.read() >> 4);

	#else //LPC code
    DEBUGOUT("DAC_write\n");
    uint16_t ADC_value;
    Chip_I2C_MasterCmdRead(bus, ADC_address, ADC_CHANNEL_SELECT[ADC_channel], &ADC_value, 2);
	#endif
	return ADC_value;
}

void update_HEMS(HEMS* engine) {
  //Set throttle;
  DAC_write(I2C_ID_SELECT[engine->bus],engine->DAC_0_device_address, map(engine->throttle_voltage, DAC_MIN, DAC_MAX, 0, 4095));

  //Record Temperatures
  int temp_counter;
  for (temp_counter = 0; temp_counter < NUM_THERMISTORS; temp_counter++) {
    uint16_t ratio = ADC_read(I2C_ID_SELECT[engine->bus],engine->ADC_0_device_address, temp_counter);
    uint16_t thermistance = (4095 - ratio) * REFERENCE_RESISTANCE / ratio;    //Calculate thermistor resistance
    int new_temperature = THERMISTOR_BETA / (log(thermistance) - THERMISTOR_OFFSET) - 272;
    engine->temperatures[temp_counter] = ((100 - THERMISTOR_AVG_WEIGHT) * new_temperature + THERMISTOR_AVG_WEIGHT * engine->temperatures[temp_counter]) / 100;

    if (new_temperature > SAFE_TEMPERATURE)
      engine->alarm |= 0b00000001;
  }

  //Record Motor Controller Current
  int new_amps = (ADC_read(I2C_ID_SELECT[engine->bus],engine->ADC_0_device_address, AMMETER_CHANNEL) * 5000.0 / 4095 - AMMETER_VCC / 2) * AMMETER_CONVERSION;
  engine->amps = new_amps;

  if (new_amps > SAFE_CURRENT)
    engine->alarm |= 0b00000010;

  //Record Motor RPM
  uint16_t current_tachometer_counter = IOX_read(I2C_ID_SELECT[engine->bus],engine->IOX_0_device_address);
  float current_time = runtime();
  uint16_t current_rpm = 60.0 * (current_tachometer_counter - engine->tachometer_counter) / (current_time - engine->timestamp) / TACHOMETER_TICKS;
  engine->rpm = ((100 - TACHOMETER_AVG_WEIGHT) * current_rpm + TACHOMETER_AVG_WEIGHT * engine->rpm) / 100;
  engine->timestamp = current_time;
  engine->tachometer_counter = current_tachometer_counter;
}

void IOX_setup(uint8_t bus, uint8_t IOX_address) {
#ifdef ARDUINO
  Wire.beginTransmission(IOX_address);
  Wire.write(MCP23017_IOCONA); //IOCON register location
  Wire.write(IOX_CONFIG);
  Wire.endTransmission(true);

#else // LPC
  uint8_t buffer_out[2] = {MCP23017_IOCONA, IOX_CONFIG};
  Chip_I2C_MasterSend(bus, IOX_address, buffer_out, 2);
#endif //ARDUINO
}

uint16_t IOX_read(uint8_t bus, uint8_t IOX_address) {
#ifdef ARDUINO
  Wire.beginTransmission(IOX_address);
  Wire.write(MCP23017_GPIOA); //GPIOAB register location
  Wire.endTransmission(false);
  Wire.requestFrom(IOX_address, 2, true);
  uint16_t IOX_value = (Wire.read() << 8) | Wire.read();	//A7 A6 A5 A4 A3 A2 A1 A0 B7 B6 B5 B4 B3 B2 B1 B0

#else // LPC
  uint16_t IOX_value;
  Chip_I2C_MasterCmdRead(I2C_ID_SELECT[bus], IOX_address, MCP23017_GPIOA, &IOX_value, 2);
#endif //ARDUINO
  return IOX_value;
}

float runtime() {
  float runtime_in_seconds;
#ifdef ARDUINO
  runtime_in_seconds = micros() / 1000000.0;

#else //LPC


#endif
  return runtime_in_seconds;
}

void DAC_write(uint8_t bus, uint8_t DAC_address, uint16_t output_voltage){
	uint8_t buffer_out[2] = {DAC_CONFIG | (output_voltage >> 8), output_voltage % 1024};
	#ifdef ARDUINO
	Wire.beginTransmission(DACaddress);
	 Wire.write(output_buffer, 2); //output the two bytes
    Wire.endTransmission(true);

    #else //LPC code
    DEBUGOUT("DAC_write\n");
    Chip_I2C_MasterSend(I2C_ID_SELECT[bus], DAC_address, buffer_out, 2);
    #endif //ARDUINO
}
