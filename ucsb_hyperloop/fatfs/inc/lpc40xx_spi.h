#ifndef __LPC40xx_SPI_H
#define __LPC40xx_SPI_H

#include "board.h"                 /* LPC17xx Definitions */

/* SPI clock rate setting. 
SSP0_CLK = SystemCoreClock / divider,
The divider must be a even value between 2 and 254!
In SPI mode, max clock speed is 20MHz for MMC and 25MHz for SD */
#define SPI_CLOCKRATE_LOW   (uint32_t) (250)   /* 100MHz / 250 = 400kHz */
#define SPI_CLOCKRATE_HIGH  (uint32_t) (4)     /* 100MHz / 4 = 25MHz */

/* Public functions */
void    SD_SPI_Init (void);
void    SD_SPI_ConfigClockRate (uint32_t SPI_CLOCKRATE);
void    SD_SPI_CS_Low (void);
void    SD_SPI_CS_High (void);
uint8_t SD_SPI_SendByte (uint8_t data);
uint8_t SD_SPI_RecvByte (void);

#endif  // __LPC40xx_SPI_H


