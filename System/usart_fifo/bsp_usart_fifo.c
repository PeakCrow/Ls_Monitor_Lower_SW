#include "bsp_usart_fifo.h"



/* ´®¿Ú1µÄGPIO PA9 PA10 ½Ó¿Ú */
#define USART1_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT                GPIOA
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_AF                    GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT                 GPIOA
#define USART1_RX_PIN                     GPIO_PIN_10
#define USART1_RX_AF                     GPIO_AF7_USART1
/* ´®¿Ú2µÄGPIO --- PA2 PA3  GPS (Ö»ÓÃRX??TX±»ÒÔÌ«ÍøÕ¼ÓÃ??*/
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()

#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_AF                     GPIO_AF7_USART2

#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_AF                     GPIO_AF7_USART2

/* ´®¿Ú3µÄGPIO --- PB10 PB11  RS485 */
#define USART3_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()

#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_PORT              GPIOB
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_TX_AF                     GPIO_AF7_USART3

#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_RX_GPIO_PORT              GPIOB
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_RX_AF                     GPIO_AF7_USART3
/* ¶¨ÒåÃ¿¸ö´®¿Ú½á¹¹Ìå±ä??*/
#if UART1_FIFO_EN == 1
    static UART_T g_tUart1;
    static uint8_t g_TxBuff1[UART1_TX_BUF_SIZE];    /* ·¢ËÍ»º³åÇø */
    static uint8_t g_RxBuff1[UART1_RX_BUF_SIZE];    /* ½ÓÊÕ»º³å??*/
#endif

#if UART2_FIFO_EN == 1
    static UART_T g_tUart2;
    static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];        /* ·¢ËÍ»º³åÇø */
    static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];        /* ½ÓÊÕ»º³å??*/
#endif

#if UART3_FIFO_EN == 1
    static UART_T g_tUart3;
    static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];        /* ·¢ËÍ»º³åÇø */
    static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];        /* ½ÓÊÕ»º³å??*/
#endif

static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart,uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);

/*
*    ?????? bsp_InitUart
*    ¹¦ÄÜËµÃ÷: Ö÷º¯ÊýÖÐµ÷ÓÃµÄ´®¿Ú³õÊ¼»¯º¯Êý
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????5??1??7??
*/
void bsp_InitUart()
{
    UartVarInit();
    InitHardUart();
}

