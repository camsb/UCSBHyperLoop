#include "logging.h"
#include "subsystems.h"

void initEventLogFile()
{

}

void initErrorLogFile()
{

}

void logEventString(char* desc)
{
	// ADD GUARDS for web app or SD card here!

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
		case CS_BRAKES_TEST_ENTER:			sig_desc = "BRAKES_TEST_ENTER";				break;
		case CS_BRAKES_TEST_EXIT:			sig_desc = "BRAKES_TEST_EXIT";				break;
		case CS_ACTUATORS_RAISE:			sig_desc = "ACTUATORS_RAISE";				break;
		case CS_ACTUATORS_LOWER:			sig_desc = "ACTUATORS_LOWER";				break;
		case CS_MAGLEV_ENGAGE:				sig_desc = "MAGLEV_ENGAGE";					break;
		case CS_MAGLEV_DISENGAGE:			sig_desc = "MAGLEV_DISENGAGE";				break;
		case CS_SERVPROP_ACTUATOR_LOWER:	sig_desc = "SERVPROP_ACTUATOR_LOWER";		break;
		case CS_SERVPROP_ACTUATOR_RAISE:	sig_desc = "SERVPROP_ACTUATOR_RAISE";		break;
		case CS_SERVPROP_ENGAGE_FORWARD:	sig_desc = "SERVPROP_ENGAGE_FORWARD";		break;
		case CS_SERVPROP_ENGAGE_REVERSE:	sig_desc = "SERVPROP_ENGAGE_REVERSE";		break;
		case CS_SERVPROP_DISENGAGE:			sig_desc = "SERVPROP_DISENGAGE";			break;
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
