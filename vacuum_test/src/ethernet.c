#include "ethernet.h"
#include "sensor_data.h"
#include "HEMS.h"
#include <string.h>
#include "stdio.h"
#include "timer.h"
#include "rtc.h"
#include "gpio.h"
#include "qpn_port.h"
#include "state_machine.h"

/* Rx Buffer Addresses */
uint16_t gSn_RX_BASE[] = {
	0xC000,	0xC800,	0xD000,	0xD800,	// Socket 0, 1, 2, 3
	0xE000,	0xE800,	0xF000,	0xF800	// Socket 4, 5, 6, 7
};

/* Tx Buffer Addresses */
uint16_t gSn_TX_BASE[] = {
	0x8000,	0x8800,	0x9000,	0x9800,	// Socket 0, 1, 2, 3
	0xA000, 0xA800,	0xB000,	0xB800	// Socket 4, 5, 6, 7
};

/* Data Send Timer Interrupt */
void TIMER2_IRQHandler(void){
	sendDataFlag = 1;
	Chip_TIMER_ClearMatch( LPC_TIMER2, 1 );
}

/* Initialize Data Send Timer */
void sendSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
	 timerInit(timer, timerInterrupt, tickRate);
}

/* SSP Initialization */
void Wiz_SSP_Init() {
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);

	Chip_SSP_Enable(LPC_SSP1);
	Chip_SSP_SetMaster(LPC_SSP1, 1);
}

/* Write 'length' bytes sequentially to address (blocking) */
void spi_Send_Blocking(uint16_t address, uint16_t length) {
	Tx_Buf[0] = (address & 0xFF00) >> 8;		// Address MSB
	Tx_Buf[1] = (address & 0x00FF);				// Address LSB
	Tx_Buf[2] = (length  & 0xFF00) >> 8 | 0x80; // OP Code (R/W) | Length MSB
	Tx_Buf[3] = (length  & 0x00FF);				// Data length LSB
	xf_setup.length = length + 4; 				// Frame length (4 byte header)
	xf_setup.tx_data = Tx_Buf;
	xf_setup.rx_data = Rx_Buf;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);
}

/* Read 'length' bytes sequentially from address (blocking) */
void spi_Recv_Blocking(uint16_t address, uint16_t length) {
	Tx_Buf[0] = (address & 0xFF00) >> 8;		// Address MSB
	Tx_Buf[1] = (address & 0x00FF);				// Address LSB
	Tx_Buf[2] = (length  & 0xFF00) >> 8; 		// OP Code (R/W) | Length MSB
	Tx_Buf[3] = (length  & 0x00FF);				// Data length LSB
	xf_setup.length = length + 4; 				// Frame length (4 byte header)
	xf_setup.tx_data = Tx_Buf;
	xf_setup.rx_data = Rx_Buf;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);
}

/* Wiznet Basic Register Initialization */
void Wiz_Init() {
	xf_setup.length = BUFFER_SIZE; // May not be necessary

	/* Interrupt Mask Registers */
	Tx_Buf[4] = 0x00; // Data
	spi_Send_Blocking(IMR, 0x0001);

	/* Retry Time-Value Register (RTR) MSB*/
	Tx_Buf[4] = 0x07; // Data
	Tx_Buf[5] = 0xD0; // Data
	spi_Send_Blocking(RTR, 0x0002);

	/* Retry Count Register (RCR) */
	Tx_Buf[4] = 0x07; // Data
	spi_Send_Blocking(RCR, 0x0001);
}

/* Wiznet Network Register Initialization */
void Wiz_Network_Init() {
	/* Wiznet Source Hardware Address Register (SHAR) */
	Tx_Buf[4] = 0x04; Tx_Buf[5] = 0x20; Tx_Buf[6] = 0xB1; // Data
	Tx_Buf[7] = 0xA2; Tx_Buf[8] = 0xE0; Tx_Buf[9] = 0x17; // Data
	spi_Send_Blocking(SHAR, 0x0006);

	/* Wiznet Gateway Address Register (GAR) */
	Tx_Buf[4] = 0xC0; Tx_Buf[5] = 0xA8; Tx_Buf[6] = 0x01; Tx_Buf[7] = 0x01; // Data
	spi_Send_Blocking(GAR, 0x0004);

	/* Wiznet Subnet Mask Register (SUMR) */
	Tx_Buf[4] = 0xFF; Tx_Buf[5] = 0xFF; Tx_Buf[6] = 0xFF; Tx_Buf[7] = 0x00; // Data
	spi_Send_Blocking(SUBR, 0x0004);

	/* Wiznet Source IP Address Register (SIPR) */
	Tx_Buf[4] = 0xC0; Tx_Buf[5] = 0xA8; Tx_Buf[6] = 0x01; Tx_Buf[7] = 0x10; // Data
	spi_Send_Blocking(SIPR, 0x0004);
}

