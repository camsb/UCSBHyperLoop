#include "ethernet.h"
#include "sensor_data.h"
#include "I2CPERIPHS.h"
#include <string.h>
#include "stdio.h"
#include "timer.h"
#include "rtc.h"
#include "gpio.h"
#include "qpn_port.h"
#include "subsystems.h"
#include "actuation.h"

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
	DEBUGOUT("MAC Address: %x:%x:%x:%x:%x:%x\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6],
			Rx_Buf[7], Rx_Buf[8], Rx_Buf[9]);

	/* Read Gateway Address Register */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(GAR, 0x0004);
	DEBUGOUT("Default Gateway: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Source Mask Address */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(SUBR, 0x0004);
	DEBUGOUT("Subnet Mask: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Source IP Register */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF; // Dummy data
	spi_Recv_Blocking(SIPR, 0x0004);
	DEBUGOUT("Source IP: %u.%u.%u.%u\n", Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);
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
	//for (i = 0; i < 5 + val_len; i++) {DEBUGOUT("%i:%c\n", i, Net_Tx_Data[i]);}
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
	// TODO: Expand this back to support other subsystems
	int pos = 0;
	memset(Net_Tx_Data, 0, 64);

	Wiz_Recv_Blocking(SOCKET_ID, Net_Rx_Data);
//	int i;
//	for (i = 0; i < DATA_BUF_SIZE; i++) {
//		DEBUGOUT("%i:%c\n", i, Net_Rx_Data[i]);
//	}
//	DEBUGOUT("Receiving Data Packet!\n");

	// Check if the message received matches any state machine control signals, issue it if so.
	int i;
	for (i = 0; i < CS_SIZE; i++){
		if (strcmp(Net_Rx_Data, control_signal_names[i]) == 0){
			DEBUGOUT(control_signal_names[i]);
			DEBUGOUT(" RECEIVED\n");
			dispatch_signal_from_webapp(i); // Corresponding entry in enum control_signals
		}
	}

	if(strstr((char *)Net_Rx_Data, SETDAC) != NULL) {	// Set the DAC
		DEBUGOUT("DAC SET RECEIVED\n");
		DEBUGOUT("DAC recieved: %s\n", Net_Rx_Data);
		int iterator;
		float dacValue = 0;
		for(iterator = 0; iterator < 30; iterator++){
			if(Net_Rx_Data[iterator] == ':'){
				dacValue = (float)(Net_Rx_Data[iterator+1] - '0');
				dacValue += (float)(Net_Rx_Data[iterator+3] - '0')/10;
				if((int)(Net_Rx_Data[iterator+4] - '0') == 9){
					dacValue += 0.1;
				}
			}
		}
		DEBUGOUT("dacValue = %f\n", dacValue);
		set_motor_throttle(0, dacValue);
		set_motor_throttle(1, dacValue);
		set_motor_throttle(2, dacValue);
		set_motor_throttle(3, dacValue);
	}
	if(strstr((char *)Net_Rx_Data, INITTIME) != NULL) {	// Initialize Time
		DEBUGOUT("Initialize Time!\n");
		DEBUGOUT("Time recieved: %s\n", Net_Rx_Data);
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
//					DEBUGOUT("year = %d\n", yearVal);
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
//					DEBUGOUT("month = %d\n", monthVal);
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
//					DEBUGOUT("month day = %d\n", mDayVal);
				}
				if(timeIterator == 3){
					wDayVal = (int)(Net_Rx_Data[iterator+1] - '0');
					wDayVal++;
//					DEBUGOUT("week day = %d\n", wDayVal);
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
//					DEBUGOUT("hour = %d\n", hourVal);
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
//					DEBUGOUT("minute = %d\n", minVal);
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
//					DEBUGOUT("second = %d\n", secVal);
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

	if(pos != 0) {
		Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);
	}
}

void sendPrototypePacket(){
	sendDataFlag = 0;

	// Copy strings to Net_Tx_Data
	int pos = 0;
	memset(Net_Tx_Data, 0, 1024); // Make sure this clears enough space

	/* DAC Output */
	snprintf(PrototypePacket.dac, 16, "%06.2f", motors[0]->throttle_voltage);
	/* Short Ranging */
	snprintf(PrototypePacket.sr1, 16, "%06.2f", (float)motors[0]->short_data[0]);
	snprintf(PrototypePacket.sr2, 16, "%06.2f", (float)motors[1]->short_data[0]);
	snprintf(PrototypePacket.sr3, 16, "%06.2f", (float)motors[2]->short_data[0]);
	snprintf(PrototypePacket.sr4, 16, "%06.2f", (float)motors[3]->short_data[0]);
	/* Photoelectric */
	snprintf(PrototypePacket.ph1, 16, "%06.2f", (float)sensorData.photoelectric);
	/* Current Output */
	snprintf(PrototypePacket.cu1, 16, "%06.2f", (float)motors[0]->amps);
	snprintf(PrototypePacket.cu2, 16, "%06.2f", (float)motors[1]->amps);
	snprintf(PrototypePacket.cu3, 16, "%06.2f", (float)motors[2]->amps);
	snprintf(PrototypePacket.cu4, 16, "%06.2f", (float)motors[3]->amps);
	/* Tachometer Output */ // TODO: EXPAND THIS TO READING BOTH VALUES
	snprintf(PrototypePacket.ta1, 16, "%06.2f", (float)motors[0]->rpm[1]);
	snprintf(PrototypePacket.ta2, 16, "%06.2f", (float)motors[1]->rpm[1]);
	snprintf(PrototypePacket.ta3, 16, "%06.2f", (float)motors[2]->rpm[1]);
	snprintf(PrototypePacket.ta4, 16, "%06.2f", (float)motors[3]->rpm[1]);
	/* Temperature Output */
	snprintf(PrototypePacket.m1tmp1, 16, "%06.f", (float)motors[0]->temperatures[0]);
	snprintf(PrototypePacket.m1tmp2, 16, "%06.f", (float)motors[0]->temperatures[1]);
	snprintf(PrototypePacket.m1tmp3, 16, "%06.f", (float)motors[0]->temperatures[2]);
	snprintf(PrototypePacket.m1tmp4, 16, "%06.f", (float)motors[0]->temperatures[3]);
	snprintf(PrototypePacket.m2tmp1, 16, "%06.f", (float)motors[1]->temperatures[0]);
	snprintf(PrototypePacket.m2tmp2, 16, "%06.f", (float)motors[1]->temperatures[1]);
	snprintf(PrototypePacket.m2tmp3, 16, "%06.f", (float)motors[1]->temperatures[2]);
	snprintf(PrototypePacket.m2tmp4, 16, "%06.f", (float)motors[1]->temperatures[3]);
	snprintf(PrototypePacket.m3tmp1, 16, "%06.f", (float)motors[2]->temperatures[0]);
	snprintf(PrototypePacket.m3tmp2, 16, "%06.f", (float)motors[2]->temperatures[1]);
	snprintf(PrototypePacket.m3tmp3, 16, "%06.f", (float)motors[2]->temperatures[2]);
	snprintf(PrototypePacket.m3tmp4, 16, "%06.f", (float)motors[2]->temperatures[3]);
	snprintf(PrototypePacket.m4tmp1, 16, "%06.f", (float)motors[3]->temperatures[0]);
	snprintf(PrototypePacket.m4tmp2, 16, "%06.f", (float)motors[3]->temperatures[1]);
	snprintf(PrototypePacket.m4tmp3, 16, "%06.f", (float)motors[3]->temperatures[2]);
	snprintf(PrototypePacket.m4tmp4, 16, "%06.f", (float)motors[3]->temperatures[3]);
	/* BMS 0 Voltage */
	snprintf(PrototypePacket.bms0volt0, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][0]);
	snprintf(PrototypePacket.bms0volt1, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][1]);
	snprintf(PrototypePacket.bms0volt2, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][2]);
	snprintf(PrototypePacket.bms0volt3, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][3]);
	snprintf(PrototypePacket.bms0volt4, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][4]);
	snprintf(PrototypePacket.bms0volt5, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[0][5]);
	snprintf(PrototypePacket.bms0volt6, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][0]);
	snprintf(PrototypePacket.bms0volt7, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][1]);
	snprintf(PrototypePacket.bms0volt8, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][2]);
	snprintf(PrototypePacket.bms0volt9, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][3]);
	snprintf(PrototypePacket.bms0volt10, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][4]);
	snprintf(PrototypePacket.bms0volt11, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[1][5]);
	snprintf(PrototypePacket.bms0volt12, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][0]);
	snprintf(PrototypePacket.bms0volt13, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][1]);
	snprintf(PrototypePacket.bms0volt14, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][2]);
	snprintf(PrototypePacket.bms0volt15, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][3]);
	snprintf(PrototypePacket.bms0volt16, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][4]);
	snprintf(PrototypePacket.bms0volt17, 16, "%06.f", (float)maglev_bmses[0]->cell_voltages[2][5]);
	/* BMS 0 Temperature */
	snprintf(PrototypePacket.bms0temp0, 16, "%06.f", (float)maglev_bmses[0]->temperatures[0][0]);
	snprintf(PrototypePacket.bms0temp1, 16, "%06.f", (float)maglev_bmses[0]->temperatures[0][1]);
	snprintf(PrototypePacket.bms0temp2, 16, "%06.f", (float)maglev_bmses[0]->temperatures[1][0]);
	snprintf(PrototypePacket.bms0temp3, 16, "%06.f", (float)maglev_bmses[0]->temperatures[1][1]);
	snprintf(PrototypePacket.bms0temp4, 16, "%06.f", (float)maglev_bmses[0]->temperatures[2][0]);
	snprintf(PrototypePacket.bms0temp5, 16, "%06.f", (float)maglev_bmses[0]->temperatures[2][1]);
	/* BMS 0 Voltage */
	snprintf(PrototypePacket.bms1volt0, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][0]);
	snprintf(PrototypePacket.bms1volt1, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][1]);
	snprintf(PrototypePacket.bms1volt2, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][2]);
	snprintf(PrototypePacket.bms1volt3, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][3]);
	snprintf(PrototypePacket.bms1volt4, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][4]);
	snprintf(PrototypePacket.bms1volt5, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[0][5]);
	snprintf(PrototypePacket.bms1volt6, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][0]);
	snprintf(PrototypePacket.bms1volt7, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][1]);
	snprintf(PrototypePacket.bms1volt8, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][2]);
	snprintf(PrototypePacket.bms1volt9, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][3]);
	snprintf(PrototypePacket.bms1volt10, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][4]);
	snprintf(PrototypePacket.bms1volt11, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[1][5]);
	snprintf(PrototypePacket.bms1volt12, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][0]);
	snprintf(PrototypePacket.bms1volt13, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][1]);
	snprintf(PrototypePacket.bms1volt14, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][2]);
	snprintf(PrototypePacket.bms1volt15, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][3]);
	snprintf(PrototypePacket.bms1volt16, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][4]);
	snprintf(PrototypePacket.bms1volt17, 16, "%06.f", (float)maglev_bmses[1]->cell_voltages[2][5]);
	/* BMS 0 Temperature */
	snprintf(PrototypePacket.bms1temp0, 16, "%06.f", (float)maglev_bmses[1]->temperatures[0][0]);
	snprintf(PrototypePacket.bms1temp1, 16, "%06.f", (float)maglev_bmses[1]->temperatures[0][1]);
	snprintf(PrototypePacket.bms1temp2, 16, "%06.f", (float)maglev_bmses[1]->temperatures[1][0]);
	snprintf(PrototypePacket.bms1temp3, 16, "%06.f", (float)maglev_bmses[1]->temperatures[1][1]);
	snprintf(PrototypePacket.bms1temp4, 16, "%06.f", (float)maglev_bmses[1]->temperatures[2][0]);
	snprintf(PrototypePacket.bms1temp5, 16, "%06.f", (float)maglev_bmses[1]->temperatures[2][1]);

	/* DAC Data */
	send_data_packet_helper(DAC, PrototypePacket.dac, &pos);
	/* Photoelectric Data */
	send_data_packet_helper(PH1, PrototypePacket.ph1, &pos);
	/* Short Ranging */
	send_data_packet_helper(SR1, PrototypePacket.sr1, &pos);
	send_data_packet_helper(SR2, PrototypePacket.sr2, &pos);
	send_data_packet_helper(SR3, PrototypePacket.sr3, &pos);
	send_data_packet_helper(SR4, PrototypePacket.sr4, &pos);
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

	// Send intitial data
	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);

	// Copy strings to Net_Tx_Data
	int pos = 0;
	memset(Net_Tx_Data, 0, 1024); // Make sure this clears enough space
	/* BMS 0 Voltage Data */
	send_data_packet_helper(V00, PrototypePacket.bms0volt0, &pos);
	send_data_packet_helper(V01, PrototypePacket.bms0volt1, &pos);
	send_data_packet_helper(V02, PrototypePacket.bms0volt2, &pos);
	send_data_packet_helper(V03, PrototypePacket.bms0volt3, &pos);
	send_data_packet_helper(V04, PrototypePacket.bms0volt4, &pos);
	send_data_packet_helper(V05, PrototypePacket.bms0volt5, &pos);
	send_data_packet_helper(V06, PrototypePacket.bms0volt6, &pos);
	send_data_packet_helper(V07, PrototypePacket.bms0volt7, &pos);
	send_data_packet_helper(V08, PrototypePacket.bms0volt8, &pos);
	send_data_packet_helper(V09, PrototypePacket.bms0volt9, &pos);
	send_data_packet_helper(V0A, PrototypePacket.bms0volt10, &pos);
	send_data_packet_helper(V0B, PrototypePacket.bms0volt11, &pos);
	send_data_packet_helper(V0C, PrototypePacket.bms0volt12, &pos);
	send_data_packet_helper(V0D, PrototypePacket.bms0volt13, &pos);
	send_data_packet_helper(V0E, PrototypePacket.bms0volt14, &pos);
	send_data_packet_helper(V0F, PrototypePacket.bms0volt15, &pos);
	send_data_packet_helper(V0G, PrototypePacket.bms0volt16, &pos);
	send_data_packet_helper(V0H, PrototypePacket.bms0volt17, &pos);
