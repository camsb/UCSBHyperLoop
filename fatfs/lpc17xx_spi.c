/**************************************************************************//**
 * @file     lpc17xx_spi.c
 * @brief    Drivers for SSP peripheral in lpc17xx.
 * @version  1.0
 * @date     18. Nov. 2010
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/

#include "lpc17xx_spi.h"

#if 0
/* Macro defines for SSP SR register */
#define SSP_SR_TFE      ((uint32_t)(1<<0)) /** SSP status TX FIFO Empty bit */
#define SSP_SR_TNF      ((uint32_t)(1<<1)) /** SSP status TX FIFO not full bit */
#define SSP_SR_RNE      ((uint32_t)(1<<2)) /** SSP status RX FIFO not empty bit */
#define SSP_SR_RFF      ((uint32_t)(1<<3)) /** SSP status RX FIFO full bit */
#define SSP_SR_BSY      ((uint32_t)(1<<4)) /** SSP status SSP Busy bit */
#define SSP_SR_BITMASK	((uint32_t)(0x1F)) /** SSP SR bit mask */
#endif // 0

/**
  * @brief  Initializes the SSP0.
  *
  * @param  None
  * @retval None 
  */
void SPI_Init (void) 
{
#if 1
	SSP_ConfigFormat ssp_format;

	// ssel
	//Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, IOCON_FUNC3);

    GPIO_Output_Init(1, 21);
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

    SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);

    SPI_CS_High ();
#endif // 0
#if 0
#if 0
    /* Enable SSPI0 block */
    LPC_SC->PCONP |= (1 << 21);
#endif // 0

#if 0
    /* Set SSEL0 as GPIO, output high */
    LPC_PINCON->PINSEL1 &= ~(3 << 0);          /* Configure P0.16(SSEL) as GPIO */
    LPC_GPIO0->FIODIR |= (1 << 16);            /* set P0.16 as output */
#endif // 0
    
    GPIO_Output_Init(1, 21);
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, 1, 21);

#if 0
    /* Configure other SSP pins: SCK, MISO, MOSI */
    LPC_PINCON->PINSEL0 &= ~(3UL << 30);
    LPC_PINCON->PINSEL0 |=  (2UL << 30);          /* P0.15: SCK0 */
    LPC_PINCON->PINSEL1 &= ~((3<<2) | (3<<4));
    LPC_PINCON->PINSEL1 |=  ((2<<2) | (2<<4));  /* P0.17: MISO0, P0.18: MOSI0 */
#endif // 0

    /* Configure other SSP pins: SCK, MISO, MOSI */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 20, IOCON_FUNC5);	// SCLK
	//Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, IOCON_FUNC3);	// SSEL
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, IOCON_FUNC5);	// MISO
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 24, IOCON_FUNC5);	// MOSI

#if 0
    /* Configure SSP0_PCLK to CCLK(100MHz), default value is CCLK/4 */
    LPC_SC->PCLKSEL1 &= ~(3 << 10);
    LPC_SC->PCLKSEL1 |=  (1 << 10);  /* SSP0_PCLK=CCLK */
#endif // 0

    /* Configure SSP0_PCLK to CCLK(100MHz), default value is CCLK/4 */
    //LPC_SYSCTL->PCLKSEL &= 0x00;
    //LPC_SYSCTL->PCLKSEL |= 0x01;  /* SSP0_PCLK=CCLK */

#if 0
    /* 8bit, SPI frame format, CPOL=0, CPHA=0, SCR=0 */  
    LPC_SSP0->CR0 = (0x07 << 0) |     /* data width: 8bit*/
                    (0x00 << 4) |     /* frame format: SPI */
                    (0x00 << 6) |     /* CPOL: low level */
                    (0x00 << 7) |     /* CPHA: first edge */
                    (0x00 << 8);      /* SCR = 0 */
#endif // 0

    Chip_SSP_Init(LPC_SSP0);

    /* 8bit, SPI frame format, CPOL=0, CPHA=0, SCR=0 */
    Chip_SSP_SetFormat(LPC_SSP0, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);

    //Chip_SSP_SetBitRate(LPC_SSP0, 1000000);

    Chip_SSP_Enable(LPC_SSP0);

#if 0
    /* Enable SSP0 as a master */
    LPC_SSP0->CR1 = (0x00 << 0) |   /* Normal mode */
                    (0x01 << 1) |   /* Enable SSP0 */
                    (0x00 << 2) |   /* Master */
                    (0x00 << 3);    /* slave output disabled */
#endif // 0

    /* Enable SSP0 as a master */
    Chip_SSP_SetMaster(LPC_SSP0, SSP_MODE_MASTER);

    /* Configure SSP0 clock rate to 400kHz (100MHz/250) */
    SPI_ConfigClockRate (SPI_CLOCKRATE_LOW);

    /* Set SSEL to high */
    SPI_CS_High ();
#endif // 0
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
void SPI_ConfigClockRate (uint32_t SPI_CLOCKRATE)
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
void SPI_CS_Low (void)
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
void SPI_CS_High (void)
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
uint8_t SPI_SendByte (uint8_t data)
{
	uint8_t recv;
	Chip_SSP_DATA_SETUP_T xf_setup;
	xf_setup.length = 1;
	xf_setup.tx_data = &data;
	xf_setup.rx_data = &recv;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(LPC_SSP0, &xf_setup);
	return recv;

#if 0
	Chip_SSP_SendFrame(LPC_SSP0, data);

	while(LPC_SSP0->SR & SSP_STAT_BSY);

	return Chip_SSP_ReceiveFrame(LPC_SSP0);
#endif // 0

#if 0
	uint32_t tx_cnt = 0, rx_cnt = 0;
	int buffer_len = 1;	// Only sending one byte.

	/* Clear all remaining frames in RX FIFO */
	while (Chip_SSP_GetStatus(LPC_SSP0, SSP_STAT_RNE)) {
		Chip_SSP_ReceiveFrame(LPC_SSP0);
	}

	/* Clear status */
	Chip_SSP_ClearIntPending(LPC_SSP0, SSP_INT_CLEAR_BITMASK);

	uint8_t recv;
	uint8_t *wdata8, *rdata8;

	wdata8 = &data;
	rdata8 = &recv;

	while (tx_cnt < buffer_len || rx_cnt < buffer_len) {
		/* write data to buffer */
		if ((Chip_SSP_GetStatus(LPC_SSP0, SSP_STAT_TNF) == SET) && (tx_cnt < buffer_len)) {
			Chip_SSP_SendFrame(LPC_SSP0, *wdata8);
			tx_cnt++;
		}

		/* Check overrun error */
		if (Chip_SSP_GetRawIntStatus(LPC_SSP0, SSP_RORRIS) == SET) {
			return ERROR;
		}

		/* Check for any data available in RX FIFO */
		while (Chip_SSP_GetStatus(LPC_SSP0, SSP_STAT_RNE) == SET && rx_cnt < buffer_len) {
			*rdata8 = Chip_SSP_ReceiveFrame(LPC_SSP0);
			rdata8++;
			rx_cnt++;
		}
	}

	return recv;
#endif // 0
}

/**
  * @brief  Receive one byte via MISO.
  *
  * @param  None.
  * @retval Returned received byte.
  */
uint8_t SPI_RecvByte (void)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SPI_SendByte (0xFF);
}

/* --------------------------------- End Of File ------------------------------ */