/* Wiznet Network Register Sanity Check */
void Wiz_Check_Network_Registers() {
	/* Read Source Hardware Address Register */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = 0xFF; // Dummy data
	Tx_Buf[7] = Tx_Buf[8] = Tx_Buf[9] = 0xFF; // Dummy data
	spi_Recv_Blocking(SHAR, 0x0006);
	printf("MAC Address: %x:%x:%x:%x:%x:%x\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6],
			Rx_Buf[7], Rx_Buf[8], Rx_Buf[9]);

	/* Read Gateway Address Register */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(GAR, 0x0004);
	printf("Default Gateway: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Source Mask Address */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(SUBR, 0x0004);
	printf("Subnet Mask: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Source IP Register */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(SIPR, 0x0004);
	printf("Source IP: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);
}

/* Socket Interrupt Initialization */
void Wiz_Int_Init(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Datasheet and addresses are backwards for
	 * Socket Interrupt Mask and General Interrupt Mask. */
	/* IMR-0x0016 is supposed to have one of 7 bits set for the corresponding bit in IR2 to be set, if it is 0 then interrupts will not go through*/
	/* IMR2-0x0032, each interrupt bit mask corresponds to a bit in the IR, if set then interrupt will happen when corresponding bit in the IR is set*/
	/* Socket Interrupt Mask */
	Tx_Buf[4] = 0x01 << n;
	spi_Send_Blocking(IMR, 0x0001);

	/* General Interrupt Mask */
	Tx_Buf[4] = 0x00;
//	Tx_Buf[4] = 0x01 << n;
	spi_Send_Blocking(IMR2, 0x0001);

	/* Socket n Interrupt Mask Register */
	Tx_Buf[4] = 0x1F;
	spi_Send_Blocking(Sn_IMR_BASE + offset, 0x0001);

	/* Configure Wiznet interrupt pin as input */
	Chip_IOCON_PinMuxSet(LPC_IOCON, WIZNET_INT_PORT, WIZNET_INT_PIN, IOCON_FUNC0);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, WIZNET_INT_PORT, WIZNET_INT_PIN);

	/* Configure the GPIO interrupt */
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, WIZNET_INT_PORT, 1 << WIZNET_INT_PIN); // Set to falling edge trigger
}

void send_method(char *method, char* val, int val_len) {

	memset(Net_Tx_Data, 0, DATA_BUF_SIZE);
	memcpy(Net_Tx_Data, method, 3);
	Net_Tx_Data[3] = ':';
	memcpy(Net_Tx_Data + 4, val, val_len);
	Net_Tx_Data[4 + val_len] = '\n';
//	int i;
	//for (i = 0; i < 5 + val_len; i++) {printf("%i:%c\n", i, Net_Tx_Data[i]);}
	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);

}

void send_data_packet_helper(char *method, char *val, int *position) {
	if (val != 0) {
		memcpy(Net_Tx_Data + *position, method, 3);
		*position += 3;
		memcpy(Net_Tx_Data + *position, ":", 1);
		*position += 1;
		memcpy(Net_Tx_Data + *position, val, 6);
		*position += 6;
		memcpy(Net_Tx_Data + *position, "\n", 1);
		*position += 1;
	}
}

void send_data_ack_helper(char *method, int *position) {
	memcpy(Net_Tx_Data + *position, method, 3);
	*position += 3;
	memcpy(Net_Tx_Data + *position, "\n\n", 1);
	*position += 1;
}


