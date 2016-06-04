#include "ranging.h"
#include "math.h"

float arcsin(float x) {
	static const float step = 2.0/256.0;
	if(x > 1.0)
		return 90.0;
	if(x < -1.0)
		return -90.0;
	uint16_t index = ((uint16_t)(((x + 1.0)/step) + 0.5));
	if(index > 1023)
		return arcSinLUT[1023];
	return arcSinLUT[index];
}

positionAttitudeData computePositionAttitudeRanging(rangingData longRangingData, rangingData shortRangingData) {

	/* y Position */
	float y_front = (longRangingData.frontRight - longRangingData.frontLeft) * 0.5;
	float y_back  = (longRangingData.backRight - longRangingData.backLeft) * 0.5;
	float y_com   = (y_front*LONG_FRONT_DIST + y_back*LONG_BACK_DIST) * LONG_AXIS_SUM_INV;

	/* Yaw */
	float yaw     = arcsin((y_front - y_com) / LONG_FRONT_DIST);

	/* z Position */
	float z_front = ((shortRangingData.frontRight + SHORT_FRONT_HEIGHT) + (shortRangingData.frontLeft + SHORT_FRONT_HEIGHT)) * 0.5;
	float z_back  = ((shortRangingData.backRight + SHORT_BACK_HEIGHT) + (shortRangingData.backLeft + SHORT_BACK_HEIGHT)) * 0.5;
	float z_com   = (z_front*SHORT_FRONT_DIST + z_back*SHORT_BACK_DIST) * short_axis_sum_inv;

	/* Pitch */
	float pitch   = arcsin((z_back - z_com) * short_back_dist_inv);

	/* Roll */
	float z_right = (short_front_right_pyth*(shortRangingData.frontRight + SHORT_FRONT_HEIGHT) +
					 short_back_right_pyth*(shortRangingData.backRight + SHORT_BACK_HEIGHT)) *
					 short_right_pyth_inv;
	float roll	  = arcsin((z_right - z_com) * short_right_avg_inv);

	uint32_t i;
	for(i = 0; i < 120000000; i++);

	positionAttitudeData positionAttitude;
	positionAttitude.y = y_com;
	positionAttitude.z = z_com;
	positionAttitude.roll = roll;
	positionAttitude.pitch = pitch;
	positionAttitude.yaw = yaw;
	return positionAttitude;
}

/* Convert voltage */
void convertVoltageShort(uint8_t sensor)
{
	uint16_t index;

	float voltage = ((float)ShortRangingDataRaw[sensor]) / 1300;

	if ((voltage < 0.34) || (voltage > 2.43)) {
//		DEBUGOUT("%d sensor voltage of %.3f V is out of operating range.\n", sensor, voltage);
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
//		DEBUGOUT("%d sensor voltage of %.3f V is out of operating range.\n", sensor, voltage);
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
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH6, &ShortRangingDataRaw[0]); // Front left
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH4, &ShortRangingDataRaw[1]); // Front right
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH7, &ShortRangingDataRaw[2]); // Back left
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH5, &ShortRangingDataRaw[3]); // Back right

	/* Print ADC value */
	convertVoltageShort(0);
	convertVoltageShort(1);
	convertVoltageShort(2);
	convertVoltageShort(3);

	/* Return rangingData structure */
	short_data.frontLeft = ShortRangingMovingAverage[0];
	short_data.frontRight = ShortRangingMovingAverage[1];
	short_data.backLeft = ShortRangingMovingAverage[2];
	short_data.backRight = ShortRangingMovingAverage[3];
	return short_data;
}

rangingData getLongDistance(void)
{
	rangingData long_data;

	/* Read ADC value */
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH3, &LongRangingDataRaw[0]); // Front Left
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH1, &LongRangingDataRaw[1]); // Front Right
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH0, &LongRangingDataRaw[2]); // Back Left
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH2, &LongRangingDataRaw[3]); // Back Right

	/* Print ADC value */
	convertVoltageLong(0);
	convertVoltageLong(1);
	convertVoltageLong(2);
	convertVoltageLong(3);

	/* Return rangingData structure */
	long_data.frontLeft = LongRangingMovingAverage[0];
	long_data.frontRight = LongRangingMovingAverage[1];
	long_data.backLeft = LongRangingMovingAverage[2];
	long_data.backRight = LongRangingMovingAverage[3];
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

	/* Compute Constants */
	short_front_right_pyth 	= sqrt((SHORT_FRONT_RIGHT_DIST - SHORT_RIGHT_DIST_AVG)*(SHORT_FRONT_RIGHT_DIST - SHORT_RIGHT_DIST_AVG) + (SHORT_FRONT_DIST)*(SHORT_FRONT_DIST));
	short_back_right_pyth 	= sqrt((SHORT_BACK_RIGHT_DIST - SHORT_RIGHT_DIST_AVG)*(SHORT_BACK_RIGHT_DIST - SHORT_RIGHT_DIST_AVG) + (SHORT_BACK_DIST)*(SHORT_BACK_DIST));
	short_right_pyth_inv 	= (1.0 / (short_front_right_pyth + short_back_right_pyth));
	short_right_avg_inv		= 1.0/((float)SHORT_RIGHT_DIST_AVG);
	short_front_dist_inv	= 1.0/((float)SHORT_FRONT_DIST);
	short_back_dist_inv		= 1.0/((float)SHORT_BACK_DIST);
	short_axis_sum_inv		= 1.0/((float)(SHORT_FRONT_DIST + SHORT_BACK_DIST));

	/* Enable all ranging sensor channels */
	initADCChannel(ADC_CH0, 0, 23, IOCON_FUNC1, LONG_FRONT_INITIAL);	// Port-front long J25
	initADCChannel(ADC_CH1, 0, 24, IOCON_FUNC1, LONG_FRONT_INITIAL);	// Starboard-front long J30
	initADCChannel(ADC_CH2, 0, 25, IOCON_FUNC1, LONG_BACK_INITIAL);		// Port-back long J22
	initADCChannel(ADC_CH3, 0, 26, IOCON_FUNC1, LONG_BACK_INITIAL);		// Starboard-back long J31
	initADCChannel(ADC_CH4, 1, 30, IOCON_FUNC3, SHORT_FRONT_INITIAL);	// Port-front short J36
	initADCChannel(ADC_CH5, 1, 31, IOCON_FUNC3, SHORT_FRONT_INITIAL);	// Starboard-front short J37
	initADCChannel(ADC_CH6, 0, 12, IOCON_FUNC3, SHORT_BACK_INITIAL);	// Port-back short J34
	initADCChannel(ADC_CH7, 0, 13, IOCON_FUNC3, SHORT_BACK_INITIAL);	// Starboard-back short J35

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
