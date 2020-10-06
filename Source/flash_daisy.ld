/* Linker script for STM32H750 */


/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
/* _estack = 0x20020000; */   /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x000;      /* required amount of heap  */
_Min_Stack_Size = 0x1000; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
/* DTCMRAM (rwx)     : ORIGIN = 0x20000000, LENGTH = 128K */
/* ITCMRAM (rwx)     : ORIGIN = 0x00000000, LENGTH = 64K */
/* LOADERRAM (rwx)   : ORIGIN = 0x30000000, LENGTH = 128K */
/* FASTHEAP (rwx)    : ORIGIN = 0x20020000, LENGTH = 64K */
PATCHRAM (rwx)    : ORIGIN = 0x24000000, LENGTH = 512K /* 512k total */
/* RAM_D3 (rwx)      : ORIGIN = 0x38000000, LENGTH = 64K */ /* Unused */
EXTRAM (rwx)      : ORIGIN = 0xc0080000, LENGTH = 6400K
}
/* Device flash is 128k, we split it between bootloader and settings
 QSPI flash is 8M, split it between FW and patches

 QSPI flash becomes available only after bootloader initializes it in memory-mapped mode
*/

/*
	NOTE: we don't have CCM RAM on H7 MCUs, so CCMHEAP/CCMRAM sections are not defined.
*/

SECTIONS
{
    .program_header :
    {
      . = ALIGN(8);
      _startprog = ABSOLUTE(.);         /* define a global symbol at program start */
      KEEP(*(.program_header))
      . = ALIGN(8);
    } > PATCHRAM

    .text :
    {
    . = ALIGN(8);
    _text_start = .;
    *(.text.Reset_Handler)
    *(.text.startup)
    *(.main*)
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    . = ALIGN(8);
    /* These are for static constructors and destructors under ELF */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(8);
    _etext = .;        /* define a global symbol at end of code */
    _text_end = .;
	} > PATCHRAM

  _text_load = LOADADDR (.text);
  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >PATCHRAM /* AT>PATCHFLASH */
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >PATCHRAM /* AT>PATCHFLASH */
  

	.ARM.attributes 0 :
	{
		*(.ARM.attributes)
	}

	.preinit_array :
	{
		PROVIDE_HIDDEN(__preinit_array_start = .);
		KEEP(*(.preinit_array*))
		PROVIDE_HIDDEN(__preinit_array_end = .);
	} > PATCHRAM

	.init_array :
	{
		PROVIDE_HIDDEN(__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array*))
		PROVIDE_HIDDEN(__init_array_end = .);
	} > PATCHRAM

	.fini_array :
	{
		PROVIDE_HIDDEN(__fini_array_start = .);
		KEEP(*(.fini_array*))
		KEEP(*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN(__fini_array_end = .);
	} > PATCHRAM

  /* used by the startup to initialize data */
  _sidata = .;  

	.data : AT ( _sidata )
	{
		. = ALIGN(8);
		_sdata = .;

		/*PROVIDE(__data_start__ = _sdata);*/
		*(.data)
		*(.data*)
		. = ALIGN(8);
		_edata = .;

		/*PROVIDE(__data_end__ = _edata);*/
	} > PATCHRAM

/*	_sidata = LOADADDR(.data);*/

	/*.bss (NOLOAD) :*/
  . = ALIGN(8);
  .bss :
	{
    _endprog = ABSOLUTE(.);         /* define a global symbol at program end */
    /* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;
    __bss_start__ = _sbss;

		/*PROVIDE(__bss_start__ = _sbss);*/
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(8);
		_ebss = .;

		/*PROVIDE(__bss_end__ = _ebss);*/
    __bss_end__ = _ebss;
	} > PATCHRAM


  .pv (NOLOAD) :
  {
    . = ALIGN(8);
    _programvector = ABSOLUTE(.);
      *(.pv)
    . = ALIGN(8);
    PROVIDE ( _eprogram = . );
    PROVIDE ( _stack = . );
  } >PATCHRAM
  _eram = ORIGIN(PATCHRAM) + LENGTH(PATCHRAM);
  _estack = ORIGIN(PATCHRAM) + LENGTH(PATCHRAM);


  	/* Example: int foo __attribute__ ((section (".extdata"))); */
  	.extdata (NOLOAD) :
  	{
    	. = ALIGN(8);
	    *(.extdata)
    	PROVIDE ( _heap = . );
    } >EXTRAM
    _eheap = ORIGIN(EXTRAM) + LENGTH(EXTRAM);

    DISCARD :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}
