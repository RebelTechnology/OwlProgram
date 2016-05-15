  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global  g_pfnVectors
;; .global  ProgramVector
;; .global  vector


/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */  
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

.section  .text.Reset_Handler
.weak  Reset_Handler
.type  Reset_Handler, %function
Reset_Handler:  
/* Copy the data segment initializers from flash to SRAM */  
  movs  r1, #0
  b  LoopCopyDataInit
CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4    
LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit

/* Zero fill the bss segment. */  
  ldr  r2, =_sbss
  b  LoopFillZerobss
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4    
LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call static constructors */
  bl __libc_init_array
	/*
	ldr r12,=__libc_init_array
	mov lr,pc
	bx r12
	*/
/* Call the application's entry point.*/
  bl  main
	/*
	lrd r12,=main
	mov lr,pc
	bx r12
	*/
  bx  lr    
.size  Reset_Handler, .-Reset_Handler

.section  .program_header,"a",%progbits
.type  g_pfnVectors, %object
.size  g_pfnVectors, .-g_pfnVectors    

g_pfnVectors:
  .word 0XDADAC0DE       /* magic */
  .word _startprog 	 /* link base address */
  .word _endprog         /* end of program */
  .word Reset_Handler    /* code entry point */
  .word _stack           /* stack start */
  .word _estack          /* stack end */
  .word _programvector
  .include "progname.s"
