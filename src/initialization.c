#include "initialization.h"
#include "temp_press.h"
#include "i2c.h"
#include "photo_electric.h"
#include "ethernet.h"
#include "pwm.h"
#include "sensor_data.h"
#include "communication.h"
#include "sdcard.h"
#include "gpio.h"

void initializeSensorsAndControls(){

    if(PWM_ACTIVE){
        Init_PWM(LPC_PWM1);
        Init_Channel(LPC_PWM1, 1);
        Set_Channel_PWM(LPC_PWM1, 1, 0.5);
    }
    if(GATHER_DATA_ACTIVE){
        gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 10);
    }
    if(SMOOSHED_ONE_ACTIVE){
        i2cInit(I2C1, SPEED_100KHZ);
        smooshedOne = temperaturePressureInit(I2C1);
        collectCalibrationData(I2C1);
        getPressure(smooshedOne, I2C1);
        getPressureFlag = 0;
    }
    if(SMOOSHED_TWO_ACTIVE){
        i2cInit(I2C2, SPEED_100KHZ);
        smooshedTwo = temperaturePressureInit(I2C2);
    }
    if(PHOTO_ELECTRIC_ACTIVE){
        photoelectricInit();
    }

    if(RANGING_SENSORS_ACTIVE){
        rangingSensorsInit();
    }
    if(GPIO_INT_ACTIVE){
        /* Enable GPIO Interrupts */
        GPIO_Interrupt_Enable();
    }

    if(MOTOR_BOARD_I2C_ACTIVE) {
    	//i2cInit(I2C0, SPEED_100KHZ);
    	i2cInit(I2C1, SPEED_100KHZ);
    	i2cInit(I2C2, SPEED_100KHZ);
    	motors[0] = initialize_HEMS(I2C1,0b01001001);	// Front Right
    	motors[1] = initialize_HEMS(I2C1,0);			// Back Right
    	motors[2] = initialize_HEMS(I2C2,0b01001001);	// Front Left
    	motors[3] = initialize_HEMS(I2C2,0);			// Back Left

    	// Enable GPIO interrupt.
    	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, 2, 1 << 11);
    	NVIC_ClearPendingIRQ(GPIO_IRQn);
    	NVIC_EnableIRQ(GPIO_IRQn);

    	prototypeRunFlag = 0;
    }
}

void initializeCommunications(){
    if(SDCARD_ACTIVE) {
        sdcardInit();
    }
    if(ETHERNET_ACTIVE){
        ethernetInit(PROTO_TCP, 0);
        sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 4);

        /* Handle all Wiznet Interrupts, including RECV */
        if(wizIntFlag) {
            wizIntFunction();
        }
    }
};
