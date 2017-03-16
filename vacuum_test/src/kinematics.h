#ifndef KINEMATICS_H_
#define KINEMATICS_H_

#include "sensor_data.h"

XYZ getInertialVelocity();
XYZ getInertialPosition();
void combinePositions();

#endif
