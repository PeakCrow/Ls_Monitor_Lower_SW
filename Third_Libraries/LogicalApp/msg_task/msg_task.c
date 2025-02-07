#include "msg_task.h"

TX_THREAD   AppTaskUserIFTCB;
uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];
TX_THREAD   AppTaskMsgProTCB;
uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];
TX_MUTEX    AppPrintfSemp;              /* 用PRINTntf互斥 */
TX_MUTEX    App_PowerDownSave;          /* 用于掉电保存 */

void App_printf(const char *fmt, ...);
int mini_printf(const char *fmt, ...);


void AppTaskMsgPro(ULONG thread_input)
{
    (void)thread_input;
    ULONG actual_events;
    UINT status;
    while(1)
    {
        status = tx_event_flags_get(&EventGroup,     /* 事件标志控制块 */
                                    DRIVER_POS_ALL,  /* 等待标志 */
                                    TX_OR_CLEAR ,    /* 等待任意bit满足即可 */
                                    &actual_events,  /* 获取实际值 */
                                    TX_WAIT_FOREVER);/* 永久等待 */
        if(status == TX_SUCCESS)
        {
        switch (actual_events)
            {
                case DRIVER1_POS:
                    /*  */
                    break;
                case DRIVER2_POS:
                    /*  */
                    break;
                case DRIVER3_POS:
                    /*  */
                    break;
                default:
                    break;
            }
        }

        tx_thread_sleep(200);
    }   
}
/*
*    函 数 名: AppTaskUserIF
*    功能说明: 按键消息处理
*    形    参 : thread_input 创建该任务时传递的形参
*    返 回 值: 无
*    优 先 级: 4
*/
void AppTaskUserIF(ULONG thread_input)
{
    (void)thread_input;
    App_printf("按键驱动初始化!\n");
    
    uint8_t _shell_get = ' ';
    uint8_t _comchar_status = 0;
    uint8_t ucKeyCode;
    
    while(1)
    {
        /* 拿到按键数值 */
        ucKeyCode = bsp_GetKey();
        
        /* 拿到串口的单个字符串 并交给 shell 终端进行处理 */
        _comchar_status = comGetChar(COM1,&_shell_get);
        
        if(0x0u != _comchar_status)
        {
            shell((char)_shell_get);
        }
        
        if (ucKeyCode != KEY_NONE)
        {
            switch(ucKeyCode)
            {
            case KEY_0_UP:
                App_printf("k0按键弹起\r\n");
                break;
            case KEY_0_DOWN:
                App_printf("k0按键按下\r\n");
                break;
            case KEY_UP_UP:
                App_printf("kup按键弹起\r\n");
                break;
            case KEY_UP_DOWN:
                App_printf("kup按键按下\r\n");
                break;
            case KEY_0_LONG:
                App_printf("k0按键长按\r\n");
                break;
            case KEY_UP_LONG:
                App_printf("kup按键长按\r\n");
                break;
            case KEY_MULTI_DOWM:
                App_printf("kmulti按键按下\r\n");
                break;
            case KEY_MULTI_UP:
                App_printf("kmulti按键弹起\r\n");
                break;
            case KEY_MULTI_LONG:
                App_printf("kmulti按键长按\r\n");
                break;
            }
        }
        tx_thread_sleep(20);
    }
}

/**
 * @brief ls command
 */
void shell_ls_cmd(char argc, char *argv)
{
    unsigned int i = 0;
    if (argc > 1)
    {
        if (!strcmp("cmd", &argv[argv[1]]))
        {
            for (i = 0; nr_shell.static_cmd[i].fp != NULL; i++)
            {
                shell_printf("%s",nr_shell.static_cmd[i].cmd);
                shell_printf("\r\n");
            }
        }
        else if (!strcmp("-v", &argv[argv[1]]))
        {
            shell_printf("ls version 1.0.\r\n");
        }
        else if (!strcmp("-h", &argv[argv[1]]))
        {
            shell_printf("useage: ls [options]\r\n");
            shell_printf("options: \r\n");
            shell_printf("\t -h \t: show help\r\n");
            shell_printf("\t -v \t: show version\r\n");
            shell_printf("\t cmd \t: show all commands\r\n");
        }
    }
    else
    {
        shell_printf("ls need more arguments!\r\n");
    }
}
NR_SHELL_CMD_EXPORT(ls, shell_ls_cmd,"shell demo cmd for ls,paras [cmd][-v][-h]")

/**
 * @brief test command
 */
void shell_test_cmd(char argc, char *argv)
{
    unsigned int i;
    shell_printf("test command:\r\n");
    for (i = 0; i < argc; i++)
    {
        shell_printf("paras %d: %s\r\n", i, &(argv[argv[i]]));
    }
}
NR_SHELL_CMD_EXPORT(test, shell_test_cmd,"shell demo cmd for test,traverse all paras")

//#ifdef NR_SHELL_USING_EXPORT_CMD
//NR_SHELL_CMD_EXPORT(ls, shell_ls_cmd);
//NR_SHELL_CMD_EXPORT(test, shell_test_cmd);
//#else
//const static_cmd_st static_cmd[] =
//	{
//		{"ls", shell_ls_cmd},
//		{"test", shell_test_cmd},
//		{"\0", NULL}};
//#endif

/*
* 
*    函 数 名: APRINTntf
*    功能说明: �PRINT�全的printf方式
*    形    参 : PRINTntf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,
*             可以用省略号指定参数表
*    返 回 值: 无
* 
*/
void App_printf(const char *fmt, ...)
{
    char  buf_str[200 + 1];/* 特别注�PRINT�果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list   v_args;
    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);
    /* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);
    PRINT("%s", buf_str);
    tx_mutex_put(&AppPrintfSemp);
}
/*
* 
*    函 数 名: App_I2C_EE_BufferWrite
*    功能说明: 线程安全的eeprom写人方式
*    形    参 : 同I2C_EE_BufferWrite的参数。
*    返 回 值: 无
* 
*/
void App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
    /* 互斥操作 */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);
    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);
    tx_mutex_put(&App_PowerDownSave);    
}

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
    const char *p = NULL, *str = NULL,*scan = NULL;
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

/**
 * @brief m_PRINT command
 */
void shell_mini_printf_cmd(char argc, char *argv)
{
    mini_printf("test for mini PRINT !\r\n");
    mini_printf("the %s value is %d !\r\n","mini",99);
    mini_printf("the %s value is 0x%x !\r\n","float",0x10);
}
NR_SHELL_CMD_EXPORT(m_printf, shell_mini_printf_cmd,"for practice mini PRINT")

