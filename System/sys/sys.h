#ifndef _SYS_H_
#define _SYS_H_


/*
*********************************************************************************************************
*                                           C Library
*********************************************************************************************************
*/
#include <stdarg.h>     /* 参数个数未知时获取函数中的参数 */
#include <stdio.h>      /* 标准输入输出函数 */
#include <stdlib.h>     /* 变量类型,一些宏和各种通用工具函数 */
#include <math.h>       /* 各种数学函数 */
#include <string.h>     /* 各种操作字符数组的函数 */
/*
*********************************************************************************************************
*                                           ST HAL 
*********************************************************************************************************
*/
#include "stm32F407xx.h"
#include "stm32f4xx.h"
/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/
#include "tx_api.h"
#include "tx_timer.h"
#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
/*
*********************************************************************************************************
*                                           FATFS
*********************************************************************************************************
*/
#include "ff.h"
/*
*********************************************************************************************************
*                                           UI
*********************************************************************************************************
*/
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "Monitor.h"
/*
*********************************************************************************************************
*                                           BSP
*********************************************************************************************************
*/
#include "ticktim.h"                /* 软件定时器组 */
#include "bsp_usart_fifo.h"         /* 串口FIFO通讯 */
#include "bsp_key.h"                /* 轻触按键 */
#include "bsp_spi_flash_demo.h"     /* spi flash demo */
#include "bsp_spi_bus.h"            /* spi1 spi2 总线 */
#include "bsp_iic_bus.h"            /* iic1 总线(AT24C02) */
#include "EPD_Test.h"               /* 墨水屏驱动 */
#include "DEV_Config.h"             /* 墨水屏底层配置 */
#include "bsp_font.h"               /* 墨水屏字库 */
#include "bsp_dwt.h"                /* 芯片DWT模块 安富莱驱动提供 */
#include "bsp_led.h"                /* 板载led灯 */
#include "bsp_spi_flash.h"          /* 外部flash模块(W25Q128FV_ID) */
#include "bsp_can_bus.h"            /* can通讯 */
#include "bsp_tim_pwm.h"            /* 定时器中断初始化和pwm初始化 */
#include "bsp_ws2812b.h"            /* WS2812B三色RGB灯 */
#include "bsp_rotation.h"           /* 轮速传感器 */
#include "bsp_mlx90614.h"           /* mlx90614红外温度传感器 */
#include "bsp_lcd.h"                /* 电容屏屏幕显示 */
#include "bsp_touch.h"              /* 电容屏屏幕触摸驱动 触摸画板Demo */
#include "bsp_sdio_sd.h"            /* SD DMA 驱动 */
#include "iwdg.h"                   /* 看门狗模块 */
#include "system_task_create.h"     /* threadx 操作系统任务 */
#include "bsp_sram.h"               /* 板载1M SRAM */



/*
*********************************************************************************************************
*                                           APP
*********************************************************************************************************
*/
#include "gui_task.h"
#include "msg_task.h"
#include "statistic_task.h"


/* 定义常用的数据类型短关键字 */
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;

/*
*********************************************************************************************************
*                                           中断通知标志
*********************************************************************************************************
*/
typedef struct _Sys_IrqNoti_Flag
{
    unsigned int can1_rx_flag    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
    unsigned int    : 1;
}Sys_IrqNoti_Flag, *Sys_IrqNoti_FlagP;


/*
*********************************************************************************************************
*                                          变量和函敿
*********************************************************************************************************
*/

#define DISABLE   0
#define ENABLE    1
#define DISABLED  0
#define ENABLED   1
#define OFF       0
#define ON        1
#define FALSE     0
#define TRUE      1

/* 方便RTOS里面使用 */
extern void SysTick_ISR(void);      /* 滴答定时器中断外部文件声明*/

#if USE_THREADX == 0
/* 裸机下中断通知标志 */
extern Sys_IrqNoti_Flag sys_irqnoti_flag;
#endif

#define bsp_ProPer1ms  SysTick_ISR

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<CM3权威指南>>第五章87页~92页.M4同M3类似,只是寄存器地址改变.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20)       //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20)       //0x40020414
#define GPIOC_ODR_Addr    (GPIOC_BASE+20)       //0x40020814
#define GPIOD_ODR_Addr    (GPIOD_BASE+20)       //0x40020C14
#define GPIOE_ODR_Addr    (GPIOE_BASE+20)       //0x40021014
#define GPIOF_ODR_Addr    (GPIOF_BASE+20)       //0x40021414
#define GPIOG_ODR_Addr    (GPIOG_BASE+20)       //0x40021814
#define GPIOH_ODR_Addr    (GPIOH_BASE+20)       //0x40021C14
#define GPIOI_ODR_Addr    (GPIOI_BASE+20)       //0x40022014
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20)       //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20)       //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16)       //0x40020010
#define GPIOB_IDR_Addr    (GPIOB_BASE+16)       //0x40020410
#define GPIOC_IDR_Addr    (GPIOC_BASE+16)       //0x40020810
#define GPIOD_IDR_Addr    (GPIOD_BASE+16)       //0x40020C10
#define GPIOE_IDR_Addr    (GPIOE_BASE+16)       //0x40021010
#define GPIOF_IDR_Addr    (GPIOF_BASE+16)       //0x40021410
#define GPIOG_IDR_Addr    (GPIOG_BASE+16)       //0x40021810
#define GPIOH_IDR_Addr    (GPIOH_BASE+16)       //0x40021C10
#define GPIOI_IDR_Addr    (GPIOI_BASE+16)       //0x40022010
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16)       //0x40022410
#define GPIOK_IDR_Addr    (GPIOK_BASE+16)       //0x40022810

//IO口操使只对单一的IO口
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)       //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)       //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)       //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)       //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)       //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)       //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)       //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)       //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)       //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)       //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)       //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)       //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)       //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)       //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)       //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)       //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)       //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)       //输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n)       //输出 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n)       //输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)       //输出 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)       //输入 




void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq);//时钟系统配置
/*
*********************************************************************************************************
*                                           汇编函数 nouseful
*********************************************************************************************************
*/

void WFI_SET(void);                    /* 执行WFI指令 */
void INTX_DISABLE(void);               /* 关闭所有中断 */
void INTX_ENABLE(void);                /* 开启所有中断*/
void MSR_MSP(uint32_t addr);           /* 设置堆栈地址 */

#define  USE_THREADX    1               /* 配置是否使用threadx操作系统 */



#endif

