#include "ranging.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief	ADC0 interrupt handler sub-routine
 * @return	Nothing
 */
void ADC_IRQHandler(void)
{
	/* Interrupt mode: Call the stream interrupt handler */
	NVIC_DisableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH7, DISABLE);
	Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);

//	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH0, &LongRangingDataRaw[0]);
//	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH1, &LongRangingDataRaw[1]);
//	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH2, &LongRangingDataRaw[2]);
//	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH3, &LongRangingDataRaw[3]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH4, &ShortRangingDataRaw[0]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH5, &ShortRangingDataRaw[1]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH6, &ShortRangingDataRaw[2]);
	Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH7, &ShortRangingDataRaw[3]);

	ADC_Interrupt_Done_Flag = 1;
	//App_print_ADC_value(dataADC);
	//convertVoltage(dataADC, 1);

	// NVIC_EnableIRQ(_LPC_ADC_IRQ);
	// Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);

	//else {Interrupt_Continue_Flag = 0; }
}

/* Print ADC value and delay */
void App_print_ADC_value(uint16_t data)
{
	volatile uint32_t j;
	j = 5000000;

	float voltage;
	voltage = ((float)data) / 1300;

	DEBUGOUT("ADC value is : 0x%04x or %f V\r\n", data, voltage);
	/* Delay */
	while (j--) {}
}

/* Convert voltage */
void convertVoltage(uint16_t data, uint8_t sensor)
{
	uint16_t index;

	float voltage = ((float)data) / 1300;

	if ((voltage < 0.34) || (voltage > 2.43)) {
		DEBUGOUT("ADC voltage of %.3f V is out of operating range.\n", voltage);
	}
	else {
		index = (uint16_t)(voltage * 100.0 + 0.5) - 33;
		ShortRangingMovingAverage[sensor] = ALPHA*ShortRangingMovingAverage[sensor] + BETA*shortRangingDistanceLUT[index];
		DEBUGOUT("%.3f V -- %.3f cm at index %d\n", voltage, ShortRangingMovingAverage[sensor],  index);
	}
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
		DEBUGOUT("%d sensor measures %.3f V -- %.3f cm at index %d\n", sensor, voltage, ShortRangingMovingAverage[sensor],  index);
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
//		DEBUGOUT("%d sensor measures %.3f V -- %.3f cm at index %d\n", sensor, voltage, LongRangingMovingAverage[sensor],  index);
	}
}

/* Process short ranging data */
void processShortRangingData(void) {
//	convertVoltageShort(0);
//	convertVoltageShort(1);
//	convertVoltageShort(2);
	convertVoltageShort(3);
}

void processLongRangingData(void) {
//	convertVoltageLong(0);
//	convertVoltageLong(1);
//	convertVoltageLong(2);
	convertVoltageLong(3);
}

/* DMA routine for ADC example */
void App_DMA_Test(void)
{
	uint16_t dataADC;

	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
	/* Setting ADC interrupt, ADC Interrupt must be disable in DMA mode */
	NVIC_DisableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);
	/* Get the free channel for DMA transfer */
	dmaChannelNum = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, _GPDMA_CONN_ADC);
	/* Enable burst mode if any, the AD converter does repeated conversions
	   at the rate selected by the CLKS field in burst mode automatically */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
	}
	/* Get  adc value until get 'x' character */
	while (DEBUGIN() != 'x') {
		/* Start A/D conversion if not using burst mode */
		if (!Burst_Mode_Flag) {
			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
		channelTC = 0;
		Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum,
						  _GPDMA_CONN_ADC,
						  (uint32_t) &DMAbuffer,
						  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
						  1);

		/* Waiting for reading ADC value completed */
		while (channelTC == 0) {}

		/* Get the ADC value fron Data register*/
		dataADC = ADC_DR_RESULT(DMAbuffer);
		App_print_ADC_value(dataADC);
	}
	/* Disable interrupts, release DMA channel */
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNum);
	NVIC_DisableIRQ(DMA_IRQn);
	/* Disable burst mode if any */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}
}

/* Interrupt routine for ADC example */
void App_Interrupt_Test(void)
{
	/* Enable ADC Interrupt */
	NVIC_EnableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);
	/* Enable burst mode if any, the AD converter does repeated conversions
	   at the rate selected by the CLKS field in burst mode automatically */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
	}
	Interrupt_Continue_Flag = 1;
	ADC_Interrupt_Done_Flag = 1;
	while (Interrupt_Continue_Flag) {
		if (!Burst_Mode_Flag && ADC_Interrupt_Done_Flag) {
			ADC_Interrupt_Done_Flag = 0;
			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
	}
	/* Disable burst mode if any */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}
	/* Disable ADC interrupt */
	NVIC_DisableIRQ(_LPC_ADC_IRQ);
}


