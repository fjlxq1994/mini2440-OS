.text		@mark for the code period
.global	_start		@mark for the global variable
_start:
		b	reset						
		ldr	pc, _undefined_instruction	  @相当于 pc = _undifined_instruction;
		ldr	pc, _software_interrupt		
		ldr	pc, _prefetch_abort			
		ldr	pc, _data_abort				
		ldr	pc, _not_used				
		ldr	pc, _irq					
		ldr	pc, _fiq


/**********************************************************
* 在内存中定义了_undifined_instruction
* 里边装了undifined_instruction的地址
* 相当于 U32 _undifined_instruction = undifined_instruction;  
*  .word表示32位
* 异常中断地址是硬件决定了的 
**********************************************************/
_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word IsrIRQ
_fiq:			.word fiq					

undefined_instruction:
		nop

software_interrupt:
		nop

prefetch_abort:
		nop

data_abort:
		nop

not_used:
		nop

//irq:
//		sub lr, lr, #4				// lr指令总是指向当前指令(PC)+8的位置,执行完中断能回到函数
//		stmfd sp! , {r0-r12,lr} 	//保存环境 批量指令stmfd  !表示sp也相应变化
//		//ldr pc,=OSIntEnter      //进入中断
// 		mov lr,pc
//		ldr pc,=Timer0_Handler
//		ldmfd sp!, {r0-r12,pc}^		//恢复环境  "^"的意思是指令完成后，把SPSR拷贝到CPSR。
						//如果没有加"^" 则中断执行后原来CPSR没办法恢复
						//模式切换的时候，CPSR会保存到SPSR中，中断执行完
						//原模式下的CPSR也得相应恢复
fiq:
		nop

reset:
	bl set_svc
	bl disable_watchdog
	bl disable_interrupt
	bl disable_mmu
	bl clock_init
	bl mem_init
	bl stack_init		//必须在调用C函数之前创建好栈
	bl nand_init		//C
	bl copy_to_ram
	bl clean_bss
	ldr pc, =main		//跳转到Main函数,进入内存
	//bl light_led
	

/* set svc mode */
set_svc:
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3  //set 0x13 to SVC mode,but the 0xd3 can close FIQ and IRQ 
	msr cpsr, r0
	mov pc, lr
	

/*******************************************************
* close watch dog 
* 2440 addr:0x53000000
* 6410 addr:0x7e004000
* 210  addr:0xe2700000
********************************************************/
#define pWTCON 0x53000000
//#define pWTCON 0x7e004000
//#define pWTCON 0xe2700000
disable_watchdog:
	ldr r0, =pWTCON		//WTCON 0x53000000  Watchdog timer control register
	mov r1, #0x0
	str r1, [r0]
	mov pc, lr
	

/* close all interrupt to 2440 */
#define pINTSUBMSK  0x4a00001c
#define pINTMSK     0x4a000008
disable_interrupt:
	mvn r1, #0x0
	ldr r0, =pINTSUBMSK		//INTSUBMSK 0X4A00001C  Interrupt sub mask
	str r1, [r0]					
	
	ldr r0, =pINTMSK			//INTMSK 0X4A000008  Interrupt mask control
	str r1, [r0]
	mov pc, lr



/******************************************************** 
* close cache and mmu 
* 1.disable I-cache and D-cache
* 2.close I/D chche and mmu (1 read, 2 write)
* in 2440 the kernel manual Table 2-10 2-16 look configurator
* 2440 6410 close cache and mmu configurator is the same
********************************************************/
disable_mmu:
	mcr p15,0,r0,c7,c7,0    //disable I/D cache,Set the coprocessor cp15 register 7
	mrc p15,0,r0,c1,c0,0    //read coprocessor cp15 register 1 control regidter 
	bic r0,r0,#0x00000007   //D-cache enable and MMU enable 
	mcr p15,0,r0,c1,c0,0    //Write back to register
	mov pc, lr
	

/*********************************************************
* set system clock to 2440
* general the CLOCK TIME keep the default
* set FCLK,HCLK,PCLK ratio
* reference u-boot clock configurator
* FCLK:HCLK:PCLK = 1:4:8
* CUP asynchronous mode coprocessor configurator to s3c2440 page 7-9
*********************************************************/
#define pCLKDIVN	0x4c000014
#define pMPLLCON  0x4c000004
#define pMPLL_405MHz (127<<12)|(2<<4)|(1<<0)

clock_init:
	ldr r0, =pCLKDIVN		//CLKDIVN 0x4C000014 Clock divider control
	mov r1, #0x5
	str r1, [r0]
	
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0
	
	ldr r0, =pMPLLCON		//MPLLCON 0x4C000004 MPLL control
	ldr r1, =pMPLL_405MHz
	str r1, [r0]
	mov pc, lr
	

