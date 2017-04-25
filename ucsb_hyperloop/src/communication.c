#include "communication.h"
#include "ethernet.h"
#include "sdcard.h"
#include "sensor_data.h"
#include "initialization.h"

void logData(){
    if (ETHERNET_ACTIVE){
        /* Need to do this cleanly, on a timer to prevent multiple attempts before a response */
        if(!connectionOpen && !connectionClosed && sendDataFlag) {
            sendDataFlag = 0;
            ethernetInit(PROTO_TCP, 0);
        }

        /* Handle all Wiznet Interrupts, including RECV */
        if(wizIntFlag) {
            wizIntFunction();
        }

        /* If Data Send Requested, Send Data */
        /* Function to write to SD card and Web App will be here*/
        if((sendDataFlag && connectionOpen)) {
            sendToWebAppSDCard();
        }

        if(SDCARD_ACTIVE) {
            DEBUGOUT("Starting sendToWebAppSDCard\n");
            sendToWebAppSDCard();
            DEBUGOUT("Completed sendToWebAppSDCard\n");
            while(1);
        }
    }
}

void send_comm_packet_helper(char *method, char *val, int *position) {
	if (val != 0) {
		memcpy(Comm_Data + *position, method, 3);
		*position += 3;
		memcpy(Comm_Data + *position, ":", 1);
		*position += 1;
		memcpy(Comm_Data + *position, val, 6);
		*position += 6;
		memcpy(Comm_Data + *position, "\n", 1);
		*position += 1;
	}
}

void sendToWebAppSDCard(){

	sendPrototypePacket();
	/* Atmospheric, Miscellaneous Data */
//	sprintf(CommPacket.bm1, "%06.2f", sensorData.pressure1);
//	sprintf(CommPacket.bm2, "%06.2f", sensorData.pressure2);
//	sprintf(CommPacket.tm1, "%06.2f", sensorData.temp1);
//	sprintf(CommPacket.tm2, "%06.2f", sensorData.temp2);
//	sprintf(CommPacket.tm3, "%06.2f", sensorData.temp3);
//	sprintf(CommPacket.tm4, "%06.2f", sensorData.temp4);
//	sprintf(CommPacket.th1, "%06.2f", sensorData.therm1);
//	sprintf(CommPacket.th2, "%06.2f", sensorData.therm2);
//	sprintf(CommPacket.th3, "%06.2f", sensorData.therm3);
//	sprintf(CommPacket.th4, "%06.2f", sensorData.therm4);
//	sprintf(CommPacket.pwr, "%06.2f", sensorData.power);
//	/* Positional Data */
//	sprintf(CommPacket.pox, "%06.2f", sensorData.positionX);
//	sprintf(CommPacket.poy, "%06.2f", sensorData.positionY);
//	sprintf(CommPacket.poz, "%06.2f", sensorData.positionZ);
//	/* Velocity Data */
//	sprintf(CommPacket.vex, "%06.2f", sensorData.velocityX);
//	sprintf(CommPacket.vey, "%06.2f", sensorData.velocityY);
//	sprintf(CommPacket.vez, "%06.2f", sensorData.velocityZ);
//	/* Acceleration Data */
//	sprintf(CommPacket.acx, "%06.2f", sensorData.accelX);
//	sprintf(CommPacket.acy, "%06.2f", sensorData.accelY);
//	sprintf(CommPacket.acz, "%06.2f", sensorData.accelZ);
//	/* Attitudinal Data */
//	sprintf(CommPacket.rol, "%06.2f", sensorData.roll);
//	sprintf(CommPacket.pit, "%06.2f", sensorData.pitch);
//	sprintf(CommPacket.yaw, "%06.2f", sensorData.yaw);
//
//	int pos = 0;
//	memset(Comm_Data, 0, 256); // Make sure this clears enough space
//
//	/* Atmospheric, Miscellaneous Data */
//	send_comm_packet_helper(BM1, CommPacket.bm1, &pos);
//	send_comm_packet_helper(BM2, CommPacket.bm2, &pos);
//	send_comm_packet_helper(TM1, CommPacket.tm1, &pos);
//	send_comm_packet_helper(TM2, CommPacket.tm2, &pos);
//	send_comm_packet_helper(TM3, CommPacket.tm3, &pos);
//	send_comm_packet_helper(TM4, CommPacket.tm4, &pos);
//	send_comm_packet_helper(TH1, CommPacket.th1, &pos);
//	send_comm_packet_helper(TH2, CommPacket.th2, &pos);
//	send_comm_packet_helper(TH3, CommPacket.th3, &pos);
//	send_comm_packet_helper(TH4, CommPacket.th4, &pos);
//	send_comm_packet_helper(PWR, CommPacket.pwr, &pos);
//	/* Positional Data */
//	send_comm_packet_helper(POX, CommPacket.pox, &pos);
//	send_comm_packet_helper(POY, CommPacket.poy, &pos);
//	send_comm_packet_helper(POZ, CommPacket.poz, &pos);
//	/* Velocity Data */
//	send_comm_packet_helper(VEX, CommPacket.vex, &pos);
//	send_comm_packet_helper(VEY, CommPacket.vey, &pos);
//	send_comm_packet_helper(VEZ, CommPacket.vez, &pos);
//	/* Acceleration Data */
//	send_comm_packet_helper(ACX, CommPacket.acx, &pos);
//	send_comm_packet_helper(ACY, CommPacket.acy, &pos);
//	send_comm_packet_helper(ACZ, CommPacket.acz, &pos);
//	/* Attitudinal Data */
//	send_comm_packet_helper(ROL, CommPacket.rol, &pos);
//	send_comm_packet_helper(PIT, CommPacket.pit, &pos);
//	send_comm_packet_helper(YAW, CommPacket.yaw, &pos);
//
//	memcpy(Comm_Data + pos, "\r\n", 2);
//
//    writeData("TIME1.TXT", Comm_Data, 256);
//    readData("TIME1.TXT");
    return;
}
