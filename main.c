/*
 * Main.c
 *
 *  Created on: Apr 1, 2016
 *      Author: benjaminhartl
 */

#include "time.h"
#include "board.h" 
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "lcd.h"

int main(void) {

#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
#endif
#endif
    lcdInit();

    struct Lines lines;
    L = &lines;

    linesInit();
    setLine( 0, "012345678910" );
    setLine( 1, "abcdefghijklm" );
    setLine( 2, "nopqrstuvwxyz" );
    setLine( 3, "~!@#$%^&*()_+-=" );

    timer0Init();
    timer1Init();

    while(1) {  // infinte loop
        __WFI();
    }

    // Currently will never reach this line because of the infinite loop
    linesDeInit();
    return 0;
}

// int main(void)
// {
//     SystemCoreClockUpdate();
//     // Set up and initialize all required blocks and
//     // functions related to the board hardware
//     Board_Init();
//     printf("Celeste is SSOOOO lame! -Bucky\n");
//     uint32_t            temperature;
//     uint32_t            pressure;
//     float*          acceleration;
//     float*          rotation;
//     struct constants    *c;

//     c = ( struct constants * ) calloc( 1, sizeof(struct constants) );

//     i2cInit();

//     initCalibrationData( c );
//     timer0Init();

//     //////////////////////////////////////////////////////////////////
//     // get temperature
// //  temperature = getTemperature( c );
// //  DEBUGOUT( "temperature = %u\n", temperature );

//     //////////////////////////////////////////////////////////////////
//     // get pressure
// //  pressure = getPressure( c );
// //  DEBUGOUT( "pressure = %u\n", pressure );

//     //////////////////////////////////////////////////////////////////
//     // get acceleration
// //  while(1) {
// //          acceleration = getAcceleration();
// //          delay(1);
// //  }
//     while( 1 )
//     {
//         acceleration = getAcceleration();
//         rotation = getRotAcceleration();

//         delay( 100 );
//     }
//     Chip_I2C_DeInit( I2C0 );
//     timer0DeInit();

//     free( c );
//     //Chip_I2C_DeInit(I2C1);

//     return 0;
// }
