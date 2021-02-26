/* Linker script for STM32F407 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size  = 0x000; /* required amount of heap  */
_Min_Stack_Size = 0x800; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  BOOTLOADER (rw)  : ORIGIN = 0x08000000, LENGTH = 16K    /* Sector 0 */
  EEPROM (rw)      : ORIGIN = 0x08004000, LENGTH = 16K    /* Sector 1 */
  FIRMWARE (rx)    : ORIGIN = 0x08008000, LENGTH = 224K   /* Sector 2 */
  PATCHFLASH (rx)  : ORIGIN = 0x08040000, LENGTH = 128K   /* Sector 6 */ /* total Flash memory is 1Mb */
  CCMHEAP (rw)     : ORIGIN = 0x10004000, LENGTH = 32K
  CCMRAM (rw)      : ORIGIN = 0x1000c000, LENGTH = 16K    /* total CCM is 64kb */
  FIRMWARERAM (rwx): ORIGIN = 0x20000000, LENGTH = 48K
  PATCHRAM (rwx)   : ORIGIN = 0x2000c000, LENGTH = 144K   /* total RAM is 192kb */
  NVRAM (rw)       : ORIGIN = 0x40024000, LENGTH = 4K
  EXTRAM (rx)      : ORIGIN = 0x68000000, LENGTH = 1M
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .program_header :
  {
    . = ALIGN(8);
    _startprog = ABSOLUTE(.);         /* define a global symbol at program start */
    KEEP(*(.program_header)) /* Startup code */
    . = ALIGN(8);
  } >PATCHRAM /* AT>PATCHFLASH */

  /* The program code and other data goes into FLASH */
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
  } >PATCHRAM /* AT>PATCHFLASH */

  _text_load = LOADADDR (.text);
  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >PATCHRAM /* AT>PATCHFLASH */
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >PATCHRAM /* AT>PATCHFLASH */

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >PATCHRAM /* AT>PATCHFLASH */
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >PATCHRAM /* AT>PATCHFLASH */
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >PATCHRAM /* AT>PATCHFLASH */

  /* used by the startup to initialize data */
  _sidata = .;

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : AT ( _sidata )
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >PATCHRAM

  /* Uninitialized data section */
  . = ALIGN(8);
  .bss :
  {
    _endprog = ABSOLUTE(.);         /* define a global symbol at program end */
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >PATCHRAM

  .pv (NOLOAD) :
  {
    . = ALIGN(8);
    _programvector = ABSOLUTE(.);
      *(.pv)
    . = ALIGN(8);
    PROVIDE ( _eprogram = . );
    PROVIDE ( _stack = . );
  } >PATCHRAM
  _eram = ORIGIN(PATCHRAM) + 80k;    /* Limit to 80k for OwlWare compatiblity */
  _estack = ORIGIN(PATCHRAM) + 80k;

  .fastheap (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE ( _fastheap = . );
    . = ALIGN(8);
  } >CCMHEAP
  _fasteheap = ORIGIN(CCMHEAP) + LENGTH(CCMHEAP);

  /* CCM section, vars must be located here explicitly */
  /* Example: int foo __attribute__ ((section (".ccmdata"))); */
  .ccmdata (NOLOAD) :
  {
    . = ALIGN(8);
    *(.ccmdata)
    . = ALIGN(8);
  } >CCMRAM

  /* /\* User_stack section, used to check that there is enough RAM left *\/ */
  /* ._user_stack (NOLOAD) : */
  /* { */
  /*   . = ALIGN(8); */
  /*   PROVIDE ( _stack = . ); */
  /*   . = . + _Min_Stack_Size; */
  /*   . = ALIGN(8); */
  /* } >CCMRAM */
  /* _estack = ORIGIN(CCMRAM) + LENGTH(CCMRAM); */

  /* External SRAM, used for heap memory */
  .extdata (NOLOAD) :
  {
    . = ALIGN(8);
    *(.extdata)
    . = ALIGN(8);
    PROVIDE ( _heap = . );
    . = . + _Min_Heap_Size;
  } >EXTRAM
  _eheap = ORIGIN(EXTRAM) + LENGTH(EXTRAM);


  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
