#include "ticktim.h"
/* 
    å…¨å±€è¿è¡Œæ—¶é—´ å•ä½1ms
    æœ€é•¿å¯ä»¥è¡¨ç¤º 24.85 å¤© å¦‚æœä½ çš„äº§å“æŒç»­è¿è¡Œæ—¶é—´è¶…è¿‡è¿™ä¸ªæ•°å€¼
    é‚£ä¹ˆå¿…é¡»è€ƒè™‘æº¢å‡ºé—®é¢˜
*/
__IO int32_t g_iRunTime = 0;

/* 
    åŸºäºè½¯ä»¶å®šæ—¶å™¨çš„ç»“æ„ä½“å˜é‡
*/
static SOFT_TIM s_tTmr[TIM_TASK_COUNT] = {0};

/* ç­‰å¾…å˜é‡åˆå§‹åŒ– */
static __IO uint8_t g_ucEnableSystickISR = 0;

/* è½¯ä»¶å®šæ—¶å™¨åˆå§‹åŒ– */
static void bsp_SoftTimerDec(SOFT_TIM *_tim);

/* systick isr func */
void SysTick_ISR(void);

/**
*Function name  :bsp_InitTimer
*Description    :é…ç½® systick ä¸­æ–­ å¹¶åˆå§‹åŒ–è½¯ä»¶å®šæ—¶å™¨å˜é‡
*Input          :none
*Output         :none
*Retrurn        :none
*Date           :2024å¹´8æœˆ31æ—¥ 18:03:54
*/
void bsp_InitTimer(void)
{
    uint8_t i;
    /* æ¸…0æ‰€æœ‰çš„è½¯ä»¶å®šæ—¶å™¨ */
    for(i = 0;i < TIM_TASK_COUNT;i++)
    {    
        s_tTmr[i].State     = 0;        
        s_tTmr[i].Count     = 0;
        s_tTmr[i].PreLoad   = 0;
        s_tTmr[i].Flag      = 0;
        s_tTmr[i].Mode      = TIM_ONCE_MODE;    /* é»˜è®¤æ˜¯å•æ¬¡å·¥ä½œæ¨¡å¼ */
        s_tTmr[i].callfunc  = 0;
    }
    /*
        é…ç½®systicä¸­æ–­å‘¨æœŸä¸º1msï¼Œå¹¶å¯åŠ¨systickä¸­æ–­ã€‚

        SystemCoreClock æ˜¯å›ºä»¶ä¸­å®šä¹‰çš„ç³»ç»Ÿå†…æ ¸æ—¶é’Ÿï¼Œå¯¹äºSTM32H7,ä¸€èˆ¬ä¸º 400MHz

        SysTick_Config() å‡½æ•°çš„å½¢å‚è¡¨ç¤ºå†…æ ¸æ—¶é’Ÿå¤šå°‘ä¸ªå‘¨æœŸåè§¦å‘ä¸€æ¬¡Systickå®šæ—¶ä¸­æ–­.
            -- SystemCoreClock / 1000  è¡¨ç¤ºå®šæ—¶é¢‘ç‡ä¸º 1000Hzï¼Œ ä¹Ÿå°±æ˜¯å®šæ—¶å‘¨æœŸä¸º  1ms
            -- SystemCoreClock / 500   è¡¨ç¤ºå®šæ—¶é¢‘ç‡ä¸º 500Hzï¼Œ  ä¹Ÿå°±æ˜¯å®šæ—¶å‘¨æœŸä¸º  2ms
            -- SystemCoreClock / 2000  è¡¨ç¤ºå®šæ—¶é¢‘ç‡ä¸º 2000Hzï¼Œ ä¹Ÿå°±æ˜¯å®šæ—¶å‘¨æœŸä¸º  500us
        å¯¹äºå¸¸è§„çš„åº”ç”¨ æˆ‘ä»¬ä¸€èˆ¬å–å®šæ—¶å‘¨æœŸ1ms å¯¹äºä½é€ŸCPUæˆ–è€…ä½åŠŸè€—åº”ç”¨ å¯ä»¥è®¾ç½®å®šæ—¶å‘¨æœŸä¸º 10ms
    */
#if USE_THREADX     == 0
    SysTick_Config(SystemCoreClock / 1000);
#endif
    g_ucEnableSystickISR = 1;    /* è¡¨ç¤ºæ‰§è¡Œ systick ä¸­æ–­ */
    
}
/************************************************
*Function name    :bsp_StartOnceTimer
*Description    :´´½¨Ò»¸öÒ»´ÎĞÔµÄ¶¨Ê±Æ÷
*Input            :_id:¶¨Ê±Æ÷È    Îñid	_period:¶¨Ê±Æ÷ÈÎÎñÖÜÆÚ,µ¥Î»:ms
                :callfunc:¹¦ÄÜº¯Êı
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ19µã39·Ö
*************************************************/
void bsp_StartOnceTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
    if(_id >= TIM_TASK_COUNT){
        /*´òÓ¡³ö´íµÄÔ´´úÂëÎÄ¼şÃû£¬º¯ÊıÃû³Æ*/
        printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
        while(1);    /*²ÎÊıÒì³££¬ËÀ»úµÈ´ı¿´ÃÅ¹·¸´Î»*/
    }
    __set_PRIMASK(1);    /*½ûÖ¹È«¾ÖÖĞ¶Ï*/
    s_tTmr[_id].State = 0;
    s_tTmr[_id].Count = _period;        /*ÊµÊ±¼ÆÊıÆ÷³õÖµ*/
    s_tTmr[_id].PreLoad = _period;        /*¼ÆÊıÆ÷×Ô¶¯ÖØ×°ÔØÖµ£¬µ¥´ÎÄ£Ê½²»Æğ×÷ÓÃ*/
    s_tTmr[_id].Flag = 0;                /*¶¨Ê±¼äµ½±êÖ¾*/
    s_tTmr[_id].Mode = TIM_ONCE_MODE;    /*Ò»´ÎĞÔ¹¤×÷Ä£Ê½*/
    s_tTmr[_id].callfunc = callfunc;    /*µ÷ÓÃ»Øµ÷º¯Êı*/
    __set_PRIMASK(0);    /*¿ªÆôÈ«¾ÖÖĞ¶Ï*/
}
/************************************************
*Function name    :bsp_StartAutoTimer
*Description    :´´½¨Ò»¸ö²»Í£¶¨Ê±µÄ¶¨Ê±Æ÷
*Input            :_id:¶¨Ê±Æ÷È    Îñid	_period:¶¨Ê±Æ÷ÈÎÎñÖÜÆÚ,µ¥Î»:ms
                :callfunc:¹¦ÄÜº¯Êı
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ19µã52·Ö
*************************************************/
void bsp_StartAutoTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
    if(_id >= TIM_TASK_COUNT){
        /*´òÓ¡³ö´íµÄÔ´´úÂëÎÄ¼şÃû£¬º¯ÊıÃû³Æ*/
        printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
        while(1);/*²ÎÊıÒì³££¬ËÀ»úµÈ´ı¿´ÃÅ¹·¸´Î»*/
    }
    __set_PRIMASK(1);    /*½ûÖ¹È«¾ÖÖĞ¶Ï*/
    s_tTmr[_id].State = 1;
    s_tTmr[_id].Count = _period;        /*ÊµÊ±¼ÆÊıÆ÷³õÖµ*/
    s_tTmr[_id].PreLoad = _period;        /*¼ÆÊıÆ÷×Ô¶¯ÖØ×°ÔØÖµ£¬½ö×Ô¶¯Ä£Ê½Æğ×÷ÓÃ*/
    s_tTmr[_id].Flag = 0;                /*¶¨Ê±Ê±¼äµ½±êÖ¾*/
    s_tTmr[_id].Mode = TIM_MULTI_MODE;    /*×Ô¶¯ÖØ¸´µ÷ÓÃÄ£Ê½*/
    s_tTmr[_id].callfunc = callfunc;    /*µ÷ÓÃ»Øµ÷º¯Êı*/
    __set_PRIMASK(0);    /*¿ªÖĞ¶Ï*/
        
}
/************************************************
*Function name    :bsp_StopTimer
*Description    :Í£Ö¹Ò»¸ö¶¨Ê±Æ÷£¬Èç¹ûĞèÒªÖØĞÂµ÷ÓÃ£¬±ØĞëÔÙ´Î´´½¨
*Input            :none
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã18·Ö
*************************************************/
void bsp_StopTimer(uint8_t _id)
{
    s_tTmr[_id].callfunc = 0;
}
/************************************************
*Function name    :bsp_SoftTimerDec
*Description    :Ã¿¸ô1ms¶ÔËùÓĞ¶¨Ê±Æ÷±äÁ¿¼õ1¡£±ØĞë±»SysTick_ISRÖÜÆÚĞÔµ÷ÓÃ¡£
*Input            :_tim:¶¨Ê±Æ÷ÈÎÎñµÄidÊı×é
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã07·Ö
*************************************************/
static void bsp_SoftTimerDec(SOFT_TIM *_tim)
{
    if(_tim->Count > 0){
        /*Èç¹û¶¨Ê±Æ÷±äÁ¿¼õµ½1ÔòÉèÖÃ¶¨Ê±Æ÷µ½´ï±êÖ¾*/
        if(--_tim->Count == 0){
            _tim->Flag = 1;
            /*Èç¹ûÊÇ×Ô¶¯Ä£Ê½£¬Ôò×Ô¶¯ÖØ×°ÔØÖµ*/
            if(_tim->Mode == TIM_MULTI_MODE){
                _tim->Count = _tim->PreLoad;
            }
        }
    }
}
/************************************************
*Function name    :SysTick_ISR
*Description    :SysTickÖĞ¶Ï·şÎñ³ÌĞò£¬Ã¿¸ô1ms½øÈë1´Î
*Input            :none
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã08·Ö
*************************************************/
void SysTick_ISR(void)
{
    static uint8_t s_count10ms = 0,s_count50ms = 0;
    uint8_t i;
    
    /*Ã¿¸ô1ms£¬¶ÔËùÓĞµÄÈí¼ş¶¨Ê±Æ÷½øĞĞ¼õĞ¡²Ù×÷*/
    for(i = 0;i < TIM_TASK_COUNT;i++){
        bsp_SoftTimerDec(&s_tTmr[i]);    /*¶¨Ê±Æ÷ÈÎÎñ¼õĞ¡1*/
        if((s_tTmr[i].Flag == 1) && (s_tTmr[i].callfunc)){
            s_tTmr[i].callfunc();        //µ÷ÓÃ¹¦ÄÜº¯Êı
            s_tTmr[i].Flag = 0;
            if(s_tTmr[i].State == 0){    //Èç¹ûÊÇÒ»´ÎĞÔº¯Êı
                s_tTmr[i].callfunc = 0;    //Çå¿Õ»Øµ÷º¯Êı²ÎÊı
            }
        }
    }    
    /*È«¾ÖÔËĞĞÊ±¼äÃ¿1ms¼Ó1*/
    g_iRunTime++;
    if(g_iRunTime == 0x7FFFFFFF){    /*uint32_tÀàĞÍµÄ×î´óÖµ*/
        g_iRunTime = 0;
    }
    bsp_RunPer1ms();        /*Ã¿¸ô1msµ÷ÓÃÒ»´Î´Ëº¯Êı*/
    if(++s_count10ms >= 10){
        s_count10ms = 0;
        bsp_RunPer10ms();    /*Ã¿¸ô10msµ÷ÓÃÒ»´Î´Ëº¯Êı*/
    }
    if(++s_count50ms >= 50){
        s_count50ms = 0;
        bsp_RunPer50ms();    /*Ã¿¸ô50msµ÷ÓÃÒ»´Î´Ëº¯Êı*/
    }
}

