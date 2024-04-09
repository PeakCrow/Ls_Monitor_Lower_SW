#ifndef __BSP_SRAM_H_
#define __BSP_SRAM_H_

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////     
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//SDRAMÇı¶¯    úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2017/4/18
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved                                      
//////////////////////////////////////////////////////////////////////////////////     

//Ê¹ÓÃNOR/SRAMµÄ Bank1.sector3,µØÖ·Î»HADDR[27,26]=10 
//¶ÔIS61LV25616/IS62WV25616,µØÖ·Ïß·¶Î§ÎªA0~A17 
//¶ÔIS61LV51216/IS62WV51216,µØÖ·Ïß·¶Î§ÎªA0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))    


extern SRAM_HandleTypeDef SRAM_Handler;    //SRAM¾ä±ú

void bsp_InitSram(void);
void FSMC_SRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n);
void FSMC_SRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n);
#endif