//	/* BMS 0 Temperature Data*/
	send_data_packet_helper(T00, PrototypePacket.bms0temp0, &pos);
	send_data_packet_helper(T01, PrototypePacket.bms0temp1, &pos);
	send_data_packet_helper(T02, PrototypePacket.bms0temp2, &pos);
	send_data_packet_helper(T03, PrototypePacket.bms0temp3, &pos);
	send_data_packet_helper(T04, PrototypePacket.bms0temp4, &pos);
	send_data_packet_helper(T05, PrototypePacket.bms0temp5, &pos);
	send_data_packet_helper(T06, PrototypePacket.bms0temp6, &pos);
	send_data_packet_helper(T07, PrototypePacket.bms0temp7, &pos);
//	/* BMS 1 Voltage Data */
	send_data_packet_helper(V10, PrototypePacket.bms1volt0, &pos);
	send_data_packet_helper(V11, PrototypePacket.bms1volt1, &pos);
	send_data_packet_helper(V12, PrototypePacket.bms1volt2, &pos);
	send_data_packet_helper(V13, PrototypePacket.bms1volt3, &pos);
	send_data_packet_helper(V14, PrototypePacket.bms1volt4, &pos);
	send_data_packet_helper(V15, PrototypePacket.bms1volt5, &pos);
	send_data_packet_helper(V16, PrototypePacket.bms1volt6, &pos);
	send_data_packet_helper(V17, PrototypePacket.bms1volt7, &pos);
	send_data_packet_helper(V18, PrototypePacket.bms1volt8, &pos);
	send_data_packet_helper(V19, PrototypePacket.bms1volt9, &pos);
	send_data_packet_helper(V1A, PrototypePacket.bms1volt10, &pos);
	send_data_packet_helper(V1B, PrototypePacket.bms1volt11, &pos);
	send_data_packet_helper(V1C, PrototypePacket.bms1volt12, &pos);
	send_data_packet_helper(V1D, PrototypePacket.bms1volt13, &pos);
	send_data_packet_helper(V1E, PrototypePacket.bms1volt14, &pos);
	send_data_packet_helper(V1F, PrototypePacket.bms1volt15, &pos);
	send_data_packet_helper(V1G, PrototypePacket.bms1volt16, &pos);
	send_data_packet_helper(V1H, PrototypePacket.bms1volt17, &pos);
