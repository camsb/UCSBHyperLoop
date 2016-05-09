#include "kinematics.h"

/* Derive velocity from accelerometer's data */
XYZ getInertialVelocity(){
	float Ts = .1;  // Sampling time.
	XYZ			velocity;

	velocity.x = sensorData.accelX*Ts + sensorData.velocityX;
	velocity.y = sensorData.accelY*Ts + sensorData.velocityY;
	velocity.z = sensorData.accelZ*Ts + sensorData.velocityZ;

	return velocity;
}

/* Derive velocity from accelerometer's data */
XYZ getInertialPosition(){
	float Ts = .1;  // Sampling time.
	XYZ			position;

	position.x = sensorData.velocityX*Ts + sensorData.positionX;
	position.y = sensorData.velocityY*Ts + sensorData.positionY;
	position.z = sensorData.velocityZ*Ts + sensorData.positionZ;

	return position;
}
