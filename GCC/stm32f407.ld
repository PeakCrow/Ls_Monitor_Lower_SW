/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
/* Must be 8-byte aligned for Cortex-M4 and ThreadX */
_estack = 0x20020000;    /* end of RAM 128k, already 8-byte aligned */

/* Generate a link error if heap and stack don't fit into RAM */
/* Sizes must be multiples of 8 for proper alignment */
_Min_Heap_Size = 0x2000;      /* required amount of heap (8kb) */
_Min_Stack_Size = 0x2000;     /* required amount of stack (8kb) */

/* Specify the memory areas */
MEMORY
{
	FLASH (rx)       : ORIGIN = 0x08000000, LENGTH = 1024K
	RAM (xrw)        : ORIGIN = 0x20000000, LENGTH = 128K
	CCMRAM (rw)      : ORIGIN = 0x10000000, LENGTH = 64K
	EXTERN_SRAM(rw)  : ORIGIN = 0x68000000, LENGTH = 1024K
}

/* Define output sections */
SECTIONS
{
 	/* The startup code goes first into FLASH */
 	._vectors :
 	{
    	. = ALIGN(4);
    	_vectors = .; 			/* Define _vectors at the end of the vector table */
    	KEEP(*(.vectors)) 		/* Startup code */
    	. = ALIGN(4);
 	} >FLASH

	/* The program code and other data goes into FLASH */
	.text :
 	{
    	. = ALIGN(4);
    	*(.text)           /* .text sections (code) */
    	*(.text*)          /* .text* sections (code) */
    	*(.glue_7)         /* glue arm to thumb code */
    	*(.glue_7t)        /* glue thumb to arm code */
    	*(.eh_frame)

    	KEEP (*(.init))
    	KEEP (*(.fini))

    	. = ALIGN(4);
    	_etext = .;        /* define a global symbol at end of code */
	} >FLASH

	/*
	 * NR Micro Shell exported command table (GNU GCC).
	 * - Keep all exported command entries even with --gc-section
	 * - Provide start/stop boundary symbols for iteration
	*/

	.nr_shell_cmd :
	{
		. = ALIGN(4);
		__start_nr_shell_cmd = .;
		KEEP(*(.nr_shell_cmd))
		__stop_nr_shell_cmd = .;
		. = ALIGN(4);
	} >FLASH

	/* Constant data goes into FLASH */
	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)         /* .rodata sections (constants, strings, etc.) */
		*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
	} >FLASH

	.ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
	.ARM : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >FLASH

	.preinit_array     :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} >FLASH
	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} >FLASH
	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} >FLASH

	/* used by the startup to initialize data */
	_sidata = LOADADDR(.data);

	/* Initialized data sections goes into RAM, load LMA copy after code */
	.data : 
	{
		. = ALIGN(4);
		_sdata = .;        /* create a global symbol at data start */
		*(.data)           /* .data sections */
		*(.data*)          /* .data* sections */

		. = ALIGN(4);
		_edata = .;        /* define a global symbol at data end */
	} >RAM AT> FLASH

	_siccmram = LOADADDR(.ccmram);

	/* CCM-RAM section 
	* 
	* IMPORTANT NOTE! 
	* If initialized variables will be placed in this section, 
	* the startup code needs to be modified to copy the init-values.  
	*/
	.ccmram :
	{
		. = ALIGN(4);
		_sccmram = .;       /* create a global symbol at ccmram start */
		*(.ccmram)
		*(.ccmram*)

		. = ALIGN(4);
		_eccmram = .;       /* create a global symbol at ccmram end */
	} >CCMRAM AT> FLASH

	.extern_sram :
	{
		. = ALIGN(4);
		_sextern_sram = .;       /* create a global symbol at ccmram start */
		*(.EXTERN_SRAM)
		*(.EXTERN_SRAM*)

		. = ALIGN(4);
		_eextern_sram = .;       /* create a global symbol at ccmram end */
	} >EXTERN_SRAM

  
	/* Uninitialized data section */
	. = ALIGN(4);
	.bss :
	{
		/* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;         /* define a global symbol at bss start */
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;         /* define a global symbol at bss end */
		__bss_end__ = _ebss;
	} >RAM

	/* User_heap_stack section, used to check that there is enough RAM left */
	._user_heap_stack :
	{
		. = ALIGN(8);
		PROVIDE ( end = . );
		PROVIDE ( _end = . );
		. = . + _Min_Heap_Size;
		. = . + _Min_Stack_Size;
		. = ALIGN(8);				/* ThreadX requires 8-byte alignent */
	} >RAM

	/* Add __RAM_segment_used_end__ here to mark the end of the RAM segment */
	__RAM_segment_used_end__ = .;


	/* Remove information from the standard libraries */
	/DISCARD/ :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}

	.ARM.attributes 0 : { *(.ARM.attributes) }
}
