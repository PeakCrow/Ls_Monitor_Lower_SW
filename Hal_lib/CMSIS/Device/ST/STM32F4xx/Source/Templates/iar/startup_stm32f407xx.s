;/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
;* File Name          : startup_stm32f407xx.s
;* Author             : MCD Application Team
;* Description        : STM32F407xx devices vector table for EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M4 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;* 
;* Redistribution and use in source and binary forms, with or without modification,
;* are permitted provided that the following conditions are met:
;*   1. Redistributions of source code must retain the above copyright notice,
;*      this list of conditions and the following disclaimer.
;*   2. Redistributions in binary form must reproduce the above copyright notice,
;*      this list of conditions and the following disclaimer in the documentation
;*      and/or other materials provided with the distribution.
;*   3. Neither the name of STMicroelectronics nor the names of its contributors
;*      may be used to endorse or promote products derived from this software
;*      without specific prior written permission.
;*
;* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*******************************************************************************
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup                   ;MODULE 是用来指定模块开始的汇编指令 ?cstartup是模块的名称

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)       ;SECTION 是一个汇编指令，用于在内存中声明一个区域， CSTACK是区域的名称，DATA 表示这个区域包含数据
                                            ;NOROOT(3) 表示这个区域不是根区域，它的优先级为3，通常用来存储堆栈数据
        SECTION .intvec:CODE:NOROOT(2)      ;SECTION 是一个汇编指令，用于在内存中声明一个区域，.intvec是区域的名称，CODE 表示这个区域包含代码
                                            ;NOROOT(2) 表示这个区域不是根区域，它的优先级为2
        EXTERN  __iar_program_start
        EXTERN  SystemInit                  ;EXTERN 用来引用外部符号
        PUBLIC  __vector_table              ;声明 __vector_table是全局的，这样可以在其他模块中使用

        DATA                                ;将内部RAM的地址赋给了__vector_table符号名
