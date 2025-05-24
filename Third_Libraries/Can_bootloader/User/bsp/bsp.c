/*
*********************************************************************************************************
*
*    模块名称 : BSP模块(For STM32F407)
*    文件名称 : bsp.c
*    版    本 : V1.0
*    说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*               bsp = Borad surport packet 板级支持包
*    修改记录 :
*        版本号  日期         作者       说明
*        V1.0    2018-07-29  Eric2013   正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"



/*
*********************************************************************************************************
*                                       函数声明
*********************************************************************************************************
*/
//static void SystemClock_Config(void);
static void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq);

/*
*********************************************************************************************************
*    函 数 名: bsp_Init
*    功能说明: 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
    /* 
       STM32H407 HAL 库初始化，此时系统用的还是F407自带的16MHz，HSI时钟:
       - 调用函数HAL_InitTick，初始化滴答时钟中断1ms。
       - 设置NVIV优先级分组为4。
     */
    HAL_Init();

    /* 
       配置系统时钟到168MHz
       - 切换使用HSE。
       - 此函数会更新全局变量SystemCoreClock，并重新配置HAL_InitTick。
    */
    //SystemClock_Config();
    Stm32_Clock_Init(336,8,2,7);        /* system tick,168Mhz */

    /* 
       Event Recorder：
       - 可用于代码执行时间测量，MDK5.25及其以上版本才支持，IAR不支持。
       - 默认不开启，如果要使能此选项，务必看V5开发板用户手册第8章
    */    
#if Enable_EventRecorder == 1  
    /* 初始化EventRecorder并开启 */
    EventRecorderInitialize(EventRecordAll, 1U);
    EventRecorderStart();
#endif
    
    bsp_InitKey();        /* 按键初始化，要放在滴答定时器之前，因为按钮检测是通过滴答定时器扫描 */
    bsp_InitTimer();      /* 初始化滴答定时器 */
    bsp_InitUart();        /* 初始化串口 */
    bsp_InitLed();        /* 初始化LED */
//    bsp_InitCan1Bus();                            /* 初始化CAN1 总线 */
//    bsp_InitCan2Bus();                            /* 初始化CAN2 总线 */
}

/*
*********************************************************************************************************
*    函 数 名: SystemClock_Config
*    功能说明: 初始化系统时钟
*                System Clock source            = PLL (HSE)
*                SYSCLK(Hz)                     = 168000000 (CPU Clock)
*                HCLK = SYSCLK / 1              = 168000000 (AHB1Periph)
*                PCLK2 = HCLK / 2               = 84000000  (APB2Periph)
*                PCLK1 = HCLK / 4               = 42000000  (APB1Periph)
*                HSE Frequency(Hz)              = 25000000
*               PLL_M                          = 25
*                PLL_N                          = 336
*                PLL_P                          = 2
*                PLL_Q                          = 4
*                VDD(V)                         = 3.3
*                Flash Latency(WS)              = 5
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
//static void SystemClock_Config(void)
//{
//    RCC_ClkInitTypeDef RCC_ClkInitStruct;
//    RCC_OscInitTypeDef RCC_OscInitStruct;

//    
//    /* 芯片内部的LDO稳压器输出的电压范围，选用的PWR_REGULATOR_VOLTAGE_SCALE1 */
//    __HAL_RCC_PWR_CLK_ENABLE();
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//    /* 使能HSE，并选择HSE作为PLL时钟源 */
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//    RCC_OscInitStruct.PLL.PLLM = 25;
//    RCC_OscInitStruct.PLL.PLLN = 336;
//    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//    RCC_OscInitStruct.PLL.PLLQ = 4;
//    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//    {
//        Error_Handler(__FILE__, __LINE__);
//    }

//    /* 
//       选择PLL的输出作为系统时钟
//        HCLK = SYSCLK / 1  (AHB1Periph)
//        PCLK2 = HCLK / 2   (APB2Periph)
//        PCLK1 = HCLK / 4   (APB1Periph)
//    */
//    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
//    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

//    /* 此函数会更新SystemCoreClock，并重新配置HAL_InitTick */
//    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//    {
//        Error_Handler(__FILE__, __LINE__);
//    }

//    /* 使能SYS时钟和IO补偿 */
//    __HAL_RCC_SYSCFG_CLK_ENABLE() ;

//    HAL_EnableCompensationCell();
//}


static void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure;
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
    __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟
    
    //下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡。
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//设置调压器输出电压级别1
    
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;    //时钟源为HSE
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;                      //打开HSE
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;//打开PLL
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLM=pllm; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.--8
    RCC_OscInitStructure.PLL.PLLN=plln; //主PLL倍频系数(PLL倍频),取值                    Ժ64~432.                      --336
    RCC_OscInitStructure.PLL.PLLP=pllp; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)--2
    RCC_OscInitStructure.PLL.PLLQ=pllq; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.  --7
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//初始化
    
    if(ret!=HAL_OK) while(1);
    
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;//AHB分频系数为1
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4; //APB1分频系数为4
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2; //APB2分频系数为2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);//同时设置FLASH延时周期为5WS，也就是6个CPU周期。
    
    if(ret!=HAL_OK) while(1);

     //STM32F405x/407x/415x/417x Z版本的器件支持预取功能
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();  //使能flash预取
    }

}

