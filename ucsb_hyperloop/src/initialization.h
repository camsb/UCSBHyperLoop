#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

// State machine variables
#define PROTOTYPE_TEST			0
#define PROTOTYPE_PRERUN		0
#define STATEMACHINE_TEST       1
#define NO_OLD_PROTOTYPE_RUN    1

// Sensors and system actuation toggles
#define PWM_ACTIVE				0
#define PHOTO_ELECTRIC_ACTIVE	0
#define RANGING_SENSORS_ACTIVE	0
#define SMOOSHED_ONE_ACTIVE		0
#define SMOOSHED_TWO_ACTIVE		0
#define MOTOR_BOARD_I2C_ACTIVE	1
#define GATHER_DATA_ACTIVE      (RANGING_SENSORS_ACTIVE || SMOOSHED_ONE_ACTIVE || SMOOSHED_TWO_ACTIVE || MOTOR_BOARD_I2C_ACTIVE)

// Communications
#define ETHERNET_ACTIVE         1
#define SDCARD_ACTIVE           0
#define COMMUNICATION_ACTIVE    (ETHERNET_ACTIVE || SDCARD_ACTIVE)

#define GPIO_INT_ACTIVE         (ETHERNET_ACTIVE || PHOTO_ELECTRIC_ACTIVE)

// Initialize all the things
void initializeSensorsAndControls();
void initializeCommunications();

#endif