__vector_table
        ;DCD 分配一个或者多个字节为单位的内存，以四字节对齐，并要求初始化这些内存，DCD分配了堆内存(?)，并且以ESR的入口地址初始化他们
        DCD     sfe(CSTACK)                 ;表示一个32位常数，其值为CSTACK段的结束地址。 使用sfe操作符计算出CSTACK段的结束地址
        DCD     Reset_Handler               ;Reset Handler

        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler

        ; External Interrupts
        DCD     WWDG_IRQHandler                   ; Window WatchDog
        DCD     PVD_IRQHandler                    ; PVD through EXTI Line detection
        DCD     TAMP_STAMP_IRQHandler             ; Tamper and TimeStamps through the EXTI line
        DCD     RTC_WKUP_IRQHandler               ; RTC Wakeup through the EXTI line
        DCD     FLASH_IRQHandler                  ; FLASH
        DCD     RCC_IRQHandler                    ; RCC
        DCD     EXTI0_IRQHandler                  ; EXTI Line0
        DCD     EXTI1_IRQHandler                  ; EXTI Line1
        DCD     EXTI2_IRQHandler                  ; EXTI Line2
        DCD     EXTI3_IRQHandler                  ; EXTI Line3
        DCD     EXTI4_IRQHandler                  ; EXTI Line4
        DCD     DMA1_Stream0_IRQHandler           ; DMA1 Stream 0
        DCD     DMA1_Stream1_IRQHandler           ; DMA1 Stream 1
        DCD     DMA1_Stream2_IRQHandler           ; DMA1 Stream 2
        DCD     DMA1_Stream3_IRQHandler           ; DMA1 Stream 3
        DCD     DMA1_Stream4_IRQHandler           ; DMA1 Stream 4
        DCD     DMA1_Stream5_IRQHandler           ; DMA1 Stream 5
        DCD     DMA1_Stream6_IRQHandler           ; DMA1 Stream 6
        DCD     ADC_IRQHandler                    ; ADC1, ADC2 and ADC3s
        DCD     CAN1_TX_IRQHandler                ; CAN1 TX
        DCD     CAN1_RX0_IRQHandler               ; CAN1 RX0
        DCD     CAN1_RX1_IRQHandler               ; CAN1 RX1
        DCD     CAN1_SCE_IRQHandler               ; CAN1 SCE
        DCD     EXTI9_5_IRQHandler                ; External Line[9:5]s
        DCD     TIM1_BRK_TIM9_IRQHandler          ; TIM1 Break and TIM9
        DCD     TIM1_UP_TIM10_IRQHandler          ; TIM1 Update and TIM10
        DCD     TIM1_TRG_COM_TIM11_IRQHandler     ; TIM1 Trigger and Commutation and TIM11
        DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare
        DCD     TIM2_IRQHandler                   ; TIM2
        DCD     TIM3_IRQHandler                   ; TIM3
        DCD     TIM4_IRQHandler                   ; TIM4
        DCD     I2C1_EV_IRQHandler                ; I2C1 Event
        DCD     I2C1_ER_IRQHandler                ; I2C1 Error
        DCD     I2C2_EV_IRQHandler                ; I2C2 Event
        DCD     I2C2_ER_IRQHandler                ; I2C2 Error
        DCD     SPI1_IRQHandler                   ; SPI1
        DCD     SPI2_IRQHandler                   ; SPI2
        DCD     USART1_IRQHandler                 ; USART1
        DCD     USART2_IRQHandler                 ; USART2
        DCD     USART3_IRQHandler                 ; USART3
        DCD     EXTI15_10_IRQHandler              ; External Line[15:10]s
        DCD     RTC_Alarm_IRQHandler              ; RTC Alarm (A and B) through EXTI Line
        DCD     OTG_FS_WKUP_IRQHandler            ; USB OTG FS Wakeup through EXTI line
        DCD     TIM8_BRK_TIM12_IRQHandler         ; TIM8 Break and TIM12
        DCD     TIM8_UP_TIM13_IRQHandler          ; TIM8 Update and TIM13
        DCD     TIM8_TRG_COM_TIM14_IRQHandler     ; TIM8 Trigger and Commutation and TIM14
        DCD     TIM8_CC_IRQHandler                ; TIM8 Capture Compare
        DCD     DMA1_Stream7_IRQHandler           ; DMA1 Stream7
        DCD     FSMC_IRQHandler                   ; FSMC
        DCD     SDIO_IRQHandler                   ; SDIO
        DCD     TIM5_IRQHandler                   ; TIM5
        DCD     SPI3_IRQHandler                   ; SPI3
        DCD     UART4_IRQHandler                  ; UART4
        DCD     UART5_IRQHandler                  ; UART5
        DCD     TIM6_DAC_IRQHandler               ; TIM6 and DAC1&2 underrun errors
        DCD     TIM7_IRQHandler                   ; TIM7
        DCD     DMA2_Stream0_IRQHandler           ; DMA2 Stream 0
        DCD     DMA2_Stream1_IRQHandler           ; DMA2 Stream 1
        DCD     DMA2_Stream2_IRQHandler           ; DMA2 Stream 2
        DCD     DMA2_Stream3_IRQHandler           ; DMA2 Stream 3
        DCD     DMA2_Stream4_IRQHandler           ; DMA2 Stream 4
        DCD     ETH_IRQHandler                    ; Ethernet
        DCD     ETH_WKUP_IRQHandler               ; Ethernet Wakeup through EXTI line
        DCD     CAN2_TX_IRQHandler                ; CAN2 TX
        DCD     CAN2_RX0_IRQHandler               ; CAN2 RX0
        DCD     CAN2_RX1_IRQHandler               ; CAN2 RX1
        DCD     CAN2_SCE_IRQHandler               ; CAN2 SCE
        DCD     OTG_FS_IRQHandler                 ; USB OTG FS
        DCD     DMA2_Stream5_IRQHandler           ; DMA2 Stream 5
        DCD     DMA2_Stream6_IRQHandler           ; DMA2 Stream 6
        DCD     DMA2_Stream7_IRQHandler           ; DMA2 Stream 7
        DCD     USART6_IRQHandler                 ; USART6
        DCD     I2C3_EV_IRQHandler                ; I2C3 event
        DCD     I2C3_ER_IRQHandler                ; I2C3 error
        DCD     OTG_HS_EP1_OUT_IRQHandler         ; USB OTG HS End Point 1 Out
        DCD     OTG_HS_EP1_IN_IRQHandler          ; USB OTG HS End Point 1 In
        DCD     OTG_HS_WKUP_IRQHandler            ; USB OTG HS Wakeup through EXTI
        DCD     OTG_HS_IRQHandler                 ; USB OTG HS
        DCD     DCMI_IRQHandler                   ; DCMI
        DCD     0                                 ; Reserved
        DCD     HASH_RNG_IRQHandler               ; Hash and RNG
        DCD     FPU_IRQHandler                    ; FPU
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB                                   ;表示后面的指令兼容THUMB指令集，16bit
                                                ;现在Cortex-M系列的都使用THUMB-2指令集，32bit，兼容16bit与32bit的指令，是THUMB的超集
        PUBWEAK Reset_Handler                   ;声明Reset_Handler为全部弱符号，相当与c语言中的weak
        SECTION .text:CODE:REORDER:NOROOT(2)    ;SECTION 是一个汇编指令，用来定义一个段， .text是段的名称，表示这是一个代码段
                                                ;CODE 是代码段的类型，表示这是一个包含可执行代码的段。
                                                ;REORDER 是一个标志，表示汇编器可以重新排序该段中的代码以优化性能
                                                ;NOROOT(2) 是一个标志，表示该段不是根段，(2)表示该段按2字节对齐