/*
*    ?????? UartVarInit
*    ¹¦ÄÜËµÃ÷: ´®¿ÚÏà¹ØµÄ±äÁ¿³õÊ¼»¯
*    ??   ?? ??
*    ?????? ??
*    Ê±¼ä??022????4??2??9??
*/
static void UartVarInit(void)
{
#if    UART1_FIFO_EN == 1
    g_tUart1.uart = USART1;                        /* STM32´®¿ÚÉè±¸ */
    g_tUart1.pTxBuf = g_TxBuff1;                /* ´®¿Ú·¢ËÍ»º³åÇøÖ¸Õë */
    g_tUart1.pRxBuf = g_RxBuff1;                /* ´®¿Ú½ÓÊÕ»º³åÇøÖ¸??*/
    g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;    /* ·¢ËÍ»º³åÇø´óÐ¡ */
    g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;    /* ½ÓÊÕ»º³åÇø´ó??*/
    g_tUart1.usTxWrite = 0;                        /* ·¢ËÍFIFOÐ´Ë÷??*/
    g_tUart1.usTxRead = 0;                        /* ·¢ËÍFIFO¶ÁË÷??*/
    g_tUart1.usRxWrite = 0;                        /* ½ÓÊÕFIFOÐ´Ë÷??*/
    g_tUart1.usRxRead = 0;                        /* ½ÓÊÕFIFO¶ÁË÷??*/
    g_tUart1.usRxCount = 0;                        /* ½ÓÊÕµ½µÄÐÂÊý¾Ý¸ö??*/
    g_tUart1.usTxCount = 0;                        /* ´ý·¢ËÍµÄÊý¾Ý¸öÊý */
    g_tUart1.SendBefor = 0;                        /* ·¢ËÍÊý¾ÝÇ°µÄ»Øµ÷º¯??*/
    g_tUart1.SendOver = 0;                        /* ·¢ËÍÍê±ÏºóµÄ»Øµ÷º¯??*/
    g_tUart1.ReciveNew = 0;                        /* ½ÓÊÕµ½ÐÂÊý¾ÝºóµÄ»Øµ÷º¯Êý */
    g_tUart1.Sending  = 0;                        /* ÕýÔÚ·¢ËÍÖÐ±êÖ¾ */
#endif

#if UART2_FIFO_EN == 1
    g_tUart2.uart = USART2;                        /* STM32 ´®¿ÚÉè±¸ */
    g_tUart2.pTxBuf = g_TxBuf2;                    /* ·¢ËÍ»º³åÇøÖ¸Õë */
    g_tUart2.pRxBuf = g_RxBuf2;                    /* ½ÓÊÕ»º³åÇøÖ¸??*/
    g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;    /* ·¢ËÍ»º³åÇø´óÐ¡ */
    g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;    /* ½ÓÊÕ»º³åÇø´ó??*/
    g_tUart2.usTxWrite = 0;                        /* ·¢ËÍFIFOÐ´Ë÷??*/
    g_tUart2.usTxRead = 0;                        /* ·¢ËÍFIFO¶ÁË÷??*/
    g_tUart2.usRxWrite = 0;                        /* ½ÓÊÕFIFOÐ´Ë÷??*/
    g_tUart2.usRxRead = 0;                        /* ½ÓÊÕFIFO¶ÁË÷??*/
    g_tUart2.usRxCount = 0;                        /* ½ÓÊÕµ½µÄÐÂÊý¾Ý¸ö??*/
    g_tUart2.usTxCount = 0;                        /* ´ý·¢ËÍµÄÊý¾Ý¸öÊý */
    g_tUart2.SendBefor = 0;                        /* ·¢ËÍÊý¾ÝÇ°µÄ»Øµ÷º¯??*/
    g_tUart2.SendOver = 0;                        /* ·¢ËÍÍê±ÏºóµÄ»Øµ÷º¯??*/
    g_tUart2.ReciveNew = 0;                        /* ½ÓÊÕµ½ÐÂÊý¾ÝºóµÄ»Øµ÷º¯Êý */
    g_tUart2.Sending = 0;                        /* ÕýÔÚ·¢ËÍÖÐ±êÖ¾ */
#endif

#if UART3_FIFO_EN == 1
    g_tUart3.uart = USART3;                        /* STM32 ´®¿ÚÉè±¸ */
    g_tUart3.pTxBuf = g_TxBuf3;                    /* ·¢ËÍ»º³åÇøÖ¸Õë */
    g_tUart3.pRxBuf = g_RxBuf3;                    /* ½ÓÊÕ»º³åÇøÖ¸??*/
    g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;    /* ·¢ËÍ»º³åÇø´óÐ¡ */
    g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;    /* ½ÓÊÕ»º³åÇø´ó??*/
    g_tUart3.usTxWrite = 0;                        /* ·¢ËÍFIFOÐ´Ë÷??*/
    g_tUart3.usTxRead = 0;                        /* ·¢ËÍFIFO¶ÁË÷??*/
    g_tUart3.usRxWrite = 0;                        /* ½ÓÊÕFIFOÐ´Ë÷??*/
    g_tUart3.usRxRead = 0;                        /* ½ÓÊÕFIFO¶ÁË÷??*/
    g_tUart3.usRxCount = 0;                        /* ½ÓÊÕµ½µÄÐÂÊý¾Ý¸ö??*/
    g_tUart3.usTxCount = 0;                        /* ´ý·¢ËÍµÄÊý¾Ý¸öÊý */
    g_tUart3.SendBefor = 0;                        /* ·¢ËÍÊý¾ÝÇ°µÄ»Øµ÷º¯??*/
    g_tUart3.SendOver = 0;                        /* ·¢ËÍÍê±ÏºóµÄ»Øµ÷º¯??*/
    g_tUart3.ReciveNew = 0;                        /* ½ÓÊÕµ½ÐÂÊý¾ÝºóµÄ»Øµ÷º¯Êý */
    g_tUart3.Sending = 0;                        /* ÕýÔÚ·¢ËÍÖÐ±êÖ¾ */
#endif
}

