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
#include "I2CPERIPHS.h"

// Initialize all sensor and control systems that are enabled via #-defines in initialization.h!
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
        sensorData.photoelectric = 0.0;
    }

    if(RANGING_SENSORS_ACTIVE){
        rangingSensorsInit();
    }
    if(GPIO_INT_ACTIVE){
        /* Enable GPIO Interrupts */
        GPIO_Interrupt_Enable();
    }

    if(MOTOR_BOARD_I2C_ACTIVE) {
        // Initialize the I^2C buses for communication with the HEMS boards
    	//i2cInit(I2C0, SPEED_100KHZ);
    	i2cInit(I2C1, SPEED_100KHZ);
    	i2cInit(I2C2, SPEED_100KHZ);

    	// Create objects to hold parameters of the HEMS boards
        motors[0] = initialize_HEMS(0);   			// Front Left
        motors[1] = initialize_HEMS(1);            	// Back Left
        motors[2] = initialize_HEMS(2);   			// Back Right
        motors[3] = initialize_HEMS(3);          	// Front Right

    	prototypeRunFlag = 0;
    }

    if (MAGLEV_BMS_ACTIVE){
    	uint8_t i;
    	for (i = 0; i < NUM_MAGLEV_BMS; i++){
    		maglev_bmses[i] = initialize_Maglev_BMS(i);
    	}
    }

    if (CONTACT_SENSOR_ACTIVE){
    	GPIO_Input_Init(GPIO_CONTACT_SENSOR_PORT, GPIO_CONTACT_SENSOR_PIN);
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