/*
*********************************************************************************************************
*    函 数 名: Error_Handler
*    形    参: file : 源代码文件名称。关键字 __FILE__ 表示源代码文件名。
*              line ：代码行号。关键字 __LINE__ 表示源代码行号
*    返 回 值: 无
*        Error_Handler(__FILE__, __LINE__);
*********************************************************************************************************
*/
void Error_Handler(char *file, uint32_t line)
{
    /* 
        用户可以添加自己的代码报告源代码文件名和代码行号，比如将错误文件和行号打印到串口
        printf("Wrong parameters value: file %s on line %d\r\n", file, line) 
    */
    
    /* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
    if (line == 0)
    {
        return;
    }
    
    while(1)
    {
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_RunPer10ms
*    功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
*            任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
    bsp_KeyScan10ms();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_RunPer1ms
*    功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
*             可以放在此函数。比如：触摸坐标扫描。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
    
}

/*
*********************************************************************************************************
*    函 数 名: bsp_Idle
*    功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
*             本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_Idle(void)
{
    /* --- 喂狗 */

    /* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */

    /* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
    //GUI_Exec();

    /* 例如 uIP 协议，可以插入uip轮询函数 */
    //TOUCH_CapScan();
}

/*
*********************************************************************************************************
*    函 数 名: HAL_Delay
*    功能说明: 重定向毫秒延迟函数。替换HAL中的函数。因为HAL中的缺省函数依赖于Systick中断，如果在USB、SD卡
*             中断中有延迟函数，则会锁死。也可以通过函数HAL_NVIC_SetPriority提升Systick中断
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
/* 当前例子使用stm32f4xx_hal.c默认方式实现，未使用下面重定向的函数 */
#if 0
void HAL_Delay(uint32_t Delay)
{
    bsp_DelayUS(Delay * 1000);
}
#endif

static const char assic[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             'a', 'b', 'c', 'd', 'e', 'f'
                            };

static const unsigned int dec_base[] = {
    1UL,
    10UL,
    100UL,
    1000UL,
    10000UL,
    100000UL,
    1000000UL,
    10000000UL,
    100000000UL,
    1000000000UL,
};

void send_char(uint8_t c)
{
    //uart_tx(TTY_UART, &c, 1);
    comSendChar(COM1, c);
}
/*
 * Format tag prototype is "%[flags][width][.precision][length]specifier"
 *      flags: -, +, space, #, O
 *      .precision: .number
 *      length: h, I, L
 *      width: number, *
 */
int mini_printf(const char *fmt, ...)
{
    va_list args;
    const char *p = NULL, *str = NULL;//*scan = NULL;
    int ival = 0, i = 0;
    unsigned int digit = 0, msb_processed = 0, index = 0, width = 8;
    float fv;

    va_start(args, fmt);

#if defined(CFG_AOS)
    //TaskSuspendAll();
#endif
//    for (scan = fmt; *scan; scan++) {
//        if (*scan == '\n') {
//            PRINT_core();
//            break;
//        }
//    }

    for (p = fmt; *p; p++) {
        if (*p != '%') {
            if (*p == '\n') {
                uint8_t c = '\r';
                send_char(c);
            }

            send_char(*p);
            continue;
        }

        p++;

        while (*p != 'c' && *p != 'd'
               && *p != 'X' && *p != 'x'
               && *p != 'p' && *p != 'u'
               && *p != 's' && *p != 'f') {
            if (*p >= '0' && *p <= '9' && *(p - 1) != '.' && *(p - 2) != '.') {
                width = *p - '0';

                if (width < 1) {
                    width = 1;
                }
            }

            p++;
        }

        switch (*p) {
        case 'd':
            ival = va_arg(args, int);

            if (ival < 0) {
                send_char('-');
                ival = 0 - ival;
            }

            msb_processed = 0;

            for (i = 9; i >= 0; i--) {
                digit = ival / dec_base[i];
                ival = ival % dec_base[i];

                if ((digit > 0) || (msb_processed)) {
                    send_char(digit + '0');
                    msb_processed = 1;
                }
            }

            /* all digits are zeros */
            if (!msb_processed) {
                send_char('0');
            }

            break;

        case 'x':
        case 'X':
        case 'u':
        case 'p':
            ival = va_arg(args, int);

            for (i = width - 1; i >= 0; i--) {
                index = (ival >> (i) * 4) & 0xFUL;
                send_char(assic[index]);
            }

            width = 8;
            break;

        case 's':
            for (str = va_arg(args, char *); *str; str++) {
                send_char(*str);
            }

            break;

        case 'c':
            send_char(va_arg(args, int));
            break;
        case 'f':

            fv = va_arg(args, double);
            ival = (int)fv;
            if (ival < 0) {
                ival = - ival;
                send_char('-');
            }

            msb_processed = 0;
            for (i = 9; i >= 0; i--) {
                digit = ival / dec_base[i];
                ival = ival % dec_base[i];
                if ((digit > 0) || (msb_processed)) {
                    send_char(digit + '0');
                    msb_processed = 1;
                }
            }
            /* all digits are zeros */
            if (!msb_processed) {
                send_char('0');
            }
            send_char('.');
            /* remain four digits after the decimal point */
            ival = (int)(fv * 10000);
            ival = ival % 10000;
            msb_processed = 0;
            for (i = 3; i >= 0; i--) {
                digit = ival / dec_base[i];
                ival = ival % dec_base[i];
                if(digit != 0)
                {
                    msb_processed = 1;
                }
                send_char(digit + '0');
            }
            /* all digits are zeros */
            if (!msb_processed) {
                send_char('0');
            }

            break;
        default:
            break;
        }
    }

#if defined(CFG_AOS)
    //xTaskResumeAll();
#endif
    va_end(args);

    return 0;
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
