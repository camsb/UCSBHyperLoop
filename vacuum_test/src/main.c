/* UC Santa Barbara Hyperloop Team
 *
 * UCSB Hyperloop Vacuum Testing
 *
 * Ricky Castro
 * Yang Ren
 *
 */

// Standard C/LPC40XX libraries
#include "stdlib.h"
#include "stdio.h"
#include "board.h"
#include "time.h"

// Hyperloop specific libraries
#if 0
#include "initialization.h"
#include "sensor_data.h"
#include "communication.h"
#include "gpio.h"
#include "qpn_port.h"
#include "state_machine.h"
#include "actuation.h"
#include "ethernet.h"
#endif // 0
#include "timer.h"

#define LED_PORT	5
#define LED_PIN		3

double pi;

// Note: Can't be used with the runtime timer. Both use LPC_TIMER0!
void TIMER0_IRQHandler(void){
	Chip_GPIO_SetPinToggle(LPC_GPIO, LED_PORT, LED_PIN);
	DEBUGOUT("pi = %20lf\n", pi);
	Chip_TIMER_ClearMatch( LPC_TIMER0, 1 );
}

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Init the LED GPIO pin.
	Chip_IOCON_PinMuxSet(LPC_IOCON, LED_PORT, LED_PIN, (IOCON_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_PORT, LED_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, LED_PORT, LED_PIN, 0);

	/* Enable and setup LPC_TIMER0 at a periodic rate */
	timerInit(LPC_TIMER0, TIMER0_IRQn, 1);

    // Calculate pi! Forever! :D
    double i = 0;
    int add = 0;

    pi = 4;
    while(true) {
    	if (add == 1) {
    		pi = pi + (4/(3+i*2));
    		add = 0;
    	} else {
    		pi = pi - (4/(3+i*2));
    		add = 1;
    	}
    	i++;
    }

    return 0;
}
