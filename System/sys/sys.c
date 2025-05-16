#include "sys.h"

//时钟系统配置函数
//Fvco=Fs*(plln/pllm);
//SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//SYSCLK:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

//外部晶振为8M的时候,推荐值:plln=336,pllm=8,pllp=2,pllq=7.
//得到:Fvco=8*(336/8)=336Mhz
//     SYSCLK=336/2=168Mhz
//     Fusb=336/7=48Mhz
//     AHB总线时钟=168/1 = 168Mhz
//     APB1总线时钟=168/4 = 42Mhz
//     APB2总线时钟=168/2 = 84Mhz
//返回值:0,成功;1,失败
void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq)
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
    RCC_OscInitStructure.PLL.PLLN=plln; //主PLL倍频系数(PLL倍频),取值                    �:64~432.  					--336
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




#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

////THUMB指令不支持汇编内联
////采用如下方法实现执行汇编指令WFI  
//__asm void WFI_SET(void)
//{
//    WFI;          
//}
////关闭所有中断(但是不包括fault和NMI中断)
//__asm void INTX_DISABLE(void)
//{
//    CPSID   I
//    BX      LR      
//}
////开启所有中断
//__asm void INTX_ENABLE(void)
//{
//    CPSIE   I
//    BX      LR  
//}
////设置栈顶地址
////addr:栈顶地址
//__asm void MSR_MSP(uint32_t addr) 
//{
//    MSR MSP, r0             //set Main Stack value
//    BX r14
//}

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


