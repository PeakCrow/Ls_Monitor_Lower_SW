/*
* 
*
*    Ä£¿éÃû³Æ : TIM»ù±¾¶¨Ê±ÖÐ¶ÏºÍPWMÇý¶¯Ä£¿é
*    ÎÄ¼þÃû³Æ : bsp_tim_pwm.c
*    °æ    ±¾ : V1.6
*    Ëµ    Ã÷ : ÀûÓÃSTM32F429ÄÚ²¿TIMÊä³öPWMÐÅºÅ£¬ ²¢ÊµÏÖ»ù±¾µÄ¶¨Ê±ÖÐ¶Ï
*    ÐÞ¸Ä¼ÇÂ¼ :
*        °æ±¾ºÅ  ÈÕÆÚ        ×÷Õß     ËµÃ÷
*        V1.0    2013-08-16 armfly  ÕýÊ½·¢²¼
*        V1.1    2014-06-15 armfly  ÍêÉÆ bsp_SetTIMOutPWM£¬µ±Õ¼¿Õ±È=0ºÍ100%Ê±£¬¹Ø±Õ¶¨Ê±Æ÷£¬GPIOÅäÖÃÎªÊä³ö
*        V1.2    2015-05-08 armfly  ½â¾öTIM8²»ÄÜÊä³öPWMµÄÎÊÌâ¡£
*        V1.3    2015-07-23 armfly  ³õÊ¼»¯¶¨Ê±Æ÷£¬±ØÐëÉèÖÃ TIM_TimeBaseInitStruct.TIM_RepetitionCounte        = 0x0000;		
*                                   TIM1 ºÍ TIM8 ±ØÐëÉèÖÃ¡£·ñÔò·äÃùÆ÷µÄ¿ØÖÆ²»Õý³£¡£
*        V1.4    2015-07-30 armfly  Ôö¼Ó·´ÏàÒý½ÅÊä³öPWMº¯Êý bsp_SetTIMOutPWM_N();
*        V1.5    2016-02-01 armfly  È¥µô TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
*        V1.6    2016-02-27 armfly  ½â¾öTIM14ÎÞ·¨ÖÐ¶ÏµÄBUG, TIM8_TRG_COM_TIM14_IRQn
*
*    Copyright (C), 2018-2030, °²¸»À³µç×Ó www.armfly.com
*
* 
*/

#include "bsp_tim_pwm.h"


/*
     ¿ÉÒÔÊä³öµ½GPIOµÄTIMÍ¨µÀ:

    TIM1_CH1, PA8,    PE9,
    TIM1_CH2, PA9,    PE11
    TIM1_CH3, PA10,    PE13
    TIM1_CH4, PA11,    PE14

    TIM2_CH1, PA15 (½öÏÞ429£¬439) 407Ã»ÓÐ´Ë½Å
    TIM2_CH2, PA1,    PB3
    TIM2_CH3, PA2,    PB10
    TIM2_CH4, PA3,    PB11

    TIM3_CH1, PA6,  PB4, PC6
    TIM3_CH2, PA7,    PB5, PC7
    TIM3_CH3, PB0,    PC8
    TIM3_CH4, PB1,    PC9

    TIM4_CH1, PB6,  PD12
    TIM4_CH2, PB7,    PD13
    TIM4_CH3, PB8,    PD14
    TIM4_CH4, PB9,    PD15

    TIM5_CH1, PA0,  PH10
    TIM5_CH2, PA1,    PH11
    TIM5_CH3, PA2,    PH12
    TIM5_CH4, PA3,    PI10

    TIM8_CH1, PC6,  PI5
    TIM8_CH2, PC7,    PI6
    TIM8_CH3, PC8,    PI7
    TIM8_CH4, PC9,    PI2

    TIM9_CH1, PA2,  PE5
    TIM9_CH2, PA3,    PE6

    TIM10_CH1, PB8,  PF6

    TIM11_CH1, PB9,  PF7

    TIM12_CH1, PB14,  PH6
    TIM12_CH2, PB15,  PH9

    TIM13_CH1, PA6,  PF8
    TIM14_CH1, PA7,  PF9

    APB1 ¶¨Ê±Æ÷ÓÐ TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
    APB2 ¶¨Ê±Æ÷ÓÐ TIM1, TIM8 ,TIM9, TIM10, TIM11
    

    APB1 ¶¨Ê±Æ÷µÄÊäÈëÊ±ÖÓ TIMxCLK = SystemCoreClock / 2; 84M
    APB2 ¶¨Ê±Æ÷µÄÊäÈëÊ±ÖÓ TIMxCLK = SystemCoreClock; 168M
*/


