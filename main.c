
#include "time.h"
#include "board.h" 
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "lcd.h"
#include "stdio.h"
#include "sensor_data.h"
#include "i2c.h"
#include "photo_electric.h"
#include "ranging.h"

int main(void)
{

//	bool end_Flag = false;
//	uint32_t _bitRate = ADC_MAX_SAMPLE_RATE;
//	uint8_t bufferUART;

	SystemCoreClockUpdate();
	Board_Init();

	Ranging_Init();

	Ranging_Int_Measure();

	while(1) {
		//App_Polling_Test();
		//getShortDistance();
		//printf("Celeste is c00l\n");
		//delay(1);

		//getShortDistance();
		//delay(1);

		if(ADC_Interrupt_Done_Flag == 1) {
			processShortRangingData();

			Ranging_Int_Measure();

			ADC_Interrupt_Done_Flag = 0;
		}
	}
	return 0;
}

//	while (!end_Flag) {
//		while (!end_Flag) {
//			bufferUART = 0xFF;
//			bufferUART = DEBUGIN();
//			if (bufferUART == 'c') {
//				DEBUGOUT(SelectMenu);
//				bufferUART = 0xFF;
//				while (bufferUART == 0xFF) {
//					bufferUART = DEBUGIN();
//					if ((bufferUART != '1') && (bufferUART != '2') && (bufferUART != '3')) {
//						bufferUART = 0xFF;
//					}
//				}
//				switch (bufferUART) {
//				case '1':		/* Polling Mode */
//					App_Polling_Test();
//					break;
//
//				case '2':		/* Interrupt Mode */
//					App_Interrupt_Test();
//					break;
//
//				case '3':		/* DMA mode */
//					App_DMA_Test();
//					break;
//				}
//				break;
//			}
//			else if (bufferUART == 'x') {
//				end_Flag = true;
//				DEBUGOUT("\r\nADC demo terminated!");
//			}
//			else if (bufferUART == 'o') {
//				_bitRate -= _bitRate > 0 ? 1000 : 0;
//				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
//				DEBUGOUT("Rate : %d Sample/s\r\n", _bitRate);
//			}
//			else if (bufferUART == 'p') {
//				_bitRate += _bitRate < 400000 ? 1000 : 0;
//				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
//				DEBUGOUT("Rate : %d Sample/s\r\n", _bitRate);
//			}
//			else if (bufferUART == 'b') {
//				Burst_Mode_Flag = !Burst_Mode_Flag;
//				ADCSetup.burstMode = Burst_Mode_Flag;
//				Chip_ADC_SetSampleRate(_LPC_ADC_ID, &ADCSetup, _bitRate);
//				if (Burst_Mode_Flag) {
//					DEBUGOUT("Burst Mode ENABLED\r\n");
//				}
//				else {
//					DEBUGOUT("Burst Mode DISABLED\r\n");
//				}
//			}
//		}
//	}
//	return 0;
//}


// int main(void)
// {
//
//    /* Initialize the board and clock */
//    SystemCoreClockUpdate();
//    Board_Init();
//
//    i2c_app_init(I2C0, SPEED_100KHZ);
//    initTempPressCalibrationData();
//
//    timer0Init();
//    Photoelectric_Init();
//
//    DEBUGOUT(" UCSB Hyperloop Controller Initialized\n");
//    DEBUGOUT("_______________________________________\n\n");
//
//    gyroAccelXYZ acceleration, rotation;
//
//    while( 1 )
//    {
//
//        if(strip_detected) {
//        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
//        	strip_detected = 0;
//        }
//
//        sensorData.temp = getTemperature();
//        DEBUGOUT( "temperature = %d\n", sensorData.temp );
//
//        sensorData.pressure = getPressure();
//        DEBUGOUT( "pressure = %u\n", sensorData.pressure );
//
//        acceleration = getAccelerometerData();
//        rotation = getGyroscopeData();
//
//        sensorData.accelX = acceleration.x;
//        DEBUGOUT( "accelX = %f\n", sensorData.accelX );
//        sensorData.accelY = acceleration.y;
//        DEBUGOUT( "accelY = %f\n", sensorData.accelY );
//        sensorData.accelZ = acceleration.z;
//        DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
//
//        sensorData.gyroX = rotation.x;
//        DEBUGOUT( "gyroX = %f\n", sensorData.gyroX );
//        sensorData.gyroY = rotation.y;
//        DEBUGOUT( "gyroY = %f\n", sensorData.gyroY );
//        sensorData.gyroZ = rotation.z;
//        DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );
//
//        delay( 100 );
//    }
//
//    return 0;
// }
