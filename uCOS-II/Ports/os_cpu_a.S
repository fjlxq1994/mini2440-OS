/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
* File : OS_CPU_A.ASM
* By   : Jean J. Labrosse
*********************************************************************************************************

*********************************************************************************************************
*                                               ARM Port
*
*                 Target           : ARM (Includes ARM7, ARM9)
*                 Ported by        : Michael Anburaj
*                 URL              : http:@geocities.com/michaelanburaj/    Email : michaelanburaj@hotmail.com
*
*********************************************************************************************************
*/


.equ	rSRCPND,  0x4a000000    @ Source pending
.equ	rINTPND,  0x4a000010    @ Interrupt request status

.equ	rEINTPEND,  0x560000a8
.equ	rINTOFFSET,  0x4a000014

@ Pre-defined constants
.equ USERMODE,    0x10
.equ FIQMODE,     0x11
.equ IRQMODE,     0x12
.equ SVCMODE,     0x13
.equ ABORTMODE,   0x17
.equ UNDEFMODE,   0x1b
.equ MODEMASK,    0x1f
.equ NOINT,       0xc0
.equ TBIT,        0x20


@*********************************************************************************************************
@                                          START MULTITASKING
@                                       void OSStartHighRdy(void)
@
@ Note : OSStartHighRdy() MUST:
@           a) Call OSTaskSwHook() then,
@           b) Set OSRunning to TRUE,
@           c) Switch to the highest priority task.
@*********************************************************************************************************

        .global  OSStartHighRdy

OSStartHighRdy: 

        bl OSTaskSwHook             @ Call user defined task switch hook

        ldr r4,=OSRunning           @ Indicate that multitasking has started
        mov r5,#1
        strb r5,[r4]

        ldr r4,=OSTCBHighRdy        @ Get highest priority task TCB address

        ldr r4,[r4]                 @ get stack pointer
        ldr sp,[r4]                 @ switch to the new stack

        ldmfd sp!,{r4}              @ pop new task's spsr
        ldmfd sp!,{r4}              @ pop new task's psr
        msr SPSR_cxsf,r4
        ldmfd sp!,{r0-r12,lr,pc}^   @ pop new task's r0-r12,lr & pc

@*********************************************************************************************************
@                                PERFORM A CONTEXT SWITCH (From task level)
@                                           void OSCtxSw(void)
@
@ Note(s):    Upon entry, 
@             OSTCBCur     points to the OS_TCB of the task to suspend
@             OSTCBHighRdy points to the OS_TCB of the task to resume
@
@*********************************************************************************************************

        .global  OSCtxSw

OSCtxSw:
@ Special optimised code below:
        stmfd sp!,{lr}              @ push pc (lr should be pushed in place of PC)
        ldr lr,=0xdead1eaf          @ Just a dummy value is pushed for lr (for actual lr is safe inside callers stack)
        stmfd sp!,{r0-r12,lr}       @ push lr & register file
        mrs r4,cpsr

.ifdef THUMB_TASKS
        orr r4,r4,#TBIT
.endif
        stmfd sp!,{r4}              @ push current psr
        mrs r4,spsr
        stmfd sp!,{r4}              @ push current spsr

        @ OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy
        ldrb r6,[r5]
        strb r6,[r4]
        
        @ Get current task TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]
        str sp,[r5]                 @ store sp in preempted tasks's TCB

        bl OSTaskSwHook             @ call Task Switch Hook

        @ Get highest priority task TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr sp,[r6]                 @ get new task's stack pointer

        @ OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 @ set new current task TCB address

        ldmfd sp!,{r4}              @ pop new task's spsr
        ldmfd sp!,{r4}              @ pop new task's psr
        msr SPSR_cxsf,r4

        ldmfd sp!,{r0-r12,lr,pc}^   @ pop new task's r0-r12,lr & pc



@*********************************************************************************************************
@                                PERFORM A CONTEXT SWITCH (From an ISR)
@                                        void OSIntCtxSw(void)
@
@ Note(s): This function only flags a context switch to the ISR Handler
@
@*********************************************************************************************************

        .global  OSIntCtxSw
OSIntCtxSw:

        @OSIntCtxSwFlag = True
        ldr r0,=OSIntCtxSwFlag
        mov r1,#1
        str r1,[r0]
        bx lr

@*********************************************************************************************************
@                                            IRQ HANDLER
@
@        This handles all the IRQs
@        Note: FIQ Handler should be written similar to this
@
@*********************************************************************************************************