/*
*    ?????? InitHardUart
*    ¹¦ÄÜËµÃ÷: ´®¿ÚÍâÉèµÄ³õÊ¼»¯
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????5??1??8??
*/
void InitHardUart(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
#if UART1_FIFO_EN == 1    /* ´®¿Ú1 */
    /* Ê¹ÄÜGPIO TX/RXÊ±ÖÓ */
    USART1_TX_GPIO_CLK_ENABLE();
    USART1_RX_GPIO_CLK_ENABLE();
    
    /* Ê¹ÄÜUSARTxÊ±ÖÓ */
    USART1_CLK_ENABLE();
    
    /* ÅäÖÃTXÒý½Å */
    GPIO_InitStruct.Pin            = USART1_TX_PIN;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed        = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate    = USART1_TX_AF;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT,&GPIO_InitStruct);
    
    /* ÅäÖÃRXÒý½Å */
    GPIO_InitStruct.Pin         = USART1_RX_PIN;
    GPIO_InitStruct.Alternate     = USART1_RX_AF;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT,&GPIO_InitStruct);
    
    /* ÅäÖÃNVIC the NVIC for UART */
    HAL_NVIC_SetPriority(USART1_IRQn,0,1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    /* ÅäÖÃ²¨ÌØÂÊ¡¢ÆæÅ¼Ð£ÑéÎ» */
    bsp_SetUartParam(USART1,UART1_BAUD,UART_PARITY_NONE,UART_MODE_TX_RX);
    
    /* Çå³ýTC·¢ËÍÍê³É±ê??*/
    CLEAR_BIT(USART1->SR,USART_SR_TC);
    /* Çå³ýRXEN½ÓÊÕ±êÖ¾ */
    CLEAR_BIT(USART1->SR,USART_SR_RXNE);
    /* Ê¹ÄÜPE£¬RX½ÓÊÕÖÐ¶Ï */
    SET_BIT(USART1->CR1,USART_CR1_RXNEIE);
#endif

#if UART2_FIFO_EN == 1        /* ´®¿Ú2 */
    /* Ê¹ÄÜ GPIO TX/RX Ê±ÖÓ */
    USART2_TX_GPIO_CLK_ENABLE();
    USART2_RX_GPIO_CLK_ENABLE();
    
    /* Ê¹ÄÜ USARTx Ê±ÖÓ */
    USART2_CLK_ENABLE();    

    /* ÅäÖÃTXÒý½Å */
    GPIO_InitStruct.Pin       = USART2_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART2_TX_AF;
    HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);    
    
    /* ÅäÖÃRXÒý½Å */
    GPIO_InitStruct.Pin = USART2_RX_PIN;
    GPIO_InitStruct.Alternate = USART2_RX_AF;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

    /* ÅäÖÃNVIC the NVIC for UART */   
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  
    /* ÅäÖÃ²¨ÌØÂÊ¡¢ÆæÅ¼Ð£??*/
    bsp_SetUartParam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);    // UART_MODE_TX_RX

    CLEAR_BIT(USART2->SR, USART_SR_TC);   /* Çå³ýTC·¢ËÍÍê³É±ê??*/
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* Çå³ýRXNE½ÓÊÕ±êÖ¾ */
    SET_BIT(USART2->CR1, USART_CR1_RXNEIE);    /* Ê¹ÄÜPE. RX½ÓÊÜÖÐ¶Ï */
#endif

#if UART3_FIFO_EN == 1            /* ´®¿Ú3 */
    /* Ê¹ÄÜ GPIO TX/RX Ê±ÖÓ */
    USART3_TX_GPIO_CLK_ENABLE();
    USART3_RX_GPIO_CLK_ENABLE();
    
    /* Ê¹ÄÜ USARTx Ê±ÖÓ */
    USART3_CLK_ENABLE();    

    /* ÅäÖÃTXÒý½Å */
    GPIO_InitStruct.Pin       = USART3_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART3_TX_AF;
    HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);    
    
    /* ÅäÖÃRXÒý½Å */
    GPIO_InitStruct.Pin = USART3_RX_PIN;
    GPIO_InitStruct.Alternate = USART3_RX_AF;
    HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

    /* ÅäÖÃNVIC the NVIC for UART */   
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  
    /* ÅäÖÃ²¨ÌØÂÊ¡¢ÆæÅ¼Ð£??*/
    bsp_SetUartParam(USART3,  UART3_BAUD, UART_PARITY_EVEN, UART_MODE_TX_RX);

    CLEAR_BIT(USART3->SR, USART_SR_TC);   /* Çå³ýTC·¢ËÍÍê³É±ê??*/
    CLEAR_BIT(USART3->SR, USART_SR_RXNE); /* Çå³ýRXNE½ÓÊÕ±êÖ¾ */
    SET_BIT(USART3->CR1, USART_CR1_RXNEIE);    /* Ê¹ÄÜPE. RX½ÓÊÜÖÐ¶Ï */
