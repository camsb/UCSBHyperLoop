#include "qpn_port.h"
#include "braking_state_machine.h"
#include <stdlib.h>
#include <stdio.h>

/* local objects -----------------------------------------------------------*/
static FILE *l_outFile = (FILE *)0;
int counts[6];

/*..........................................................................*/
int main(int argc, char *argv[]) {
	initializeBrakingStateMachine();



    if (l_outFile == (FILE *)0) {                   /* interactive version? */
        l_outFile = stdout;            /* use the stdout as the output file */

        printf("Braking test, built on %s at %s,\n"
               "QP-nano: %s.\nPress ESC to quit...\n",
               __DATE__, __TIME__, QP_getVersion());

        for (;;) {                                            /* event loop */
            int c;

            printf("\n>");
            c = getchar();
            printf(": ");


            if (c == '0') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_STOP_SIG);
            }
	    else if (c == '1') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_ENGAGE);
            }
            else if (c == '2') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_DISENGAGE);
            }
            else if (c == '3') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_TEST_ENTER);
            }
            else if (c == '4') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_TEST_EXIT);
            }
            else if (c == '5') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_PUSHSENS_FAULT);
            }
            else if (c == '6') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_ACC_FAULT);
            }
            else if (c == '7') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_PUSHSENS_GO);
            }
            else if (c == '8') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_ACC_GO);
            }
            else if (c == '9') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_BOTHSENS_GO);
            }
            else if (c == 'a') {                        /* in range? */
                Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_TIMER_PERMIT);
            }

            QHsm_dispatch((QHsm *)&Braking_HSM);      /* dispatch the event */
        }
    }

    return 0;
}

/*..........................................................................*/

/*..........................................................................*/
void BSP_display(char const *msg) {
    fprintf(l_outFile, "%s\n", msg);

}
/*..........................................................................*/
void BSP_exit(void) {
    printf("Bye, Bye!");
    exit(0);
}
/*..........................................................................*/

