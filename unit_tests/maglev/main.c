#include "qpn_port.h"
#include "maglev_state_machine.h"
#include <stdlib.h>
#include <stdio.h>

/* local objects -----------------------------------------------------------*/
static FILE *l_outFile = (FILE *)0;
int counts[6];

/*..........................................................................*/
int main(int argc, char *argv[]) {
	initializeMaglevStateMachine();



    if (l_outFile == (FILE *)0) {                   /* interactive version? */
        l_outFile = stdout;            /* use the stdout as the output file */

        printf("Maglev test, built on %s at %s,\n"
               "QP-nano: %s.\nPress ESC to quit...\n",
               __DATE__, __TIME__, QP_getVersion());

        for (;;) {                                            /* event loop */
            int c;

            printf("\n>");
            c = getchar();
            printf(": ");


            if (c == '1') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_ENGAGE);
            }
            else if (c == '2') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_DISENGAGE);
            }
            else if (c == '3') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNUP);
            }
            else if (c == '4') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNDOWN);
            }
            else if (c == '5') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_REC);
            }
            else if (c == '6') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_UNREC);
            }
            else if (c == '7') {                        /* in range? */
                Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_REC_CLEAR);
            }

            QHsm_dispatch((QHsm *)&Maglev_HSM);      /* dispatch the event */
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