void recvDataPacket() {
	enum Hyperloop_Signals profile[4] = { ENGAGE_ENGINES_SIG, ENGINES_REVED_SIG, DISENGAGE_ENGINES_SIG, ENGINES_STOPPED_SIG};
	int pos = 0;
	memset(Net_Tx_Data, 0, 64);

	Wiz_Recv_Blocking(SOCKET_ID, Net_Rx_Data);
//	int i;
//	for (i = 0; i < DATA_BUF_SIZE; i++) {
//		printf("%i:%c\n", i, Net_Rx_Data[i]);
//	}
//	printf("Receiving Data Packet!\n");

	if(strcmp((char *)Net_Rx_Data, ENGAGE_ENGINES) == 0){
		printf("ENGAGE ENGINES SIG RECEIVED\n");
		Q_SIG((QHsm *)&HSM_Hyperloop) = (QSignal)(profile[0]);
		QHsm_dispatch((QHsm *)&HSM_Hyperloop);
	}
	if(strcmp((char *)Net_Rx_Data, DISENGAGE_ENGINES) == 0){
		printf("DISENGAGE ENGINES SIG RECEIVED\n");
		Q_SIG((QHsm *)&HSM_Hyperloop) = (QSignal)(profile[2]);
		QHsm_dispatch((QHsm *)&HSM_Hyperloop);
	}
	if(strcmp((char *)Net_Rx_Data, ENGINES_REVED) == 0){
		printf("ENGINES REVED SIG RECEIVED\n");
		Q_SIG((QHsm *)&HSM_Hyperloop) = (QSignal)(profile[1]);
		QHsm_dispatch((QHsm *)&HSM_Hyperloop);
	}
	if(strcmp((char *)Net_Rx_Data, ENGINES_STOPPED) == 0){
		printf("ENGINES STOPPED SIG RECEIVED\n");
		Q_SIG((QHsm *)&HSM_Hyperloop) = (QSignal)(profile[3]);
		QHsm_dispatch((QHsm *)&HSM_Hyperloop);
	}
	if(strcmp((char *)Net_Rx_Data, STOP) == 0){
		printf("STOP SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, ENGAGE_BRAKES) == 0){
		printf("ENGAGE BRAKES SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, DISENGAGE_BRAKES) == 0){
		printf("DISENGAGE BRAKES SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, FORWARD) == 0){
		printf("FORWARD SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, REVERSE) == 0){
		printf("REVERSE SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, TERMINATE) == 0){
		printf("TERMINATE SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, IGNORE) == 0){
		printf("IGNORE SIG RECEIVED\n");
	}
	if(strcmp((char *)Net_Rx_Data, MAXSIG) == 0){
		printf("MAX SIG RECEIVED\n");
	}
//	if(strstr((char *)Net_Rx_Data, EBRAKE) != NULL) {	// Emergency Brake
//		eBrakeFlag = 1;
//		printf("Emergency Brake!\n");
//		send_data_ack_helper(BAK, &pos);
//	}
	if(strstr((char *)Net_Rx_Data, SETDAC) != NULL) {	// Set the DAC
		printf("DAC SET RECEIVED\n");
		printf("DAC recieved: %s\n", Net_Rx_Data);
		int iterator;
		float dacValue = 0;
		for(iterator = 0; iterator < 30; iterator++){
			if(Net_Rx_Data[iterator] == ':'){
				dacValue = (float)(Net_Rx_Data[iterator+1] - '0');
				dacValue += (float)(Net_Rx_Data[iterator+3] - '0')/10;
				if((int)(Net_Rx_Data[iterator+4] - '0') == 9){
					dacValue += 0.01;
				}
			}
		}
		if(dacValue)
		printf("dacValue = %f\n", dacValue);
		set_motor_throttle(0, dacValue);
		set_motor_throttle(1, dacValue);
		set_motor_throttle(2, dacValue);
		set_motor_throttle(3, dacValue);
	}
	if(strstr((char *)Net_Rx_Data, INITTIME) != NULL) {	// Initialize Time
		printf("Initialize Time!\n");
		printf("Time recieved: %s\n", Net_Rx_Data);
		int iterator, yearVal, monthVal, mDayVal, wDayVal, hourVal, minVal, secVal;
		int timeIterator = 0;
		// year: 4 dig, month: 0-11, month day: 1-31, week day: 0-6, hour: 0-23, minute: 0-59, second: 0-59
		for(iterator = 0; iterator < 30; iterator++){
			if(Net_Rx_Data[iterator] == ':'){
				if(timeIterator == 0){
					yearVal = (int)(Net_Rx_Data[iterator+1] - '0')*1000;
					yearVal += (int)(Net_Rx_Data[iterator+2] - '0')*100;
					yearVal += (int)(Net_Rx_Data[iterator+3] - '0')*10;
					yearVal += (int)(Net_Rx_Data[iterator+4] - '0');
//					printf("year = %d\n", yearVal);
				}
				if(timeIterator == 1){
					// One Digit
					if(Net_Rx_Data[iterator+2] == ':'){
						monthVal = (int)(Net_Rx_Data[iterator+1] - '0');
					}
					// Two Digits
					else{
						monthVal = (int)(Net_Rx_Data[iterator+1] - '0')*10;
						monthVal += (int)(Net_Rx_Data[iterator+2] - '0');
					}
					monthVal++;
//					printf("month = %d\n", monthVal);
				}
				if(timeIterator == 2){
					// One Digit
					if(Net_Rx_Data[iterator+2] == ':'){
						mDayVal = (int)(Net_Rx_Data[iterator+1] - '0');
					}
					// Two Digits
					else{
						mDayVal = (int)(Net_Rx_Data[iterator+1] - '0')*10;
						mDayVal += (int)(Net_Rx_Data[iterator+2] - '0');
					}
//					printf("month day = %d\n", mDayVal);
				}
				if(timeIterator == 3){
					wDayVal = (int)(Net_Rx_Data[iterator+1] - '0');
					wDayVal++;
//					printf("week day = %d\n", wDayVal);
				}
				if(timeIterator == 4){
					// One Digit
					if(Net_Rx_Data[iterator+2] == ':'){
						hourVal = (int)(Net_Rx_Data[iterator+1] - '0');
					}
					// Two Digits
					else{
						hourVal = (int)(Net_Rx_Data[iterator+1] - '0')*10;
						hourVal += (int)(Net_Rx_Data[iterator+2] - '0');
					}
//					printf("hour = %d\n", hourVal);
				}
				if(timeIterator == 5){
					// One Digit
					if(Net_Rx_Data[iterator+2] == ':'){
						minVal = (int)(Net_Rx_Data[iterator+1] - '0');
					}
					// Two Digits
					else{
						minVal = (int)(Net_Rx_Data[iterator+1] - '0')*10;
						minVal += (int)(Net_Rx_Data[iterator+2] - '0');
					}
//					printf("minute = %d\n", minVal);
				}
				if(timeIterator == 6){
					// One Digit
					if(Net_Rx_Data[iterator+2] == ':'){
						secVal = (int)(Net_Rx_Data[iterator+1] - '0');
					}
					// Two Digits
					else{
						secVal = (int)(Net_Rx_Data[iterator+1] - '0')*10;
						secVal += (int)(Net_Rx_Data[iterator+2] - '0');
					}
//					printf("second = %d\n", secVal);
				}
				timeIterator++;
			}
		}
//		rtc_initialize();
		RTC theTime;
		theTime.year = (WORD)yearVal;
		theTime.month = (BYTE)monthVal;
		theTime.mday = (BYTE)mDayVal;
		theTime.wday = (BYTE)wDayVal;
		theTime.hour = (BYTE)hourVal;
		theTime.min = (BYTE)minVal;
		theTime.sec = (BYTE)secVal;
//		rtc_settime(&theTime);
		send_data_ack_helper(TAK, &pos);
	}
//	if(strstr((char *)Net_Rx_Data, POWRUP) != NULL) {	// Pod Start Flag
//		powerUpFlag = 1;
//		send_data_ack_helper(PAK, &pos);
//		printf("Power Up!\n");
//	}
//	if(strstr((char *)Net_Rx_Data, PWRDWN) != NULL) {	// Pod Stop Flag
//		powerDownFlag = 1;
//		powerUpFlag = 0;
//		printf("Power Down!\n");
//	}
//	if(strstr((char *)Net_Rx_Data, SERPRO) != NULL) {	// Service Propulsion Start
//		serPropulsionWheels = 1;
//		send_data_ack_helper(WAK, &pos);
//		printf("Service Propulsion Activated!\n");
//	}
//	if(strstr((char *)Net_Rx_Data, SERSTP) != NULL) {	// Service Propulsion Stop
//		serPropulsionWheels = 0;
//		printf("Service Propulsion Disactivated!\n");
//	}

	if(pos != 0) {
		Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);
	}
}

void sendDataPacket() {

	sendDataFlag = 0;

	// Copy strings to Net_Tx_Data
	int pos = 0;
	memset(Net_Tx_Data, 0, 256); // Make sure this clears enough space

	/* Atmospheric, Miscellaneous Data */
	sprintf(DataPacket.bm1, "%06.2f", sensorData.pressure1);
	sprintf(DataPacket.bm2, "%06.2f", sensorData.pressure2);
	sprintf(DataPacket.tm1, "%06.2f", sensorData.temp1);
	sprintf(DataPacket.tm2, "%06.2f", sensorData.temp2);
	sprintf(DataPacket.tm3, "%06.2f", sensorData.temp3);
	sprintf(DataPacket.tm4, "%06.2f", sensorData.temp4);
	sprintf(DataPacket.ta1, "%06.2f", sensorData.tacho1);
	sprintf(DataPacket.ta2, "%06.2f", sensorData.tacho2);
	sprintf(DataPacket.ta3, "%06.2f", sensorData.tacho3);
	sprintf(DataPacket.ta4, "%06.2f", sensorData.tacho4);
	sprintf(DataPacket.pwr, "%06.2f", sensorData.power);
	/* Positional Data */
	sprintf(DataPacket.pox, "%06.2f", sensorData.positionX);
	sprintf(DataPacket.poy, "%06.2f", sensorData.positionY);
	sprintf(DataPacket.poz, "%06.2f", sensorData.positionZ);
	/* Velocity Data */
	sprintf(DataPacket.vex, "%06.2f", sensorData.velocityX);
	sprintf(DataPacket.vey, "%06.2f", sensorData.velocityY);
	sprintf(DataPacket.vez, "%06.2f", sensorData.velocityZ);
	/* Acceleration Data */
	sprintf(DataPacket.acx, "%06.2f", sensorData.accelX);
	sprintf(DataPacket.acy, "%06.2f", sensorData.accelY);
	sprintf(DataPacket.acz, "%06.2f", sensorData.accelZ);
	/* Attitudinal Data */
	sprintf(DataPacket.rol, "%06.2f", sensorData.roll);
	sprintf(DataPacket.pit, "%06.2f", sensorData.pitch);
	sprintf(DataPacket.yaw, "%06.2f", sensorData.yaw);

	/* Atmospheric, Miscellaneous Data */
	send_data_packet_helper(BM1, DataPacket.bm1, &pos);
	send_data_packet_helper(BM2, DataPacket.bm2, &pos);
	send_data_packet_helper(TM1, DataPacket.tm1, &pos);
	send_data_packet_helper(TM2, DataPacket.tm2, &pos);
	send_data_packet_helper(TM3, DataPacket.tm3, &pos);
	send_data_packet_helper(TM4, DataPacket.tm4, &pos);
	send_data_packet_helper(TA1, DataPacket.ta1, &pos);
	send_data_packet_helper(TA2, DataPacket.ta2, &pos);
	send_data_packet_helper(TA3, DataPacket.ta3, &pos);
	send_data_packet_helper(TA4, DataPacket.ta4, &pos);
	send_data_packet_helper(PWR, DataPacket.pwr, &pos);
	/* Positional Data */
	send_data_packet_helper(POX, DataPacket.pox, &pos);
	send_data_packet_helper(POY, DataPacket.poy, &pos);
	send_data_packet_helper(POZ, DataPacket.poz, &pos);
	/* Velocity Data */
	send_data_packet_helper(VEX, DataPacket.vex, &pos);
	send_data_packet_helper(VEY, DataPacket.vey, &pos);
	send_data_packet_helper(VEZ, DataPacket.vez, &pos);
	/* Acceleration Data */
	send_data_packet_helper(ACX, DataPacket.acx, &pos);
	send_data_packet_helper(ACY, DataPacket.acy, &pos);
	send_data_packet_helper(ACZ, DataPacket.acz, &pos);
	/* Attitudinal Data */
	send_data_packet_helper(ROL, DataPacket.rol, &pos);
	send_data_packet_helper(PIT, DataPacket.pit, &pos);
	send_data_packet_helper(YAW, DataPacket.yaw, &pos);

	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);

}

void sendPrototypePacket(){
	sendDataFlag = 0;

	// Copy strings to Net_Tx_Data
	int pos = 0;
	memset(Net_Tx_Data, 0, 512); // Make sure this clears enough space

	/* DAC Output */
	sprintf(PrototypePacket.dac, "%06.2f", motors[0]->throttle_voltage);
	/* Short Ranging */
//	sprintf(PrototypePacket.sr1, "%06.2f", sensorData.shortRangingData.frontLeft);
//	sprintf(PrototypePacket.sr1, "%06.2f", sensorData.shortRangingData.frontRight);
//	sprintf(PrototypePacket.sr1, "%06.2f", sensorData.shortRangingData.backLeft);
//	sprintf(PrototypePacket.sr1, "%06.2f", sensorData.shortRangingData.backRight);
	/* Current Output */
	sprintf(PrototypePacket.cu1, "%06.2f", (float)motors[0]->amps);
	sprintf(PrototypePacket.cu2, "%06.2f", (float)motors[1]->amps);
	sprintf(PrototypePacket.cu3, "%06.2f", (float)motors[2]->amps);
	sprintf(PrototypePacket.cu4, "%06.2f", (float)motors[3]->amps);
	/* Tachometer Output */
	sprintf(PrototypePacket.ta1, "%06.2f", (float)motors[0]->rpm);
	sprintf(PrototypePacket.ta2, "%06.2f", (float)motors[1]->rpm);
	sprintf(PrototypePacket.ta3, "%06.2f", (float)motors[2]->rpm);
	sprintf(PrototypePacket.ta4, "%06.2f", (float)motors[3]->rpm);
	/* Temperature Output */
	sprintf(PrototypePacket.m1tmp1, "%06.f", (float)motors[0]->temperatures[0]);
	sprintf(PrototypePacket.m1tmp2, "%06.f", (float)motors[0]->temperatures[1]);
	sprintf(PrototypePacket.m1tmp3, "%06.f", (float)motors[0]->temperatures[2]);
	sprintf(PrototypePacket.m1tmp4, "%06.f", (float)motors[0]->temperatures[3]);
	sprintf(PrototypePacket.m2tmp1, "%06.f", (float)motors[1]->temperatures[0]);
	sprintf(PrototypePacket.m2tmp2, "%06.f", (float)motors[1]->temperatures[1]);
	sprintf(PrototypePacket.m2tmp3, "%06.f", (float)motors[1]->temperatures[2]);
	sprintf(PrototypePacket.m2tmp4, "%06.f", (float)motors[1]->temperatures[3]);
	sprintf(PrototypePacket.m3tmp1, "%06.f", (float)motors[2]->temperatures[0]);
	sprintf(PrototypePacket.m3tmp2, "%06.f", (float)motors[2]->temperatures[1]);
	sprintf(PrototypePacket.m3tmp3, "%06.f", (float)motors[2]->temperatures[2]);
	sprintf(PrototypePacket.m3tmp4, "%06.f", (float)motors[2]->temperatures[3]);
	sprintf(PrototypePacket.m4tmp1, "%06.f", (float)motors[3]->temperatures[0]);
	sprintf(PrototypePacket.m4tmp2, "%06.f", (float)motors[3]->temperatures[1]);
	sprintf(PrototypePacket.m4tmp3, "%06.f", (float)motors[3]->temperatures[2]);
	sprintf(PrototypePacket.m4tmp4, "%06.f", (float)motors[3]->temperatures[3]);

	/* DAC Data */
	send_data_packet_helper(DAC, PrototypePacket.dac, &pos);
	/* Current Data */
//	send_data_packet_helper(SR1, PrototypePacket.sr1, &pos);
//	send_data_packet_helper(SR2, PrototypePacket.sr2, &pos);
//	send_data_packet_helper(SR3, PrototypePacket.sr3, &pos);
//	send_data_packet_helper(SR4, PrototypePacket.sr4, &pos);
	/* Current Data */
	send_data_packet_helper(CU1, PrototypePacket.cu1, &pos);
	send_data_packet_helper(CU2, PrototypePacket.cu2, &pos);
	send_data_packet_helper(CU3, PrototypePacket.cu3, &pos);
	send_data_packet_helper(CU4, PrototypePacket.cu4, &pos);
	/* Tachometer Data */
	send_data_packet_helper(TA1, PrototypePacket.ta1, &pos);
	send_data_packet_helper(TA2, PrototypePacket.ta2, &pos);
	send_data_packet_helper(TA3, PrototypePacket.ta3, &pos);
	send_data_packet_helper(TA4, PrototypePacket.ta4, &pos);
	/* Temperature Data */
	send_data_packet_helper(TM1, PrototypePacket.m1tmp1, &pos);
	send_data_packet_helper(TM2, PrototypePacket.m1tmp2, &pos);
	send_data_packet_helper(TM3, PrototypePacket.m1tmp3, &pos);
	send_data_packet_helper(TM4, PrototypePacket.m1tmp4, &pos);
	send_data_packet_helper(TM5, PrototypePacket.m2tmp1, &pos);
	send_data_packet_helper(TM6, PrototypePacket.m2tmp2, &pos);
	send_data_packet_helper(TM7, PrototypePacket.m2tmp3, &pos);
	send_data_packet_helper(TM8, PrototypePacket.m2tmp4, &pos);
	send_data_packet_helper(TM9, PrototypePacket.m3tmp1, &pos);
	send_data_packet_helper(T10, PrototypePacket.m3tmp2, &pos);
	send_data_packet_helper(T11, PrototypePacket.m3tmp3, &pos);
	send_data_packet_helper(T12, PrototypePacket.m3tmp4, &pos);
	send_data_packet_helper(T13, PrototypePacket.m4tmp1, &pos);
	send_data_packet_helper(T14, PrototypePacket.m4tmp2, &pos);
	send_data_packet_helper(T15, PrototypePacket.m4tmp3, &pos);
	send_data_packet_helper(T16, PrototypePacket.m4tmp4, &pos);

	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);
}

// Singular, will change to multiple, or do an interrupt or something
void rec_method(char *method, char *val, int *val_len) {

	memset (Net_Rx_Data, 0, DATA_BUF_SIZE);
	if(Wiz_Check_Socket(SOCKET_ID)) {
		Wiz_Recv_Blocking(SOCKET_ID, Net_Rx_Data);
		memcpy(method, Net_Rx_Data, 3);
		method[3] = '\0';
		*val_len = 0;
		while(Net_Rx_Data[*val_len] != '\n') (*val_len)++;
		memcpy(val, Net_Rx_Data + 4, *val_len);
		val[*val_len] = '\0';
	}
}

/* Handle Wiznet Interrupt */
void wizIntFunction() {
	uint16_t offset = 0;// 0x0100*n;
	uint8_t socket_int, n;

	/* Read Socket Interrupts */
	for(n = 0; n < 8; n++) {
		if(activeSockets >> n & 0x01) {
//			offset = 0x0010 * n;
			offset = 0x0100 * n;

			/* Read Socket n Interrupt Register */
			Tx_Buf[4] = 0xFF;
			spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
			socket_int = Rx_Buf[4];
			/* Handle Interrupt Request */
			if( socket_int & SEND_OK ) {	 // Send Completed
//				printf("send ok interrupt\n");
			}
			if( socket_int & TIMEOUT ) { // Timeout Occurred
				printf("Timeout interrupt\n");
			}
			if( socket_int & RECV_PKT ) {	 // Packet Received
				printf("Packet interrupt\n");
				recvDataPacket();
			}
			if( socket_int & DISCON_SKT ) {	 // FIN/FIN ACK Received
				printf("Discon interrupt\n");
				if(connectionOpen)
					connectionClosed = 1;
				connectionOpen = 0;
			}
			if( socket_int & Sn_CON ) {	 // Socket Connection Completed
				printf("socket connection completed interrupt\n");
				connectionOpen = 1;
//				Tx_Buf[4] = 0x01 | Tx_Buf[4];
			}
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IR2 + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				printf("IR2 S0 int\n");
			/* Clear Socket n Interrupt Register */
			Tx_Buf[4] = 0xFF;
			spi_Send_Blocking(Sn_IR_BASE + offset, 0x0001);
//			if(Wiz_Check_Socket(0)){
//				printf("Interrupt not cleared\n");
//			}
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(Sn_IMR_BASE + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				printf("Sn_IMR connect\n");
//			if((Rx_Buf[4] & 0x08) == 0x08) // Bit 4 is timeout
//				printf("Sn_IMR timeout\n");
//			if((Rx_Buf[4] & 0x04) == 0x04) // Bit 3 is data received interrupt
//				printf("Sn_IMR recv\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IR2 + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				printf("IR2 S0 int\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IMR + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				printf("IMR S0 int for IR2\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(Sn_SR_BASE + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x17) == 0x17) // Bit 1 is connected interrupt
//				printf("Socket still established\n");
		}
	}

	wizIntFlag = 0;
}

uint8_t Wiz_Int_Clear(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Read Socket n Interrupt Register */
	Tx_Buf[4] = 0xFF;
	spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
	uint8_t result = Tx_Buf[4];

//	/* Clear Socket n Interrupt Register */
//	Tx_Buf[4] = result;
//	spi_Send_Blocking(Sn_IR_BASE + offset, 0x0001);

	wizIntFlag = 0;
	return result;
}

/* Memory Initialization */
void Wiz_Memory_Init() {
	uint16_t offset;
	uint8_t n;

	for(n = 0; n < 8; n++) {
		offset = 0x0100*n;

		/* Rx Buffer Init */
		Tx_Buf[4] = 0x02;	// 2K
		spi_Send_Blocking(Sn_RXMEM_SIZE + offset, 0x0001);

		/* Tx Buffer Init */
		Tx_Buf[4] = 0x02;	// 2K
		spi_Send_Blocking(Sn_TXMEM_SIZE + offset, 0x0001);
	}
}

void Wiz_TCP_Init(uint8_t n) {
/* TCP Initialization */
	uint8_t reserv_bit;
	uint16_t offset = 0x0100*n;

	/* Read Socket n Mode Register (Sn_MR) */
	Tx_Buf[4] = 0xFF; // Dummy data
	spi_Recv_Blocking(Sn_MR_BASE + offset, 0x0001);
	reserv_bit = Rx_Buf[4] & 0x10;

	/* Socket n Mode Register: 001X0001 for TCP,
	 * 001x For No Delayed Ack */
	Tx_Buf[4] = 0x01 | reserv_bit; // 001X 0001
	spi_Send_Blocking(Sn_MR_BASE + offset, 0x0001);

	/* Socket n Source Port Register (Sn_PORT) */
	Tx_Buf[4] = 0xAC;
	Tx_Buf[5] = 0xDC;
	spi_Send_Blocking(Sn_PORT_BASE + offset, 0x0002);

	/* Socket n Control Register (Sn_CR) */
	Tx_Buf[4] = 0x01;	// OPEN
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	/* Read Socket n Status Register (Sn_SR) */
	Tx_Buf[4] = 0xFF;
	spi_Recv_Blocking(Sn_SR_BASE + offset, 0x0001);
	if(Rx_Buf[4] == 0x13) {
		printf("Socket %u TCP Initialized Successfully\n", n);
		activeSockets |= 1 << n;
	} else
		printf("Socket %u TCP Initialization Failed\n", n);
}

/* UDP Initialization */
void Wiz_UDP_Init(uint8_t n) {
	uint8_t reserv_bit;
	uint16_t offset = 0x0100*n;

	/* Read Socket n Mode Register (Sn_MR) */
	Tx_Buf[4] = 0xFF; // Dummy data
	spi_Recv_Blocking(Sn_MR_BASE + offset, 0x0001);
	reserv_bit = Rx_Buf[4] & 0x10;

	/* Socket n Mode Register: 001X0010 for UDP,
	 * 001x For No Delayed Ack */
	Tx_Buf[4] = 0x02 | reserv_bit; // 001X 0010
	spi_Send_Blocking(Sn_MR_BASE + offset, 0x0001);

	/* Socket n Source Port Register (Sn_PORT) */
	Tx_Buf[4] = 0xD5;
	Tx_Buf[5] = 0x11;
	spi_Send_Blocking(Sn_PORT_BASE + offset, 0x0002);

	/* Socket n Control Register (Sn_CR) */
	Tx_Buf[4] = 0x01;	// OPEN
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	/* Read Socket n Status Register (Sn_SR) */
	Tx_Buf[4] = 0xFF;
	spi_Recv_Blocking(Sn_SR_BASE + offset, 0x0001);
	if(Rx_Buf[4] == 0x22) {
		printf("Socket %u UDP Initialized Successfully\n", n);
		activeSockets |= 1 << n;
	} else
		printf("Socket %u UDP Initialization Failed\n", n);
}

void Wiz_Destination_Init(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Socket n Destination IP Register (Sn_DIPR) */
	Tx_Buf[4] = REMOTE_IP0; // 192
	Tx_Buf[5] = REMOTE_IP1;	// 168
	Tx_Buf[6] = REMOTE_IP2; // 1
	Tx_Buf[7] = REMOTE_IP3; // 100
	spi_Send_Blocking(Sn_DIPR_BASE + offset, 0x0004);

	/* Socket n Destination Port Register (Sn_PORT) */
	Tx_Buf[4] = REMOTE_PORT0; // 41234
	Tx_Buf[5] = REMOTE_PORT1;
	spi_Send_Blocking(Sn_DPORT_BASE + offset, 0x0002);
}

void Wiz_Address_Check(uint8_t n) {
	uint16_t offset = 0x0100*n;
	uint16_t port;

	/* Read Socket n Source Port Register (Sn_PORT) */
	Tx_Buf[4] = Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_PORT_BASE + offset, 0x0002);
	port = ((uint16_t)Rx_Buf[4] << 8) + (uint16_t)Rx_Buf[5];
	printf("Socket %u Source Port: %u\n", n, port);

	/* Read Socket n Destination IP Register (Sn_DIPR) */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF;
	spi_Recv_Blocking(Sn_DIPR_BASE + offset, 0x0004);
	printf("Socket %u Destination IP: %u.%u.%u.%u\n", n, Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Socket n Destination Port Register (Sn_DPORT) */
	Tx_Buf[4] = Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_DPORT_BASE + offset, 0x0002);
	port = ((uint16_t)Rx_Buf[4] << 8) + (uint16_t)Rx_Buf[5];
	printf("Dest Port: %u, %u\n", Rx_Buf[4], Rx_Buf[5]);
	printf("Socket %u Destination Port: %u\n", n, port);
}

/* Create TCP Connection */
void Wiz_TCP_Connect(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Socket n Destination IP Register (Sn_DIPR) */
	Tx_Buf[4] = REMOTE_IP0; // 192
	Tx_Buf[5] = REMOTE_IP1;	// 168
	Tx_Buf[6] = REMOTE_IP2; // 1
	Tx_Buf[7] = REMOTE_IP3; // 100
	spi_Send_Blocking(Sn_DIPR_BASE + offset, 0x0004);

	/* Socket n Destination Port Register (Sn_PORT) */
	Tx_Buf[4] = REMOTE_PORT0; // 41234
	Tx_Buf[5] = REMOTE_PORT1;
	spi_Send_Blocking(Sn_DPORT_BASE + offset, 0x0002);

	/* TCP Connect */
	Tx_Buf[4] = CONNECT;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	/* TODO: If this fails, try to establish connection once more */
	printf("Establishing TCP connection...\n");
	do {
		/* Wait for connection */
		Rx_Buf[4] = 0xFF;
		spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
	} while((Rx_Buf[4] & 0x01) != 0x01);
	printf("Connected\n");
}

/* Close TCP Socket */
void Wiz_TCP_Close(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Disconnect */
	Tx_Buf[4] = DISCON;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	do {
		/* Disconnect */
		Rx_Buf[4] = 0xFF;
		spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
	} while((Rx_Buf[4] & 0x02) != 0x02);

	/* Clear Socket Interrupts */
	Tx_Buf[4] = 0xFF;
	spi_Send_Blocking(Sn_IR_BASE + offset, 0x0001);

	/* Close Socket */
	Tx_Buf[4] = CLOSE;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);
}

/* Close UDP Socket */
void Wiz_UDP_Close(uint8_t n) {
	uint16_t offset = 0x0100*n;

	/* Clear Socket Interrupts */
	Tx_Buf[4] = 0xFF;
	spi_Send_Blocking(Sn_IR_BASE + offset, 0x0001);

	/* Close Socket */
	Tx_Buf[4] = CLOSE;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);
}

/* Clear socket buffer */
void Wiz_Clear_Buffer(uint8_t n) {
	uint16_t dst_mask = (Sn_TX_WR_BASE) & (TX_MAX_MASK);
	uint16_t dst_ptr = gSn_TX_BASE[n] + dst_mask;

	memset((void *)Tx_Buf, '\0', BUFFER_SIZE);
	spi_Send_Blocking(dst_ptr, BUFFER_SIZE);
}


/* Send Outgoing Data */
uint16_t Wiz_Send_Blocking(uint8_t n, uint8_t* message) {
	uint16_t length;
	uint16_t offset = 0x0100*n;
	uint16_t dst_mask, dst_ptr, wr_base;
	uint16_t upper_size, left_size;

	/* Read current Tx Write Pointer */
	Tx_Buf[4] = Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_TX_WR_BASE + offset, 0x0002);
	wr_base = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	/* Calculate Tx Buffer Address */
	dst_mask = wr_base & (TX_MAX_MASK);
	dst_ptr = gSn_TX_BASE[n] + dst_mask;
	length = strlen((char *)message);

	/* Load data into Tx Buffer */
	if((dst_mask + length) > (TX_MAX_MASK + 1)) {

		/* Copy upper_size bytes of src_addr to dest_addr */
		upper_size = (TX_MAX_MASK + 1) - dst_mask;
		memcpy((uint8_t *)Tx_Buf + 4, (char *)message, upper_size);
		spi_Send_Blocking(dst_ptr, upper_size);

		/* Copy left_size bytes of src_addr to gSn_TX_BASE */
		left_size = length - upper_size;
		memcpy((uint8_t *)Tx_Buf + 4, (char *)message + upper_size, left_size);
		spi_Send_Blocking(gSn_TX_BASE[n], left_size);

	} else {
		/* Setup, send data to Wiznet */
		sprintf(((char *)Tx_Buf) + 4, (char *)message);
		spi_Send_Blocking(dst_ptr, length);
	}

	/* Update Tx Write Pointer */
	wr_base += length;
	Tx_Buf[4] = ((uint8_t)((wr_base & 0xFF00) >> 8));
	Tx_Buf[5] = ((uint8_t)(wr_base & 0x00FF));
	spi_Send_Blocking(Sn_TX_WR_BASE + offset, 0x0002);

	/* SEND Command */
	Tx_Buf[4] = SEND;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	return length;
}

