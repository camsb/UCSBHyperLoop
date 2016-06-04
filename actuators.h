#ifndef ACTUATORS_H_
#define ACTUATORS_H_

#include "board.h"

#define CMP_ID		0
#define CMP_VREF	0
#define CONNECT_LADDER_VAL	28 // Should complete actuation at ~30
#define RETRACT_LADDER_VAL  2
#define CMP_PORT	1
#define CMP_PIN		6
#define FORWARD		1
#define BACKWARD 	0
#define START		1
#define STOP		0
#define SLEEP		0
#define WAKE		1
#define MOTOR_ACTUATOR_PORT 		4
#define MOTOR_ACTUATOR_DIR_PIN		22
#define MOTOR_ACTUATOR_START_PIN	23
#define MOTOR_ACTUATOR_SLEEP_PIN	24

void actuatorsInit();
void motorActuatorConnect();
void motorActuatorDisconnect();
void verticalActuatorsInit();
void verticalActuatorsActuate(uint8_t start, uint8_t dir);
void verticalActuatorsUp();
void verticalActuatorsDown();
void motorActuatorInit();
void motorActuatorActuate(uint8_t start, uint8_t dir);

#endif
