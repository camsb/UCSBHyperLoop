#include "communication.h"
#include "ethernet.h"
#include "sdcard.h"
#include "sensor_data.h"
#include "initialization.h"

void logData(){
    if (ETHERNET_ACTIVE){
        /* Need to do this cleanly, on a timer to prevent multiple attempts before a response */
        if(!connectionOpen && !connectionClosed && sendDataFlag) {
            sendDataFlag = 0;
            ethernetInit(PROTO_TCP, 0);
        }

        /* Handle all Wiznet Interrupts, including RECV */
        if(wizIntFlag) {
            wizIntFunction();
        }

        /* If Data Send Requested, Send Data */
        /* Function to write to SD card and Web App will be here*/
        if((sendDataFlag && connectionOpen)) {
            sendToWebAppSDCard();
        }

        if(SDCARD_ACTIVE) {
            DEBUGOUT("Starting sendToWebAppSDCard\n");
            sendToWebAppSDCard();
            DEBUGOUT("Completed sendToWebAppSDCard\n");
            while(1);
        }
    }
}

void sendToWebAppSDCard(){
	sendPrototypePacket();
	// Send to SD card here
    return;
}
