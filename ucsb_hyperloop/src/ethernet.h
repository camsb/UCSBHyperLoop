#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "board.h"

/* Wiznet Interrupt Input Pin */
#define WIZNET_INT_PORT					0
#define WIZNET_INT_PIN					4

/* SSP Constants */
#define LPC_SSP           				LPC_SSP1
#define SSP_IRQ           				SSP1_IRQn
#define SSPIRQHANDLER     				SSP1_IRQHandler

/* Ethernet */
#define PROTO_UDP						0
#define PROTO_TCP						1

#define BUFFER_SIZE                    (0x0800)			// 2K
#define DATA_BUF_SIZE					BUFFER_SIZE - 4		// BUFFER_SIZE - (Header Size)
#define SSP_DATA_BITS                  (SSP_BITS_8)
#define SSP_DATA_BIT_NUM(databits)     (databits + 1)
#define SSP_DATA_BYTES(databits)       (((databits) > SSP_BITS_8) ? 2 : 1)
#define SSP_LO_BYTE_MSK(databits)      ((SSP_DATA_BYTES(databits) > 1) ? 0xFF : (0xFF >> \
												               (8 - SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)      ((SSP_DATA_BYTES(databits) > 1) ? (0xFF >> \
												               (16 - SSP_DATA_BIT_NUM(databits))) : 0)
#define SSP_MODE_SEL                   (0x31)
#define SSP_TRANSFER_MODE_SEL          (0x32)
#define SSP_MASTER_MODE_SEL            (0x31)
#define SSP_SLAVE_MODE_SEL             (0x32)
#define SSP_POLLING_SEL                (0x31)
#define SSP_INTERRUPT_SEL              (0x32)
#define SSP_DMA_SEL                    (0x33)

/* Wiznet Registers 		*/
/* Initialization Registers */
#define MR 				0x0000	// Mode Register
#define GAR  			0x0001	// Gateway Address (4B)
#define SUBR 			0x0005	// Subnet Mask Address (4B)
#define SHAR 			0x0009	// Source Hardware Address (6B)
#define SIPR 			0x000F	// Source IP Address (4B)
#define IR				0x0015	// Interrupt Register
#define IMR 			0x0016 	// Socket Mask Register
#define RTR 			0x0017	// Retry Time-Value (2B)
#define RCR 			0x0019 	// Retry Count Register
#define IR2				0x0034	// Socket Interrupt Register
#define IMR2			0x0036	// General Interrupt Mask

#define RX_SIZE			  (0x800)	// 2K
#define TX_SIZE			  (0x800)	// 2K
#define RX_MAX_MASK		RX_SIZE-1
#define TX_MAX_MASK		TX_SIZE-1

/* Socket n (Sn) Control/Status Register Base Address */
#define Sn_MR_BASE 		0x4000	// Mode Register
#define Sn_CR_BASE		0x4001	// Command Register
#define Sn_IR_BASE		0x4002	// Interrupt Register
#define Sn_SR_BASE		0x4003	// Status Register
#define Sn_PORT_BASE	0x4004	// Source Port 			(2B)
#define Sn_DHAR_BASE	0x4006	// Destination MAC 		(6B)
#define Sn_DIPR_BASE	0x400C	// Destination IP 		(4B)
#define Sn_DPORT_BASE	0x4010	// Destination port 	(2B)
#define Sn_MSS_BASE		0x4012	// Maximum Segment Size (2B)
#define Sn_IMR_BASE		0x402C	// Socket Interrupt Mask

/* Socket Control Register Commands */
#define OPEN			0x01
#define LISTEN			0x02
#define CONNECT			0x04
#define DISCON			0x08
#define CLOSE			0x10
#define SEND			0x20
#define SEND_MAC		0x21
#define SEND_KEEP		0x22
#define RECV			0x40

/* Socket Interrupt Bitmasks */
#define SEND_OK			0x10
#define TIMEOUT			0x08
#define RECV_PKT		0x04
#define DISCON_SKT		0x02
#define Sn_CON			0x01

/* Socket n (Sn) Data Pointer Base Registers */
#define Sn_RXMEM_SIZE	0x401E	// Rx Memory size
#define Sn_TXMEM_SIZE	0x401F	// Tx Memory size
#define Sn_TX_FSR_BASE	0x4020	// Tx Free Size			(2B)
#define Sn_TX_RD_BASE	0x4022	// Tx Read Pointer		(2B)
#define Sn_TX_WR_BASE	0x4024	// Tx Write Pointer 	(2B)
#define Sn_RX_RSR_BASE	0x4026	// Rx Received Size 	(2B)
#define Sn_RX_RD_BASE	0x4028	// Rx Read Pointer 		(2B)
#define Sn_RX_WR_BASE	0x402A	// Rx Write Pointer 	(2B)

