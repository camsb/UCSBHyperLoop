//Hyperloop Peripheral Management System
//Kevin Kha

//For use with HEMSv3, Maglev BMS, I2C Hub
//If using this on the Arduino, this should be saved as a .cpp file. Otherwise it should be saved as a .c

//#include "I2CPERIPHS.h"
#include "HEMS.h"

//POD PIN MAPPING, CALIBRATION
//Hub Data:
const uint8_t HUB_I2C_BUS[NUM_HUBS] = {0, 1, 2};
const uint8_t HUB_AUX_PINS[NUM_HUBS][NUM_HUB_PORTS] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

//HEMS Data:
const uint8_t HEMS_HUB_PORT[NUM_HEMS][2] = {    //Max 2 per I2C bus or Hub
  {1, 2},   //{Hub, Port};
  {1, 3},
  {2, 2},
  {2, 3}
};
const uint8_t HEMS_I2C_DIP[NUM_HEMS] = {0,  255,  0,  255};
const float HEMS_CAL_5V0REF[NUM_HEMS] = {5.08,  5.08, 5.08, 5.08};
const float HEMS_CAL_3V3REF[NUM_HEMS] = {3.28,  3.28, 3.28, 3.30};

//Maglev_BMS Data:
const uint8_t MAGLEV_BMS_HUB_PORT[NUM_MAGLEV_BMS][2] = {   //Max 1 per I2C bus or Hub
  {1, 1},   //{Hub, Port};
  {2, 1}
};
const float MAGLEV_BMS_CAL_CONVERSIONS[NUM_MAGLEV_BMS][3][6] = {
  {   //BMS0
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99},   //SubBMS0
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99},   //SubBMS1
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99}    //SubBMS2
  },
  {   //BMS1
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99},   //SubBMS0
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99},   //SubBMS1
    {2.0, 2.0, 3.0, 4.0, 5.02, 5.99}    //SubBMS2
  }
};








