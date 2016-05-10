#include "ranging.h"
#include "math.h"

/*
 *  Left-front long J25
 *  Right-front long J30
 *  Left-back long J22
 *  Right-back long J31
 *
 *  Left-front short J36
 *  Right-front short J37
 *  Left-back short J34
 *  Right-back short J35
 */

float sin_inv(float x) {
	return 0;
}

void computePositionAttitudeRanging() {
	/* Compute Constants */
//	static const short_front_left_pyth 	= sqrt((SHORT_FRONT_LEFT_DIST^2.0) + (SHORT_FRONT_DIST^2.0));
	static const short_front_right_pyth = sqrt((SHORT_FRONT_RIGHT_DIST^2.0) + (SHORT_FRONT_DIST^2.0));
//	static const short_back_left_pyth 	= sqrt((SHORT_BACK_LEFT_DIST^2.0) + (SHORT_BACK_DIST^2.0));
	static const short_back_right_pyth 	= sqrt((SHORT_BACK_RIGHT_DIST^2.0) + (SHORT_BACK_DIST^2.0));
//	static const short_left_pyth_inv 	= (0.5 / (short_front_left_pyth + short_back_left_pyth));
	static const short_right_pyth_inv 	= (0.5 / (short_front_right_pyth + short_back_right_pyth));

	/* y Position */
	float y_front = (sensorData.longRangingJ30 - sensorData.longRangingJ25) * 0.5;
	float y_back  = (sensorData.longRangingJ31 - sensorData.longRangingJ22) * 0.5;
	float y_com   = (y_front*LONG_FRONT_DIST + y_back*LONG_BACK_DIST) * LONG_AXIS_SUM_INV;

	/* Yaw */
	float yaw     = sin_inv((y_front - y_com) / LONG_FRONT_DIST);

	/* z Position */
	float z_front = ((sensorData.shortRangingJ36 + SHORT_FRONT_HEIGHT) + (sensorData.shortRangingJ37 + SHORT_BACK_HEIGHT)) * 0.5;
	float z_back  = ((sensorData.shortRangingJ34 + SHORT_FRONT_HEIGHT) + (sensorData.shortRangingJ35 + SHORT_BACK_HEIGHT)) * 0.5;
	float z_com   = (z_front*SHORT_FRONT_DIST + z_back*SHORT_BACK_DIST) * SHORT_AXIS_SUM_INV;

	/* Pitch */
	float pitch   = sin_inv((z_back - z_com) * SHORT_BACK_DIST_INV);

	/* Roll */
	float z_right = (short_front_right_pyth*(sensorData.shortRangingJ37 + SHORT_FRONT_HEIGHT) +
					 short_back_right_pyth*(sensorData.shortRangingJ35 + SHORT_BACK_HEIGHT)) *
					 short_right_pyth_inv;
	float roll	  = sin_inv((z_right - z_com)*SHORT_RIGHT_AVG_INV);

}

/* Convert voltage */
void convertVoltageShort(uint8_t sensor)
{
	uint16_t index;

	float voltage = ((float)ShortRangingDataRaw[sensor]) / 1300;

	if ((voltage < 0.34) || (voltage > 2.43)) {
		DEBUGOUT("%d sensor voltage of %.3f V is out of operating range.\n", sensor, voltage);
	}
	else {
		index = (uint16_t)(voltage * 100.0 + 0.5) - 34;
		ShortRangingMovingAverage[sensor] = ALPHA*ShortRangingMovingAverage[sensor] + BETA*shortRangingDistanceLUT[index];
	}
}

/* Convert voltage */
void convertVoltageLong(uint8_t sensor)
{
	uint16_t index;

	float voltage = ((float)LongRangingDataRaw[sensor]) / 1300.0;

	if ((voltage < 0.49) || (voltage > 2.73)) {
		DEBUGOUT("%d sensor voltage of %.3f V is out of operating range.\n", sensor, voltage);
	}
	else {
		index = (uint16_t)(voltage * 100.0 + 0.5) - 49;
		LongRangingMovingAverage[sensor] = ALPHA*LongRangingMovingAverage[sensor] + BETA*longRangingDistanceLUT[index];
	}
}

