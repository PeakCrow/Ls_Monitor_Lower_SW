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

/*
*    函 数 名: App_I2C_EE_BufferWrite
*    功能说明: 线程安全的eeprom写人方式
*    形    参 : 同I2C_EE_BufferWrite的参数。
*    返 回 值: 无
*/
void App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
    /* 互斥操作 */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);
    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);
    tx_mutex_put(&App_PowerDownSave);    
}


/*
*    函 数 名: shell_ls_cmd
*    功能说明: 将注册的所有用例展示出来
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
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

/*
*    函 数 名: shell_test_cmd
*    功能说明: 能够打印出所有的传入的参数
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
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

/*
*    函 数 名: can1_sent_demo
*    功能说明: can1发送8帧报文 ID 0x55
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
void can1_sent_demo(char argc, char *argv)
{
    uint32_t _id = 0x55;
    uint8_t _buf[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    for(uint8_t i = 0;i < 8;i++)
    {
        bsp_Can1_Send_buf(_id,_buf,8);
        tx_thread_sleep(20);
    }
}
NR_SHELL_CMD_EXPORT(can1_send, can1_sent_demo,"can1 to send once!")


/*
*    函 数 名: can2_sent_demo
*    功能说明: can2发送8帧报文 ID 0x77
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
void can2_sent_demo(char argc, char *argv)
{
    uint32_t _id = 0x77;
    uint8_t _buf[8] = {0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77};
    for(uint8_t i = 0;i < 8;i++)
    {
        bsp_Can2_Send_buf(_id,_buf,8);
        tx_thread_sleep(20);
    }
}
NR_SHELL_CMD_EXPORT(can2_send, can2_sent_demo,"can2 to send once!")


/*
*    函 数 名: JumpToApp
*    功能说明: boot 跳转 app
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
static void JumpToApp(char argc, char *argv)
{
    uint32_t i=0;
    
    /* 声明一个函数指针 */
    void (*SysMemBootJump)(void);
    
    /* 配置app地址 */
    __IO uint32_t BootAddr = 0x08014000;
    
    /* 关闭全局中断 */
    __set_PRIMASK(1);
    
    /* 关闭嘀嗒定时器 复位到默认状态 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    /* 配置所有时钟到默认状态 使用 HSI 时钟 */
    HAL_RCC_DeInit();
    
    /* 关闭所有中断 清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }
    
    /* 使能全局中断 */
    __set_PRIMASK(0);
    
    /* 跳转到系统 bootloader 首地址是 MSP 地址加 4 是复位中断服务程序地址 */
    SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));
    
    /* 配置主堆栈指针 */
    __set_MSP(*(uint32_t *)BootAddr);
    
    /* 在 RTOS 工程 这条语句很重要 设置为特权模式 使用 MSP 指针 */
    __set_CONTROL(0);
    
    /* 跳转到系统 bootloader */
    SysMemBootJump(); 
    
    /* 跳转成功的话 不会执行到这里 用户可以在这里添加代码 */
    while (1)
    {
    
    }
}
NR_SHELL_CMD_EXPORT(jumptoapp, JumpToApp,"JumpToApp!")


/*
*    函 数 名: JumpToBoot
*    功能说明: app 跳转 boot
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
static void JumpToBoot(char argc, char *argv)
{
    uint32_t i=0;
    
    /* 声明一个函数指针 */
    void (*SysMemBootJump)(void);
    
    /* 配置boot地址 */
    __IO uint32_t BootAddr = 0x08000000;
    
    /* 关闭全局中断 */
    __set_PRIMASK(1);
    
    /* 关闭嘀嗒定时器 复位到默认状态 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    /* 配置所有时钟到默认状态 使用 HSI 时钟 */
    HAL_RCC_DeInit();
    
    /* 关闭所有中断 清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }
    
    /* 使能全局中断 */
    __set_PRIMASK(0);
    
    /* 跳转到系统 bootloader 首地址是 MSP 地址加 4 是复位中断服务程序地址 */
    SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));
    
    /* 配置主堆栈指针 */
    __set_MSP(*(uint32_t *)BootAddr);
    
    /* 在 RTOS 工程 这条语句很重要 设置为特权模式 使用 MSP 指针 */
    __set_CONTROL(0);
    
    /* 跳转到系统 bootloader */
    SysMemBootJump(); 
    
    /* 跳转成功的话 不会执行到这里 用户可以在这里添加代码 */
    while (1)
    {
    
    }
}
NR_SHELL_CMD_EXPORT(jumptoboot, JumpToBoot,"JumpToBoot!")


//#ifdef NR_SHELL_USING_EXPORT_CMD
//NR_SHELL_CMD_EXPORT(ls, shell_ls_cmd);
//NR_SHELL_CMD_EXPORT(test, shell_test_cmd);
//#else
//const static_cmd_st static_cmd[] =
//  {
//      {"ls", shell_ls_cmd},
//      {"test", shell_test_cmd},
//      {"\0", NULL}};
//#endif

/*
*    函 数 名: APRINTntf
*    功能说明: 互斥的printf方式
*    形    参 : PRINTntf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,
*             可以用省略号指定参数表
*    返 回 值: 无
*/
void App_printf(const char *fmt, ...)
{
    char  buf_str[200 + 1];/* 特别注意如果传入的printf变量比较多，注意此局部变量的大小是否够用 */
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
*    函 数 名: shell_mini_printf_cmd
*    功能说明: 测试独立的mini_printf是否打印正常
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
void shell_mini_printf_cmd(char argc, char *argv)
{
    mini_printf("test for mini PRINT !\r\n");
    mini_printf("the %s value is %d !\r\n","mini",99);
    mini_printf("the %s value is 0x%x !\r\n","float",0x10);
}
NR_SHELL_CMD_EXPORT(m_printf, shell_mini_printf_cmd,"for practice mini PRINT")


/*
*    函 数 名: onchip_flash_opertion
*    功能说明: 测试擦写pflash的最后的128k的 
*            : 0x080e0000 地址的 8 字节写入是否正常
*    形    参 : argc 传入参数的个数
*            : arhv 传入参数的地址索引
*    返 回 值: 无
*/
void onchip_flash_opertion(char argc, char *argv)
{
    mini_printf("onchip_flash_opertion !\r\n");
    uint8_t _w_flash_draft[8] = {0x66,0x66,0x66,0x66,0x77,0x77,0x77,0x88};
    
    uint8_t _r_flsah_data[8];
    memset(_r_flsah_data,0,sizeof(_r_flsah_data));

    uint8_t _ret =  bsp_WriteCpuFlash(FLASH_BASE_ADDR,_w_flash_draft,sizeof(_w_flash_draft));
    mini_printf("flash write is voer,the ret is %d\r\n",_ret);

    _ret = bsp_ReadCpuFlash(FLASH_BASE_ADDR,_r_flsah_data,sizeof(_r_flsah_data));
    mini_printf("flash read is voer,the ret is %d\r\n",_ret);

    mini_printf("\r\n");
    for(uint8_t i = 0;i < sizeof(_r_flsah_data);i++)
    {
        mini_printf("memory read data is 0x%02x\r\n",*((uint8_t *)(0x080e0000 +i) ));
    }
}
NR_SHELL_CMD_EXPORT(onchip_flash, onchip_flash_opertion,"erase and write stm32f407 onchip flash")







