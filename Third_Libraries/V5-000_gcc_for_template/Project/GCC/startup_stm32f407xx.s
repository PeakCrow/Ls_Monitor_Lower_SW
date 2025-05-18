/* startup_stm32f407xx.s
 *
 * 针对 STM32F407xx 的启动文件示例
 * 使用 GNU 汇编器 (arm-none-eabi-as) 格式。
 */

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

/*---------------------------------------------------------------------*/
/* 外部符号声明：这些符号将在链接脚本中定义 */
/*---------------------------------------------------------------------*/
.extern  SystemInit      /* 系统初始化函数（时钟等配置） */
.extern  main            /* 用户的 main() 函数 */
.extern  _estack         /* 堆栈顶，由链接脚本定义 */

/* 数据段复制和 BSS 清零所需符号，从链接脚本获得 */
.extern  _sidata         /* FLASH 中.data初始化数据起始地址 */
.extern  _sdata          /* RAM中 .data 段起始地址 */
.extern  _edata          /* RAM中 .data 段结束地址 */
.extern  _sbss           /* RAM中 .bss 段起始地址 */
.extern  _ebss           /* RAM中 .bss 段结束地址 */

/*---------------------------------------------------------------------*/
/* 弱中断处理函数声明，默认指向 Default_Handler */
/*---------------------------------------------------------------------*/
.weak   NMI_Handler
.weak   HardFault_Handler
.weak   MemManage_Handler
.weak   BusFault_Handler
.weak   UsageFault_Handler
.weak   SVC_Handler
.weak   DebugMon_Handler
.weak   PendSV_Handler
.weak   SysTick_Handler

.thumb_set NMI_Handler,       Default_Handler
.thumb_set HardFault_Handler, Default_Handler
.thumb_set MemManage_Handler, Default_Handler
.thumb_set BusFault_Handler,  Default_Handler
.thumb_set UsageFault_Handler,Default_Handler
.thumb_set SVC_Handler,       Default_Handler
.thumb_set DebugMon_Handler,  Default_Handler
.thumb_set PendSV_Handler,    Default_Handler
.thumb_set SysTick_Handler,   Default_Handler

/*---------------------------------------------------------------------*/
/* 中断向量表 */
/*---------------------------------------------------------------------*/
.section .isr_vector, "a", %progbits
.align  2
.global g_pfnVectors
g_pfnVectors:
    .word  _estack               /* 初始堆栈指针 */
    .word  Reset_Handler         /* Reset Handler */
    .word  NMI_Handler           /* NMI Handler */
    .word  HardFault_Handler     /* Hard Fault Handler */
    .word  MemManage_Handler     /* MPU Fault Handler */
    .word  BusFault_Handler      /* Bus Fault Handler */
    .word  UsageFault_Handler    /* Usage Fault Handler */
    .word  0                     /* Reserved */
    .word  0                     /* Reserved */
    .word  0                     /* Reserved */
    .word  0                     /* Reserved */
    .word  SVC_Handler           /* SVCall Handler */
    .word  DebugMon_Handler      /* Debug Monitor Handler */
    .word  0                     /* Reserved */
    .word  PendSV_Handler        /* PendSV Handler */
    .word  SysTick_Handler       /* SysTick Handler */
    /* 如需更多中断，可以在此后追加 */

/*---------------------------------------------------------------------*/
/* Reset Handler 实现 */
/*---------------------------------------------------------------------*/
.section .text.Reset_Handler, "ax", %progbits
.align  2
.global Reset_Handler
.type   Reset_Handler, %function
Reset_Handler:
    /* 调用 SystemInit 对时钟进行配置等初始化 */
    bl SystemInit

    /* 初始化 .data 段：从 FLASH 把初始化数据复制到 RAM */
    ldr   r0, =_sidata       /* FLASH中.data初始化数据开始地址 */
    ldr   r1, =_sdata        /* RAM中 .data 开始地址 */
    ldr   r2, =_edata        /* RAM中 .data 结束地址 */
DataCopyLoop:
    cmp   r1, r2
    bcc   CopyData
    b     DataCopyDone
CopyData:
    ldr   r3, [r0], #4       /* 复制4字节 */
    str   r3, [r1], #4
    b     DataCopyLoop
DataCopyDone:

    /* 零初始化 .bss 段 */
    ldr   r0, =_sbss         /* .bss 开始地址 */
    ldr   r1, =_ebss         /* .bss 结束地址 */
    movs  r2, #0
BSSZeroLoop:
    cmp   r0, r1
    bcc   ZeroBSS
    b     BSSZeroDone
ZeroBSS:
    str   r2, [r0], #4
    b     BSSZeroLoop
BSSZeroDone:

    /* 调用用户的 main() 函数 */
    bl main

    /* 如果 main 返回，则进入死循环 */
InfiniteLoop:
    b InfiniteLoop

/*---------------------------------------------------------------------*/
/* 默认异常处理函数 */
/*---------------------------------------------------------------------*/
.thumb_func
.global Default_Handler
.type Default_Handler, %function
Default_Handler:
Infinite_Default:
    b Infinite_Default
