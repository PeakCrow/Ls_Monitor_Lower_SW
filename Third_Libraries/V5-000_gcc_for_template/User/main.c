/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 跑马灯。
*              实验目的：
*                1. 学习F407平台的跑马灯实现。
*              实验内容：
*                1、启动一个自动重装软件定时器，每100ms翻转一次LED1和LED2。
*                2、再启动一个自动重装软件定时器，每500ms翻转一次LED3和LED4。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-04-23   Eric2013     1. CMSIS软包版本 V5.5.0
*                                         2. HAL库版本 V2.4.0
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"			/* 底层硬件驱动 */



/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V5-跑马灯"
#define EXAMPLE_DATE	"2019-04-23"
#define DEMO_VER		"1.0"

static void mini_printfLogo(void);
static void mini_printfHelp(void);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参: 无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{

	bsp_Init();		/* 硬件初始化 */
	
	mini_printfLogo();	/* 打印例程名称和版本等信息 */
	mini_printfHelp();	/* 打印操作提示 */

	/* 先做个LED1的亮灭显示 */
	bsp_LedOn(1);
	bsp_DelayMS(100);
	bsp_LedOff(1);
	bsp_DelayMS(100);
	
	bsp_StartAutoTimer(0, 100); /* 启动1个100ms的自动重装的定时器 */
	bsp_StartAutoTimer(1, 500);	/* 启动1个500ms的自动重装的定时器 */
	
	/* 进入主程序循环体 */
	while (1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */

		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			/* 每隔100ms 进来一次 */  
			bsp_LedToggle(1);			
		}
		
		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(1))	
		{
			/* 每隔500ms 进来一次 */ 
			bsp_LedToggle(2);			
			bsp_LedToggle(3);			
			bsp_LedToggle(4);
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: mini_printfHelp
*	功能说明: 打印操作提示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void mini_printfHelp(void)
{
	mini_printf("操作提示:\r\n");
	mini_printf("1. 启动一个自动重装软件定时器，每100ms翻转一次LED1和LED2\r\n");
	mini_printf("2. 再启动一个自动重装软件定时器，每500ms翻转一次LED3和LED4\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: mini_printfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void mini_printfLogo(void)
{
	mini_printf("*************************************************************\n\r");
	
	/* 检测CPU ID */
	{
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;
		
		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

		mini_printf("\r\nCPU : STM32F407IGT6, LQFP176, 主频: %dMHz\r\n", SystemCoreClock / 1000000);
		mini_printf("UID = %08X %08X %08X\n\r", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	mini_printf("\n\r");
	mini_printf("*************************************************************\n\r");
//	mini_printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	mini_printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	mini_printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST的HAL库版本 */
	mini_printf("* HAL库版本  : V2.4.0 (STM32F407 HAL Driver)\r\n");
	mini_printf("* \r\n");	/* 打印一行空格 */
	mini_printf("* QQ    : 1295744630 \r\n");
	mini_printf("* 旺旺  : armfly\r\n");
	mini_printf("* Email : armfly@qq.com \r\n");
	mini_printf("* 微信公众号: armfly_com \r\n");
	mini_printf("* 淘宝店: armfly.taobao.com\r\n");
	mini_printf("* Copyright www.armfly.com 安富莱电子\r\n");
	mini_printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
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