rangingData getShortDistance(void)
{
	rangingData short_data;

	/* Read ADC value */
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH4, &ShortRangingDataRaw[0]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH5, &ShortRangingDataRaw[1]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH6, &ShortRangingDataRaw[2]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH7, &ShortRangingDataRaw[3]);

	/* Print ADC value */
	convertVoltageShort(0);
	convertVoltageShort(1);
	convertVoltageShort(2);
	convertVoltageShort(3);

	/* Return rangingData structure */
	short_data.sensor0 = ShortRangingMovingAverage[0];
	short_data.sensor1 = ShortRangingMovingAverage[1];
	short_data.sensor2 = ShortRangingMovingAverage[2];
	short_data.sensor3 = ShortRangingMovingAverage[3];
	return short_data;
}

rangingData getLongDistance(void)
{
	rangingData long_data;

	/* Read ADC value */
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH0, &LongRangingDataRaw[0]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH1, &LongRangingDataRaw[1]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH2, &LongRangingDataRaw[2]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH3, &LongRangingDataRaw[3]);

	/* Print ADC value */
	convertVoltageLong(0);
	convertVoltageLong(1);
	convertVoltageLong(2);
	convertVoltageLong(3);

	/* Return rangingData structure */
	long_data.sensor0 = LongRangingMovingAverage[0];
	long_data.sensor1 = LongRangingMovingAverage[1];
	long_data.sensor2 = LongRangingMovingAverage[2];
	long_data.sensor3 = LongRangingMovingAverage[3];
	return long_data;
}

void initADCChannel(uint8_t channel, uint8_t port, uint8_t pin, uint8_t func, float init_val){
	Chip_ADC_EnableChannel(_LPC_ADC_ID, channel, ENABLE);
	Chip_IOCON_PinMux(LPC_IOCON, port, pin, IOCON_ADMODE_EN, func);
	LongRangingMovingAverage[0] = init_val;
}

void rangingSensorsInit(void)  {
	Chip_ADC_Init(_LPC_ADC_ID, &ADCSetup);

	Burst_Mode_Flag = 1;
	ADC_Interrupt_Done_Flag = 0;
	uint32_t _bitRate = ADC_MAX_SAMPLE_RATE;

	/* Enable all ranging sensor channels */
	initADCChannel(ADC_CH0, 0, 23, IOCON_FUNC1, LONG_FRONT_INITIAL);	// Left-front long J25
	initADCChannel(ADC_CH1, 0, 24, IOCON_FUNC1, LONG_FRONT_INITIAL);	// Right-front long J30
	initADCChannel(ADC_CH2, 0, 25, IOCON_FUNC1, LONG_BACK_INITIAL);		// Left-back long J22
	initADCChannel(ADC_CH3, 0, 26, IOCON_FUNC1, LONG_BACK_INITIAL);		// Right-back long J31
	initADCChannel(ADC_CH4, 1, 30, IOCON_FUNC3, SHORT_FRONT_INITIAL);	// Left-front short J36
	initADCChannel(ADC_CH5, 1, 31, IOCON_FUNC3, SHORT_FRONT_INITIAL);	// Right-front short J37
	initADCChannel(ADC_CH6, 0, 12, IOCON_FUNC3, SHORT_BACK_INITIAL);	// Left-back short J34
	initADCChannel(ADC_CH7, 0, 13, IOCON_FUNC3, SHORT_BACK_INITIAL);	// Right-back short J35

	Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
	Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
}

void ADC_IRQHandler(void)
{
	/* Interrupt mode: Call the stream interrupt handler */
	NVIC_DisableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH7, DISABLE);
	Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);

	ADC_Interrupt_Done_Flag = 1;
}

void Ranging_Int_Measure() {
	/* Enable ADC Interrupt */
	NVIC_EnableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH7, ENABLE);
	/* Enable burst mode if any, the AD converter does repeated conversions
	   at the rate selected by the CLKS field in burst mode automatically */
	Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
}
