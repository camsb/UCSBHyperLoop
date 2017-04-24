#include "qpn_port.h"
#include "payload_actuator_sm.h"
#include <stdlib.h>
#include <stdio.h>

/* local objects -----------------------------------------------------------*/
static FILE *l_outFile = (FILE *)0;
int counts[6];

/*..........................................................................*/
int main(int argc, char *argv[]) {
	initializePayloadActuatorStateMachine();



    if (l_outFile == (FILE *)0) {                   /* interactive version? */
        l_outFile = stdout;            /* use the stdout as the output file */

        printf("Payload Actuator test, built on %s at %s,\n"
               "QP-nano: %s.\nPress ESC to quit...\n",
               __DATE__, __TIME__, QP_getVersion());

        for (;;) {                                            /* event loop */
            int c;

            printf("\n>");
            c = getchar();
            printf(": ");


            if (c == '1') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_ADVANCE);
            }
            else if (c == '2') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_RETRACT);
            }
            else if (c == '3') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_ADVANCE_DONE);
            }
            else if (c == '4') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_RETRACT_DONE);
            }
            else if (c == '5') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_ML_SUPPORT_GAINED);
            }
            else if (c == '6') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_ML_SUPPORT_LOST);
            }
            else if (c == '7') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_FAULT_REC);
            }
            else if (c == '8') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_FAULT_UNREC);
            }
            else if (c == '9') {                        /* in range? */
                Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_FAULT_REC_CLEAR);
            }

            QHsm_dispatch((QHsm *)&Payload_Actuator_HSM);      /* dispatch the event */
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

