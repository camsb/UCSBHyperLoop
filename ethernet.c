#include "ethernet.h"

/*****************************************************************************
 * Public functions
 ****************************************************************************/

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

/**
 * @brief	SSP interrupt handler sub-routine
 * @return	Nothing
 */
void SSPIRQHANDLER(void)
{
	Chip_SSP_Int_Disable(LPC_SSP1);	/* Disable all interrupt */
	if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
		Chip_SSP_Int_RWFrames8Bits(LPC_SSP1, &xf_setup);
	}
	else {
		Chip_SSP_Int_RWFrames16Bits(LPC_SSP1, &xf_setup);
	}

	if ((xf_setup.rx_cnt != xf_setup.length) || (xf_setup.tx_cnt != xf_setup.length)) {
		Chip_SSP_Int_Enable(LPC_SSP1);	/* enable all interrupts */
	}
	else {
		isXferCompleted = 1;
	}
}

/**
 * @brief	DMA interrupt handler sub-routine. Set the waiting flag when transfer is successful
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChSSPTx) == SUCCESS) {
		isDmaTxfCompleted = 1;
	}

	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChSSPRx) == SUCCESS) {
		isDmaRxfCompleted = 1;
	}
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

/* Interrupt based write. NOT WORKING YET. TODO: FIX THIS! */
void spi_Send_Int(uint16_t address, uint16_t length) {
	isXferCompleted = 0;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_Int_FlushData(LPC_SSP1); /* flush data from SSP FiFO */

	Tx_Buf[0] = (address & 0xFF00) >> 8;		// Address MSB
	Tx_Buf[1] = (address & 0x00FF);				// Address LSB
	Tx_Buf[2] = (length  & 0xFF00) >> 8 | 0x80; // OP Code (R/W) | Length MSB
	Tx_Buf[3] = (length  & 0x00FF);				// Data length LSB

	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 6, 0);
	Chip_SSP_Int_RWFrames8Bits(LPC_SSP1, &xf_setup);
	Chip_SSP_Int_Enable(LPC_SSP1);	/* enable interrupt */
	while (!isXferCompleted) {}
	printf("%u, %u, %u, %u, %u\n", Tx_Buf[0], Tx_Buf[1], Tx_Buf[2], Tx_Buf[3], Tx_Buf[4]);
	printf("%u, %u, %u, %u, %u\n", Rx_Buf[0], Rx_Buf[1], Rx_Buf[2], Rx_Buf[3], Rx_Buf[4]);
	Chip_SSP_Int_FlushData(LPC_SSP1);
	Chip_SSP_Int_Disable(LPC_SSP1);	/* disable interrupt */
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 6, 1);
}

/* Interrupt based read. NOT WORKING YET. TODO: FIX THIS! */
void spi_Recv_Int(uint16_t address) {
	isXferCompleted = 0;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_Int_FlushData(LPC_SSP1); /* flush data from SSP1 FiFO */

	Tx_Buf[0] = (address & 0xFF00) >> 8;		// Address MSB
	Tx_Buf[1] = (address & 0x00FF);				// Address LSB
	Tx_Buf[2] = 0x00; 							// OP Code (R/W) | Length MSB
	Tx_Buf[3] = 0x01;							// Data length LSB
	Tx_Buf[4] = 0x00;							// Dummy data

	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 6, 0);
	Chip_SSP_Int_RWFrames8Bits(LPC_SSP1, &xf_setup);
	Chip_SSP_Int_Enable(LPC_SSP1);	/* enable interrupt */
	while (!isXferCompleted) {}
	printf("%u, %u, %u, %u, %u\n", Rx_Buf[0], Rx_Buf[1], Rx_Buf[2], Rx_Buf[3], Rx_Buf[4]);
	Chip_SSP_Int_FlushData(LPC_SSP1);
	Chip_SSP_Int_Disable(LPC_SSP1);	/* disable interrupt */
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 6, 1);
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

	/* Software Reset Wiznet (Mode Register) */
	Tx_Buf[4] = 0x80; // Data
	spi_Send_Blocking(MR, 0x0001);

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

