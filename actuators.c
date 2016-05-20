#include "actuators.h"
#include "board.h"
#include "time.h"

void actuatorsInit(){

}

void motorActuatorInit(){
	/* Initialize CMP0_IN[3] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, CMP_PORT, CMP_PIN, (IOCON_FUNC5 | IOCON_MODE_INACT | IOCON_ADMODE_EN));

	/* Initialize Comparator */
	Chip_CMP_Init();

	/* Enables current flow for the comparator */
	Chip_CMP_EnableCurrentSrc(CMP_ENCTRL_ENABLE);

	/* Not sure if this is necessary yet */
	Chip_CMP_EnableBandGap(CMP_ENCTRL_ENABLE);

	/* Enables a specific comparator */
	Chip_CMP_Enable(CMP_ID, CMP_ENCTRL_ENABLE);

	/* I have no idea what this does */
	Chip_CMP_SetHysteresis(CMP_ID, CMP_HYS_NONE);

	/* Enable voltage ladder to power up */
	Chip_CMP_EnableVoltLadder(CMP_ID, CMP_ENCTRL_ENABLE);

}

void setLadderValue(uint8_t ladderVal, uint8_t dir){

	/* Set up voltage ladder for particular voltage value */
	Chip_CMP_SetupVoltLadder(CMP_ID, ladderVal, CMP_VREF);

	/* Set up comparator positive and negative inputs */
	if(dir){
		Chip_CMP_SetPosVoltRef(CMP_ID, CMP_INPUT_CMPx_IN3);
		Chip_CMP_SetNegVoltRef(CMP_ID, CMP_INPUT_VREF_DIV);
	}
	else{
		Chip_CMP_SetPosVoltRef(CMP_ID, CMP_INPUT_VREF_DIV);
		Chip_CMP_SetNegVoltRef(CMP_ID, CMP_INPUT_CMPx_IN3);
	}

}

void motorActuatorActuate(uint8_t start, uint8_t dir){
	if(start) {
		if(dir) {
			printf("Started Forward\n");
		} else {
			printf("Started Backward\n");
		}
	} else {
		if(dir) {
			printf("Stopped Forward\n");
		} else {
			printf("Stopped Backward\n");
		}
	}
}

void motorActuatorConnect(){

	setLadderValue(CONNECT_LADDER_VAL, FORWARD);
	delayMs(1);
	motorActuatorActuate(START, FORWARD);
	while (1) {
		if(Chip_CMP_GetCmpStatus(CMP_ID)) {
			motorActuatorActuate(STOP, FORWARD);
			return;
		}
	}
}

void motorActuatorDisconnect(){

	setLadderValue(RETRACT_LADDER_VAL, BACKWARD);
	delayMs(1);
	motorActuatorActuate(START, BACKWARD);
	while (1) {
		if(Chip_CMP_GetCmpStatus(CMP_ID)) {
			motorActuatorActuate(STOP, BACKWARD);
			return;
		}
	}
}

void verticalActuatorsUp(){

}

void verticalActuatorsDown(){

}
