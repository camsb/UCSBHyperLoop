#ifndef LCD_H_
#define LCD_H_

#include <cr_section_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"

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
//#define TICKRATE_HZ1    20000                   // Not sure the ticks per second, but can go higher to be instant, ex: 1000
#define TICKRATE_HZ2    2                     // 1 number of ticks per second
#define LCDPORT         3

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// structs
typedef struct Lines {
  int   row;                 // the line number
  int   column;              // points to the current letter to be printed
  char* lines[ NUMOFROWS ];  // array of 4 pointers to chars
} Lines;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// function declarations
void writeLine(int line);
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
void startLCD();

void TIMER0_IRQHandler();
void TIMER1_IRQHandler();

#endif /* LCD_H_ */
