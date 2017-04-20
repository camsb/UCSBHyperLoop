#ifndef COMMUNICATION_H__
#define COMMUNICATION_H__

#include "board.h"

#define COMMSIZE (0x0800) - 4

uint8_t Comm_Data[COMMSIZE];

struct comm_packet {
	// all pointers are ini
	// pass in pointer to null terminated data string
	// packet construction will be taken care of by function
	// format with ###.## zero padded

	/* Atmospheric Data */
	char bm1[6];	// Pressure1
	char bm2[6];	// Pressure2
	char tm1[6];	// Temperature1
	char tm2[6];	// Temperature2
	char tm3[6];	// Temperature3
	char tm4[6];	// Temperature4
	char ta1[6];	// Tachometer1
	char ta2[6];	// Tachometer2
	char ta3[6];	// Tachometer3
	char ta4[6];	// Tachometer4
	char pwr[6];	// Power Consumption
	/* Positional Data */
	char pox[6];	// x Position
	char poy[6];	// y Position
	char poz[6];	// z Position
	/* Velocity Data */
	char vex[6];	// x Velocity
	char vey[6];	// y Velocity
	char vez[6];	// z Velocity
	/* Acceleration Data */
	char acx[6];	// x Acceleration
	char acy[6];	// y Acceleration
	char acz[6];	// z Acceleration
	/* Attitudinal Data */
	char rol[6];	// Roll
	char pit[6];	// Pitch
	char yaw[6];	// Yaw

};

struct comm_packet CommPacket;

void logData();
void sendToWebAppSDCard();
void send_comm_packet_helper(char *method, char *val, int *position);

#endif