/********************************************************* 
* initial sdram to 2440 
* 6410 210 sdram_init code not in this file,the sdram_init 
* file is mem_6410 or mem_210,when using this file,you should
* on Makefile in the depend add mem_6410.o or mem_210.o 
**********************************************************/
#define mem_control  0x48000000
mem_init:
	ldr r0, =mem_control
	add r3, r0, #4*13
	adrl r1, men_data
	
0:
	ldr r2, [r1], #4
	str r2, [r0], #4
	cmp r0, r3
	bne 0b
	mov pc, lr
	
men_data:							//create array
	.long 0x22000000		//BWSCON    0x48000000	 Bus width & wait status control register
	.long 0x00000700		//BANKCON0  0x48000004	 Bank 0 control register
	.long 0x00000700		//BANKCON1  0x48000008  Bank 1 control register
	.long 0x00000700		//BANKCON2  0x4800000c  Bank 2 control register
	.long 0x00000700		//BANKCON3  0x48000010  Bank 3 control register
	.long 0x00000700		//BANKCON4  0x48000014  Bank 4 control register
	.long 0x00000700		//BANKCON5  0x48000018  Bank 5 control register
	.long 0x00018001		//BANKCON6  0x4800001c  Bank 6 control register
	.long 0x00018001		//BANKCON7  0x48000020  Bank 7 control register
	.long 0x008c04f5		//REFRESH   0x48000024  SDRAM refresh control register
	.long 0x000000b1		//BANKSIZE  0x48000028  Flexible bank size register
	.long 0x00000030		//MRSRB6    0x4800002C  Mode register set register bank6
	.long 0x00000030		//MRSRB7    0x48000030  Mode register set register bank7


/*************************************************************
* 2440 copy sram code to sdram 
* 起点：sram起始地址0x0       终点：链接地址0x30008000
* 我们自己编写的g-boot不够4KB，在CPU上电的时候，自动把nandflash
* 前4KB的代码搬到sram中运行，也就是说g-boot都被拷贝到sram中去了，
* 可以不用拷贝到sdram中。由于要掌握代码搬移的过程，所以就把代码
* 拷贝到内存中，虽然说是多此一举，但重要的是要明白代码搬移的过程
* 掌握新的知识点，在nandflash课程中就会讲怎样冲nandflash搬移到sdram
**************************************************************/
/*************************************************************
* 之前还没学习nandflash,所以只能把sram中运行的4K大小的程序拷贝到
* 内存中，现在是正式把程序从nandflash拷贝到内存中
* 在调用函数时，形参<=4部分是保存在r0-r3中，>4部分则是存放在栈中
**************************************************************/
copy_to_ram:  

	mov r0, #0					//起始页 从nandflash的0地址开始读
	ldr r1, =_start			//起始地址	拷贝到内存起始地址
	ldr r2, =bss_end		//结束地址
	sub r2, r2, r1			//计算出程序大小
	
	mov ip, lr					//保存返回地址(ip is r12)  调用函数会改变lr的值
	bl nand_to_ram			//调用nand_to_ram函数
	mov lr, ip					//恢复地址
	mov pc, lr					//返回
	

/**********************************************************
* unifiy support 2440 6410 210 initial stack
* select 64M volume, stack start address(drop stack) is:     
* 2440 0x34000000  6410 0x54000000  210 0x24000000 
***********************************************************/
stack_init:
	msr cpsr_c, #0xd2     //enter irq mode configurator irq sp(r13_irq)
	ldr sp, =0x33000000		//initial irq sp(r13_irq)
	msr cpsr_c, #0xd3			//enter svc mode
	ldr sp, =0x34000000		//svc mode sp(r13_svc),
	mov pc, lr						//but when enterr interrupt,the sp must is irq 
	

/**********************************************************
* unifiy support 2440 6410 210 clean the bss
* bss段大小不是固定的，每个程序未初始化的全局变量的数量
* 是不定的，所以bss段的大小也是不确定的。
***********************************************************/
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	cmp r0, r1
	moveq pc, lr

clean_loop:
	mov r2, #0
	str r2, [r0], #4
	cmp r0, r1
	bne clean_loop
	mov pc, lr
	
	

	
#define pGPBCON  0x56000010
#define pGPBDAT  0x56000014
.global light_led
light_led:
	ldr r0, =pGPBCON			//GPBCON 0x56000010  Configures the pins of port B
	ldr r1, =0x400
	str r1, [r0]
	
	ldr r0, =pGPBDAT		  //GPBDAT 0x56000014  The data register for port B
	ldr r1, =0x7ff
	str r1, [r0]
	mov pc, lr
	
	