#endif
}
/*
*********************************************************************************************************
*    ?????? ComToUart
*    ¹¦ÄÜËµÃ÷: ½«COM¶Ë¿ÚºÅ×ª»»ÎªUARTÖ¸Õë
*    ??   ?? _ucPort: ¶Ë¿Ú??COM1 - COM8)
*    ?????? uartÖ¸Õë
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1)
    {
        #if UART1_FIFO_EN == 1
            return &g_tUart1;
        #else
            return 0;
        #endif
    }
    else if (_ucPort == COM2)
    {
        #if UART2_FIFO_EN == 1
            return &g_tUart2;
        #else
            return 0;
        #endif
    }
    else if (_ucPort == COM3)
    {
        #if UART3_FIFO_EN == 1
            return &g_tUart3;
        #else
            return 0;
        #endif
    }
    else
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
        return 0;
    }
}

/*
*    ?????? bsp_SetUartParam
*    ¹¦ÄÜËµÃ÷: ÅäÖÃ´®¿ÚµÄÓ²¼þ²Î??
*    ??   ?? Instance    USART_TypeDefÀàÐÍ½á¹¹??
                BaudRate    ²¨ÌØ??
                Partity        Ð£ÑéÀàÐÍ£¬ÆæÐ£Ñé»òÕßÅ¼Ð£Ñé
                Mode        ·¢ËÍºÍ½ÓÊÕÄ£Ê½Ê¹ÄÜ
*    ?????? ??
*    Ê±¼ä??022????4??3??3??
*/
void bsp_SetUartParam(USART_TypeDef *Instance,uint32_t BaudRate,uint32_t Parity,uint32_t Mode)
{
    UART_HandleTypeDef UartHandle;
    
/*    ´®¿Ú1Ó²¼þÅäÖÃ²ÎÊý
    Òì²½´®¿ÚÄ£Ê½
    -×Ö    ¤	= 8??
    -Í£Ö¹??= 1¸öÍ£Ö¹Î»
    -Ð£    é	= ²ÎÊýParity
    -²¨ÌØ??= ²ÎÊýBaudRate
    -Ó²¼þÁ÷¿ØÖÆ¹Ø??RTS and CTS signals) */
    UartHandle.Instance            = Instance;
    UartHandle.Init.BaudRate    = BaudRate;
    UartHandle.Init.WordLength    = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits    = UART_STOPBITS_1;
    UartHandle.Init.Parity        = Parity;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = Mode;
    UartHandle.Init.OverSampling= UART_OVERSAMPLING_16;
    
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__); 
    }
    
}

