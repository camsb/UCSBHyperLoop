#ifndef SLAVE_BOARD_H_
#define SLAVE_BOARD_H_

#include "board.h"

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


/* SPI Global Variables */
SSP_ConfigFormat ssp_format;

// These seem important.
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


void submodule_board_init();
void submodule_board_read();
void submodule_board_deinit();

#endif
