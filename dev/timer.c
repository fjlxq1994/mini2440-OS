/**
 * 计时器
 * 使用Timer4，该计时器有中断的功能
 * 可以在每次计时来临时调用系统的OSTimeTick()
 */

// define Timer register

#include "interrupt.h"
#include "timer.h"
#include "os_cfg.h"


#define TIMER4_OFFSET 20
//启动Timer4
void timer4_start()
{
   rTCON |= (0x1 << TIMER4_OFFSET); // 定时器开启
}


void timer4_stop()
{
   rTCON &= ~(0x1 << TIMER4_OFFSET);// 定时器停止

}

#define INT_TIMER4 14
void timer4_init()
{
   timer4_stop();
   //定时器的时钟频率 PCLK/1/16 = 3.125
   //FCLK:HCLK:PCLK = 1:4:8
   //FCLK = 400MHZ
   rTCFG1 &= ~(0xf << 16);
   rTCFG1 |= (0x3 << 16);  // Timer4 16分频
   rTCFG0 &= ~(0xff << 8);
   rTCFG0 |= (0 << 8); // PCLK预分频为1
   
   rTCNTB4 = 3125; // System Tick设1ms
   rTCON |= (0x1 << 21); // 更新计数值
   rTCON &= ~(0x1 << 21);// 清除
   rTCON |= (0x1 << 22); // 自动重装载
  
   //开中断
   rINTMOD &= ~(1 <<INT_TIMER4); // Timer4 IRQ 模式
   rINTMSK &= ~(1 <<INT_TIMER4); // Timer4开中断
}

void Timer0_Handler(void)
{
	OSTimeTick();
	rSRCPND |=( 1 << 10);
	rINTPND |= ( 1 << 10);	
}

/*********************************************************************************************************
Initial Timer0 use for ucos time tick
********************************************************************************************************/
#define PCLK 50000000
void Timer0Init(void)
{
	// 定时器设置

	
	rTCON = rTCON & (~0xf) ;			// clear manual update bit, stop Timer0
	
	
	rTCFG0 	&= 0xffffff00;					// set Timer 0&1 prescaler 0
	rTCFG0 |= 15;							//prescaler = 15+1

	rTCFG1 	&= 0xfffffff0;					// set Timer 0 MUX 1/4
	rTCFG1  |= 0x00000001;					// set Timer 0 MUX 1/4
        rTCNTB0 = (PCLK / (4 *15* OS_TICKS_PER_SEC)) - 1;
       //rTCNTB0 = 100000;
    
    rTCON = rTCON & (~0xf) |0x02;              // updata 		
	rTCON = rTCON & (~0xf) |0x09; 	// star

    rINTMSK &= ~(1<<10);			// 打开TIMER0中断允许
 }
