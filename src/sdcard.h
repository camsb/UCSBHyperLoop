#ifndef __SD_CARD_H
#define __SD_CARD_H

#include <string.h>
#include "board.h"
#include "chip.h"
#include "ff.h"

#define debugstr(str)  DEBUGOUT(str)

/* buffer size (in byte) for R/W operations */
#define SD_BUFFER_SIZE     4096

enum sd_file_indicies {
	LOGFILE = 0,
	DATAFILE,
	MAX_FILES
};

FIL sd_files[MAX_FILES];

void sdcardInit();
void readTest();
void writeTest();
void writeData(char* filename, const void *buff, int size);
void readData(char* filename);

// Open/close a new set of files for this session.
void initSessionFiles();
void deinitSessionFiles();


#endif	// __SD_CARD_H