.globl IsrIRQ
IsrIRQ:

        @ Fix the return address
		sub lr,lr,#4
        stmfd sp!,{r0-r3,r12,lr}

        bl OSIntEnter   	

	LDR     R0, =rINTOFFSET
        LDR     R0, [R0]
       
        //LDR     R1, =HandleEINT0
        //MOV     LR, PC                          @ Save LR befor jump to the C function we need return back
        //LDR     PC, [R1, R0, LSL #2]            @ Call OS_CPU_IRQ_ISR_handler();   
        MOV LR,PC
 	LDR PC,=handle_int
        bl OSIntExit

        @ Is OSIntCtxSwFlag == True ?
        ldr r0,=OSIntCtxSwFlag
        ldr r1,[r0]
        cmp r1,#1
        beq _IntCtxSw

		ldmfd sp!,{r0-r3,r12,pc}^


_IntCtxSw:
        @ OSIntCtxSwFlag = False
        mov r1,#0
        str r1,[r0]

        @ Get old task's sp & lr
        mrs r0,spsr
        orr r0,r0,#NOINT
        bic r0,r0,#TBIT
        msr cpsr_c,r0
        mov r1,sp
        mov r2,lr
        bic r0,r0,#MODEMASK
        orr r0,r0,#IRQMODE
        msr cpsr_c,r0

        @ Unwind IRQ's sp without poping r0-r3
        mov r3,sp
        add sp,sp,#(4*4)
        ldmfd sp!,{r12,lr}

        stmfd r1!,{r2,lr}           @ push old task's pc & lr
        stmfd r1!,{r4-r12}          @ push old task's r12-r4

        @ Get r0-r3 from IRQ's stack
        mov r6,r1
        ldmfd r3,{r0-r3}
        stmfd r6!,{r0-r3}           @ push old task's r3-r0
        
        mrs r3,spsr
        stmfd r6!,{r3}              @ push old task's psr
        stmfd r6!,{r3}              @ push old task's spsr (don't care)
        
        @ OSPrioCur = OSPrioHighRdy
        ldr r4,=OSPrioCur
        ldr r5,=OSPrioHighRdy
        ldrb r5,[r5]
        strb r5,[r4]


        
        @ Get current task's TCB address
        ldr r4,=OSTCBCur
        ldr r5,[r4]
        str r6,[r5]                 @ store sp in preempted tasks's TCB

        bl OSTaskSwHook             @ call Task Switch Hook

        @ Get highest priority task's TCB address
        ldr r6,=OSTCBHighRdy
        ldr r6,[r6]
        ldr r5,[r6]                 @ get new task's stack pointer

        @ OSTCBCur = OSTCBHighRdy
        str r6,[r4]                 @ set new current task TCB address

        ldmfd r5!,{r4}              @ pop new task's spsr (don't care)
        ldmfd r5!,{r4}              @ pop new task's psr

        @ Leave IRQ mode
        orr r0,r4,#NOINT
        bic r0,r0,#TBIT
        msr cpsr_c,r0

        msr spsr_cxsf,r4
        mov sp,r5

        ldmfd sp!,{r0-r12,lr,pc}^   @ pop new task's r0-r12,lr & pc


@*********************************************************************************************************
@                                   CRITICAL SECTION METHOD 3 FUNCTIONS
@
@ Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
@              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
@              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
@              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
@              into the CPU's status register.
@
@              OS_CPU_SR OSCPUSaveSR()
@ Arguments  : none
@
@ Returns    : OS_CPU_SR
@
@              OSCPURestoreSR(OS_CPU_SR cpu_sr)
@ Arguments  : OS_CPU_SR
@
@ Returns    : none
@
@ Note(s)    : These functions are used in general like this,
@
@            void Task (void *data)
@            {
@                    #if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
@                          OS_CPU_SR  cpu_sr@
@                    #endif
@                         :
@                         :
@                    OS_ENTER_CRITICAL()@ /* cpu_sr = OSCPUSaveSR()@ */
@                         :
@                         :
@                    OS_EXIT_CRITICAL()@  /* OSCPURestoreSR(cpu_sr)@ */
@                         :
@                         :
@            }
@*********************************************************************************************************

        .global  OSCPUSaveSR
OSCPUSaveSR:

        mrs r0,CPSR
        orr r1,r0,#NOINT
        msr CPSR_c,r1
        bx lr


        .global  OSCPURestoreSR
OSCPURestoreSR:

        msr CPSR_c,r0
        bx lr
