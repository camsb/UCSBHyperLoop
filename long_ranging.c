#include "ranging.h"

///* Convert voltage */
//void convertVoltageLong(uint8_t sensor)
//{
//	uint16_t index;
//
//	float voltage = ((float)LongRangingDataRaw[sensor]) / 1300;
//
//	if ((voltage < 0.34) || (voltage > 2.43)) {
//		DEBUGOUT("ADC voltage of %.3f V is out of operating range.\n", voltage);
//	}
//	else {
//		index = (uint16_t)(voltage * 100.0 + 0.5) - 33;
//		LongRangingMovingAverage[sensor] = ALPHA*LongRangingMovingAverage[sensor] + BETA*longRangingDistanceLUT[index];
//		DEBUGOUT("%.3f V -- %.3f cm at index %d\n", voltage, LongRangingMovingAverage[sensor],  index);
//	}
//}
//
///* Process short ranging data */
//void processLongRangingData(void) {
//	convertVoltageLong(0);
//	convertVoltageLong(1);
//	convertVoltageLong(2);
//	convertVoltageLong(3);
//}
