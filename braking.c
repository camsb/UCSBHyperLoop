#include "braking.h"
#include "gpio.h"

void emergencyBrake(){
  emergencyBrakeFlag = 0;
  // Rest of function here.
}

#define EDDY_CURRENT_BRAKE_PORT 4
#define EDDY_CURRENT_BRAKE_PIN  18
#define FRICTIONAL_BRAKE_PORT	4
#define FRICTIONAL_BRAKE_PIN	20

void eddyCurrentBrakeInit(){
	GPIO_Output_Init(EDDY_CURRENT_BRAKE_PORT, EDDY_CURRENT_BRAKE_PIN);
	Chip_GPIO_WritePortBit(LPC_GPIO, EDDY_CURRENT_BRAKE_PORT, EDDY_CURRENT_BRAKE_PIN, 0);
}

void eddyCurrentBrakeEngage(){
	Chip_GPIO_WritePortBit(LPC_GPIO, EDDY_CURRENT_BRAKE_PORT, EDDY_CURRENT_BRAKE_PIN, 1);
}
void eddyCurrentBrakeDisengage(){
	Chip_GPIO_WritePortBit(LPC_GPIO, EDDY_CURRENT_BRAKE_PORT, EDDY_CURRENT_BRAKE_PIN, 0);
}

void frictionalBrakeInit(){
	GPIO_Output_Init(FRICTIONAL_BRAKE_PORT, FRICTIONAL_BRAKE_PIN);
	Chip_GPIO_WritePortBit(LPC_GPIO, FRICTIONAL_BRAKE_PORT, FRICTIONAL_BRAKE_PIN, 0);
}
void frictionalBrakeEngage(){
	Chip_GPIO_WritePortBit(LPC_GPIO, FRICTIONAL_BRAKE_PORT, FRICTIONAL_BRAKE_PIN, 1);
}
void frictionalBrakeDisengage(){
	Chip_GPIO_WritePortBit(LPC_GPIO, FRICTIONAL_BRAKE_PORT, FRICTIONAL_BRAKE_PIN, 0);
}
