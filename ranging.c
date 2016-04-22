#include "ranging.h"

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

void getShortDistance(void)
{
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
}

void getLongDistance(void)
{
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
