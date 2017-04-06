/*******************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
* CYPRESS PROVIDES THIS SOFTWARE "AS IS" AND MAKES NO WARRANTY
* OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS SOFTWARE,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
* PURPOSE.
*******************************************************************************/

/*
// <h> Stack Configuration
//   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
// </h>
*/

	.equ    Stack_Size, 0x00000400
    .section ".stack", "w"
    .align  3
    .globl  __FM_stack_mem
    .globl  __FM_stack_size
__FM_stack_mem:
    .if     Stack_Size
    .space  Stack_Size
    .endif
    .size   __FM_stack_mem,  . - __FM_stack_mem
    .set    __FM_stack_size, . - __FM_stack_mem


/*
// <h> Heap Configuration
//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
// </h>
*/

    .equ    Heap_Size,  0x00000400
    
    .section ".heap", "w"
    .align  3
    .globl  __FM_heap_start
    .globl  __FM_heap_end
__FM_heap_start:
    .if     Heap_Size
    .space  Heap_Size
    .endif
__FM_heap_end:


/* Vector Table */

    .section ".FM.interrupt_vector"
    .globl  __FM_interrupt_vector_cortex_m
    .type   __FM_interrupt_vector_cortex_m, %object

__FM_interrupt_vector_cortex_m:
    .long   __FM_stack                 /* Top of Stack                 */
    .long   __FM_reset                 /* Reset Handler                */
    .long   NMI_Handler                 /* NMI Handler                  */
    .long   HardFault_Handler           /* Hard Fault Handler           */
    .long   MemManage_Handler           /* MPU Fault Handler            */
    .long   BusFault_Handler            /* Bus Fault Handler            */
    .long   UsageFault_Handler          /* Usage Fault Handler          */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   SVC_Handler                 /* SVCall Handler               */
    .long   DebugMon_Handler            /* Debug Monitor Handler        */
    .long   0                           /* Reserved                     */
    .long   PendSV_Handler              /* PendSV Handler               */
    .long   SysTick_Handler             /* SysTick Handler              */

    /* External Interrupts */

    .long   CSV_IRQHandler		/* IRQ #0 */
    .long   SWDT_IRQHandler		/* IRQ #1 */
    .long   LVD_IRQHandler		/* IRQ #2 */
    .long   IRQ003SEL_IRQHandler		/* IRQ #3 */
    .long   IRQ004SEL_IRQHandler		/* IRQ #4 */
    .long   IRQ005SEL_IRQHandler		/* IRQ #5 */
    .long   IRQ006SEL_IRQHandler		/* IRQ #6 */
    .long   IRQ007SEL_IRQHandler		/* IRQ #7 */
    .long   IRQ008SEL_IRQHandler		/* IRQ #8 */
    .long   IRQ009SEL_IRQHandler		/* IRQ #9 */
    .long   IRQ010SEL_IRQHandler		/* IRQ #10 */
    .long   EXINT0_IRQHandler		/* IRQ #11 */
    .long   EXINT1_IRQHandler		/* IRQ #12 */
    .long   EXINT2_IRQHandler		/* IRQ #13 */
    .long   EXINT3_IRQHandler		/* IRQ #14 */
    .long   EXINT4_IRQHandler		/* IRQ #15 */
    .long   EXINT5_IRQHandler		/* IRQ #16 */
    .long   EXINT6_IRQHandler		/* IRQ #17 */
    .long   EXINT7_IRQHandler		/* IRQ #18 */
    .long   QPRC0_IRQHandler		/* IRQ #19 */
    .long   QPRC1_IRQHandler		/* IRQ #20 */
    .long   MFT0_WFG_DTIF_IRQHandler		/* IRQ #21 */
    .long   MFT1_WFG_DTIF_IRQHandler		/* IRQ #22 */
    .long   MFT2_WFG_DTIF_IRQHandler		/* IRQ #23 */
    .long   MFT0_FRT_PEAK_IRQHandler		/* IRQ #24 */
    .long   MFT0_FRT_ZERO_IRQHandler		/* IRQ #25 */
    .long   MFT0_ICU_IRQHandler		/* IRQ #26 */
    .long   MFT0_OCU_IRQHandler		/* IRQ #27 */
    .long   MFT1_FRT_PEAK_IRQHandler		/* IRQ #28 */
    .long   MFT1_FRT_ZERO_IRQHandler		/* IRQ #29 */
    .long   MFT1_ICU_IRQHandler		/* IRQ #30 */
    .long   MFT1_OCU_IRQHandler		/* IRQ #31 */
    .long   MFT2_FRT_PEAK_IRQHandler		/* IRQ #32 */
    .long   MFT2_FRT_ZERO_IRQHandler		/* IRQ #33 */
    .long   MFT2_ICU_IRQHandler		/* IRQ #34 */
    .long   MFT2_OCU_IRQHandler		/* IRQ #35 */
    .long   PPG00_02_04_IRQHandler		/* IRQ #36 */
    .long   PPG08_10_12_IRQHandler		/* IRQ #37 */
    .long   PPG16_18_20_IRQHandler		/* IRQ #38 */
    .long   BT0_IRQHandler		/* IRQ #39 */
    .long   BT1_IRQHandler		/* IRQ #40 */
    .long   BT2_IRQHandler		/* IRQ #41 */
    .long   BT3_IRQHandler		/* IRQ #42 */
    .long   BT4_IRQHandler		/* IRQ #43 */
    .long   BT5_IRQHandler		/* IRQ #44 */
    .long   BT6_IRQHandler		/* IRQ #45 */
    .long   BT7_IRQHandler		/* IRQ #46 */
    .long   DT_IRQHandler		/* IRQ #47 */
    .long   WC_IRQHandler		/* IRQ #48 */
    .long   EXTBUS_ERR_IRQHandler		/* IRQ #49 */
    .long   RTC_IRQHandler		/* IRQ #50 */
    .long   EXINT8_IRQHandler		/* IRQ #51 */
    .long   EXINT9_IRQHandler		/* IRQ #52 */
    .long   EXINT10_IRQHandler		/* IRQ #53 */
    .long   EXINT11_IRQHandler		/* IRQ #54 */
    .long   EXINT12_IRQHandler		/* IRQ #55 */
    .long   EXINT13_IRQHandler		/* IRQ #56 */
    .long   EXINT14_IRQHandler		/* IRQ #57 */
    .long   EXINT15_IRQHandler		/* IRQ #58 */
    .long   TIM_IRQHandler		/* IRQ #59 */
    .long   MFS0_RX_IRQHandler		/* IRQ #60 */
    .long   MFS0_TX_IRQHandler		/* IRQ #61 */
    .long   MFS1_RX_IRQHandler		/* IRQ #62 */
    .long   MFS1_TX_IRQHandler		/* IRQ #63 */
    .long   MFS2_RX_IRQHandler		/* IRQ #64 */
    .long   MFS2_TX_IRQHandler		/* IRQ #65 */
    .long   MFS3_RX_IRQHandler		/* IRQ #66 */
    .long   MFS3_TX_IRQHandler		/* IRQ #67 */
    .long   MFS4_RX_IRQHandler		/* IRQ #68 */
    .long   MFS4_TX_IRQHandler		/* IRQ #69 */
    .long   MFS5_RX_IRQHandler		/* IRQ #70 */
    .long   MFS5_TX_IRQHandler		/* IRQ #71 */
    .long   MFS6_RX_IRQHandler		/* IRQ #72 */
    .long   MFS6_TX_IRQHandler		/* IRQ #73 */
    .long   MFS7_RX_IRQHandler		/* IRQ #74 */
    .long   MFS7_TX_IRQHandler		/* IRQ #75 */
    .long   ADC0_IRQHandler		/* IRQ #76 */
    .long   ADC1_IRQHandler		/* IRQ #77 */
    .long   USB0_F_IRQHandler		/* IRQ #78 */
    .long   USB0_H_F_IRQHandler		/* IRQ #79 */
    .long   CAN0_IRQHandler		/* IRQ #80 */
    .long   CAN1_CANFD0_IRQHandler		/* IRQ #81 */
    .long   ETHER0_IRQHandler		/* IRQ #82 */
    .long   DMAC0_IRQHandler		/* IRQ #83 */
    .long   DMAC1_IRQHandler		/* IRQ #84 */
    .long   DMAC2_IRQHandler		/* IRQ #85 */
    .long   DMAC3_IRQHandler		/* IRQ #86 */
    .long   DMAC4_IRQHandler		/* IRQ #87 */
    .long   DMAC5_IRQHandler		/* IRQ #88 */
    .long   DMAC6_IRQHandler		/* IRQ #89 */
    .long   DMAC7_IRQHandler		/* IRQ #90 */
    .long   DSTC_IRQHandler		/* IRQ #91 */
    .long   EXINT16_19_IRQHandler		/* IRQ #92 */
    .long   EXINT20_23_IRQHandler		/* IRQ #93 */
    .long   EXINT24_27_IRQHandler		/* IRQ #94 */
    .long   EXINT28_31_IRQHandler		/* IRQ #95 */
    .long   QPRC2_IRQHandler		/* IRQ #96 */
    .long   QPRC3_IRQHandler		/* IRQ #97 */
    .long   BT8_IRQHandler		/* IRQ #98 */
    .long   BT9_IRQHandler		/* IRQ #99 */
    .long   BT10_IRQHandler		/* IRQ #100 */
    .long   BT11_IRQHandler		/* IRQ #101 */
    .long   BT12_15_IRQHandler		/* IRQ #102 */
    .long   MFS8_RX_IRQHandler		/* IRQ #103 */
    .long   MFS8_TX_IRQHandler		/* IRQ #104 */
    .long   MFS9_RX_IRQHandler		/* IRQ #105 */
    .long   MFS9_TX_IRQHandler		/* IRQ #106 */
    .long   MFS10_RX_IRQHandler		/* IRQ #107 */
    .long   MFS10_TX_IRQHandler		/* IRQ #108 */
    .long   MFS11_RX_IRQHandler		/* IRQ #109 */
    .long   MFS11_TX_IRQHandler		/* IRQ #110 */
    .long   ADC2_IRQHandler		/* IRQ #111 */
    .long   DSTC_HW_IRQHandler		/* IRQ #112 */
    .long   USB1_F_IRQHandler		/* IRQ #113 */
    .long   USB1_H_F_IRQHandler		/* IRQ #114 */
    .long   HSSPI_IRQHandler		/* IRQ #115 */
    .long   Default_Handler		/* IRQ #116 */
    .long   PCRC_I2S0_1_IRQHandler		/* IRQ #117 */
    .long   SD_IRQHandler		/* IRQ #118 */
    .long   FLASHIF_IRQHandler		/* IRQ #119 */
    .long   MFS12_RX_IRQHandler		/* IRQ #120 */
    .long   MFS12_TX_IRQHandler		/* IRQ #121 */
    .long   MFS13_RX_IRQHandler		/* IRQ #122 */
    .long   MFS13_TX_IRQHandler		/* IRQ #123 */
    .long   MFS14_RX_IRQHandler		/* IRQ #124 */
    .long   MFS14_TX_IRQHandler		/* IRQ #125 */
    .long   MFS15_RX_IRQHandler		/* IRQ #126 */
    .long   MFS15_TX_IRQHandler		/* IRQ #127 */


    .size   __FM_interrupt_vector_cortex_m, . - __FM_interrupt_vector_cortex_m


    .thumb