//	/* BMS 1 Temperature Data*/
	send_data_packet_helper(TB0, PrototypePacket.bms1temp0, &pos);
	send_data_packet_helper(TB1, PrototypePacket.bms1temp1, &pos);
	send_data_packet_helper(TB2, PrototypePacket.bms1temp2, &pos);
	send_data_packet_helper(TB3, PrototypePacket.bms1temp3, &pos);
	send_data_packet_helper(TB4, PrototypePacket.bms1temp4, &pos);
	send_data_packet_helper(TB5, PrototypePacket.bms1temp5, &pos);
	send_data_packet_helper(TB6, PrototypePacket.bms1temp6, &pos);
	send_data_packet_helper(TB7, PrototypePacket.bms1temp7, &pos);

	Wiz_Send_Blocking(SOCKET_ID, Net_Tx_Data);

//	int i;
//	for (i = 0; i < DATA_BUF_SIZE; i++) {
//		printf("%i:%c\n", i, Net_Tx_Data[i]);
//	}
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
//				DEBUGOUT("send ok interrupt\n");
			}
			if( socket_int & TIMEOUT ) { // Timeout Occurred
				DEBUGOUT("Timeout interrupt\n");
			}
			if( socket_int & RECV_PKT ) {	 // Packet Received
				DEBUGOUT("Packet interrupt\n");
				recvDataPacket();
			}
			if( socket_int & DISCON_SKT ) {	 // FIN/FIN ACK Received
				DEBUGOUT("Discon interrupt\n");
				if(connectionOpen)
					connectionClosed = 1;
				connectionOpen = 0;
			}
			if( socket_int & Sn_CON ) {	 // Socket Connection Completed
				DEBUGOUT("socket connection completed interrupt\n");
				connectionOpen = 1;
//				Tx_Buf[4] = 0x01 | Tx_Buf[4];
			}
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IR2 + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				DEBUGOUT("IR2 S0 int\n");
			/* Clear Socket n Interrupt Register */
			Tx_Buf[4] = 0xFF;
			spi_Send_Blocking(Sn_IR_BASE + offset, 0x0001);
