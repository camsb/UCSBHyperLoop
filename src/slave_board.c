#include "slave_board.h"
#include <string.h>
#include "stdio.h"
#include "timer.h"

// J158 is SSP0:
//   *** J158 Label *** (upside-down)
// SSEL (don't use) / MOSI / MISO / SSEL

// on J9:
//   *** J9 Label *** (right-side up, start on left side)
// X / X / X / SCLK

// On Uno:
//  10  /  11  /  12  /  13
// SSEL / MOSI / MISO / SCLK

#define PORT LPC_SSP0

/* SSP Initialization */
void submodule_board_init(){
	Board_SSP_Init(PORT);
	Chip_SSP_Init(PORT);
	// Actual init
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 20, IOCON_FUNC5);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, IOCON_FUNC3);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, IOCON_FUNC5);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 24, IOCON_FUNC5);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(PORT, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);

	Chip_SSP_Enable(PORT);
	Chip_SSP_SetMaster(PORT, 1);
}
int i = 0;
void submodule_board_read(){
	//DEBUGOUT("read");
	i++;
	Tx_Buf[0] = i;
	xf_setup.length = 1;
	xf_setup.tx_data = Tx_Buf;
	xf_setup.rx_data = Rx_Buf;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(PORT, &xf_setup);
    DEBUGOUT("sent %d\n", Tx_Buf[0]);
	DEBUGOUT("got %d\n\n", Rx_Buf[0]);

}


/* Write 'length' bytes sequentially to address (blocking) */
/*
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
*/

/* Read 'length' bytes sequentially from address (blocking) */
/*
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
*/



void submodule_board_deinit(){
	/* DeInitialize SSP peripheral */
	Chip_SSP_DeInit(LPC_SSP1);
}