/* TCP/IP Defines */
#define REMOTE_IP0 		192
#define REMOTE_IP1 		168
#define REMOTE_IP2 		1
#define REMOTE_IP3 		100
#define REMOTE_PORT0 	0xA1 // 41234
#define REMOTE_PORT1 	0x12

/* Protocol Methods */
/* Miscellaneous */
#define MSG 			"MSG"	// Message
#define PWR 			"PWR"	// Power
#define PASSWORD 		"gaucholoop" // Default Password
/* Web Application Commands */
#define INITTIME		"INITTIME"	// Initialize Timer
#define SETDAC			"SETDAC"	// Set the DAC value
/* Web Application Level Acknowledgments */
#define WAK				"WAK"		// Service Propulsion Acknowledgment
#define BAK				"BAK"		// Emergency Brake Acknowledgment
#define PAK				"PAK"		// Power Up Acknowledgment
#define TAK				"TAK"		// Timer initiated Acknowledgment

#define SOCKET_ID 		0

/* Prototype methods */
#define DAC				"DAC"		// DAC
#define SR1				"SR1"		// Short Ranging 1
#define SR2				"SR2"		// Short Ranging 2
#define SR3				"SR3"		// Short Ranging 3
#define SR4				"SR4"		// Short Ranging 4
#define PH1				"PH1"		// Photoelectric 1
#define TA1				"TA1"		// Motor 1 Tachometer
#define TA2				"TA2"		// Motor 2 Tachometer
#define TA3				"TA3"		// Motor 3 Tachometer
#define TA4				"TA4"		// Motor 4 Tachometer
#define CU1				"CU1"		// Motor 1 Current
#define CU2				"CU2"		// Motor 2 Current
#define CU3				"CU3"		// Motor 3 Current
#define CU4				"CU4"		// Motor 4 Current
#define TM1				"TM1"		// Motor 1 Temperature 1
#define TM2				"TM2"		// Motor 1 Temperature 2
#define TM3				"TM3"		// Motor 1 Temperature 3
#define TM4				"TM4"		// Motor 1 Temperature 4
#define TM5				"TM5"		// Motor 2 Temperature 1
#define TM6				"TM6"		// Motor 2 Temperature 2
#define TM7				"TM7"		// Motor 2 Temperature 3
#define TM8				"TM8"		// Motor 2 Temperature 4
#define TM9				"TM9"		// Motor 3 Temperature 1
#define T10				"T10"		// Motor 3 Temperature 2
#define T11				"T11"		// Motor 3 Temperature 3
#define T12				"T12"		// Motor 3 Temperature 4
#define T13				"T13"		// Motor 4 Temperature 1
#define T14				"T14"		// Motor 4 Temperature 2
#define T15				"T15"		// Motor 4 Temperature 3
#define T16				"T16"		// Motor 4 Temperature 4
#define V00				"V00"		// BMS0 Voltage 0
#define V01				"V01"		// BMS0 Voltage 1
#define V02 			"V02"		// BMS0 Voltage 2
#define V03				"V03"		// BMS0 Voltage 3
#define V04				"V04"		// BMS0 Voltage 4
#define V05				"V05"		// BMS0 Voltage 5
#define V06				"V06"		// BMS0 Voltage 6
#define V07				"V07"		// BMS0 Voltage 7
#define V08				"V08"		// BMS0 Voltage 8
#define V09				"V09"		// BMS0 Voltage 9
#define V0A				"V0A"		// BMS0 Voltage 10
#define V0B				"V0B"		// BMS0 Voltage 11
#define V0C				"V0C"		// BMS0 Voltage 12
#define V0D				"V0D"		// BMS0 Voltage 13
#define V0E				"V0E"		// BMS0 Voltage 14
#define V0F				"V0F"		// BMS0 Voltage 15
#define V0G				"V0G"		// BMS0 Voltage 16
#define V0H				"V0H"		// BMS0 Voltage 17
#define T00				"T00"		// BMS0 Temperature 0
#define T01				"T01"		// BMS0 Temperature 1
#define T02				"T02"		// BMS0 Temperature 2
#define T03				"T03"		// BMS0 Temperature 3
#define T04				"T04"		// BMS0 Temperature 4
#define T05				"T05"		// BMS0 Temperature 5
#define T06				"T06"		// BMS0 Temperature 6
#define T07				"T07"		// BMS0 Temperature 7
#define V10				"V10"		// BMS1 Voltage 0
#define V11				"V11"		// BMS1 Voltage 1
#define V12 			"V12"		// BMS1 Voltage 2
#define V13				"V13"		// BMS1 Voltage 3
#define V14				"V14"		// BMS1 Voltage 4
#define V15				"V15"		// BMS1 Voltage 5
#define V16				"V16"		// BMS1 Voltage 6
#define V17				"V17"		// BMS1 Voltage 7
#define V18				"V18"		// BMS1 Voltage 8
#define V19				"V19"		// BMS1 Voltage 9
#define V1A				"V1A"		// BMS1 Voltage 10
#define V1B				"V1B"		// BMS1 Voltage 11
#define V1C				"V1C"		// BMS1 Voltage 12
#define V1D				"V1D"		// BMS1 Voltage 13
#define V1E				"V1E"		// BMS1 Voltage 14
#define V1F				"V1F"		// BMS1 Voltage 15
#define V1G				"V1G"		// BMS1 Voltage 16
#define V1H				"V1H"		// BMS1 Voltage 17
#define TA0				"TA0"		// BMS1 Temperature 0
#define TA1				"TA1"		// BMS1 Temperature 1
#define TA2				"TA2"		// BMS1 Temperature 2
#define TA3				"TA3"		// BMS1 Temperature 3
#define TA4				"TA4"		// BMS1 Temperature 4
#define TA5				"TA5"		// BMS1 Temperature 5
#define TA6				"TA6"		// BMS1 Temperature 6
#define TA7				"TA7"		// BMS1 Temperature 7
#define V00				"V00"		// BMS0 Voltage 0
#define V01				"V01"		// BMS0 Voltage 1
#define V02 			"V02"		// BMS0 Voltage 2
#define V03				"V03"		// BMS0 Voltage 3
#define V04				"V04"		// BMS0 Voltage 4
#define V05				"V05"		// BMS0 Voltage 5
#define V06				"V06"		// BMS0 Voltage 6
#define V07				"V07"		// BMS0 Voltage 7
#define V08				"V08"		// BMS0 Voltage 8
#define V09				"V09"		// BMS0 Voltage 9
#define V0A				"V0A"		// BMS0 Voltage 10
#define V0B				"V0B"		// BMS0 Voltage 11
#define V0C				"V0C"		// BMS0 Voltage 12
#define V0D				"V0D"		// BMS0 Voltage 13
#define V0E				"V0E"		// BMS0 Voltage 14
#define V0F				"V0F"		// BMS0 Voltage 15
#define V0G				"V0G"		// BMS0 Voltage 16
#define V0H				"V0H"		// BMS0 Voltage 17
#define T00				"T00"		// BMS0 Temperature 0
#define T01				"T01"		// BMS0 Temperature 1
#define T02				"T02"		// BMS0 Temperature 2
#define T03				"T03"		// BMS0 Temperature 3
#define T04				"T04"		// BMS0 Temperature 4
#define T05				"T05"		// BMS0 Temperature 5
#define T06				"T06"		// BMS0 Temperature 6
#define T07				"T07"		// BMS0 Temperature 7
#define V10				"V10"		// BMS1 Voltage 0
#define V11				"V11"		// BMS1 Voltage 1
#define V12 			"V12"		// BMS1 Voltage 2
#define V13				"V13"		// BMS1 Voltage 3
#define V14				"V14"		// BMS1 Voltage 4
#define V15				"V15"		// BMS1 Voltage 5
#define V16				"V16"		// BMS1 Voltage 6
#define V17				"V17"		// BMS1 Voltage 7
#define V18				"V18"		// BMS1 Voltage 8
#define V19				"V19"		// BMS1 Voltage 9
#define V1A				"V1A"		// BMS1 Voltage 10
#define V1B				"V1B"		// BMS1 Voltage 11
#define V1C				"V1C"		// BMS1 Voltage 12
#define V1D				"V1D"		// BMS1 Voltage 13
#define V1E				"V1E"		// BMS1 Voltage 14
#define V1F				"V1F"		// BMS1 Voltage 15
#define V1G				"V1G"		// BMS1 Voltage 16
#define V1H				"V1H"		// BMS1 Voltage 17
#define TB0				"TB0"		// BMS1 Temperature 0
#define TB1				"TB1"		// BMS1 Temperature 1
#define TB2				"TB2"		// BMS1 Temperature 2
#define TB3				"TB3"		// BMS1 Temperature 3
#define TB4				"TB4"		// BMS1 Temperature 4
#define TB5				"TB5"		// BMS1 Temperature 5
#define TB6				"TB6"		// BMS1 Temperature 6
#define TB7				"TB7"		// BMS1 Temperature 7

