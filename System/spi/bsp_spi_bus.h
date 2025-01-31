#ifndef __BSP_SPI_BUS_H_
#define __BSP_SPI_BUS_H_

#include "sys.h"


/* 扇区大小--4k */
#define SPI_BUFFER_SIZE        (4 * 1024)


#define SPI_BAUDRATEPRESCALER_42M                SPI_BAUDRATEPRESCALER_2
#define    SPI_BAUDRATEPRESCALER_21M                SPI_BAUDRATEPRESCALER_4
#define SPI_BAUDRATEPRESCALER_10_5M                SPI_BAUDRATEPRESCALER_8
#define SPI_BAUDRATEPRESCALER_5_25M                SPI_BAUDRATEPRESCALER_16
#define SPI_BAUDRATEPRESCALER_2_625M            SPI_BAUDRATEPRESCALER_32
#define SPI_BAUDRATEPRESCALER_1_3125M            SPI_BAUDRATEPRESCALER_64
#define SPI_BAUDRATEPRESCALER_656_25K            SPI_BAUDRATEPRESCALER_128
#define SPI_BAUDRATEPRESCALER_328_125K            SPI_BAUDRATEPRESCALER_256


#define    SPI_APB1_BAUDRATEPRESCALER_21M            SPI_BAUDRATEPRESCALER_2
#define    SPI_APB1_BAUDRATEPRESCALER_10_5M        SPI_BAUDRATEPRESCALER_4
#define SPI_APB1_BAUDRATEPRESCALER_5_25M        SPI_BAUDRATEPRESCALER_8
#define    SPI_APB1_BAUDRATEPRESCALER_2_625M        SPI_BAUDRATEPRESCALER_16
#define    SPI_APB1_BAUDRATEPRESCALER_1_315M        SPI_BAUDRATEPRESCALER_32
#define    SPI_APB1_BAUDRATEPRESCALER_656_25K        SPI_BAUDRATEPRESCALER_64
#define    SPI_APB1_BAUDRATEPRESCALER_328_125K        SPI_BAUDRATEPRESCALER_128
#define    SPI_APB1_BAUDRATEPRESCALER_164_0625K    SPI_BAUDRATEPRESCALER_256


/* 
    SPI1在APB2总线上 SPI2 SPI3在APB1总线上
    APB2总线频率84mhz
*/

/* SPI1引脚配置 */
#define SPIx                    SPI1
#define SPIx_CLK_ENABLE()        __HAL_RCC_SPI1_CLK_ENABLE()

#define DMAx_CLK_ENABLE()        __HAL_RCC_DMA2_CLK_ENABLE()

#define SPIx_FORCE_RESET()        __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()    __HAL_RCC_SPI1_RELEASE_RESET()
/* PB3 */
#define    SPIx_SCK_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define    SPIx_SCK_GPIO            GPIOB
#define    SPIx_SCK_PIN            GPIO_PIN_3
#define SPIx_SCK_AF                GPIO_AF5_SPI1

/* PB4 */
#define SPIx_MISO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO            GPIOB
#define SPIx_MISO_PIN            GPIO_PIN_4
#define SPIx_MISO_AF            GPIO_AF5_SPI1

/* PB5 */
#define SPIx_MOSI_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO            GPIOB
#define SPIx_MOSI_PIN            GPIO_PIN_5
#define SPIx_MOSI_AF            GPIO_AF5_SPI1

/* DMA2 */
#define SPIx_TX_DMA_CHANNEL        DMA_CHANNEL_3
#define SPIx_TX_DMA_STREAM        DMA2_Stream5
#define SPIx_RX_DMA_CHANNEL        DMA_CHANNEL_3
#define SPIx_RX_DMA_STREAM        DMA2_Stream0

/* 中断 */
#define SPIx_IRQn                SPI1_IRQn
#define SPIx_IRQHandler            SPI1_IRQHandler
#define SPIx_DMA_TX_IRQn        DMA2_Stream5_IRQn
#define SPIx_DMA_RX_IRQn        DMA2_Stream0_IRQn
#define SPIx_DMA_TX_IRQHandler    DMA2_Stream5_IRQHandler
#define SPIx_DMA_RX_IRQHandler    DMA2_Stream0_IRQHandler

/* SPI2引脚配置 */
#define SPIx_INK                    SPI2
#define    SPIx_INK_CLK_ENBALE()        __HAL_RCC_SPI2_CLK_ENABLE()

/* PC2 */
#define    SPIx_INK_MISO_CLK_ENBALE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define    SPIx_INK_MISO_GPIO            GPIOC
#define    SPIx_INK_MISO_PIN            GPIO_PIN_2
#define    SPIx_INK_MISO_AF            GPIO_AF5_SPI2

/* PC3 */
#define SPIx_INK_MOSI_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define    SPIx_INK_MOSI_GPIO            GPIOC
#define    SPIx_INK_MOSI_PIN            GPIO_PIN_3
#define    SPIx_INK_MOSI_AF            GPIO_AF5_SPI2

/* PB13 */
#define    SPIx_INK_SCK_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define    SPIx_INK_SCK_GPIO            GPIOB
#define    SPIx_INK_SCK_PIN            GPIO_PIN_13
#define    SPIx_INK_SCK_AF                GPIO_AF5_SPI2

enum{
    TRANSFER_WAIT = 0,
    TRANSFER_COMPLETE,
    TRANSFER_ERROR
};

//#define USE_SPI_DMA        /* DMA方式传输数据 */
//#define USE_SPI_INT        /* 中断方式传输数据 */
#define USE_SPI_POLL        /* 轮询方式传输数据 */

extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
extern uint32_t g_spiLen;
extern uint8_t g_spi_busy;


void bsp_InitSPI1Bus(void);
void bsp_spi1Transfer(void);
void bsp_InitSPI1Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity);

void bsp_Spi1BusEnter(void);
void bsp_Spi1BusExit(void);
uint8_t bsp_SpiBusBusy(void);

void bsp_InitSPI2Bus(void);
void bsp_InitSPI2Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity);
void bsp_spi2Transfer(uint8_t _value);





#endif




