#ifndef LOGGING_H_
#define LOGGING_H_

#include <string.h>

#include "ethernet.h"
#include "subsystems.h"

typedef enum {
	MAX_EVT
} Event;

typedef enum {
	MAX_ERR
} Error;

void initEventLogFile();
void initErrorLogFile();
void logEventString(char* desc);
void logErrorString(char* desc);
void logStateMachineEvent(int sig);
void logEvent(Event evt);
void logError(Error err);

#endif // LOGGING_H_