/* Read Incoming Data */
uint16_t Wiz_Recv_Blocking(uint8_t n, uint8_t *message) {
	uint16_t length;
	uint16_t offset = 0x0100*n;
	uint16_t src_mask, src_ptr, rd_base;
	uint16_t upper_size, left_size;

	/* Read Socket Received Size */
	Tx_Buf[4] = Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_RX_RSR_BASE + offset, 0x0002);
	length = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	/* Find socket Received Address */
	Tx_Buf[4] = 0xFF;
	Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_RX_RD_BASE + offset, 0x0002);
	rd_base = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	/* Calculate Tx Buffer Address */
	src_mask = rd_base & (RX_MAX_MASK);
	src_ptr = gSn_RX_BASE[n] + src_mask;

	/* Clear output buffer (may not be necessary) */
	memset(message, '\0', DATA_BUF_SIZE);

	/* Load data into Tx Buffer */
	if((src_mask + length) > (RX_MAX_MASK + 1)) {

		/* Copy upper_size bytes of src_addr to dest_addr */
		upper_size = (TX_MAX_MASK + 1) - src_mask;
		spi_Recv_Blocking(src_ptr, upper_size);
		memcpy((char *)message, (uint8_t *)Rx_Buf + 4, upper_size);

		/* Copy left_size bytes of src_addr to gSn_TX_BASE */
		left_size = length - upper_size;
		spi_Recv_Blocking(gSn_RX_BASE[n], left_size);
		memcpy((char *)message + upper_size, (uint8_t *)Rx_Buf + 4, left_size);

	} else {
		spi_Recv_Blocking(src_ptr, length);
		memcpy(message, &Rx_Buf[4], length);	// SPI HDR 4B, TCP HDR 8B
	}

	/* Update the Rx Read Pointer */
	rd_base += length;
	Tx_Buf[4] = ((uint8_t)((rd_base & 0xFF00) >> 8));
	Tx_Buf[5] = ((uint8_t)(rd_base & 0x00FF));
	spi_Send_Blocking(Sn_RX_RD_BASE + offset, 0x0002);

	/* Send RECV Command */
	Tx_Buf[4] = RECV;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	return length;
}