static DMA_HandleTypeDef hdma_ch1 = {0};
TIM_HandleTypeDef  g_TimHandle = {0};

/*
* 
*    º¯ Êý Ãû: bsp_RCC_GPIO_Enable
*    ¹¦ÄÜËµÃ÷: Ê¹ÄÜGPIOÊ±ÖÓ
*    ÐÎ    ²Î: GPIOx GPIOA - GPIOI
*    ·µ »Ø Öµ: ÎÞ
* 
*/
void bsp_RCC_GPIO_Enable(GPIO_TypeDef* GPIOx)
{
    if (GPIOx == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (GPIOx == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (GPIOx == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (GPIOx == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (GPIOx == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (GPIOx == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (GPIOx == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (GPIOx == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
    else if (GPIOx == GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
}

/*
* 
*    º¯ Êý Ãû: bsp_RCC_TIM_Enable
*    ¹¦ÄÜËµÃ÷: Ê¹ÄÜTIM RCC Ê±ÖÓ
*    ÐÎ    ²Î: TIMx TIM1 - TIM14
*    ·µ »Ø Öµ: ÎÞ
* 
*/
void bsp_RCC_TIM_Enable(TIM_TypeDef* TIMx)
{
    if (TIMx == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
    else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
    else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_ENABLE();
    else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_ENABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();
    else
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }    
}

/*
* 
*    º¯ Êý Ãû: bsp_RCC_TIM_Disable
*    ¹¦ÄÜËµÃ÷: ¹Ø±ÕTIM RCC Ê±ÖÓ
*    ÐÎ    ²Î: TIMx TIM1 - TIM14
*    ·µ »Ø Öµ: TIMÍâÉèÊ±ÖÓÃû
* 
*/
void bsp_RCC_TIM_Disable(TIM_TypeDef* TIMx)
{
    /*
        APB1 ¶¨Ê±Æ÷ÓÐ TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
        APB2 ¶¨Ê±Æ÷ÓÐ TIM1, TIM8 ,TIM9, TIM10, TIM11
    */
    if (TIMx == TIM1) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_DISABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_DISABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_DISABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_DISABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_DISABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_DISABLE();
    else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_DISABLE();
    else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_DISABLE();
    else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_DISABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_DISABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_DISABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_DISABLE();
    else
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }
}

/*
* 
*    º¯ Êý Ãû: bsp_GetAFofTIM
*    ¹¦ÄÜËµÃ÷: ¸ù¾ÝTIM µÃµ½AF¼Ä´æÆ÷ÅäÖÃ
*    ÐÎ    ²Î: TIMx TIM1 - TIM14
*    ·µ »Ø Öµ: AF¼Ä´æÆ÷ÅäÖÃ
* 
*/
uint8_t bsp_GetAFofTIM(TIM_TypeDef* TIMx)
{
    uint8_t ret = 0;

    if (TIMx == TIM1) ret = GPIO_AF1_TIM1;
    else if (TIMx == TIM2) ret = GPIO_AF1_TIM2;
    
    else if (TIMx == TIM3) ret = GPIO_AF2_TIM3;
    else if (TIMx == TIM4) ret = GPIO_AF2_TIM4;
    else if (TIMx == TIM5) ret = GPIO_AF2_TIM5;
    
    else if (TIMx == TIM8) ret = GPIO_AF3_TIM8;
    else if (TIMx == TIM9) ret = GPIO_AF3_TIM9;
    else if (TIMx == TIM10) ret = GPIO_AF3_TIM10;
    else if (TIMx == TIM11) ret = GPIO_AF3_TIM11;
    
    else if (TIMx == TIM12) ret = GPIO_AF9_TIM12;
    else if (TIMx == TIM13) ret = GPIO_AF9_TIM13;
    else if (TIMx == TIM14) ret = GPIO_AF9_TIM14;
    else
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }
    
    return ret;
}

/*
* 
*    º¯ Êý Ãû: bsp_ConfigTimGpio
*    ¹¦ÄÜËµÃ÷: ÅäÖÃGPIOºÍTIMÊ±ÖÓ£¬ GPIOÁ¬½Óµ½TIMÊä³öÍ¨µÀ
*    ÐÎ    ²Î: GPIOx : GPIOA - GPIOK
*              GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*              TIMx : TIM1 - TIM14
*    ·µ »Ø Öµ: ÎÞ
* 
*/
void bsp_ConfigTimGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX, TIM_TypeDef* TIMx)
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    /* Ê¹ÄÜGPIOÊ±ÖÓ */
    bsp_RCC_GPIO_Enable(GPIOx);

      /* Ê¹ÄÜTIMÊ±ÖÓ */
    bsp_RCC_TIM_Enable(TIMx);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = bsp_GetAFofTIM(TIMx);
    GPIO_InitStruct.Pin = GPIO_PinX;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*
* 
*    º¯ Êý Ãû: bsp_ConfigGpioOut
*    ¹¦ÄÜËµÃ÷: ÅäÖÃGPIOÎªÍÆÍìÊä³ö¡£Ö÷ÒªÓÃÓÚPWMÊä³ö£¬Õ¼¿Õ±ÈÎª0ºÍ100µÄÇé¿ö¡£
*    ÐÎ    ²Î: GPIOx : GPIOA - GPIOK
*              GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*    ·µ »Ø Öµ: ÎÞ
* 
*/
void bsp_ConfigGpioOut(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX)
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    bsp_RCC_GPIO_Enable(GPIOx);        /* Ê¹ÄÜGPIOÊ±ÖÓ */

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin = GPIO_PinX;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop_DMA(&g_TimHandle, TIM_CHANNEL_1);
}

void MX_DMA_Init(void)
{
    
        
        __HAL_RCC_DMA1_CLK_ENABLE();//¿ªÆôDMA1Ê±ÖÓ
        __DMA1_CLK_ENABLE();
        
        hdma_ch1.Instance                     = DMA1_Stream4;
        hdma_ch1.Init.Channel                 = DMA_CHANNEL_5;            //Í¨µÀÑ¡Ôñ2--
        hdma_ch1.Init.Direction                = DMA_MEMORY_TO_PERIPH;        //´æ´¢Æ÷µ½ÍâÉè
        hdma_ch1.Init.PeriphInc             = DMA_PINC_DISABLE;            //ÍâÉè·ÇÔöÁ¿Ä£Ê½
        hdma_ch1.Init.MemInc                = DMA_MINC_ENABLE;            //´æ´¢Æ÷ÔöÁ¿Ä£Ê½
        hdma_ch1.Init.PeriphDataAlignment     = DMA_PDATAALIGN_HALFWORD;        //ÍâÉèÊý¾Ý³¤¶È16Î»
        hdma_ch1.Init.MemDataAlignment         = DMA_MDATAALIGN_HALFWORD;        //´æ´¢Æ÷Êý¾Ý³¤¶È16Î»
        hdma_ch1.Init.Mode                     = DMA_CIRCULAR;                //ÍâÉèÖÜÆÚÄ£Ê½¿ÉÒÔ·¢ËÍÊý×éÄÚµÄÊý¾Ý£¬Õý³£Ä£Ê½Ö»»á·¢ËÍµÚÒ»¸öÊý¾Ý
        hdma_ch1.Init.Priority                 = DMA_PRIORITY_LOW;            //ÖÐµÈÓÅÏÈ¼¶
        hdma_ch1.Init.FIFOMode                 = DMA_FIFOMODE_DISABLE;
        hdma_ch1.Init.FIFOThreshold         = DMA_FIFO_THRESHOLD_FULL;
        hdma_ch1.Init.MemBurst                 = DMA_MBURST_SINGLE;        //´æ´¢Æ÷µ¥´Î´«Êä
        hdma_ch1.Init.PeriphBurst             = DMA_MBURST_SINGLE;        //ÍâÉèÍ»·¢µ¥´Î´«Êä
        
        __HAL_LINKDMA(&g_TimHandle,hdma[TIM_DMA_ID_CC1],hdma_ch1);//½«DMAÓëTIMÁª    µÆðÀ´	
        
        if(HAL_DMA_DeInit(&hdma_ch1) != HAL_OK)
            {
                printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
            }
        if(HAL_DMA_Init((&g_TimHandle)->hdma[TIM_DMA_ID_CC1]) != HAL_OK)//4--
            {
                printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
            }
        
        HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);//6--
        HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    
}
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_ch1);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TimHandle);
}


/*
*   å‡½ æ•° å: bsp_SetTIMOutPWM
*   åŠŸèƒ½è¯´æ˜Ž: è®¾ç½®å¼•è„šè¾“å‡ºçš„PWMä¿¡å·çš„é¢‘çŽ‡å’Œå ç©ºæ¯”.  å½“é¢‘çŽ‡ä¸º0ï¼Œå¹¶ä¸”å ç©ºä¸º0æ—¶ï¼Œå…³é—­å®šæ—¶å™¨ï¼ŒGPIOè¾“å‡º0ï¼›
*             å½“é¢‘çŽ‡ä¸º0ï¼Œå ç©ºæ¯”ä¸º100%æ—¶ï¼ŒGPIOè¾“å‡º1.
*   å½¢    å‚: GPIOx : GPIOA - GPIOK
*             GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
*             TIMx : TIM1 - TIM14
*             _ucChannelï¼šä½¿ç”¨çš„å®šæ—¶å™¨é€šé“ï¼ŒèŒƒå›´1 - 4
*             _ulFreq : PWMä¿¡å·é¢‘çŽ‡ï¼Œå•ä½Hz (å®žé™…æµ‹è¯•ï¼Œå¯ä»¥è¾“å‡º100MHzï¼‰ï¼Œ0 è¡¨ç¤ºç¦æ­¢è¾“å‡º
*             _ulDutyCycle : PWMä¿¡å·å ç©ºæ¯”ï¼Œå•ä½: ä¸‡åˆ†ä¹‹ä¸€ã€‚å¦‚5000ï¼Œè¡¨ç¤º50.00%çš„å ç©ºæ¯”
*   è¿” å›ž å€¼: æ— 
*/
void bsp_SetTIMOutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
     uint32_t _ulFreq, uint32_t _ulDutyCycle)
{                                    //GRB
    static uint16_t pwm_led4[240 +12 * 24]  = {    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
                                                30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30
                                        };

    TIM_HandleTypeDef TimHandle = {0};
    TIM_OC_InitTypeDef sConfig = {0};    
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t pulse;
    uint32_t uiTIMxCLK;
    const uint16_t TimChannel[6+1] = {0, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};

    if (_ucChannel > 6)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }
    
    if (_ulDutyCycle == 0)
    {        
        //bsp_RCC_TIM_Disable(TIMx);        /* ¹Ø±ÕTIMÊ±ÖÓ, ¿ÉÄÜÓ°Ï        äËûÍ¨µÀ */		
        bsp_ConfigGpioOut(GPIOx, GPIO_Pin);    /* ÅäÖÃGPIOÎªÍÆÍì                 */				
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);    /* PWM = 0 */        
        return;
    }
    else if (_ulDutyCycle == 10000)
    {
        //bsp_RCC_TIM_Disable(TIMx);        /* ¹Ø±ÕTIMÊ±ÖÓ, ¿ÉÄÜÓ°ÏìÆäËûÍ¨µÀ */
        bsp_ConfigGpioOut(GPIOx, GPIO_Pin);    /* ÅäÖÃGPIOÎªÍÆÍì        ³ö */		
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);    /* PWM = 1 */            
        return;
    }
    
    /* ÏÂÃæÊÇPWMÊä³ö */
    
    bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx);    /* Ê¹ÄÜGPIOºÍTIMÊ±ÖÓ£¬²¢Á¬½ÓTIMÍ¨µÀµ½GPIO */
    
    if(TIMx == TIM3)
        MX_DMA_Init();
    /*-----------------------------------------------------------------------
        system_stm32f4xx.c ÎÄ¼þÖÐ void SetSysClock(void) º¯Êý¶ÔÊ±ÖÓµÄÅäÖÃÈçÏÂ£º

        HCLK = SYSCLK / 1     (AHB1Periph)
        PCLK2 = HCLK / 2      (APB2Periph)
        PCLK1 = HCLK / 4      (APB1Periph)

        ÒòÎªAPB1 prescaler != 1, ËùÒÔ APB1ÉÏµÄTIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
        ÒòÎªAPB2 prescaler != 1, ËùÒÔ APB2ÉÏµÄTIMxCLK = PCLK2 x 2 = SystemCoreClock;

        APB1 ¶¨Ê±Æ÷ÓÐ TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
        APB2 ¶¨Ê±Æ÷ÓÐ TIM1, TIM8 ,TIM9, TIM10, TIM11

    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
    {
        /* APB2 ¶¨Ê±Æ÷Ê±ÖÓ = 168M */
        uiTIMxCLK = SystemCoreClock;
    }
    else    
    {
        /* APB1 ¶¨Ê±Æ÷ = 84M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;                    /* ·ÖÆµ±È = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;        /* ×Ô¶¯ÖØ×°µÄÖµ */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;                    /* ·ÖÆµ±È = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;        /* ×Ô¶¯ÖØ×°µÄÖµ */
    }
    else    /* ´óÓÚ4KµÄÆµÂÊ£¬ÎÞÐè·ÖÆµ */
    {
        usPrescaler = 0;                    /* ·ÖÆµ±È = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1;    /* ×Ô¶¯ÖØ×°µÄÖµ */
    }
    pulse = (_ulDutyCycle * usPeriod) / 10000;

    if(TIMx == TIM3)
        HAL_TIM_PWM_DeInit(&g_TimHandle);
    else
        HAL_TIM_PWM_DeInit(&TimHandle);
    if(TIMx == TIM3)
        {
            /*  PWMÆµÂÊ = TIMxCLK / usPrescaler + 1£©/usPeriod + 1£©*/
            g_TimHandle.Instance = TIMx;
            g_TimHandle.Init.Prescaler         = usPrescaler;
            g_TimHandle.Init.Period            = usPeriod;
            g_TimHandle.Init.ClockDivision     = 0;
            g_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
            g_TimHandle.Init.RepetitionCounter = 0;
            g_TimHandle.Init.AutoReloadPreload = 0;
        }
    else
        {    /*  PWMÆµÂÊ = TIMxCLK / usPrescaler + 1£©/usPeriod + 1£©*/
            TimHandle.Instance = TIMx;
            TimHandle.Init.Prescaler         = usPrescaler;
            TimHandle.Init.Period            = usPeriod;
            TimHandle.Init.ClockDivision     = 0;
            TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
            TimHandle.Init.RepetitionCounter = 0;
            TimHandle.Init.AutoReloadPreload = 0;
        }
    if(TIMx == TIM3)
        {    
            
            if (HAL_TIM_PWM_Init(&g_TimHandle) != HAL_OK)
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
            
        }
    else
        {
            if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
        }


    /* ÅäÖÃ¶¨Ê±Æ÷PWMÊä³öÍ¨µÀ */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* Õ¼¿Õ±È */
    sConfig.Pulse = pulse;
    if(TIMx == TIM3)
        {
            if (HAL_TIM_PWM_ConfigChannel(&g_TimHandle, &sConfig, TimChannel[_ucChannel]) != HAL_OK)
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
        }
    else
        {
            if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TimChannel[_ucChannel]) != HAL_OK)
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
        }

    
    /* Æô¶¯PWMÊä³ö */
    if(TIMx == TIM3)
        {
            if (HAL_TIM_PWM_Start_DMA(&g_TimHandle,TimChannel[_ucChannel],(uint32_t*)pwm_led4,sizeof(pwm_led4)/sizeof(pwm_led4[0])) != HAL_OK)//ÒÔDMAÄ£Ê½¿ªÆôPWMÉú³É
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
        }
    else
        {
            if (HAL_TIM_PWM_Start(&TimHandle, TimChannel[_ucChannel]) != HAL_OK)
                {
                    printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
                }
        }
}


