#include "lpc40xx_spi.h"

/**
  * @brief  Initializes the SSP0.
  *
  * @param  None
  * @retval None 
  */
void SD_SPI_Init (void)
{
	SSP_ConfigFormat ssp_format;

	// ssel
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 21);
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 1, 21);

	// miso
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, IOCON_FUNC5);
	// mosi
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 24, IOCON_FUNC5);
	// sclk
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 20, IOCON_FUNC5);

	Chip_SSP_Init(LPC_SSP0);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP0, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);

	Chip_SSP_Enable(LPC_SSP0);
	Chip_SSP_SetMaster(LPC_SSP0, 1);

	SD_SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);

	SD_SPI_CS_High ();
}

/**
  * @brief  Configure SSP0 clock rate.
  *
  * @param  SPI_CLOCKRATE: Specifies the SPI clock rate.
  *         The value should be SPI_CLOCKRATE_LOW or SPI_CLOCKRATE_HIGH.
  * @retval None 
  *
  * SSP0_CLK = CCLK / SPI_CLOCKRATE
  */
void SD_SPI_ConfigClockRate (uint32_t SPI_CLOCKRATE)
{
    /* CPSR must be an even value between 2 and 254 */
    LPC_SSP0->CPSR = (SPI_CLOCKRATE & 0xFE);
}

/**
  * @brief  Set SSEL to low: select spi slave.
  *
  * @param  None.
  * @retval None 
  */
void SD_SPI_CS_Low (void)
{
    /* SSEL is GPIO, set to low.  */
	// LPC_GPIO0->FIOPIN &= ~(1 << 16);

	Chip_GPIO_SetPinOutLow(LPC_GPIO, 1, 21);
}

/**
  * @brief  Set SSEL to high: de-select spi slave.
  *
  * @param  None.
  * @retval None 
  */
void SD_SPI_CS_High (void)
{
    /* SSEL is GPIO, set to high.  */
    // LPC_GPIO0->FIOPIN |= (1 << 16);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 1, 21);
}

/**
  * @brief  Send one byte via MOSI and simutaniously receive one byte via MISO.
  *
  * @param  data: Specifies the byte to be sent out.
  * @retval Returned byte.
  *
  * Note: Each time send out one byte at MOSI, Rx FIFO will receive one byte. 
  *
  * Based off of Chip_SSP_WriteFrames_Blocking and Chip_SSP_ReadFrames_Blocking in ssp_17xx_40xx.c.
  */
uint8_t SD_SPI_SendByte (uint8_t data)
{
	uint8_t recv;
	Chip_SSP_DATA_SETUP_T xf_setup;
	xf_setup.length = 1;
	xf_setup.tx_data = &data;
	xf_setup.rx_data = &recv;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(LPC_SSP0, &xf_setup);
	return recv;
}

/**
  * @brief  Receive one byte via MISO.
  *
  * @param  None.
  * @retval Returned received byte.
  */
uint8_t SD_SPI_RecvByte (void)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SD_SPI_SendByte (0xFF);
}