// Global variables.
const uint8_t ADC_Address_Select[4] = {0x8, 0xA, 0x1A, 0x28};
const uint8_t DAC_Address_Select[2] = {0x62, 0x63};
const uint8_t IOX_Address_Select[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

//ADC Channel Selection Commands
//To select the channel, we can OR ADC_CONFIG with channel selection bits.
const uint8_t ADC_CHANNEL_SELECT[8] = {
  LTC2309_CHN_0 | ADC_CONFIG,
  LTC2309_CHN_1 | ADC_CONFIG,
  LTC2309_CHN_2 | ADC_CONFIG,
  LTC2309_CHN_3 | ADC_CONFIG,
  LTC2309_CHN_4 | ADC_CONFIG,
  LTC2309_CHN_5 | ADC_CONFIG,
  LTC2309_CHN_6 | ADC_CONFIG,
  LTC2309_CHN_7 | ADC_CONFIG
};

HEMS* initialize_HEMS(uint8_t identity) {
  HEMS* engine = malloc(sizeof(HEMS));
  engine->identity = identity;
  engine->bus = HUB_I2C_BUS[HEMS_HUB_PORT[engine->identity][0]];
  engine->ADC_device_address[0] = ADC_Address_Select[(HEMS_I2C_DIP[engine->identity] >> 6) & 0b11];
  engine->DAC_device_address[0] = DAC_Address_Select[(HEMS_I2C_DIP[engine->identity] >> 5) & 0b1];
  engine->IOX_device_address[0] = IOX_Address_Select[(HEMS_I2C_DIP[engine->identity] >> 2) & 0b110];
  engine->IOX_device_address[1] = IOX_Address_Select[((HEMS_I2C_DIP[engine->identity] >> 2) & 0b110) + 1];

  // Initialize tachometer counters and rpm data
  int n;
  for (n = 0; n < 2; n++) {
    IOX_setup(engine->bus, engine->IOX_device_address[n]);
    engine->tachometer_counter[n] = IOX_read(engine->bus, engine->IOX_device_address[n]);
    engine->rpm[n] = 0;
  }

  // Initialize thermistor moving averages (with first read)
  int temp_counter;
  for (temp_counter = 0; temp_counter < 4; temp_counter++) {
	  engine->temperatures[temp_counter] = calculate_temperature(ADC_read(engine->bus, engine->ADC_device_address[0], temp_counter + 1));
  }

#ifdef LPC
  // Initialize short ranging moving averages (with first read)
  int short_counter;
  for (short_counter = 0; short_counter < NUM_SHORTIR; short_counter++){
  	  float ShortRangingDataRaw = ADC_read(engine->bus, engine->ADC_device_address[0], short_counter + 5);
  	  float voltage = ((float)ShortRangingDataRaw) / 1300;

  		if (!((voltage < 0.34) || (voltage > 2.43))){
  			uint16_t index = (uint16_t)(voltage * 100.0 + 0.5) - 34;
  			engine->short_data[short_counter] = shortRangingDistanceLUT[index];
  		}
  }
#endif

  engine->amps = 0;
  engine->throttle_voltage = 0;
  engine->timestamp = 0;

  engine->alarm = 0;

  return engine;
}

uint8_t update_HEMS(HEMS* engine) {
  //Set throttle;
  DAC_write(engine->bus, engine->DAC_device_address[0], engine->throttle_voltage * MAX12BITVAL / HEMS_CAL_5V0REF[engine->identity]);
  engine->DAC_diagnostic = ADC_read(engine->bus, engine->ADC_device_address[0], 0) / MAX12BITVAL * HEMS_CAL_5V0REF[engine->identity];

  //Record Temperatures
  int temp_counter;
  for (temp_counter = 0; temp_counter < 4; temp_counter++) {
    int new_temperature = calculate_temperature(ADC_read(engine->bus, engine->ADC_device_address[0], temp_counter + 1));
    new_temperature = ((1 - THERMISTOR_AVG_WEIGHT) * new_temperature + THERMISTOR_AVG_WEIGHT * engine->temperatures[temp_counter]);
    engine->temperatures[temp_counter] = new_temperature;
    if (new_temperature > HEMS_MAX_TEMP || new_temperature < HEMS_MIN_TEMP)
      engine->alarm |= 0b00000001;
  }

#ifdef LPC
  // Record short ranging sensor data
  int short_counter;
  float ShortRangingMovingAverage;
	for (short_counter = 0; short_counter < NUM_SHORTIR; short_counter++){
	  ShortRangingMovingAverage = engine->short_data[short_counter];
	  float ShortRangingDataRaw = ADC_read(engine->bus, engine->ADC_device_address[0], short_counter + 5);
	  float voltage = ((float)ShortRangingDataRaw) / 1300;

		if (!((voltage < 0.34) || (voltage > 2.43))){
			uint16_t index = (uint16_t)(voltage * 100.0 + 0.5) - 34;
			ShortRangingMovingAverage = ALPHA*ShortRangingMovingAverage + BETA*shortRangingDistanceLUT[index];
		}
		engine->short_data[short_counter] = ShortRangingMovingAverage;
	}
#endif

  //Record Motor Controller Current
  //With no current, the ACS759x150B should output 3.3V/2
  uint16_t ammeter_ratio = ADC_read(engine->bus, engine->ADC_device_address[0], 7);
  uint8_t new_amps = abs(1000 * ammeter_ratio * HEMS_CAL_5V0REF[engine->identity] / MAX12BITVAL - 1000 * HEMS_CAL_3V3REF[engine->identity] / 2) / AMMETER_150A_SENSITIVITY; //Done in mV
  engine->amps = new_amps;

  if (new_amps > HEMS_MAX_CURRENT)
    engine->alarm |= 0b00000010;

  //Record RPMs
  uint16_t current_tachometer_counter[2];
  uint16_t previous_tachometer_counter[2];

  int n;
  float current_time[2];
  uint16_t current_rpm[2];
  for (n = 0; n < 2; n++) {
    current_tachometer_counter[n] = IOX_read(engine->bus, engine->IOX_device_address[n]) & 0x0FFF;
    previous_tachometer_counter[n] = engine->tachometer_counter[n];
    current_time[n] = runtime();

    uint16_t delta_counter; //Calculate # of pulses since last checked
    if (current_tachometer_counter[n] >= previous_tachometer_counter[n])
      delta_counter = current_tachometer_counter[n] - previous_tachometer_counter[n];
    else  //Account for edge case where the binary counter overflows and resets back to 0.
      delta_counter = current_tachometer_counter[n] + (4096 - previous_tachometer_counter[n]);

    current_rpm[n] = 60.0 / TACHOMETER_TICKS * delta_counter / (current_time[n] - engine->timestamp);
    engine->rpm[n] = (1 - TACHOMETER_AVG_WEIGHT) * current_rpm[n] + TACHOMETER_AVG_WEIGHT * engine->rpm[n];
    engine->tachometer_counter[n] = current_tachometer_counter[n];
  }
  engine->timestamp = runtime();

  return engine->alarm;
}







const uint8_t I2C_ADC_Maglev_subBMS_Addresses[3] = {0x19, 0x0B, 0x18};
Maglev_BMS* initialize_Maglev_BMS(uint8_t identity) {
  Maglev_BMS* bms = malloc(sizeof(Maglev_BMS));
  bms->identity = identity;
  bms->bus = MAGLEV_BMS_HUB_PORT[bms->identity][0];
  bms->relay_pin = MAGLEV_BMS_HUB_PORT[bms->identity][1];
  bms->relay_active_low = 1;

  int batt;
  for (batt = 0; batt < 3; batt++) {
	  // Initialize battery voltages to a default value
	  bms->battery_voltage[batt] = 23.0; // TODO: Is this a good starting value? 23V?

	  // Initialize cell voltages to a default value
	  int i = 0;
	  for (i = 0; i < 6; i++){
		  bms->cell_voltages[batt][i] = 2.833; // TODO: Is this a good starting value? 23V / 6 cells?
	  }

	  // Initialize thermistor moving averages (with first read)
      int temp_counter = 0;
      for (temp_counter = 0; temp_counter < 2; temp_counter++) {
    	bms->temperatures[batt][temp_counter] = calculate_temperature(ADC_read(bms->bus, I2C_ADC_Maglev_subBMS_Addresses[batt], temp_counter + 6));
      }
  }

  bms->amps = 0;
  bms->timestamp = 0;
  bms->alarm = 0;
  return bms;
}

uint8_t update_Maglev_BMS(Maglev_BMS* bms) {
  int batt, i;
  float prev_voltage;

  //0x19 FLOAT LOW
  //0x0B FLOAT HIGH
  //0x18 FLOAT FLOAT
  for (batt = 0; batt < 3; batt++) {
    prev_voltage = 0;
    for (i = 0; i < 6; i++) {
      float voltage = ADC_read(bms->bus, I2C_ADC_Maglev_subBMS_Addresses[batt], i) / MAX12BITVAL * 5.0 * MAGLEV_BMS_CAL_CONVERSIONS[bms->identity][batt][i];
      bms->cell_voltages[batt][i] = voltage - prev_voltage;
      prev_voltage = voltage;
    }
    bms->battery_voltage[batt] = prev_voltage;

    //Record Temperatures
    int temp_counter = 0;
    for (temp_counter = 0; temp_counter < 2; temp_counter++) {
		int new_temperature = calculate_temperature(ADC_read(bms->bus, I2C_ADC_Maglev_subBMS_Addresses[batt], temp_counter + 6));
		new_temperature = ((1 - THERMISTOR_AVG_WEIGHT) * new_temperature + THERMISTOR_AVG_WEIGHT * bms->temperatures[batt][temp_counter]);
		bms->temperatures[batt][temp_counter] = new_temperature;
		if (new_temperature > BATT_MAX_TEMP || new_temperature < BATT_MIN_TEMP){
		  bms->alarm |= 0b00000001;
		}
    }
  }
  return bms->alarm;
}














int calculate_temperature(uint16_t therm_adc_val) {
  //Calculate thermistor resistance
  float thermistance = therm_adc_val / MAX12BITVAL * REFERENCE_RESISTANCE / (1 - therm_adc_val / MAX12BITVAL);

  //Calculate temperature based on the thermistor resistance (formula based on https://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation and datasheet values)
  return THERMISTOR_BETA / (log(thermistance) - THERMISTOR_OFFSET) - 272;
}


uint16_t ADC_read(uint8_t bus, uint8_t ADC_address, uint8_t ADC_channel) {
  uint8_t input_buffer[2];

#ifdef ARDUINO
  Wire.beginTransmission(ADC_address);
  Wire.write(ADC_CHANNEL_SELECT[ADC_channel]);
  Wire.endTransmission(true);
  Wire.requestFrom(ADC_address, 2, true);
  input_buffer[0] = Wire.read();  //D11 D10 D9 D8 D7 D6 D5 D4
  input_buffer[1] = Wire.read();  //D3 D2 D1 D0 X X X X
#endif //ARDUINO
#ifdef LPC
  Chip_I2C_MasterSend(bus, ADC_address, &ADC_CHANNEL_SELECT[ADC_channel], 1);
  Chip_I2C_MasterRead(bus, ADC_address, input_buffer, 2);
#endif //LPC

  uint16_t ADC_value = (input_buffer[0] << 4) | (input_buffer[1] >> 4);
  return ADC_value;
}




void DAC_write(uint8_t bus, uint8_t DAC_address, uint16_t output_voltage) {
  uint8_t output_buffer[2] = {DAC_CONFIG | (output_voltage >> 8), output_voltage % 256};

#ifdef ARDUINO
  Wire.beginTransmission(DAC_address);
  Wire.write(output_buffer, 2);   //output the two bytes
  Wire.endTransmission(true);
#endif //ARDUINO
#ifdef LPC
  Chip_I2C_MasterSend(bus, DAC_address, output_buffer, 2);
#endif //LPC

}




void IOX_setup(uint8_t bus, uint8_t IOX_address) {
  uint8_t output_buffer[2] = {MCP23017_IOCONA, IOX_CONFIG};

#ifdef ARDUINO
  Wire.beginTransmission(IOX_address);
  Wire.write(output_buffer, 2); //IOCON register location, Configuration Byte
  Wire.endTransmission(true);
#endif //ARDUINO
#ifdef LPC
  Chip_I2C_MasterSend(bus, IOX_address, output_buffer, 2);
#endif //LPC

}

uint16_t IOX_read(uint8_t bus, uint8_t IOX_address) {
  uint8_t input_buffer[2];

#ifdef ARDUINO
  Wire.beginTransmission(IOX_address);
  Wire.write(MCP23017_GPIOA); //GPIOAB register location
  Wire.endTransmission(false);
  Wire.requestFrom(IOX_address, 2, true);
  input_buffer[0] = Wire.read();	//A7 A6 A5 A4 A3 A2 A1 A0
  input_buffer[1] = Wire.read();  //B7 B6 B5 B4 B3 B2 B1 B0
#endif //ARDUINO
#ifdef LPC
  Chip_I2C_MasterCmdRead(bus, IOX_address, MCP23017_GPIOA, input_buffer, 2);
#endif //LPC

  uint16_t IOX_value = (input_buffer[0] << 8) | (input_buffer[1] >> 0);
  return IOX_value;
}



float runtime() {
  float runtime_in_seconds;

#ifdef ARDUINO
  runtime_in_seconds = millis() / 1000.0;
#endif //ARDUINO
#ifdef LPC
  runtime_in_seconds = getRuntime() / 1000.0;
#endif // LPC

  return runtime_in_seconds;
}