struct prototype_packet {
	// all pointers are ini
	// pass in pointer to null terminated data string
	// packet construction will be taken care of by function
	// format with ###.## zero padded

	/* DAC Data */
	char dac[6];	// DAC
	/* Current Data */
	char cu1[6];	// Motor 1 Current
	char cu2[6];	// Motor 2 Current
	char cu3[6];	// Motor 3 Current
	char cu4[6];	// Motor 4 Current
	/* Short Ranging Data */
	char sr1[6];	// Short Ranging 1
	char sr2[6];	// Short Ranging 2
	char sr3[6];	// Short Ranging 3
	char sr4[6];	// Short Ranging 4
	/* Photoelectric Data */
	char ph1[6];     // Photoelectric 1
	/* Tachometer Data*/
	char ta1[6]; 	// Motor 1 Tachometer
	char ta2[6]; 	// Motor 2 Tachometer
	char ta3[6]; 	// Motor 3 Tachometer
	char ta4[6]; 	// Motor 4 Tachometer
	/* Temperature Data */
	char m1tmp1[6]; // Motor 1 Temperature1
	char m1tmp2[6]; // Motor 1 Temperature2
	char m1tmp3[6]; // Motor 1 Temperature3
	char m1tmp4[6]; // Motor 1 Temperature4
	char m2tmp1[6]; // Motor 2 Temperature1
	char m2tmp2[6]; // Motor 2 Temperature2
	char m2tmp3[6]; // Motor 2 Temperature3
	char m2tmp4[6]; // Motor 2 Temperature4
	char m3tmp1[6]; // Motor 3 Temperature1
	char m3tmp2[6]; // Motor 3 Temperature2
	char m3tmp3[6]; // Motor 3 Temperature3
	char m3tmp4[6]; // Motor 3 Temperature4
	char m4tmp1[6]; // Motor 4 Temperature1
	char m4tmp2[6]; // Motor 4 Temperature2
	char m4tmp3[6]; // Motor 4 Temperature3
	char m4tmp4[6]; // Motor 4 Temperature4
	/* BMS 0 */
	char bms0volt0[6]; // BMS 0 Voltage 0
	char bms0volt1[6]; // BMS 0 Voltage 1
	char bms0volt2[6]; // BMS 0 Voltage 2
	char bms0volt3[6]; // BMS 0 Voltage 3
	char bms0volt4[6]; // BMS 0 Voltage 4
	char bms0volt5[6]; // BMS 0 Voltage 5
	char bms0volt6[6]; // BMS 0 Voltage 6
	char bms0volt7[6]; // BMS 0 Voltage 7
	char bms0volt8[6]; // BMS 0 Voltage 8
	char bms0volt9[6]; // BMS 0 Voltage 9
	char bms0volt10[6]; // BMS 0 Voltage 10
	char bms0volt11[6]; // BMS 0 Voltage 11
	char bms0volt12[6]; // BMS 0 Voltage 12
	char bms0volt13[6]; // BMS 0 Voltage 13
	char bms0volt14[6]; // BMS 0 Voltage 14
	char bms0volt15[6]; // BMS 0 Voltage 15
	char bms0volt16[6]; // BMS 0 Voltage 16
	char bms0volt17[6]; // BMS 0 Voltage 17
	char bms0temp0[6]; // BMS 0 Temperature 0
	char bms0temp1[6]; // BMS 0 Temperature 1
	char bms0temp2[6]; // BMS 0 Temperature 2
	char bms0temp3[6]; // BMS 0 Temperature 3
	char bms0temp4[6]; // BMS 0 Temperature 4
	char bms0temp5[6]; // BMS 0 Temperature 5
	char bms0temp6[6]; // BMS 0 Temperature 6
	char bms0temp7[6]; // BMS 0 Temperature 7
	/* BMS 1 */
	char bms1volt0[6]; // BMS 1 Voltage 0
	char bms1volt1[6]; // BMS 1 Voltage 1
	char bms1volt2[6]; // BMS 1 Voltage 2
	char bms1volt3[6]; // BMS 1 Voltage 3
	char bms1volt4[6]; // BMS 1 Voltage 4
	char bms1volt5[6]; // BMS 1 Voltage 5
	char bms1volt6[6]; // BMS 1 Voltage 6
	char bms1volt7[6]; // BMS 1 Voltage 7
	char bms1volt8[6]; // BMS 1 Voltage 8
	char bms1volt9[6]; // BMS 1 Voltage 9
	char bms1volt10[6]; // BMS 1 Voltage 10
	char bms1volt11[6]; // BMS 1 Voltage 11
	char bms1volt12[6]; // BMS 1 Voltage 12
	char bms1volt13[6]; // BMS 1 Voltage 13
	char bms1volt14[6]; // BMS 1 Voltage 14
	char bms1volt15[6]; // BMS 1 Voltage 15
	char bms1volt16[6]; // BMS 1 Voltage 16
	char bms1volt17[6]; // BMS 1 Voltage 17
	char bms1temp0[6]; // BMS 1 Temperature 0
	char bms1temp1[6]; // BMS 1 Temperature 1
	char bms1temp2[6]; // BMS 1 Temperature 2
	char bms1temp3[6]; // BMS 1 Temperature 3
	char bms1temp4[6]; // BMS 1 Temperature 4
	char bms1temp5[6]; // BMS 1 Temperature 5
	char bms1temp6[6]; // BMS 1 Temperature 6
	char bms1temp7[6]; // BMS 1 Temperature 7
};

