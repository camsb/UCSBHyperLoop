//Hyperloop Hover Engine Management System
//Kevin Kha
//If using on the Arduino, this should be saved as a .cpp file. Otherwise it should be saved as a .c

#include "HEMS.h"
#include "initialization.h"

// Global variables.
const uint8_t ADC_Address_Select[4] = {0x8, 0xA, 0x1A, 0x14};
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
  engine->ADC_0_device_address = ADC_Address_Select[(I2C_DIP >> 6) & 0b11];
  engine->DAC_0_device_address = DAC_Address_Select[(I2C_DIP >> 3) & 0b1];
  engine->IOX_0_device_address = IOX_Address_Select[(I2C_DIP >> 0) & 0b111];


  IOX_setup(engine->bus, engine->IOX_0_device_address);
  engine->tachometer_counter = IOX_read(engine->bus, engine->IOX_0_device_address);
  engine->throttle_voltage = 0;
  engine->timestamp = 0;

  engine->alarm = 0;

  return engine;
}

void set_motor_target_throttle(uint8_t motor_num, float voltage){
  // Set the motor's target throttle, but only if HEMS is enabled
  #if MOTOR_BOARD_I2C_ACTIVE
  if (motor_num < NUM_MOTORS){
      if (voltage <= MAX_THROTTLE_VOLTAGE && voltage >= 0){
          motors[motor_num]->target_throttle_voltage = voltage;
      }
      else{
          DEBUGOUT("Invald voltage specified in set_motor_target_throttle");
      }
  }
  else{
      DEBUGOUT("Invalid motor number in set_motor_target_throttle!\n");
  }
  #endif
}

void set_motor_throttle(uint8_t motor_num, float voltage){
  // Set the motor's throttle directly, but only if HEMS is enabled
  #if MOTOR_BOARD_I2C_ACTIVE
    if (motor_num < NUM_MOTORS){
        if (voltage <= MAX_THROTTLE_VOLTAGE && voltage >= 0){
            motors[motor_num]->throttle_voltage = voltage;
        }
        else{
            DEBUGOUT("Invald voltage specified in set_motor_target_throttle");
        }
    }
    else{
        DEBUGOUT("Invalid motor number in set_motor_target_throttle!\n");
    }
  #endif
}

void update_HEMS(HEMS* engine) {
#if 0
	if(engine->throttle_voltage > engine->target_throttle_voltage) {
		engine->throttle_voltage -= 0.1;
	}
	if(engine->throttle_voltage < engine->target_throttle_voltage) {
		engine->throttle_voltage += 0.1;
	}

	if(engine->throttle_voltage > 5) {
		engine->throttle_voltage = 5;
	}
#endif
	//Set throttle;
	DAC_write(engine->bus, engine->DAC_0_device_address, engine->throttle_voltage * 4095 / 5);

#if 1
  //Record Temperatures
  int temp_counter;
  for (temp_counter = 0; temp_counter < NUM_THERMISTORS; temp_counter++) {
    float thermistor_ratio = ADC_read(engine->bus, engine->ADC_0_device_address, temp_counter);

    //Calculate thermistor resistance
    float thermistance = (4095 - thermistor_ratio) * REFERENCE_RESISTANCE / thermistor_ratio;

    //Calculate temperature based on the thermistor resistance (formula based on https://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation and datasheet values)
    int new_temperature = THERMISTOR_BETA / (log(thermistance) - THERMISTOR_OFFSET) - 272;

    //Exponential average and record new temperature
    new_temperature = ((1 - THERMISTOR_AVG_WEIGHT) * new_temperature + THERMISTOR_AVG_WEIGHT * engine->temperatures[temp_counter]);

    engine->temperatures[temp_counter] = new_temperature;
    if (new_temperature > SAFE_TEMPERATURE)
      engine->alarm |= 0b00000001;
  }

  //Record Motor Controller Current
  //With no current, the ACS759x150B should output 3.3V/2
  uint16_t ammeter_ratio = ADC_read(engine->bus, engine->ADC_0_device_address, AMMETER_CHANNEL);
  uint8_t new_amps = abs(ammeter_ratio * 5000.0 / 4095 - 1000 * AMMETER_VCC / 2) * AMMETER_CONVERSION; //Done in mV
  engine->amps = new_amps;

  if (new_amps > SAFE_CURRENT)
    engine->alarm |= 0b00000010;

  //Record Motor RPM
  uint16_t current_tachometer_counter = IOX_read(engine->bus, engine->IOX_0_device_address);
  uint16_t previous_tachometer_counter = engine->tachometer_counter;
  float current_time = runtime();

  uint16_t delta_counter; //Calculate # of pulses since last checked
  if(current_tachometer_counter >= previous_tachometer_counter)
    delta_counter = current_tachometer_counter - previous_tachometer_counter;
  else  //Account for edge case where the binary counter overflows and resets back to 0.
    delta_counter = current_tachometer_counter + (4095 - previous_tachometer_counter);

  uint16_t current_rpm = 60.0 / TACHOMETER_TICKS * delta_counter / (current_time - engine->timestamp);
  engine->rpm = (1 - TACHOMETER_AVG_WEIGHT) * current_rpm + TACHOMETER_AVG_WEIGHT * engine->rpm;
  engine->timestamp = current_time;
  engine->tachometer_counter = current_tachometer_counter;
#endif	// 0
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
  runtime_in_seconds = getRuntime()/1000.0;
#endif // LPC

  return runtime_in_seconds;
}