/* Reset Handler */

    .section .FM.reset,"x",%progbits
    .thumb_func
    .globl  __FM_reset_cortex_m
    .type   __FM_reset_cortex_m, %function
__FM_reset_cortex_m:
    .fnstart
.ifdef Debug_RAM
/* this routine is used for .bss sections initialization by RAM debugging "Debug_RAM = 1" */
/* Clear .bss section (Zero init) */
	MOV     R0, #0
	LDR     R1, =_sbss
	LDR     R2, =_ebss
	CMP     R1,R2
	BEQ     start_main
Loop_bss:
	CMP     R1, R2
	BHS		start_main
	STR   	R0, [R1]
	ADD		R1, #4
	BLO     Loop_bss

.else
/* this routine is used for .data and .bss sections initialization by ROM debugging "Debug_RAM = 0" */
init_data:
	LDR		R0, =_sdata
	LDR		R1, =_etext
	LDR		R2, =data_size
	MOV		R3, #0
loop_init_data:
	CMP		R3, R2
	BCS		zero_bss
    LDR		R4,[R1,#0]
	STR		R4,[R0,#0]
	ADD		R3, #4
	ADD		R1, #4
	ADD		R0, #4
	B		loop_init_data

/* Clear .bss section (Zero init) */

zero_bss:
	LDR		R0, =_sbss
	LDR		R2, =bss_size
	MOV		R3, #0
	MOV		R4, #0
loop_zero_bss:
	CMP		R3, R2
	BCS		start_main
	STR		R4,[R0,#0]
	ADD		R3, #4
	ADD		R0, #4
	B		loop_zero_bss
.endif
start_main:

  /*enable fpu begin*/
  /*; enable cp10,cp11 */
  
  
  ldr     r0, =0xe000ed88           
  ldr     r1,[r0]
  ldr     r2, =0xf00000
  orr     r1,r1,r2
  str     r1,[r0]
  
  
  /*enable fpu end*/ 
  
	LDR     R0, =SystemInit
    BLX     R0
    LDR     R0,=main
    BX      R0

    .pool
    .cantunwind
    .fnend
    .size   __FM_reset_cortex_m,.-__FM_reset_cortex_m

    .section ".text"

/* Exception Handlers */

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    B       .
    .size   NMI_Handler, . - NMI_Handler

    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    B       .
    .size   HardFault_Handler, . - HardFault_Handler

    .weak   MemManage_Handler
    .type   MemManage_Handler, %function
MemManage_Handler:
    B       .
    .size   MemManage_Handler, . - MemManage_Handler

    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    B       .
    .size   BusFault_Handler, . - BusFault_Handler

    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    B       .
    .size   UsageFault_Handler, . - UsageFault_Handler

    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    B       .
    .size   SVC_Handler, . - SVC_Handler

    .weak   DebugMon_Handler
    .type   DebugMon_Handler, %function
DebugMon_Handler:
    B       .
    .size   DebugMon_Handler, . - DebugMon_Handler

    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    B       .
    .size   PendSV_Handler, . - PendSV_Handler

    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    B       .
    .size   SysTick_Handler, . - SysTick_Handler


/* IRQ Handlers */

    .globl  Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    B       .
    .size   Default_Handler, . - Default_Handler

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

	IRQ     CSV_IRQHandler		/* IRQ #0 */
	IRQ     SWDT_IRQHandler		/* IRQ #1 */
	IRQ     LVD_IRQHandler		/* IRQ #2 */
	IRQ     IRQ003SEL_IRQHandler		/* IRQ #3 */
	IRQ     IRQ004SEL_IRQHandler		/* IRQ #4 */
	IRQ     IRQ005SEL_IRQHandler		/* IRQ #5 */
	IRQ     IRQ006SEL_IRQHandler		/* IRQ #6 */
	IRQ     IRQ007SEL_IRQHandler		/* IRQ #7 */
	IRQ     IRQ008SEL_IRQHandler		/* IRQ #8 */
	IRQ     IRQ009SEL_IRQHandler		/* IRQ #9 */
	IRQ     IRQ010SEL_IRQHandler		/* IRQ #10 */
	IRQ     EXINT0_IRQHandler		/* IRQ #11 */
	IRQ     EXINT1_IRQHandler		/* IRQ #12 */
	IRQ     EXINT2_IRQHandler		/* IRQ #13 */
	IRQ     EXINT3_IRQHandler		/* IRQ #14 */
	IRQ     EXINT4_IRQHandler		/* IRQ #15 */
	IRQ     EXINT5_IRQHandler		/* IRQ #16 */
	IRQ     EXINT6_IRQHandler		/* IRQ #17 */
	IRQ     EXINT7_IRQHandler		/* IRQ #18 */
	IRQ     QPRC0_IRQHandler		/* IRQ #19 */
	IRQ     QPRC1_IRQHandler		/* IRQ #20 */
	IRQ     MFT0_WFG_DTIF_IRQHandler		/* IRQ #21 */
	IRQ     MFT1_WFG_DTIF_IRQHandler		/* IRQ #22 */
	IRQ     MFT2_WFG_DTIF_IRQHandler		/* IRQ #23 */
	IRQ     MFT0_FRT_PEAK_IRQHandler		/* IRQ #24 */
	IRQ     MFT0_FRT_ZERO_IRQHandler		/* IRQ #25 */
	IRQ     MFT0_ICU_IRQHandler		/* IRQ #26 */
	IRQ     MFT0_OCU_IRQHandler		/* IRQ #27 */
	IRQ     MFT1_FRT_PEAK_IRQHandler		/* IRQ #28 */
	IRQ     MFT1_FRT_ZERO_IRQHandler		/* IRQ #29 */
	IRQ     MFT1_ICU_IRQHandler		/* IRQ #30 */
	IRQ     MFT1_OCU_IRQHandler		/* IRQ #31 */
	IRQ     MFT2_FRT_PEAK_IRQHandler		/* IRQ #32 */
	IRQ     MFT2_FRT_ZERO_IRQHandler		/* IRQ #33 */
	IRQ     MFT2_ICU_IRQHandler		/* IRQ #34 */
	IRQ     MFT2_OCU_IRQHandler		/* IRQ #35 */
	IRQ     PPG00_02_04_IRQHandler		/* IRQ #36 */
	IRQ     PPG08_10_12_IRQHandler		/* IRQ #37 */
	IRQ     PPG16_18_20_IRQHandler		/* IRQ #38 */
	IRQ     BT0_IRQHandler		/* IRQ #39 */
	IRQ     BT1_IRQHandler		/* IRQ #40 */
	IRQ     BT2_IRQHandler		/* IRQ #41 */
	IRQ     BT3_IRQHandler		/* IRQ #42 */
	IRQ     BT4_IRQHandler		/* IRQ #43 */
	IRQ     BT5_IRQHandler		/* IRQ #44 */
	IRQ     BT6_IRQHandler		/* IRQ #45 */
	IRQ     BT7_IRQHandler		/* IRQ #46 */
	IRQ     DT_IRQHandler		/* IRQ #47 */
	IRQ     WC_IRQHandler		/* IRQ #48 */
	IRQ     EXTBUS_ERR_IRQHandler		/* IRQ #49 */
	IRQ     RTC_IRQHandler		/* IRQ #50 */
	IRQ     EXINT8_IRQHandler		/* IRQ #51 */
	IRQ     EXINT9_IRQHandler		/* IRQ #52 */
	IRQ     EXINT10_IRQHandler		/* IRQ #53 */
	IRQ     EXINT11_IRQHandler		/* IRQ #54 */
	IRQ     EXINT12_IRQHandler		/* IRQ #55 */
	IRQ     EXINT13_IRQHandler		/* IRQ #56 */
	IRQ     EXINT14_IRQHandler		/* IRQ #57 */
	IRQ     EXINT15_IRQHandler		/* IRQ #58 */
	IRQ     TIM_IRQHandler		/* IRQ #59 */
	IRQ     MFS0_RX_IRQHandler		/* IRQ #60 */
	IRQ     MFS0_TX_IRQHandler		/* IRQ #61 */
	IRQ     MFS1_RX_IRQHandler		/* IRQ #62 */
	IRQ     MFS1_TX_IRQHandler		/* IRQ #63 */
	IRQ     MFS2_RX_IRQHandler		/* IRQ #64 */
	IRQ     MFS2_TX_IRQHandler		/* IRQ #65 */
	IRQ     MFS3_RX_IRQHandler		/* IRQ #66 */
	IRQ     MFS3_TX_IRQHandler		/* IRQ #67 */
	IRQ     MFS4_RX_IRQHandler		/* IRQ #68 */
	IRQ     MFS4_TX_IRQHandler		/* IRQ #69 */
	IRQ     MFS5_RX_IRQHandler		/* IRQ #70 */
	IRQ     MFS5_TX_IRQHandler		/* IRQ #71 */
	IRQ     MFS6_RX_IRQHandler		/* IRQ #72 */
	IRQ     MFS6_TX_IRQHandler		/* IRQ #73 */
	IRQ     MFS7_RX_IRQHandler		/* IRQ #74 */
	IRQ     MFS7_TX_IRQHandler		/* IRQ #75 */
	IRQ     ADC0_IRQHandler		/* IRQ #76 */
	IRQ     ADC1_IRQHandler		/* IRQ #77 */
	IRQ     USB0_F_IRQHandler		/* IRQ #78 */
	IRQ     USB0_H_F_IRQHandler		/* IRQ #79 */
	IRQ     CAN0_IRQHandler		/* IRQ #80 */
	IRQ     CAN1_CANFD0_IRQHandler		/* IRQ #81 */
	IRQ     ETHER0_IRQHandler		/* IRQ #82 */
	IRQ     DMAC0_IRQHandler		/* IRQ #83 */
	IRQ     DMAC1_IRQHandler		/* IRQ #84 */
	IRQ     DMAC2_IRQHandler		/* IRQ #85 */
	IRQ     DMAC3_IRQHandler		/* IRQ #86 */
	IRQ     DMAC4_IRQHandler		/* IRQ #87 */
	IRQ     DMAC5_IRQHandler		/* IRQ #88 */
	IRQ     DMAC6_IRQHandler		/* IRQ #89 */
	IRQ     DMAC7_IRQHandler		/* IRQ #90 */
	IRQ     DSTC_IRQHandler		/* IRQ #91 */
	IRQ     EXINT16_19_IRQHandler		/* IRQ #92 */
	IRQ     EXINT20_23_IRQHandler		/* IRQ #93 */
	IRQ     EXINT24_27_IRQHandler		/* IRQ #94 */
	IRQ     EXINT28_31_IRQHandler		/* IRQ #95 */
	IRQ     QPRC2_IRQHandler		/* IRQ #96 */
	IRQ     QPRC3_IRQHandler		/* IRQ #97 */
	IRQ     BT8_IRQHandler		/* IRQ #98 */
	IRQ     BT9_IRQHandler		/* IRQ #99 */
	IRQ     BT10_IRQHandler		/* IRQ #100 */
	IRQ     BT11_IRQHandler		/* IRQ #101 */
	IRQ     BT12_15_IRQHandler		/* IRQ #102 */
	IRQ     MFS8_RX_IRQHandler		/* IRQ #103 */
	IRQ     MFS8_TX_IRQHandler		/* IRQ #104 */
	IRQ     MFS9_RX_IRQHandler		/* IRQ #105 */
	IRQ     MFS9_TX_IRQHandler		/* IRQ #106 */
	IRQ     MFS10_RX_IRQHandler		/* IRQ #107 */
	IRQ     MFS10_TX_IRQHandler		/* IRQ #108 */
	IRQ     MFS11_RX_IRQHandler		/* IRQ #109 */
	IRQ     MFS11_TX_IRQHandler		/* IRQ #110 */
	IRQ     ADC2_IRQHandler		/* IRQ #111 */
	IRQ     DSTC_HW_IRQHandler		/* IRQ #112 */
	IRQ     USB1_F_IRQHandler		/* IRQ #113 */
	IRQ     USB1_H_F_IRQHandler		/* IRQ #114 */
	IRQ     HSSPI_IRQHandler		/* IRQ #115 */
	IRQ     PCRC_I2S0_1_IRQHandler		/* IRQ #117 */
	IRQ     SD_IRQHandler		/* IRQ #118 */
	IRQ     FLASHIF_IRQHandler		/* IRQ #119 */
	IRQ     MFS12_RX_IRQHandler		/* IRQ #120 */
	IRQ     MFS12_TX_IRQHandler		/* IRQ #121 */
	IRQ     MFS13_RX_IRQHandler		/* IRQ #122 */
	IRQ     MFS13_TX_IRQHandler		/* IRQ #123 */
	IRQ     MFS14_RX_IRQHandler		/* IRQ #124 */
	IRQ     MFS14_TX_IRQHandler		/* IRQ #125 */
	IRQ     MFS15_RX_IRQHandler		/* IRQ #126 */
	IRQ     MFS15_TX_IRQHandler		/* IRQ #127 */


    .end