struct prototype_packet PrototypePacket;

extern uint16_t gSn_RX_BASE[];
extern uint16_t gSn_TX_BASE[];

/* SPI Global Variables */
SSP_ConfigFormat ssp_format;
Chip_SSP_DATA_SETUP_T xf_setup;
uint8_t Tx_Buf[BUFFER_SIZE];
uint8_t Tx_Data[DATA_BUF_SIZE];
uint8_t Rx_Buf[BUFFER_SIZE];
uint8_t Rx_Data[DATA_BUF_SIZE];
uint8_t activeSockets, connectionOpen, connectionClosed;
uint16_t int_length;
uint16_t int_dst_mask, int_dst_ptr, int_wr_base, int_rd_ptr0, int_rd_ptr1;
uint16_t int_src_mask, int_src_ptr, int_rd_base;
volatile uint8_t sendDataFlag;
volatile uint8_t wizIntFlag;
uint8_t eBrakeFlag, powerUpFlag, powerDownFlag, serPropulsionWheels;

// DATA_BUF_SIZE is the size of a packet, which we don't expect to exceed
uint8_t Net_Tx_Data[DATA_BUF_SIZE];
uint8_t Net_Rx_Data[DATA_BUF_SIZE];

void wizIntFunction();
void rec_method(char *method, char *val, int *val_len);
void send_method(char *method, char* val, int val_len);
void sendPrototypePacket();
void sendSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);
void Wiz_Restart();
void Wiz_Init();
void Wiz_SSP_Init();
void Wiz_Network_Init();
void Wiz_Check_Network_Registers();
void Wiz_Memory_Init();
void Wiz_Int_Init(uint8_t n);
void Wiz_TCP_Init(uint8_t n);
void Wiz_UDP_Init(uint8_t n);
void Wiz_Destination_Init(uint8_t n);
void Wiz_Address_Check(uint8_t n);
void Wiz_TCP_Connect(uint8_t n);
void Wiz_TCP_Close(uint8_t n);
void Wiz_UDP_Close(uint8_t n);
void Wiz_Clear_Buffer(uint8_t n);
void ethernetInit(uint8_t protocol, uint8_t socket);
void Wiz_Deinit(uint8_t protocol, uint8_t socket);
void spi_Send_Blocking(uint16_t address, uint16_t length);
void spi_Recv_Blocking(uint16_t address, uint16_t length);
void TIMER2_IRQHandler(void);
void sendSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);
void send_data_packet_helper(char *method, char *val, int *position);
void send_data_ack_helper(char *method, int *position);
uint8_t Wiz_Check_Socket(uint8_t n);
uint8_t Wiz_Int_Clear(uint8_t n);
uint16_t Wiz_Send_Blocking(uint8_t n, uint8_t* message);
uint16_t Wiz_Recv_Blocking(uint8_t n, uint8_t* message);

#endif