/* TCP Initialization */
void Wiz_TCP_Init(uint8_t n) {
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
	if(Rx_Buf[4] == 0x13)
		printf("Socket %u TCP Initialized Successfully\n", n);
	else
		printf("Socket %u TCP Initialization Failed\n", n);
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
	if(Rx_Buf[4] == 0x22)
		printf("Socket %u UDP Initialized Successfully\n", n);
	else
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

	do {
		/* Wait for connection */
		Rx_Buf[4] = 0xFF;
		spi_Recv_Blocking(Sn_IR_BASE + offset, 0x0001);
	} while((Rx_Buf[4] & 0x01) != 0x01);
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
uint16_t Wiz_Send(uint8_t n, uint8_t* message) {
	uint16_t length;
	uint16_t offset = 0x0100*n;
	uint16_t dst_mask, dst_ptr, wr_base, rd_ptr0, rd_ptr1;

	/* Read current Tx Write Pointer */
	Tx_Buf[4] = 0xFF;
	Tx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_TX_WR_BASE + offset, 0x0002);
	wr_base = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	/* Calculate Tx Buffer Address */
	dst_mask = wr_base & (TX_MAX_MASK);
	dst_ptr = gSn_TX_BASE[n] + dst_mask;

	/* Setup data and length for send */
	sprintf(((char *)Tx_Buf) + 4, (char *)message);
	length = strlen((char *)message);
	Tx_Buf[4 + length++] = '\r';
	Tx_Buf[4 + length++] = '\n';
	Tx_Buf[4 + length++] = '\0';

	/* Load data into Tx Buffer */
	if((dst_mask + length) > (TX_MAX_MASK + 1)) {
		printf("Overflow!\n");
		// TODO: Handle overflow
		// Do stuff (In W5200 datasheet)
	} else {
		spi_Send_Blocking(dst_ptr, length);
	}

	/* Update Tx Write Pointer */
	wr_base += length;
	Tx_Buf[4] = ((uint8_t)((wr_base & 0xFF00) >> 8));
	Tx_Buf[5] = ((uint8_t)(wr_base & 0x00FF));
	spi_Send_Blocking(Sn_TX_WR_BASE + offset, 0x0002);

	/* Read Tx Read Pointer */
	Rx_Buf[4] = Rx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_TX_RD_BASE + offset, 0x0002);
	rd_ptr0 = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	/* SEND Command */
	Tx_Buf[4] = SEND;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	/* Read Tx Read Pointer */
	Rx_Buf[4] = Rx_Buf[5] = 0xFF;
	spi_Recv_Blocking(Sn_TX_RD_BASE + offset, 0x0002);
	rd_ptr1 = (((uint16_t)Rx_Buf[4]) << 8) + ((uint16_t)Rx_Buf[5]);

	return rd_ptr1 - rd_ptr0;
}

/* Read Incoming Data */
uint16_t Wiz_Recv(uint8_t n, uint8_t *message) {
	uint16_t length;
	uint16_t offset = 0x0100*n;
	uint16_t src_mask, src_ptr, rd_base;

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

	/* Load data into Tx Buffer */
	if((src_mask + length) > (RX_MAX_MASK + 1)) {
		printf("Overflow!\n");
		// TODO: Handle overflow
		// Do stuff (In W5200 datasheet)
	} else {
		spi_Recv_Blocking(src_ptr, length);
	}
	memset(message, '\0', DATA_BUF_SIZE);
	memcpy(message, &Rx_Buf[4], length);	// SPI HDR 4B, TCP HDR 8B
//	Rx_Data[length-4] = '\0';

	rd_base += length;
	Tx_Buf[4] = ((uint8_t)((rd_base & 0xFF00) >> 8));
	Tx_Buf[5] = ((uint8_t)(rd_base & 0x00FF));
	spi_Send_Blocking(Sn_RX_RD_BASE + offset, 0x0002);

	Tx_Buf[4] = RECV;
	spi_Send_Blocking(Sn_CR_BASE + offset, 0x0001);

	return length;
}

/* Check socket for incoming data */
uint8_t Wiz_Check_Socket(uint8_t n) {
	/* Read Socket n Interrupt Register */
	Tx_Buf[4] = 0xFF;
	spi_Recv_Blocking(Sn_IR_BASE + 0x0100*n, 0x0001);
	if((Rx_Buf[4] & 0x04) == 0x04) // Bit 3 is data received interrupt
		return 1;
	return 0;
}

/* Initialize Wiznet Device */
void Wiz_Khalifa(uint8_t protocol, uint8_t socket) {
	Wiz_SSP_Init();
	Wiz_Init();
	Wiz_Network_Init();
	Wiz_Check_Network_Registers();
	Wiz_Memory_Init();
	if(protocol)
		Wiz_TCP_Init(socket);
	else
		Wiz_UDP_Init(socket);
	Wiz_Clear_Buffer(socket);

	if(protocol) {
		Wiz_TCP_Connect(socket);
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

//int main(void) {
//	SystemCoreClockUpdate();
//	Board_Init();
//
//	uint8_t socket = 0;				// Socket 0
//	uint8_t protocol = 1;			// TCP
//	uint8_t done = 0;
//	uint16_t length;
//
//	/* Initialize SPI and Wiznet Module */
//	Wiz_Khalifa(protocol, socket);
//
//	char* message = (char *)Tx_Data;
//	sprintf(message, "Hi there, how are you?");
//	if(!protocol) {
//		Wiz_Destination_Init(socket);
//	}
//	length = Wiz_Send(socket, message);
//	printf("%u bytes sent to %u.%u.%u.%u\n",
//		length, REMOTE_IP0, REMOTE_IP1, REMOTE_IP2, REMOTE_IP3);
//	while(!done) {
//		if(Wiz_Check_Socket(socket)) {
//			printf("Data received!\n");
//			length = Wiz_Recv(socket);
//			printf("%s\n", (char *)Rx_Data);
//			done = 1;
//		}
//	}
//
//	/* Uninitialize SPI and Wiznet Module */
//	Wiz_Deinit(protocol, socket);
//	return 0;
//}
