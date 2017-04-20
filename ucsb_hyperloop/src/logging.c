#include "logging.h"
#include "subsystems.h"
#include "initialization.h"

void initEventLogFile()
{

}

void initErrorLogFile()
{

}

void logEventString(char* desc)
{
	// ADD GUARDS for web app or SD card here!
#if ETHERNET_ACTIVE
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
#endif
#if SDCARD_ACTIVE
	// Save to SD card file.
#endif

}

void logErrorString(char* desc)
{
	// Send to web app.

	// Save to SD card file.

}

void logStateMachineEvent(int sig)
{
	// Get the name of a control signal that
	char desc[64] = "Control signal: ";
	char *sig_desc;
	sig_desc = control_signal_names[sig];
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
