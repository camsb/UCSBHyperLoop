//Hyperloop Peripheral Management System
//Kevin Kha

//For use with HEMSv3, Maglev BMS, I2C Hub
//If using this on the Arduino, this should be saved as a .cpp file. Otherwise it should be saved as a .c

#include "HEMS.h"


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

HEMS* initialize_HEMS(uint8_t I2C_BUS, uint8_t I2C_DIP) {
  HEMS* engine = malloc(sizeof(HEMS));
  engine->bus = I2C_BUS;
  engine->ADC_device_address[0] = ADC_Address_Select[(I2C_DIP >> 6) & 0b11];
  engine->DAC_device_address[0] = DAC_Address_Select[(I2C_DIP >> 5) & 0b1];
  engine->IOX_device_address[0] = IOX_Address_Select[(I2C_DIP >> 2) & 0b110];
  engine->IOX_device_address[1] = IOX_Address_Select[((I2C_DIP >> 2) & 0b110) + 1];

  int n;
  for (n = 0; n < 2; n++) {
    IOX_setup(engine->bus, engine->IOX_device_address[n]);
    engine->tachometer_counter[n] = IOX_read(engine->bus, engine->IOX_device_address[n]);
  }

  engine->throttle_voltage = 0;
  engine->timestamp = 0;

  engine->alarm = 0;

  return engine;
}

uint8_t update_HEMS(HEMS* engine) {
  //Set throttle;
  DAC_write(engine->bus, engine->DAC_device_address[0], engine->throttle_voltage * 4095 / 5);
  engine->DAC_diagnostic = ADC_read(engine->bus, engine->ADC_device_address[0], 0) / 4095.0 * 5;

  //Record Temperatures
  int temp_counter;
  for (temp_counter = 0; temp_counter < NUM_THERMISTORS; temp_counter++) {
    int new_temperature = calculate_temperature(ADC_read(engine->bus, engine->ADC_device_address[0], temp_counter + 1));
    new_temperature = ((1 - THERMISTOR_AVG_WEIGHT) * new_temperature + THERMISTOR_AVG_WEIGHT * engine->temperatures[temp_counter]);
    engine->temperatures[temp_counter] = new_temperature;
    if (new_temperature > HEMS_MAX_TEMP || new_temperature < HEMS_MIN_TEMP)
      engine->alarm |= 0b00000001;
  }

  int short_counter;
  float ShortRangingMovingAverage = 0;
	for (short_counter = 0; short_counter < NUM_SHORTIR; short_counter++){
	  float ShortRangingDataRaw = ADC_read(engine->bus, engine->ADC_device_address[0], short_counter + 5);
	  float voltage = ((float)ShortRangingDataRaw) / 1300;

		if (!((voltage < 0.34) || (voltage > 2.43))){
			uint16_t index = (uint16_t)(voltage * 100.0 + 0.5) - 34;
			ShortRangingMovingAverage = ALPHA*ShortRangingMovingAverage + BETA*shortRangingDistanceLUT[index];
		}
		engine->short_data[short_counter] = ShortRangingMovingAverage;
	}

  //Record Motor Controller Current
  //With no current, the ACS759x150B should output 3.3V/2
  uint16_t ammeter_ratio = ADC_read(engine->bus, engine->ADC_device_address[0], 7);
  //DEBUGOUT("Current sensor ADC: %d\n", ammeter_ratio);
  uint8_t new_amps = abs(ammeter_ratio * VREF / 4096.0 * 1000 - 1650) / 8.8; //Done in mV
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
    current_tachometer_counter[n] = (IOX_read(engine->bus, engine->IOX_device_address[n]) & (0x0FFF));
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










uint8_t I2C_ADC_Maglev_BMS_Addresses[3] = {0x19, 0x0B, 0x18};
Maglev_BMS* intialize_Maglev_BMS(uint8_t I2C_BUS) {
  Maglev_BMS* bms = malloc(sizeof(Maglev_BMS));
  bms->bus = I2C_BUS;

  bms->relay_active = 1;

  bms->timestamp = 0;
  bms->alarm = 0;
  return bms;
}

// This appears unfinished. Don't run it.
uint8_t update_Maglev_BMS(Maglev_BMS* bms) {
  int batt, i, cell;
  float prev_voltage;

  //0x19 FLOAT LOW
  //0x0B FLOAT HIGH
  //0x18 FLOAT FLOAT
  for (batt = 0; batt < 3; batt++) {
    prev_voltage = 0;
    for (i = 0; i < 6; i++) {
      float voltage = ADC_read(bms->bus, I2C_ADC_Maglev_BMS_Addresses[batt], i) / 4096.0 * 5.0 * bms->conversion[batt][i];
      bms->cell_voltages[batt][i] = voltage - prev_voltage;
      prev_voltage = voltage;
    }
    bms->battery_voltage[batt] = prev_voltage;

  }
  return 0; // For compilation
}














int calculate_temperature(uint16_t therm_adc_val) {
  //Calculate thermistor resistance
  float thermistance = therm_adc_val / 4095.0 * REFERENCE_RESISTANCE / (1 - therm_adc_val / 4095.0);

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

void set_motor_throttle(uint8_t motor_num, float voltage){
  // Set the motor's throttle directly, but only if HEMS is enabled
  #if MOTOR_BOARD_I2C_ACTIVE
    if (motor_num < 4){
        if (voltage <= MAX_THROTTLE_VOLTAGE && voltage >= 0){
            motors[motor_num]->throttle_voltage = voltage;
        }
        else{
            DEBUGOUT("Invalid voltage specified in set_motor_target_throttle");
        }
    }
    else{
        DEBUGOUT("Invalid motor number in set_motor_target_throttle!\n");
    }
  #endif
}
