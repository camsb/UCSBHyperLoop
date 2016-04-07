#ifndef LCD_H_
#define LCD_H_

#include <cr_section_macros.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// macros, not modified
#define EPIN            10
#define HIGH            1
#define LOW             0
#define NULLCHAR        '\0'
#define NUMOFDATAPINS   11
#define NUMOFCOLS       17
#define NUMOFINITSTEPS  10
#define NUMOFITERATIONS NUMOFCOLS * NUMOFROWS
#define NUMOFROWS       4
#define PRINTDEBUG      FALSE
#define RSPIN           8
#define RWPIN           9
#define SEVENTEENSPACES "                "    // initialize each array to be all spaces
#define SPACE           32
#define TICKRATE_HZ1    200                   // Not sure the ticks per second, but can go higher to be instant, ex: 1000
#define TICKRATE_HZ2    2                     // 1 number of ticks per second


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// structs
struct Lines {
  int   row;                 // the line number
  int   column;              // points to the current letter to be printed
  char* lines[ NUMOFROWS ];  // array of 4 pointers to chars
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// arrays, not modified
//int  DATAPINS[]         = {    3,   14,   22,   27,   25,   15,   23,   26,   20,   24,   28, }; // need to keep in correct order
//int  DATAPORTS[]        = {    5,    2,    2,    2,    2,    2,    2,    2,    1,    1,    1, }; // ports with matching pins
int  DATAPINS[]         = {    3,    4,    5,    6,    7,    8,    9,   10,    0,    1,    2, }; // need to keep in correct order
int  DATAPORTS[]        = {    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3, }; // ports with matching pins
char INITVALUES[]       = { 0x30, 0x30, 0x30, 0x38, 0x10, 0xC0, 0x06, 0x01, 0x0F, 0x0D };        // hex values of initialization commands for LCD
char LINES[]            = { 0x80, 0xC0, 0x90, 0xD0 };                                            // start location of each line

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// globals, are modified
int    CharCount        = 0;
int    InitCount        = 0;
int    InterruptCount   = 0;
int    IsNewData        = TRUE;
struct Lines* L         = NULL;
int    TIMER1COUNT      = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// function declarations
void command( int v );
char convertChar( char c );
void flipE();
void incrementColumn();
void incrementRow();
void lcdInit();
void linesDeInit();
void linesInit();
void nextChar();
void nextInit();
void setData( int v );
void setDataPinsToOutputAndLow();
void setLine( int lineNum, char* str );
void setRS(   int v );
void setRW(   int v );
void timer0Init();
void timer1Init();
void write(   int v );

void TIMER0_IRQHandler();
void TIMER1_IRQHandler();

#endif /* LCD_H_ */