//			if(Wiz_Check_Socket(0)){
//				DEBUGOUT("Interrupt not cleared\n");
//			}
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(Sn_IMR_BASE + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				DEBUGOUT("Sn_IMR connect\n");
//			if((Rx_Buf[4] & 0x08) == 0x08) // Bit 4 is timeout
//				DEBUGOUT("Sn_IMR timeout\n");
//			if((Rx_Buf[4] & 0x04) == 0x04) // Bit 3 is data received interrupt
//				DEBUGOUT("Sn_IMR recv\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IR2 + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				DEBUGOUT("IR2 S0 int\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(IMR + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x01) == 0x01) // Bit 1 is connected interrupt
//				DEBUGOUT("IMR S0 int for IR2\n");
//			Tx_Buf[4] = 0xFF;
//			spi_Recv_Blocking(Sn_SR_BASE + 0x0100*n, 0x0001);
//			if((Rx_Buf[4] & 0x17) == 0x17) // Bit 1 is connected interrupt
//				DEBUGOUT("Socket still established\n");
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
		DEBUGOUT("Socket %u TCP Initialized Successfully\n", n);
		activeSockets |= 1 << n;
	} else
		DEBUGOUT("Socket %u TCP Initialization Failed\n", n);
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
		DEBUGOUT("Socket %u UDP Initialized Successfully\n", n);
		activeSockets |= 1 << n;
	} else
		DEBUGOUT("Socket %u UDP Initialization Failed\n", n);
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
	DEBUGOUT("Socket %u Source Port: %u\n", n, port);

	/* Read Socket n Destination IP Register (Sn_DIPR) */
	Tx_Buf[4] = Tx_Buf[5] = Tx_Buf[6] = Tx_Buf[7] = 0xFF;
	spi_Recv_Blocking(Sn_DIPR_BASE + offset, 0x0004);
	DEBUGOUT("Socket %u Destination IP: %u.%u.%u.%u\n", n, Rx_Buf[4], Rx_Buf[5], Rx_Buf[6], Rx_Buf[7]);

	/* Read Socket n Destination Port Register (Sn_DPORT) */
	Tx_Buf[4] = Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_DPORT_BASE + offset, 0x0002);
	port = ((uint16_t)Rx_Buf[4] << 8) + (uint16_t)Rx_Buf[5];
	DEBUGOUT("Dest Port: %u, %u\n", Rx_Buf[4], Rx_Buf[5]);
	DEBUGOUT("Socket %u Destination Port: %u\n", n, port);
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
	DEBUGOUT("Establishing TCP connection...\n");
	do {
		/* Wait for connection */
		Rx_Buf[4] = 0xFF;
		spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
	} while((Rx_Buf[4] & 0x01) != 0x01);
	DEBUGOUT("Connected\n");
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
		DEBUGOUT("Sn_IR connect\n");
	if((Rx_Buf[4] & 0x08) == 0x08) // Bit 4 is timeout
		DEBUGOUT("Sn_IR timeout\n");
	if((Rx_Buf[4] & 0x04) == 0x04){ // Bit 3 is data received interrupt
		DEBUGOUT("Sn_IR recv\n");
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
		DEBUGOUT("Established TCP connection\n");
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
