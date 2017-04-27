#include "logging.h"
#include "subsystems.h"
#include "ethernet.h"
#include "HEMS.h"
#include "sensor_data.h"
#include "sdcard.h"

void get_filepath(char* filepath, char* dir, char* name, int index) {
	// Concat the dir and name variables and save to the filepath variable.

}

void initSDCard() {
	// Create a new directory for this session.
	char path[32];


	// Create new files and add headers.
	create_position_csv();
	int i;
	for(i=0; i<4; i++) {
		create_hems_csv(i);
	}

}

void create_csv(char* dir, char* filetype, int index)
{
	FIL fileObj;	/* File object */
	char filepath[32];
	get_filepath(filepath, dir, filetype, index);
	f_open(&fileObj, filepath, FA_WRITE | FA_CREATE_ALWAYS);

	// Add headers.
	if(strcmp(filetype, FILE_POSITION) == 0) {

	}
	if(strcmp(filetype, FILE_HEMS) == 0) {

	}
	if(strcmp(filetype, FILE_BMS) == 0) {

	}

	f_close(&fileObj);
}

void logData(){
	ethernet_prepare_packet();
	
	logPosition();

	int i;
	for(i=0; i<4; i++) {
		logHEMS(i);
	}

	// logBMS()

	if((sendDataFlag && connectionOpen))
		ethernet_send_packet();

}

void logPosition(){

	char data[6];
	sprintf(data, "%06.2f", sensorData.photoelectric);
	ethernet_add_data_to_packet(PH, -1, data);
}

void logHEMS(int index){
	char data[6];
	// DAC
	if(index == 0) {
		sprintf(data, "%06.2f", motors[index]->throttle_voltage);
		ethernet_add_data_to_packet(DAC, -1, data);
	}

	// Current
	sprintf(data, "%06.2f", (float)motors[index]->amps);
	ethernet_add_data_to_packet(CU, index, data);

	// RPM
	sprintf(data, "%06.2f", (float)motors[index]->rpm[1]);
	ethernet_add_data_to_packet(TA, index, data);

	// Temperature (0 to 3)
	int i;
	for(i=0; i<4; i++){
		sprintf(data, "%06.f", (float)motors[index]->temperatures[i]);
		ethernet_add_data_to_packet(TM, index*4 + i, data);
	}

	// Short Ranging
	sprintf(data, "%06.2f", motors[index]->short_data[0]);
	ethernet_add_data_to_packet(SR, index, data);

}

void logBMS(int index){

}


void initEventLogFile()
{

}

void initErrorLogFile()
{

}

void logEventString(char* desc)
{
	// Send to web app.
	// Copy strings to Net_Tx_Data
//	int pos = 0;
//	memset(Net_Tx_Data, 0, 512); // Make sure this clears enough space

	/* DAC Output */
//	char errorMsg[512];

//	sprintf(errorMsg, "%s", desc);

	/* DAC Data */
//	send_data_packet_helper(DAC, errorMsg, &pos);

//	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);
	// Save to SD card file.

}

void logErrorString(char* desc)
{
	// Send to web app.

	// Save to SD card file.

}

void logStateMachineEvent(int sig)
{
	char desc[64] = "Control signal: ";
	char *sig_desc;
	switch(sig)
	{
		case CS_GO:							sig_desc = "GO";							break;
		case CS_ALL_STOP:					sig_desc = "ALL_STOP";						break;
		case CS_BRAKES_ENGAGE:				sig_desc = "BRAKES_ENGAGE";					break;
		case CS_BRAKES_DISENGAGE:			sig_desc = "BRAKES_DISENGAGE";				break;
		case CS_BRAKES_EMERGENCY:			sig_desc = "BRAKES_EMERGENCY";				break;
		case CS_BRAKES_EMERGENCY_RELEASE:	sig_desc = "BRAKES_EMERGENCY_RELEASE";		break;
		case CS_BRAKES_TEST_ENTER:			sig_desc = "BRAKES_TEST_ENTER";				break;
		case CS_BRAKES_TEST_EXIT:			sig_desc = "BRAKES_TEST_EXIT";				break;
		case CS_ACTUATORS_RAISE:			sig_desc = "ACTUATORS_RAISE";				break;
		case CS_ACTUATORS_LOWER:			sig_desc = "ACTUATORS_LOWER";				break;
		case CS_MAGLEV_ENGAGE:				sig_desc = "MAGLEV_ENGAGE";					break;
		case CS_MAGLEV_DISENGAGE:			sig_desc = "MAGLEV_DISENGAGE";				break;
		case CS_SURFPROP_ACTUATOR_LOWER:	sig_desc = "SURFPROP_ACTUATOR_LOWER";		break;
		case CS_SURFPROP_ACTUATOR_RAISE:	sig_desc = "SURFPROP_ACTUATOR_RAISE";		break;
		case CS_SURFPROP_ENGAGE_FORWARD:	sig_desc = "SURFPROP_ENGAGE_FORWARD";		break;
		case CS_SURFPROP_ENGAGE_REVERSE:	sig_desc = "SURFPROP_ENGAGE_REVERSE";		break;
		case CS_SURFPROP_DISENGAGE:			sig_desc = "SURFPROP_DISENGAGE";			break;
		default: 							sig_desc = "UNKNOWN_SIG";					break;
	}
	strcat(desc, sig_desc);
	logEventString(desc);
}

void logEvent(Event evt)
{
	char desc[64] = "Event: ";
	char *evt_desc;
	switch(evt)
	{
		// case ???:				evt_desc = "???";					break;
		default:					evt_desc = "UNKNOWN_EVT";			break;
	}
	strcat(desc, evt_desc);
	logEventString(desc);
}

void logError(Error err)
{
	char desc[64] = "Error: ";
	char *err_desc;
	switch(err)
	{
		// case ???:				err_desc = "???";					break;
		default:					err_desc = "UNKNOWN_ERR";			break;
	}
	strcat(desc, err_desc);
	logEventString(desc);
}