/*
* 
*    º¯ Êý Ãû: bsp_SetTIMforInt
*    ¹¦ÄÜËµÃ÷: ÅäÖÃTIMºÍNVIC£¬ÓÃÓÚ¼òµ¥µÄ¶¨Ê±ÖÐ¶Ï£¬¿ªÆô¶¨Ê±ÖÐ¶Ï¡£ÁíÍâ×¢ÒâÖÐ¶Ï·þÎñ³ÌÐòÐèÒªÓÉÓÃ»§Ó¦ÓÃ³ÌÐòÊµÏÖ¡£
*    ÐÎ    ²Î: TIMx : ¶¨Ê±Æ÷
*              _ulFreq : ¶¨Ê±ÆµÂÊ £¨Hz£©¡£ 0 ±íÊ¾¹Ø±Õ¡£
*              _PreemptionPriority : ÇÀÕ¼ÓÅÏÈ¼¶
*              _SubPriority : ×ÓÓÅÏÈ¼¶
*    ·µ »Ø Öµ: ÎÞ
* 
*/
/*    
TIM¶¨Ê±ÖÐ¶Ï·þÎñ³ÌÐò·¶Àý£¬±ØÐëÇåÖÐ¶Ï±êÖ¾
void TIM6_DAC_IRQHandler(void)
{
    if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM6->SR = ~ TIM_FLAG_UPDATE;
        //Ìí¼ÓÓÃ»§´úÂë
    }
}
*/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
    TIM_HandleTypeDef   TimHandle = {0};
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;
    
    /* Ê¹ÄÜTIMÊ±ÖÓ */
    bsp_RCC_TIM_Enable(TIMx);
    
    /*-----------------------------------------------------------------------
        system_stm32f4xx.c ÎÄ¼þÖÐ void SetSysClock(void) º¯Êý¶ÔÊ±ÖÓµÄÅäÖÃÈçÏÂ£º

        HCLK = SYSCLK / 1     (AHB1Periph)
        PCLK2 = HCLK / 2      (APB2Periph)
        PCLK1 = HCLK / 4      (APB1Periph)

        ÒòÎªAPB1 prescaler != 1, ËùÒÔ APB1ÉÏµÄTIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
        ÒòÎªAPB2 prescaler != 1, ËùÒÔ APB2ÉÏµÄTIMxCLK = PCLK2 x 2 = SystemCoreClock;

        APB1 ¶¨Ê±Æ÷ÓÐ TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
        APB2 ¶¨Ê±Æ÷ÓÐ TIM1, TIM8 ,TIM9, TIM10, TIM11

    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
    {
        /* APB2 ¶¨Ê±Æ÷Ê±ÖÓ = 168M */
        uiTIMxCLK = SystemCoreClock;
    }
    else    
    {
        /* APB1 ¶¨Ê±Æ÷ = 84M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;                    /* ·ÖÆµ±È = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;        /* ×Ô¶¯ÖØ×°µÄÖµ */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;                    /* ·ÖÆµ±È = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;        /* ×Ô¶¯ÖØ×°µÄÖµ */
    }
    else    /* ´óÓÚ4KµÄÆµÂÊ£¬ÎÞÐè·ÖÆµ */
    {
        usPrescaler = 0;                    /* ·ÖÆµ±È = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1;    /* ×Ô¶¯ÖØ×°µÄÖµ */
    }

    /* 
       ¶¨Ê±Æ÷ÖÐ¶Ï¸üÐÂÖÜÆÚ = TIMxCLK / usPrescaler + 1£©/usPeriod + 1£©
    */
    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler         = usPrescaler;
    TimHandle.Init.Period            = usPeriod;    
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }

    /* Ê¹ÄÜ¶¨Ê±Æ÷ÖÐ¶Ï  */
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
    

    /* ÅäÖÃTIM¶¨Ê±¸üÐÂÖÐ¶Ï (Update) */
    {
        uint8_t irq = 0;    /* ÖÐ¶ÏºÅ, ¶¨ÒåÔÚ stm32h7xx.h */

        if (TIMx == TIM1) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM2) irq = TIM2_IRQn;
        else if (TIMx == TIM3) irq = TIM3_IRQn;
        else if (TIMx == TIM4) irq = TIM4_IRQn;
        else if (TIMx == TIM5) irq = TIM5_IRQn;
        else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
        else if (TIMx == TIM7) irq = TIM7_IRQn;
        else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM9) irq = TIM1_BRK_TIM9_IRQn;
        else if (TIMx == TIM10) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM11) irq =  TIM1_TRG_COM_TIM11_IRQn;
        else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
        else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
        else
        {
            printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
        }    
        HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);        
    }
    
    HAL_TIM_Base_Start(&TimHandle);
}

/***************************** °²¸»À³µç×Ó www.armfly.com (END OF FILE) *********************************/