/************************************************
*Function name    :bsp_RunPer1ms
*Description    :1msÖÜÆÚµ÷ÓÃº¯Êı£¬user¿ÉÒÔÖØĞ´
*Input            :none
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã09·Ö
*************************************************/
__weak void bsp_RunPer1ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */     
}
/************************************************
*Function name    :bsp_RunPer1ms
*Description    :10msÖÜÆÚµ÷ÓÃº¯Êı£¬user¿ÉÒÔÖØĞ´
*Input            :none
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã09·Ö
*************************************************/
__weak void bsp_RunPer10ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */     
}
/************************************************
*Function name    :bsp_RunPer1ms
*Description    :50msÖÜÆÚµ÷ÓÃº¯Êı£¬user¿ÉÒÔÖØĞ´
*Input            :none
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã22·Ö
*************************************************/
__weak void bsp_RunPer50ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */     
}

#if USE_THREADX == 0
/************************************************
*Function name    :bsp_DelayUs
*Description    :us¼¶ÑÓ³Ù¡£ ±ØĞëÔÚsystick¶¨Ê±Æ÷Æô¶¯ºó²ÅÄÜµ÷ÓÃ´Ëº¯Êı¡£
*Input            :n:ÑÓÊ±¶àÉÙÎ¢Ãë
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã26·Ö
*************************************************/
void bsp_DelayUs(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
    reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);     /* ĞèÒªµÄ½ÚÅÄÊı */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* ¸Õ½øÈëÊ±µÄ¼ÆÊıÆ÷Öµ */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICKÊÇÒ»¸öµİ¼õµÄ¼ÆÊıÆ÷ */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* ÖØĞÂ×°ÔØµİ¼õ */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* Ê±¼ä³¬¹ı/µÈÓÚÒªÑÓ³ÙµÄÊ±¼ä,ÔòÍË³ö */
            if (tcnt >= ticks)
            {
                break;
            }
        }  
    }    
}
/************************************************
*Function name    :bsp_DelayUs
*Description    :ms¼¶ÑÓ³Ù¡£ ±ØĞëÔÚsystick¶¨Ê±Æ÷Æô¶¯ºó²ÅÄÜµ÷ÓÃ´Ëº¯Êı¡£
*Input            :n:ÑÓÊ±¶àÉÙºÁÃë
                :
*Output            :none
*Retrurn        :none
*Author            :trx
*Date            :2022Äê4ÔÂ5ÈÕ22µã26·Ö
*************************************************/
void bsp_DelayMs(uint32_t n)
{
    uint32_t i;
    for(i = 0;i < n;i++){
        bsp_DelayUs(1000);
    }
}

/**
  * @brief  This function handles SysTick Handler.µ×²ãÖĞ¶Ïº¯Êı
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
    if(g_ucEnableSystickISR == 0){    /*´ú±íÃ»ÓĞ½øĞĞ¶à¶¨Ê±Æ÷ÈÎÎñ³õÊ¼»¯*/
        return;
    }
    SysTick_ISR();                    /*ĞèÒªÔÚÖĞ¶ÏÖĞ²»Í£µÄµ÷    ÃµÄÖĞ¶Ï·şÎñ³ÌĞò*/	
}

#endif

int32_t bsp_GetRunTime(void)
{
    int32_t runtime;

    __set_PRIMASK(1);    /* å…³é—­å…¨å±€ä¸­æ–­ */
    runtime = g_iRunTime;
    __set_PRIMASK(0);    /* æ‰“å¼€å…¨å±€ä¸­æ–­ */
    return runtime;
}


