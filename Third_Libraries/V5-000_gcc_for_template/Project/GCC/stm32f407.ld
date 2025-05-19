/* stm32f407.ld
 *
 * 示例链接脚本，适用于 1MB FLASH、192KB RAM 的 STM32F407 系列芯片。
 * 根据具体芯片的 FLASH、SRAM 大小调整 ORIGIN 和 LENGTH。
 */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K   /* 1MB 闪存 */
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 192K      /* 192KB 内部 RAM */
}

/* 指定程序入口点 */
ENTRY(Reset_Handler)

SECTIONS
{
  /* 中断向量表，必须放在 FLASH 中 */
  .text : 
  {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    *(.init*)     /* 把 .init 段内容放进来 */
    *(.fini*)
    *(.ARM.exidx*)    /* 将异常处理表合入 text 段 */
    _etext = .;
  } > FLASH

  /* 初始化数据段：在 FLASH 中存放初始化值，运行时复制到 RAM */
  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    . = ALIGN(4);
    _sdata = .;       /* .data 在 RAM 中开始位置 */
    *(.data*)
    . = ALIGN(4);
    _edata = .;       /* .data 在 RAM 中结束位置 */
  } > RAM
  _sidata = LOADADDR(.data);   /* .data 对应FLASH中的加载地址 */

  /* 未初始化数据段放在 RAM 中 */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;        /* .bss 在 RAM 中开始地址 */
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;        /* .bss 在 RAM 中结束地址 */
  } > RAM

  /* 定义 end 符号，指向静态分配区的末尾 */
  PROVIDE(end = .);

  /* 定义堆栈结束符（堆栈从 RAM 顶部向下降） */
  _estack = ORIGIN(RAM) + LENGTH(RAM);
}