///* Custom polling and data conversion */
///*static */void getShortDistance(void)
//{
//	uint16_t dataADC;
//
//	/* Select using burst mode or not */
//	if (Burst_Mode_Flag) {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
//	}
//	else {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
//	}
//
//	/* Get  adc value until get 'x' character */
//	while (DEBUGIN() != 'x') {
//		/* Start A/D conversion if not using burst mode */
//		if (!Burst_Mode_Flag) {
//			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
//		}
//		/* Waiting for A/D conversion complete */
//		while (Chip_ADC_ReadStatus(_LPC_ADC_ID, _ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {}
//		/* Read ADC value */
//		Chip_ADC_ReadValue(_LPC_ADC_ID, _ADC_CHANNEL, &dataADC);
//		/* Print ADC value */
//		convertVoltage(dataADC, 1);
//	}
//
//	/* Disable burst mode, if any */
//	if (Burst_Mode_Flag) {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
//	}
//}

/*static */void getShortDistance(void)
{
	uint8_t i;
	uint8_t channel;

	/* Select using burst mode or not */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
	}
	else {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}

	/* Get  adc value */
	for(i = 0; i < 1; i++) {
//		if(i == 0)
//			channel = ADC_CH4;
//		if(i == 1)
//			channel = ADC_CH5;
//		if(i == 2)
//			channel = ADC_CH6;
//		else
		channel = ADC_CH7;
		/* Start A/D conversion if not using burst mode */
		shortChannelChange(3);
		if (!Burst_Mode_Flag) {
			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(_LPC_ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}

		/* Read ADC value */
		Chip_ADC_ReadValue(_LPC_ADC_ID, channel, &ShortRangingDataRaw[3]);

		/* Print ADC value */
//		convertVoltage(dataADC, 1);
		convertVoltageShort(3);
	}

	//processShortRangingData();
	/* Disable burst mode, if any */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}
}


/*static */void getLongDistance(void)
{
	uint8_t i;
	uint8_t channel;

	/* Select using burst mode or not */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
	}
	else {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}

	/* Get  adc value */
	for(i = 0; i < 1; i++) {
//		if(i == 0)
//			channel = ADC_CH4;
//		if(i == 1)
//			channel = ADC_CH5;
//		if(i == 2)
//			channel = ADC_CH6;
//		else
		channel = ADC_CH3;
		/* Start A/D conversion if not using burst mode */
		shortChannelChange(3);
		if (!Burst_Mode_Flag) {
			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(_LPC_ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}

		/* Read ADC value */
//		Chip_ADC_ReadValue(_LPC_ADC_ID, channel, &LongRangingDataRaw[3]);
		Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH1, &LongRangingDataRaw[1]);
		Chip_ADC_ReadValue(_LPC_ADC_ID, ADC_CH3, &LongRangingDataRaw[3]);

		/* Print ADC value */
//		convertVoltageLong(0);
		convertVoltageLong(1);
//		convertVoltageLong(2);
		convertVoltageLong(3);
	}

	//processShortRangingData();
	/* Disable burst mode, if any */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
	}
}

/* Polling routine for ADC example */
///*static */void App_Polling_Test(void)
//{
//	uint16_t dataADC;
//
//	/* Select using burst mode or not */
//	if (Burst_Mode_Flag) {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
//	}
//	else {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
//	}
//
//	/* Get  adc value until get 'x' character */
//	while (DEBUGIN() != 'x') {
//		/* Start A/D conversion if not using burst mode */
//		if (!Burst_Mode_Flag) {
//			Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
//		}
//		/* Waiting for A/D conversion complete */
//		while (Chip_ADC_ReadStatus(_LPC_ADC_ID, _ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {}
//		/* Read ADC value */
//		Chip_ADC_ReadValue(_LPC_ADC_ID, _ADC_CHANNEL, &dataADC);
//		/* Print ADC value */
//		App_print_ADC_value(dataADC);
//	}
//
//	/* Disable burst mode, if any */
//	if (Burst_Mode_Flag) {
//		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, DISABLE);
//	}
//}

void shortChannelChange(uint8_t sensor) {
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH0, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH1, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH2, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH3, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH4, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH5, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH6, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH7, DISABLE);

	switch(sensor) {
	case 0:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH4, ENABLE);
		break;
	case 1:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH5, ENABLE);
		break;
	case 2:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH6, ENABLE);
		break;
	case 3:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH7, ENABLE);
		break;
	}
}

void longChannelChange(uint8_t sensor) {
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH0, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH1, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH2, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH3, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH4, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH5, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH6, DISABLE);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH7, DISABLE);

	switch(sensor) {
	case 0:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH0, ENABLE);
		break;
	case 1:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH1, ENABLE);
		break;
	case 2:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH2, ENABLE);
		break;
	case 3:
		Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH3, ENABLE);
		break;
	}
}