/*
*    ?????? UartIRQ
*    ¹¦ÄÜËµÃ÷: ´®¿Ú·¢ËÍ½ÓÊÕÖÐ¶ÏµÄ´¦Àíº¯Êý
*    ??   ?? _pUart:´®¿ÚÍâÉè×Ô¶¨Òå½á¹¹ÌåµÄÈ«¾Ö±äÁ¿
*    ?????? none
*    Ê±¼ä??022????5??1??0??
*/
static void UartIRQ(UART_T *_pUart)
{
    
    /* »ñÈ¡ÏàÓ¦¼Ä´æÆ÷µÄÊý¾Ý */
    uint32_t isrflags    = READ_REG(_pUart->uart->SR);
    uint32_t cr1its        = READ_REG(_pUart->uart->CR1);
    uint32_t cr3its        = READ_REG(_pUart->uart->CR3);
    
    /* ´¦Àí½ÓÊÕÖÐ¶Ï */
    if((isrflags & USART_SR_RXNE) != RESET)/* ½øÐÐ Î»Óë ÅÐ¶ÏÊÇ·ñ×¼±¸ºÃ¶ÁÈ¡½ÓÊÕµ½µÄÊý??*/
    {
        /* ´Ó´®¿Ú½ÓÊÕÊý¾Ý¼Ä´æÆ÷¶ÁÈ¡Êý¾Ý´æ·Åµ½½ÓÊÕFIFO */
        uint8_t ch;
        /* Êý¾Ý¼Ä´æÆ÷DRÖ»ÓÐ??Î»ÓÐ??*/
        ch = READ_REG(_pUart->uart->DR);
        _pUart->pRxBuf[_pUart->usRxWrite] = ch;
        /* ½ÓÊÕÊý¾Ý¶ÁÖ¸Õë´óÓÚbuf£¬Ôò??£¬·ñÔò¼ÆÊý×Ô??*/
        if(++_pUart->usRxWrite >= _pUart->usRxBufSize)
        {
            _pUart->usRxWrite = 0;
        }
        /* Ã¿½ÓÊÕÒ»´ÎÊý¾Ý£¬FIFO?? */
        if(_pUart->usRxCount < _pUart->usRxBufSize)
        {
            _pUart->usRxCount++;
        }
        /* »Øµ÷º¯Êý£¬Í¨ÖªÓ¦ÓÃ³ÌÐò½ÓÊÕµ½ÐÂÊý¾Ý£¬Ò»°ãÊÇ·¢ËÍÒ»¸öÏûÏ¢»òÕß±êÖ¾ÖÃ1 */
        if(_pUart->ReciveNew)
        {
            _pUart->ReciveNew(ch);
        }
    }
    
    /* ´¦Àí·¢ËÍ»º³åÇø¿ÕÖÐ??*/
    //Êý¾Ý´«Êäµ½                        ´æÆ÷						·¢ËÍÖÐ¶ÏÊ¹??
    if(((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
    {
        if(_pUart->usTxCount == 0)
        {
            /* ·¢ËÍ»º³åÇøµÄÊý¾ÝÒÑ¾­È¡ÍêÊ±£¬½ûÖ¹·¢ËÍ»º³åÇøÖÐ¶Ï(´ËÊ±×îºóÒ»¸öÊý¾Ý»¹Î´ÕæÕý·¢ËÍÍê?? */
            CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
            
            /* Ê¹ÄÜÊý¾Ý·¢ËÍÍê±ÏÖÐ??*/
            SET_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
        }else
        {
            /* ÕýÔÚ·¢ËÍ´®¿ÚÊý??*/
            _pUart->Sending = 1;
            
            /* ´Ó·¢ËÍFIFOÖÐÈ¡1¸ö×Ö½ÚÐ´Èë´®¿Ú·¢ËÍÊý¾Ý¼Ä´æÆ÷ */
            _pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
            /* ·¢ËÍ»º³åÇø¶ÁÖ¸Õë´óÓÚ»º³åÇø´óÐ¡ */
            if(++_pUart->usTxRead >= _pUart->usTxBufSize)
            {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }
    /* Êý¾ÝÎ»È«²¿·¢ËÍÍê±ÏµÄÖÐ¶Ï */
    if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
    {
        if(_pUart->usTxCount == 0)
        {
            /* Èç¹û·¢ËÍFIFOµÄÊý¾ÝÈ«²¿·¢ËÍÍê±Ï£¬½ûÖ¹Êý¾Ý·¢ËÍÍê±ÏÖÐ??*/
            CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
            /* »Øµ÷º¯Êý£¬Ò»°ãÓÃÀ´´¦ÀíRS485Í¨ÐÅ */
            if(_pUart->SendOver)
            {
                _pUart->SendOver();
            }
            _pUart->Sending = 0;
        }else
        {
            /* Õý³£Çé¿öÏÂ£¬²»»á½øÈë´Ë·Ö??*/
            /* Èç¹û·¢ËÍFIFOµÄÊý¾Ý»¹Î´Íê±Ï£¬Ôò´Ó·¢ËÍFIFO??¸öÊý¾ÝÐ´Èë·¢ËÍÊý¾Ý¼Ä´æÆ÷ */
            _pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
            if(++_pUart->usTxRead >= _pUart->usTxBufSize)
            {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }
}

/*
*    ?????? USART1_IRQHandler
*    ¹¦ÄÜËµÃ÷: HAL¿âµÄÖÐ¶Î´¦Àíº¯Êý
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????5??1??1??
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
    UartIRQ(&g_tUart1);
}
#endif
#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
    UartIRQ(&g_tUart2);
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
    UartIRQ(&g_tUart3);
}
#endif
/*
*    ?????? UartSend
*    ¹¦ÄÜËµÃ÷: Ê¹ÄÜ´®¿Ú·¢ËÍÖÐ??
*    ??   ?? _pUart£º´®¿ÚÍâÉè×Ô¶¨Òå½á¹¹ÌåÖ¸Õë£»_ucaBuf£ºÒª·¢ËÍµÄbuff£»_usLen£º·¢ËÍµÄbuff³¤¶È
*    ?????? none
*    Ê±¼ä??022????5??1??8??
*/
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen)
{
    uint16_t i;
    
    for(i = 0;i < _usLen;i++)
    {
        /* Èç¹û·¢ËÍ»º³åÇøÒÑ¾­ÂúÁË£¬ÔòµÈ´ý»º³åÇø¿Õ */
        while(1)
        {
            __IO uint16_t usCount;
            
            __set_PRIMASK(1);    /* ½ûÖ¹È«¾ÖÖÐ¶Ï */
            usCount = _pUart->usTxCount;
            __set_PRIMASK(0);    /* ´ò¿ªÈ«¾ÖÖÐ¶Ï */
            if(usCount < _pUart->usTxBufSize)
            {
                break;    
            }else if(usCount == _pUart->usTxBufSize)/* Êý¾ÝÒÑ¾­ÌîÂú»º³å??*/
            {
                /* ·¢ËÍÖÐ¶ÏÊ¹??*/
                if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
                {
                    SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
                }
            }
        }
        
        /* ½«Êý¾ÝÌîÈë·¢ËÍ»º³åÇø */
        _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];
        
        __set_PRIMASK(1);    /* ½ûÖ¹È«¾ÖÖÐ¶Ï */
        if(++_pUart->usTxWrite >= _pUart->usTxBufSize)
        {
            _pUart->usTxWrite = 0;
        }
        _pUart->usTxCount++;
        __set_PRIMASK(0);    /* ´ò¿ªÈ«¾ÖÖÐ¶Ï */
    }
    /* Ê¹ÄÜ·¢ËÍÖÐ??»º³åÇø¿Õ) */
    SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
}

/*
*    ?????? comSenBuf
*    ¹¦ÄÜËµÃ÷: ÅäÖÃ´®¿Ú·¢ËÍ×Ö·û´®
*    ??   ?? _ucPort£º×¼±¸·¢ËÍµÄ´®¿ÚÍâÉè£»_ucaBuf£ºÒª·¢ËÍµÄÊý¾Ýbuff£»_usLen£ºÒª·¢ËÍµÄbuff³¤¶È
*    ?????? none
*    Ê±¼ä??022????5??1??0??
*/
void comSendBuf(COM_PORT_E _ucPort,uint8_t *_ucaBuf,uint16_t _usLen)
{
    UART_T *pUart;
    
    pUart = ComToUart(_ucPort);
    if(pUart == 0)
    {
        return;
    }
    if(pUart->SendBefor != 0)
    {
        pUart->SendBefor();/* Èç¹ûÊÇRS485Í¨ÐÅ£¬¿ÉÒÔÔÚÕâ¸öº¯ÊýÖÐ½«RS485ÉèÖÃÎª·¢ËÍÄ£??*/
    }
    UartSend(pUart,_ucaBuf,_usLen);
}


/*
*    ?????? comSendChar
*    ¹¦ÄÜËµÃ÷: ´®¿Ú·¢ËÍÒ»¸ö×Ö??
*    ??   ?? _ucPort:´®¿ÚÍâÉèÃ¶¾Ù±äÁ¿£»_ucByte£º·¢ËÍµÄ×Ö½ÚÊý¾Ý
*    ?????? none
*    Ê±¼ä??022????5??1??6??
*/
void comSendChar(COM_PORT_E _ucPort,uint8_t _ucByte)
{
    comSendBuf(_ucPort,&_ucByte,1);
}

/*
*    ?????? UartGetChar
*    ¹¦ÄÜËµÃ÷: ´Ó´®¿ÚÍâÉèµÄ½ÓÊÕ»º³åÇøÖÐ¶ÁÈ¡Ò»¸ö×Ö??
*    ??   ?? _pUart£º´®¿ÚÍâÉè×Ô¶¨Òå½á¹¹Ìå£»_pByte£º´æ·Å¶ÁÈ¡Êý¾ÝµÄÖ¸Õë
*    ?????? 0£º±íÊ¾ÎÞÊý¾Ý¶ÁÈ¡??£º±íÊ¾¶ÁÈ¡µ½Êý¾Ý
*    Ê±¼ä??022????6??4??9??
*/
static uint8_t UartGetChar(UART_T *_pUart,uint8_t * _pByte)
{
    uint16_t usCount;
    
    /* usRxWrite ±äÁ¿ÔÚÖÐ¶Ïº¯ÊýÖÐ±»¸ÄÐ´£¬Ö÷³ÌÐò¶ÁÈ¡¸Ã±äÁ¿Ê±£¬±ØÐë½øÐÐÁÙ½çÇø±£??*/
    __set_PRIMASK(1);    /* ½ûÖ¹È«¾ÖÖÐ¶Ï */
    usCount = _pUart->usRxCount;
    __set_PRIMASK(0);    /* ¿ªÆôÈ«¾ÖÖÐ¶Ï */

    
    /* Èç¹û¶ÁºÍÐ´Ë÷ÒýÏàÍ¬£¬Ôò·µ?? */
    if(usCount == 0)
    {
        return 0;
    }
    else
    {
        /* ´Ó´®¿Ú½ÓÊÕFIFO??¸öÊý??*/
        *_pByte = _pUart->pRxBuf[_pUart->usRxRead];
        
        /* ¸ÄÐ´FIFO¶ÁË÷??*/
        __set_PRIMASK(1);    /* ½ûÖ¹È«¾ÖÖÐ¶Ï */
        if(++_pUart->usRxRead >= _pUart->usRxBufSize)
        {
            _pUart->usRxRead = 0;
        }
        _pUart->usRxCount--;
        __set_PRIMASK(0);    /* ¿ªÆôÈ«¾ÖÖÐ¶Ï */
        return 1;
    }
}
/*
*    ?????? comGetChar
*    ¹¦ÄÜËµÃ÷: ´Ó´®¿Ú½ÓÊÕ»º³åÇøÖÐ¶ÁÈ¡Ò»¸öÊý??
*    ??   ?? _usPort:´®¿ÚÍâÉèºÅµÄÃ¶¾Ù±äÁ¿£»_pByte£º´æ·ÅÊý¾ÝµÄÖ¸Õë
*    ?????? 0£º±íÊ¾ÎÞÊý¾Ý??£º±íÊ¾¶ÁÈ¡µ½ÓÐÐ§Êý¾Ý
*    Ê±¼ä??022????6??5??5??
*/
uint8_t comGetChar(COM_PORT_E _usPort,uint8_t *_pByte)
{
    UART_T *pUart;
    pUart = ComToUart(_usPort);
    if(pUart == 0)
    {
        return 0;
    }
    return UartGetChar(pUart,_pByte);
}
/*
*    ?????? fputc
*    ¹¦ÄÜËµÃ÷: ÖØ¶¨ÏòC±ê×¼¿âµÄprintfº¯Êý
*    ??   ?? ...
*    ?????? ·µ»ØÐ´ÈëµÄ×Ö·û×ÜÊý£¬·ñÔò·µ»ØÒ»¸ö¸º??
*    Ê±¼ä??022????5??1??1??
*/
#if DEBUG_SWITCH_EN == 0
int fputc(int ch,FILE *f)
{
#if 1
    comSendChar(COM1, ch);
    return ch;
#else
    USART1->DR = ch;
    
    while((USART1->SR & USART_SR_TC) == 0){}
        return ch;
#endif
}
#endif


/*
*    ?????? fgetc
*    ¹¦ÄÜËµÃ÷: ÖØ¶¨Òågetcº¯Êý£¬ÕâÑù¿ÉÒÔÊ¹ÓÃgetcharº¯Êý´Ó´®??ÊäÈëÊý¾Ý
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????6??5??9??
*/
#if DEBUG_SWITCH_EN == 0
int fgetc(FILE *f)
{
#if 1
    /* ´Ó´®??½ÓÊÕFIFOÖÐÈ¡1¸öÊý¾Ý£¬Ö»ÓÐÈ¡µ½Êý¾Ý²Å·µ??*/
    uint8_t ucData;
    while(comGetChar(COM1,&ucData) == 0);
    
    return ucData;
#else
    while((USART1->SR & USART_SR_RXNE) == 0){}
    return (int)USART1->DR;
#endif
}
#endif