Reset_Handler

        LDR     R0, =SystemInit                 ;LDR 从存储器中加载一个字到一个寄存器中
        BLX     R0                              ;跳转到由寄存器给出的地址，并根据寄存器的LSE确定处理器的状态，
                                                ;还要把跳转前的下条指令地址保存到LR
        LDR     R0, =__iar_program_start             
        BX      R0                              ;LDR 从存储器中加载一个字节到一个寄存器中
                                                ;跳转到由寄存器/标号给出的地址，不用返回
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)    ;这部分代码应该放在输出文件的.text部分 CODE 表示这个区段包含的是代码
                                                ;REORDER 允许汇编器重新排序这个区段中的函数 以优化程序的执行
                                                ;NOROOT(1) 阻止链接器删除这个区段 链接器有时会删除未被使用的代码区段
                                                ;以减小最终程序的大小 NOROOT 指示链接器 即使这个区段看起来没有被使用
                                                ;也不应该删除它
NMI_Handler
        B NMI_Handler                           ;B 是一个汇编指令，表示无条件分支， NMI_Handler是一个符号名，表示分支的目标地址
                                                ;这条指令的作用是将程序执行流程跳转到NMI_Handler标记的位置

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WWDG_IRQHandler  
        B WWDG_IRQHandler

        PUBWEAK PVD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PVD_IRQHandler  
        B PVD_IRQHandler

        PUBWEAK TAMP_STAMP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TAMP_STAMP_IRQHandler  
        B TAMP_STAMP_IRQHandler

        PUBWEAK RTC_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_WKUP_IRQHandler  
        B RTC_WKUP_IRQHandler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FLASH_IRQHandler  
        B FLASH_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RCC_IRQHandler  
        B RCC_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI0_IRQHandler  
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI1_IRQHandler  
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI2_IRQHandler  
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI4_IRQHandler  
        B EXTI4_IRQHandler

        PUBWEAK DMA1_Stream0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream0_IRQHandler  
        B DMA1_Stream0_IRQHandler

        PUBWEAK DMA1_Stream1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream1_IRQHandler  
        B DMA1_Stream1_IRQHandler

        PUBWEAK DMA1_Stream2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream2_IRQHandler  
        B DMA1_Stream2_IRQHandler

        PUBWEAK DMA1_Stream3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream3_IRQHandler  
        B DMA1_Stream3_IRQHandler

        PUBWEAK DMA1_Stream4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream4_IRQHandler  
        B DMA1_Stream4_IRQHandler

        PUBWEAK DMA1_Stream5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream5_IRQHandler  
        B DMA1_Stream5_IRQHandler

        PUBWEAK DMA1_Stream6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream6_IRQHandler  
        B DMA1_Stream6_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler  
        B ADC_IRQHandler

        PUBWEAK CAN1_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN1_TX_IRQHandler  
        B CAN1_TX_IRQHandler

        PUBWEAK CAN1_RX0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN1_RX0_IRQHandler  
        B CAN1_RX0_IRQHandler

        PUBWEAK CAN1_RX1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN1_RX1_IRQHandler  
        B CAN1_RX1_IRQHandler

        PUBWEAK CAN1_SCE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN1_SCE_IRQHandler  
        B CAN1_SCE_IRQHandler

        PUBWEAK EXTI9_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI9_5_IRQHandler  
        B EXTI9_5_IRQHandler

        PUBWEAK TIM1_BRK_TIM9_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_BRK_TIM9_IRQHandler  
        B TIM1_BRK_TIM9_IRQHandler

        PUBWEAK TIM1_UP_TIM10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_UP_TIM10_IRQHandler  
        B TIM1_UP_TIM10_IRQHandler

        PUBWEAK TIM1_TRG_COM_TIM11_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_TRG_COM_TIM11_IRQHandler  
        B TIM1_TRG_COM_TIM11_IRQHandler
        
        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_CC_IRQHandler  
        B TIM1_CC_IRQHandler

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM2_IRQHandler  
        B TIM2_IRQHandler

        PUBWEAK TIM3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM3_IRQHandler  
        B TIM3_IRQHandler

        PUBWEAK TIM4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM4_IRQHandler  
        B TIM4_IRQHandler

        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_EV_IRQHandler  
        B I2C1_EV_IRQHandler

        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_ER_IRQHandler  
        B I2C1_ER_IRQHandler

        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_EV_IRQHandler  
        B I2C2_EV_IRQHandler

        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_ER_IRQHandler  
        B I2C2_ER_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI1_IRQHandler  
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI2_IRQHandler  
        B SPI2_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_IRQHandler  
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART2_IRQHandler  
        B USART2_IRQHandler

        PUBWEAK USART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART3_IRQHandler  
        B USART3_IRQHandler

        PUBWEAK EXTI15_10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI15_10_IRQHandler  
        B EXTI15_10_IRQHandler

        PUBWEAK RTC_Alarm_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_Alarm_IRQHandler  
        B RTC_Alarm_IRQHandler

        PUBWEAK OTG_FS_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_FS_WKUP_IRQHandler  
        B OTG_FS_WKUP_IRQHandler
      
        PUBWEAK TIM8_BRK_TIM12_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_BRK_TIM12_IRQHandler  
        B TIM8_BRK_TIM12_IRQHandler

        PUBWEAK TIM8_UP_TIM13_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_UP_TIM13_IRQHandler  
        B TIM8_UP_TIM13_IRQHandler

        PUBWEAK TIM8_TRG_COM_TIM14_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_TRG_COM_TIM14_IRQHandler  
        B TIM8_TRG_COM_TIM14_IRQHandler

        PUBWEAK TIM8_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM8_CC_IRQHandler  
        B TIM8_CC_IRQHandler

        PUBWEAK DMA1_Stream7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream7_IRQHandler  
        B DMA1_Stream7_IRQHandler

        PUBWEAK FSMC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FSMC_IRQHandler  
        B FSMC_IRQHandler

        PUBWEAK SDIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SDIO_IRQHandler  
        B SDIO_IRQHandler

        PUBWEAK TIM5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM5_IRQHandler  
        B TIM5_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI3_IRQHandler  
        B SPI3_IRQHandler

        PUBWEAK UART4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART4_IRQHandler  
        B UART4_IRQHandler

        PUBWEAK UART5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART5_IRQHandler  
        B UART5_IRQHandler

        PUBWEAK TIM6_DAC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM6_DAC_IRQHandler  
        B TIM6_DAC_IRQHandler

        PUBWEAK TIM7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM7_IRQHandler  
        B TIM7_IRQHandler

        PUBWEAK DMA2_Stream0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream0_IRQHandler  
        B DMA2_Stream0_IRQHandler

        PUBWEAK DMA2_Stream1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream1_IRQHandler  
        B DMA2_Stream1_IRQHandler

        PUBWEAK DMA2_Stream2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream2_IRQHandler  
        B DMA2_Stream2_IRQHandler

        PUBWEAK DMA2_Stream3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream3_IRQHandler  
        B DMA2_Stream3_IRQHandler

        PUBWEAK DMA2_Stream4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream4_IRQHandler  
        B DMA2_Stream4_IRQHandler

        PUBWEAK ETH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ETH_IRQHandler  
        B ETH_IRQHandler

        PUBWEAK ETH_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ETH_WKUP_IRQHandler  
        B ETH_WKUP_IRQHandler

        PUBWEAK CAN2_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN2_TX_IRQHandler  
        B CAN2_TX_IRQHandler

        PUBWEAK CAN2_RX0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN2_RX0_IRQHandler  
        B CAN2_RX0_IRQHandler

        PUBWEAK CAN2_RX1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN2_RX1_IRQHandler  
        B CAN2_RX1_IRQHandler

        PUBWEAK CAN2_SCE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN2_SCE_IRQHandler  
        B CAN2_SCE_IRQHandler

        PUBWEAK OTG_FS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_FS_IRQHandler  
        B OTG_FS_IRQHandler

        PUBWEAK DMA2_Stream5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream5_IRQHandler  
        B DMA2_Stream5_IRQHandler

        PUBWEAK DMA2_Stream6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream6_IRQHandler  
        B DMA2_Stream6_IRQHandler

        PUBWEAK DMA2_Stream7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream7_IRQHandler  
        B DMA2_Stream7_IRQHandler

        PUBWEAK USART6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART6_IRQHandler  
        B USART6_IRQHandler

        PUBWEAK I2C3_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C3_EV_IRQHandler  
        B I2C3_EV_IRQHandler

        PUBWEAK I2C3_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C3_ER_IRQHandler  
        B I2C3_ER_IRQHandler

        PUBWEAK OTG_HS_EP1_OUT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_HS_EP1_OUT_IRQHandler  
        B OTG_HS_EP1_OUT_IRQHandler

        PUBWEAK OTG_HS_EP1_IN_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_HS_EP1_IN_IRQHandler  
        B OTG_HS_EP1_IN_IRQHandler

        PUBWEAK OTG_HS_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_HS_WKUP_IRQHandler  
        B OTG_HS_WKUP_IRQHandler

        PUBWEAK OTG_HS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_HS_IRQHandler  
        B OTG_HS_IRQHandler

        PUBWEAK DCMI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DCMI_IRQHandler  
        B DCMI_IRQHandler

        PUBWEAK HASH_RNG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
HASH_RNG_IRQHandler  
        B HASH_RNG_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FPU_IRQHandler  
        B FPU_IRQHandler

        END                                         ;文件结束
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
