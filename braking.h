#ifndef BRAKING_H_
#define BRAKING_H_

#include "board.h"

#define EDDY_CURRENT_BRAKE_PORT 4
#define EDDY_CURRENT_BRAKE_PIN  18
#define FRICTIONAL_BRAKE_PORT	4
#define FRICTIONAL_BRAKE_PIN	20

void emergencyBrake();
void eddyCurrentBrakeInit();
void eddyCurrentBrakeEngage();
void eddyCurrentBrakeDisengage();
void frictionalBrakeInit();
void frictionalBrakeEngage();
void frictionalBrakeDisengage();

uint8_t emergencyBrakeFlag;

#endif
