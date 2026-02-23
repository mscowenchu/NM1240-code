;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2018 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/



Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
    IF :LNOT: :DEF: Heap_Size
Heap_Size       EQU     0x00000000
    ENDIF
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     __initial_sp            ; Top of Stack
                DCD     Reset_Handler           ; Reset Handler
                DCD     NMI_Handler             ; NMI Handler
                DCD     HardFault_Handler       ; Hard Fault Handler
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     SVC_Handler             ; SVCall Handler
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     PendSV_Handler          ; PendSV Handler
                DCD     SysTick_Handler         ; SysTick Handler

                ; External Interrupts
                ; maximum of 32 External Interrupts are possible
                DCD     BOD_IRQHandler          ;  0: Brown-Out low voltage detected interrupt
                DCD     WDT_IRQHandler          ;  1: Watchdog Timer interrupt
                DCD     Default_Handler         ;  2: Reserved
                DCD     USCI1_IRQHandler        ;  3: USCI1 interrupt
                DCD     GPIO_IRQHandler         ;  4: External interrupt from GPA ~ GPF pins
                DCD     EPWM_IRQHandler         ;  5: EPWM interrupt
                DCD     BRAKE0_IRQHandler       ;  6: EPWM brake interrupt from PWM0 or PWM_BRAKE pin
                DCD     BRAKE1_IRQHandler       ;  7: EPWM brake interrupt from PWM1
                DCD     BPWM0_IRQHandler        ;  8: BPWM0 interrupt
                DCD     BPWM1_IRQHandler        ;  9: BPWM1 interrupt
                DCD     Default_Handler         ; 10: Reserved
                DCD     USCI2_IRQHandler        ; 11: USCI2 interrupt
                DCD     Default_Handler         ; 12: Reserved
                DCD     Default_Handler         ; 13: Reserved
                DCD     Default_Handler         ; 14: Reserved
                DCD     ECAP_IRQHandler         ; 15: Enhanced Input Capture interrupt
                DCD     CCAP_IRQHandler         ; 16: Continues Input Capture interrupt
                DCD     Default_Handler         ; 17: Reserved
                DCD     Default_Handler         ; 18: Reserved
				DCD     GDMA0_IRQHandler        ; 19: GDMA_CH0 interrupt                                         
				DCD     GDMA1_IRQHandler        ; 20: GDMA_CH1 interrupt 
                DCD     HIRCTRIM_IRQHandler     ; 21: HIRC TRIM interrupt
                DCD     TMR0_IRQHandler         ; 22: Timer 0 interrupt
                DCD     TMR1_IRQHandler         ; 23: Timer 1 interrupt
                DCD     TMR2_IRQHandler         ; 24: Timer 2 interrupt
                DCD     Default_Handler         ; 25: Reserved
                DCD     ACMP_IRQHandler         ; 26: Analog Comparator 0 or Comparator 1 interrupt
                DCD     Default_Handler         ; 27: Reserved
                DCD     PWRWU_IRQHandler        ; 28: Chip wake-up from Power-down state interrupt
                DCD     ADC0_IRQHandler         ; 29: ADC0 interrupt
                DCD     ADC1_IRQHandler         ; 30: ADC1 interrupt
                DCD     Default_Handler         ; 31: Reserved

                AREA    |.text|, CODE, READONLY

; Reset Handler

                ENTRY

Reset_Handler   PROC
                EXPORT  Reset_Handler           [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                LDR     R0, =0x50000100
                ; Unlock Register
                LDR     R1, =0x59
                STR     R1, [R0]
                LDR     R1, =0x16
                STR     R1, [R0]
                LDR     R1, =0x88
                STR     R1, [R0]

                ; Init POR
                LDR     R2, =0x50000024
                LDR     R1, =0x00005AA5
                STR     R1, [R2]

                ; Lock register
                MOVS    R1, #0
                STR     R1, [R0]

                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler             [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler       [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler             [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler          [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler         [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  BOD_IRQHandler          [WEAK]
                EXPORT  WDT_IRQHandler          [WEAK]
                EXPORT  USCI1_IRQHandler        [WEAK]
                EXPORT  GPIO_IRQHandler         [WEAK]
                EXPORT  EPWM_IRQHandler         [WEAK]
                EXPORT  BRAKE0_IRQHandler       [WEAK]
                EXPORT  BRAKE1_IRQHandler       [WEAK]
                EXPORT  BPWM0_IRQHandler        [WEAK]
                EXPORT  BPWM1_IRQHandler        [WEAK]
                EXPORT  USCI2_IRQHandler        [WEAK]
                EXPORT  ECAP_IRQHandler         [WEAK]
                EXPORT  CCAP_IRQHandler         [WEAK]
                EXPORT  GDMA0_IRQHandler        [WEAK]
                EXPORT  GDMA1_IRQHandler        [WEAK]
                EXPORT  HIRCTRIM_IRQHandler     [WEAK]
                EXPORT  TMR0_IRQHandler         [WEAK]
                EXPORT  TMR1_IRQHandler         [WEAK]
                EXPORT  ACMP_IRQHandler         [WEAK]
                EXPORT  PWRWU_IRQHandler        [WEAK]
                EXPORT  ADC0_IRQHandler         [WEAK]
                EXPORT  ADC1_IRQHandler         [WEAK]



BOD_IRQHandler
WDT_IRQHandler
USCI1_IRQHandler
GPIO_IRQHandler
EPWM_IRQHandler
BRAKE0_IRQHandler
BRAKE1_IRQHandler
BPWM0_IRQHandler
BPWM1_IRQHandler
USCI2_IRQHandler
ECAP_IRQHandler
CCAP_IRQHandler
GDMA0_IRQHandler
GDMA1_IRQHandler
HIRCTRIM_IRQHandler
TMR0_IRQHandler
TMR1_IRQHandler
TMR2_IRQHandler
ACMP_IRQHandler
PWRWU_IRQHandler
ADC0_IRQHandler
ADC1_IRQHandler

                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

                END
