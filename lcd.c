
#include "lcd.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// from timer.c                                         

  const int DATAPINS[11]    = {    3,    4,    5,    6,    7,    8,    9,   10,    0,    1,    2 };
  const char INITVALUES[10] = { 0x30, 0x30, 0x30, 0x38, 0x10, 0xC0, 0x06, 0x01, 0x0F, 0x0D };        
  const char LINES[4]       = { 0x80, 0xC0, 0x90, 0xD0 }; 
  int CharCount        = 0;
  int InitCount        = 0;
  int InterruptCount   = 0;
  int IsNewData        = TRUE;
  Lines L;
  int TIMER1COUNT      = 0;

void writeLine(int line){
  command(LINES[1]);
  delay(1);
  write('a');
}

// void TIMER0_IRQHandler()
// {
//   if ( Chip_TIMER_MatchPending( LPC_TIMER0, 1 ) )
//   {
//     if( IsNewData )
//     {
//       if( InitCount < NUMOFINITSTEPS )
//       {
//         nextInit();
//         if( PRINTDEBUG ) printf( "Initializing Step %i: command( %i )\n", InitCount, INITVALUES[ InitCount ] );
//         InitCount++;
//       }
//       else // after initialization, it should only go here
//       {
//         InterruptCount++;                 //
//         nextChar();   // DO NOT increment COUNT so it will always go to this statement
//         if( PRINTDEBUG ) printf( "Row %i: Column %i: write( %i )\n", L.row, L.column, L.lines[ L.row ][ L.column ] );

//         if( InterruptCount >= NUMOFITERATIONS )
//         {
//           IsNewData      = FALSE;
//           InterruptCount = 0;
//         }
//       }
//       Chip_TIMER_ClearMatch( LPC_TIMER0, 1 );
//     }
//   }

//   return;
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// from timer.c
// void TIMER1_IRQHandler()
// {
//   if ( Chip_TIMER_MatchPending( LPC_TIMER1, 1 ) )
//   {
//     TIMER1COUNT++;

//     if( TIMER1COUNT == 5 )
//     {
//       TIMER1COUNT = 0;

//       if( PRINTDEBUG ) printf( "TIMER1IRQHIT = %i times\n", TIMER1COUNT );

//       int i;
//       int j;
//       char array[ NUMOFCOLS ];

//       for( i = 0; i < NUMOFROWS; i++ )
//       {
//         for( j = 0; j< NUMOFCOLS; j++ )
//         {
//           array[ j ] = L.lines[ i ][ j ] + 1;
//         }

//         setLine( i, array );
//       }
//     }
//     Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
//   }

