#include "logging.h"

void initEventLogFile()
{

}

void initErrorLogFile()
{

}

void logEventString(char* desc)
{
	// Send to web app.

	// Save to SD card file.

}

void logErrorString(char* desc)
{
	// Send to web app.

	// Save to SD card file.

}

void logStateMachineEvent(int sig)
{
	char desc[64] = "State machine signal: ";
	char *sig_desc;
	switch(sig)
	{
		case STOP_SIG:				sig_desc = "STOP_SIG";				break;
		case FORWARD_SIG:			sig_desc = "FORWARD_SIG";			break;
		case REVERSE_SIG:			sig_desc = "REVERSE_SIG";			break;
		case ENGAGE_ENGINES_SIG:	sig_desc = "ENGAGE_ENGINES_SIG";	break;
		case DISENGAGE_ENGINES_SIG: sig_desc = "DISENGAGE_ENGINES_SIG";	break;
		case ENGAGE_BRAKES_SIG:		sig_desc = "ENGAGE_BRAKES_SIG";		break;
		case DISENGAGE_BRAKES_SIG:	sig_desc = "DISENGAGE_BRAKES_SIG";	break;
		case ENGINES_REVED_SIG:		sig_desc = "DISENGAGE_BRAKES_SIG";	break;
		case ENGINES_STOPPED_SIG:	sig_desc = "ENGINES_STOPPED_SIG";	break;
		case TERMINATE_SIG:			sig_desc = "TERMINATE_SIG";			break;
		default:					sig_desc = "UNKNOWN_SIG";			break;
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
