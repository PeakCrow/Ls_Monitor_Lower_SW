#include "sys.h"

#if USE_THREADX == 0
/* 裸机下中断通知标志 */
Sys_IrqNoti_Flag sys_irqnoti_flag;
#endif


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
    RCC_OscInitStructure.PLL.PLLM=pllm; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63    .--8
    RCC_OscInitStructure.PLL.PLLN=plln; //主PLL倍频系数(PLL倍频),取值范围:64~432.                 --336
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
#if USE_THREADX == 0
    /* 裸机下中断通知标志 */
    sys_irqnoti_flag.can1_rx_flag = 0;
#endif
}

/**
  * @FunctionName: JumpToBootloader
  * @Author:       trx
  * @DateTime:     2024年4月8日13点54分
  * @Purpose:      跳转系统 bootloader uart
  * @param:        none
  * @return:       none
**/
void JumpToBootloader(void)
{
    uint32_t i=0;
    void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
    __IO uint32_t BootAddr = 0x1FFF0000; /* STM32F4的系统BootLoader地址 */
    
    /* 关闭全局中断 */
    __set_PRIMASK(1);
    
    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    /* 设置所有时钟到默认状态，使用HSI时钟 */
    HAL_RCC_DeInit();
    
    /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }
    
    /* 使能全局中断 */
    __set_PRIMASK(0);
    
    /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
    SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));
    
    /* 设置主堆栈指针 */
    __set_MSP(*(uint32_t *)BootAddr);
    
    /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
    __set_CONTROL(0);
    
    /* 跳转到系统BootLoader */
    SysMemBootJump(); 
    
    /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
    while (1)
    {
    
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