//   return;
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Instruction
void command( int v )
{
  setData( v );
  setRS( LOW );
  setRW( LOW );
  flipE();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// convert the value of each char to the lcd equivalent
char convertChar( char c )
{
  char returnValue;

  if( c > 9 )
    returnValue = c;
  else
    returnValue = c + 48;

  return returnValue;
}

// send values to LCD screen
void flipE()
{
  Chip_GPIO_SetPinOutHigh( LPC_GPIO, LCDPORT, DATAPINS[ EPIN ] );
  Chip_GPIO_SetPinOutLow( LPC_GPIO, LCDPORT, DATAPINS[ EPIN ] );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void incrementColumn()
{
  L.column++;

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// get the new row number, and reset the column to 0
void incrementRow()
{
  if( L.row == NUMOFROWS - 1 ) // check if row overflows
    L.row = 0;
  else
    L.row++;

  L.column = 0;

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void startLCD(){
  int i;
  for(i = 0; i < 10; i++){
    delay(30);
    command(INITVALUES[ i ]);
  }
}

void lcdInit()
{
  setDataPinsToOutputAndLow();
  startLCD();
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// globals, are modified 
  
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// free the space malloced in lines
void linesDeInit()
{
  int i;
  for( i = 0; i < NUMOFROWS; i++ )
    free( L.lines[ i ] );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// malloc space for each line for L to have access
void linesInit()
{
  int i;
  for( i = 0; i < NUMOFROWS; i++ )
  {
    L.lines[ i ] = malloc( NUMOFCOLS * sizeof( char ) );             // malloc each line
    L.lines[ i ][ NUMOFCOLS - 1 ] = LINES[ ( i + 1 ) % NUMOFROWS ];  // set the last value to address of next value
  }
                            // L -> lines                                       address
                              //                  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _______
  L.row    = 0;            // L.lines[ 0 ] = |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_| LINE1 |
  L.column = 0;            // L.lines[ 1 ] = |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_| LINE2 |
                              // L.lines[ 2 ] = |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_| LINE3 |
  return;                   // L.lines[ 3 ] = |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_| LINE0 |
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// either write the next char or move to the next line
void nextChar()
{
  int value = L.lines[ L.row ][ L.column ];
  if( ( L.column + 1 ) - NUMOFCOLS == 0 ) // need to go to the next line
  {
    //printf( "NEXTLINE\n" );
    command( value );
    incrementRow();
  }
  else                             // just place the next char in line
  {
    write( value );
    incrementColumn();
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void nextInit()
{
  command( INITVALUES[ InitCount ] );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// set the 8 data pins
void setData( int v )
{
  int i;
  for( i = 0; i < 8; i++ )
  {
    char highLow = ( v >> ( 7 - i ) ) & 1;
    if( highLow == 0 )
      Chip_GPIO_SetPinOutLow(  LPC_GPIO, LCDPORT, DATAPINS[ 7 - i ] );
    else
      Chip_GPIO_SetPinOutHigh( LPC_GPIO, LCDPORT, DATAPINS[ 7 - i ] );
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize the data pins to be output and low
void setDataPinsToOutputAndLow()
{
  int i;
  for( i = 0; i < NUMOFDATAPINS; i++ )
  {
    Chip_IOCON_PinMuxSet(LPC_IOCON, LCDPORT, DATAPINS[ i ], (IOCON_FUNC0));
    Chip_GPIO_SetPinDIROutput( LPC_GPIO, LCDPORT, DATAPINS[ i ] );
    Chip_GPIO_SetPinOutLow(    LPC_GPIO, LCDPORT, DATAPINS[ i ] );
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// write the values to each line
void setLine( int lineNum, char* str )
{
  int  i;
  int  placeInt;                      // toggle when end of string is reached to place blanks
  char placeValue;

  placeInt = TRUE;

  for( i = 0; i < NUMOFCOLS - 1; i++ ) // iterate through all but the last column, it holds next line
  {
    if( str[ i ] == NULLCHAR )       // check if need to place spaces or each char
      placeInt = FALSE;

    if( placeInt )                  // print str to line
      placeValue = str[ i ];
    else                             // print spaces
      placeValue = SPACE;

    L.lines[ lineNum ][ i ] = convertChar( placeValue );

    if( PRINTDEBUG ) printf( "%i: %i = %i\n", lineNum, L.column, L.lines[ lineNum ][ i ] );
  }

  IsNewData = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// set Register Select pin
//    0 = Instruction
//    1 = Place Character
void setRS( int v )
{
  if( v == LOW )
    Chip_GPIO_SetPinOutLow( LPC_GPIO, LCDPORT, DATAPINS[ RSPIN ] );
  else
    Chip_GPIO_SetPinOutHigh( LPC_GPIO, LCDPORT, DATAPINS[ RSPIN ] );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// set Read/Write pin
//    0 = Writing Data
//    1 = Reading Data
void setRW( int v )
{
  if( v == LOW )
    Chip_GPIO_SetPinOutLow( LPC_GPIO, LCDPORT, DATAPINS[ RWPIN] );
  else
    Chip_GPIO_SetPinOutHigh( LPC_GPIO, LCDPORT, DATAPINS[ RWPIN ] );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// create timer that calls interrupt

// void timer0Init()
// {
//     /* Enable timer 1 clock */
//   uint32_t timerFreq;

//     Chip_TIMER_Init( LPC_TIMER0 );

//     /* Timer rate is system clock rate */
//     timerFreq = Chip_Clock_GetSystemClockRate();

//     /* Timer setup for match and interrupt at TICKRATE_HZ */
//   Chip_TIMER_Reset( LPC_TIMER0 );
//   Chip_TIMER_MatchEnableInt( LPC_TIMER0, 1 );
//   Chip_TIMER_SetMatch( LPC_TIMER0, 1, ( timerFreq / TICKRATE_HZ1 ) );
//   Chip_TIMER_ResetOnMatchEnable( LPC_TIMER0, 1 );
//   Chip_TIMER_Enable( LPC_TIMER0 );

//   /* Enable timer interrupt */
//   NVIC_ClearPendingIRQ( TIMER0_IRQn );
//   NVIC_EnableIRQ( TIMER0_IRQn );

//   return;
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// create timer that calls interrupt
void timer1Init()
{
    /* Enable timer 1 clock */
  uint32_t timerFreq;

    Chip_TIMER_Init( LPC_TIMER1 );

    /* Timer rate is system clock rate */
    timerFreq = Chip_Clock_GetSystemClockRate();

    /* Timer setup for match and interrupt at TICKRATE_HZ */
  Chip_TIMER_Reset( LPC_TIMER1 );
  Chip_TIMER_MatchEnableInt( LPC_TIMER1, 1 );
  Chip_TIMER_SetMatch( LPC_TIMER1, 1, ( timerFreq / TICKRATE_HZ2 ) );
  Chip_TIMER_ResetOnMatchEnable( LPC_TIMER1, 1 );
  Chip_TIMER_Enable( LPC_TIMER1 );

  /* Enable timer interrupt */
  NVIC_ClearPendingIRQ( TIMER1_IRQn );
  NVIC_EnableIRQ( TIMER1_IRQn );

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Write Character
void write( int v )
{
  setData( v );
  setRS( HIGH );
  setRW( LOW );
  flipE();

  return;
}