void Ranging_Init(void)  {
	Chip_ADC_Init(_LPC_ADC_ID, &ADCSetup);

	// Burst_Mode_Flag = 1;
	Burst_Mode_Flag = 1;
	ADC_Interrupt_Done_Flag = 0;
	uint32_t _bitRate = ADC_MAX_SAMPLE_RATE;

	/* Enable port-front long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH0, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH0, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 23, IOCON_ADMODE_EN, IOCON_FUNC1);
	LongRangingMovingAverage[0] = LONG_FRONT_INITIAL;

	/* Enable starboard-front long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH1, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH1, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 24, IOCON_ADMODE_EN, IOCON_FUNC1);
	LongRangingMovingAverage[1] = LONG_FRONT_INITIAL;

	/* Enable port-back long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH2, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH2, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_ADMODE_EN, IOCON_FUNC1);
	LongRangingMovingAverage[2] = LONG_BACK_INITIAL;

	/* Enable starboard-back long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH3, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH3, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 26, IOCON_ADMODE_EN, IOCON_FUNC1);
	LongRangingMovingAverage[3] = LONG_BACK_INITIAL;
//
//	/* Enable port-front long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH4, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH4, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 1, 30, IOCON_ADMODE_EN, IOCON_FUNC3);
	ShortRangingMovingAverage[0] = SHORT_FRONT_INITIAL;

	/* Enable starboard-front long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH5, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH5, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 1, 31, IOCON_ADMODE_EN, IOCON_FUNC3);
	ShortRangingMovingAverage[1] = SHORT_FRONT_INITIAL;
//
	/* Enable port-back long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH6, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH6, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 12, IOCON_ADMODE_EN, IOCON_FUNC3);
	ShortRangingMovingAverage[2] = SHORT_BACK_INITIAL;

	/* Enable starboard-back long ranging sensor */
	Chip_ADC_EnableChannel(_LPC_ADC_ID, ADC_CH7, ENABLE);
//	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH7, DISABLE);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 13, IOCON_ADMODE_EN, IOCON_FUNC3);
	ShortRangingMovingAverage[3] = SHORT_BACK_INITIAL;

	Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
}

void Ranging_Int_Measure() {
	/* Enable ADC Interrupt */
	NVIC_EnableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, ADC_CH7, ENABLE);
	/* Enable burst mode if any, the AD converter does repeated conversions
	   at the rate selected by the CLKS field in burst mode automatically */
	if (Burst_Mode_Flag) {
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/



///**
// * @brief	DMA interrupt handler sub-routine
// * @return	Nothing
// */
//void DMA_IRQHandler(void)
//{
//	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum) == SUCCESS) {
//		channelTC++;
//	}
//	else {
//		/* Process error here */
//	}
//}

/**
 * @brief	Main routine for ADC example
 * @return	Nothing
 */
//int main(void)
//{
//  Burst_Mode_Flag = 0
//	bool end_Flag = false;
//	uint32_t _bitRate = ADC_MAX_SAMPLE_RATE;
//	uint8_t bufferUART;
//
//	SystemCoreClockUpdate();
//	Board_Init();
//
//	Chip_IOCON_PinMux(0, 12, IOCON_ADMODE_EN, IOCON_FUNC4, IOCON_MODE_INACT);
//
//	/*ADC Init */
//	Chip_ADC_Init(_LPC_ADC_ID, &ADCSetup);
//	Chip_ADC_EnableChannel(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);
//
//	while(1) {
//		App_Polling_Test();
//	}
//
////	while (!end_Flag) {
////		while (!end_Flag) {
////			bufferUART = 0xFF;
////			bufferUART = DEBUGIN();
////			if (bufferUART == 'c') {
////				DEBUGOUT(SelectMenu);
////				bufferUART = 0xFF;
////				while (bufferUART == 0xFF) {
////					bufferUART = DEBUGIN();
////					if ((bufferUART != '1') && (bufferUART != '2') && (bufferUART != '3')) {
////						bufferUART = 0xFF;
////					}
////				}
////				switch (bufferUART) {
////				case '1':		/* Polling Mode */
////					App_Polling_Test();
////					break;
////
////				case '2':		/* Interrupt Mode */
////					App_Interrupt_Test();
////					break;
////
////				case '3':		/* DMA mode */
////					App_DMA_Test();
////					break;
////				}
////				break;
////			}
////			else if (bufferUART == 'x') {
////				end_Flag = true;
////				DEBUGOUT("\r\nADC demo terminated!");
////			}
////			else if (bufferUART == 'o') {
////				_bitRate -= _bitRate > 0 ? 1000 : 0;
////				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
////				DEBUGOUT("Rate : %d Sample/s\r\n", _bitRate);
////			}
////			else if (bufferUART == 'p') {
////				_bitRate += _bitRate < 400000 ? 1000 : 0;
////				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
////				DEBUGOUT("Rate : %d Sample/s\r\n", _bitRate);
////			}
////			else if (bufferUART == 'b') {
////				Burst_Mode_Flag = !Burst_Mode_Flag;
////				ADCSetup.burstMode = Burst_Mode_Flag;
////				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
////				if (Burst_Mode_Flag) {
////					DEBUGOUT("Burst Mode ENABLED\r\n");
////				}
////				else {
////					DEBUGOUT("Burst Mode DISABLED\r\n");
////				}
////			}
////		}
////	}
//	return 0;
//}
//
///**
// * @}
// */
