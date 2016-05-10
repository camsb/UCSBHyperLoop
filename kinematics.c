#include "kinematics.h"

/* Derive velocity from accelerometer's data */
XYZ getInertialVelocity(){
	float Ts = .1;  // Sampling time.
	XYZ			velocity;

	float alpha = 0.4;
	float beta = 1 - alpha;

	/* Update v[k] = a*t + v[k-1]  */
	velocity.x = sensorData.accelX*Ts + sensorData.velocityX;
	velocity.y = sensorData.accelY*Ts + sensorData.velocityY;
	velocity.z = sensorData.accelZ*Ts + sensorData.velocityZ;

	/* Moving average */
	velocity.x = alpha*velocity.x + beta*sensorData.velocityX;
	velocity.y = alpha*velocity.y + beta*sensorData.velocityY;
	velocity.z = alpha*velocity.z + beta*sensorData.velocityZ;

	return velocity;
}

/* Derive velocity from accelerometer's data */
XYZ getInertialPosition(){
	float Ts = .1;  // Sampling time.
	XYZ			position;

	float alpha = 0.4;
	float beta = 1 - alpha;

	/* Update p[k] = v*t + p[k-1] */
	position.x = sensorData.velocityX*Ts + sensorData.positionX;
	position.y = sensorData.velocityY*Ts + sensorData.positionY;
	position.z = sensorData.velocityZ*Ts + sensorData.positionZ;

	/* Moving average */
	position.x = alpha*position.x + beta*sensorData.positionX;
	position.y = alpha*position.y + beta*sensorData.positionY;
	position.z = alpha*position.z + beta*sensorData.positionZ;

	return position;
}
