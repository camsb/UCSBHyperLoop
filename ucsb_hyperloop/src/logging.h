#ifndef LOGGING_H_
#define LOGGING_H_

#include <string.h>

#include "ethernet.h"
#include "subsystems.h"

#define FILE_POSITION "position"
#define FILE_HEMS "maglev"
#define FILE_BMS "bms"

typedef enum {
	MAX_EVT
} Event;

typedef enum {
	MAX_ERR
} Error;

#define ERR

void logData();
void logPosition();
void logHEMS(int index);
void logBMS(int index);

void initEventLogFile();
void initErrorLogFile();
void logEventString(char* desc);
void logErrorString(char* desc);
void logStateMachineEvent(int sig);
void logEvent(Event evt);
void logError(Error err);

#endif // LOGGING_H_
