#ifndef     _SFDEMO_H
#define     _SFDEMO_H

#include "stm32f4xx.h"
#include "ticktim.h"
#include "bsp_spi_flash.h"
#include "stdio.h"
#include "bsp_usart_fifo.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
void DemoSpiFlash(void);

#ifdef __cplusplus
}
#endif
#endif  // BSP_SPI_FLASH_DEMO_H

