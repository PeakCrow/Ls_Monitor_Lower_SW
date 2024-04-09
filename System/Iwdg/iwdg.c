#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////     
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//¶ÀÁ¢¿´ÃÅ¹    Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2017/4/7
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved                                      
////////////////////////////////////////////////////////////////////////////////// 

IWDG_HandleTypeDef IWDG_Handler; //¶ÀÁ¢¿´ÃÅ¹·¾ä±ú

//³õÊ¼»¯¶ÀÁ¢¿´ÃÅ¹·
//prer:·ÖÆµÊı:IWDG_PRESCALER_4~IWDG_PRESCALER_256
//rlr:×Ô¶¯ÖØ×°ÔØÖµ,0~0XFFF.
//Ê±¼ä¼ÆËã(´ó¸Å):Tout=((4*2^prer)*rlr)/32 (ms).
void IWDG_Init(u8 prer,u16 rlr)
{
    IWDG_Handler.Instance=IWDG;
    IWDG_Handler.Init.Prescaler=prer;    //ÉèÖÃIWDG·ÖÆµÏµÊı
    IWDG_Handler.Init.Reload=rlr;        //ÖØ×°ÔØÖµ
    HAL_IWDG_Init(&IWDG_Handler);        //³õÊ¼»¯IWDG,Ä¬ÈÏ»á¿    Æô¶ÀÁ¢¿´ÃÅ¹·	
}
    
//Î¹¶ÀÁ¢¿´ÃÅ¹·
void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&IWDG_Handler);     //Î¹¹·
}
