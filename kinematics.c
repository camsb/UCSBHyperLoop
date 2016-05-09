#include "kinematics.h"


XYZ getVelocity(){
//	DEBUGOUT( "accelX = %f\t", sensorData.accelX );
//	DEBUGOUT( "accelY = %f\t", sensorData.accelY );
//	DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
	XYZ		velocity;
	static XYZ previousVelocity;

	// Need to subtract calibration?
	velocity.x = sensorData.accelX*1;
	velocity.z = sensorData.accelY;
	velocity.z = sensorData.accelZ;
}