/* Check socket for incoming data */
uint8_t Wiz_Check_Socket(uint8_t n) {
	/* Read Socket n Interrupt Register */
	Tx_Buf[4] = 0xFF;
	spi_Recv_Blocking(Sn_IR_BASE + 0x0100*n, 0x0001);
	if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
		printf("Sn_IR connect\n");
	if((Rx_Buf[4] & 0x08) == 0x08) // Bit 4 is timeout
		printf("Sn_IR timeout\n");
	if((Rx_Buf[4] & 0x04) == 0x04){ // Bit 3 is data received interrupt
		printf("Sn_IR recv\n");
		return 1;
	}
	return 0;
}

void Wiz_Restart() {
	/* Software Reset Wiznet (Mode Register) */
	Tx_Buf[4] = 0x80; // Data
	spi_Send_Blocking(MR, 0x0001);
}

/* Initialize Wiznet Device */
void ethernetInit(uint8_t protocol, uint8_t socket) {
	Wiz_SSP_Init();
	Wiz_Restart();
	uint16_t i, j;
	for (i = 0; i < 60000; i++) {
		for (j = 0; j < 60; j++) { }
	}

	Wiz_Init();
	Wiz_Network_Init();
	Wiz_Check_Network_Registers();
	Wiz_Memory_Init();
	if(protocol) {
		Wiz_Int_Init(socket);
		Wiz_TCP_Init(socket);
	} else {
		Wiz_UDP_Init(socket);
	}
	Wiz_Clear_Buffer(socket);

	if(protocol) {
		Wiz_TCP_Connect(socket);
		printf("Established TCP connection\n");
	}
}

void Wiz_Deinit(uint8_t protocol, uint8_t socket) {
	/* Disconnect and close socket */
	if(protocol)
		Wiz_TCP_Close(socket);
	else
		Wiz_UDP_Close(socket);

	/* DeInitialize SSP peripheral */
	Chip_SSP_DeInit(LPC_SSP1);
}
