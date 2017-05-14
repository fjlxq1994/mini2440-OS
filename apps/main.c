#include "os_cpu.h"
#include "uart.h"
#include "led.h"
#include "comm.h"
#include "shell.h" 
#include "app_cfg.h"
#include "timer.h"
#include "uart_lib.h"
/*****************【任务相关自定义变量】*****************/
OS_STK  MainTaskStk[MainTaskStkLengh];  //主任务栈
OS_STK	Task0Stk [Task0StkLengh];       // 任务0栈
OS_STK	Task1Stk [Task1StkLengh];       // 任务1栈
OS_STK	Task2Stk [Task2StkLengh];       // 任务2栈
OS_STK	Task3Stk [Task2StkLengh];
OS_STK	Task4Stk [Task2StkLengh];

OS_EVENT *SemLedOn;  // led的打开信号量 用于shell控制
OS_EVENT *SemBeepOn; // 蜂鸣器的打开信号量 用于shell控制
OS_EVENT *SemLedOff; // led的关闭信号量 用于shell控制
OS_EVENT *SemBeepOff; // 蜂鸣器的打开信号量 用于shell控制


INT8U err;

void MainTask(void *pdata) //Main Task create taks0 and task1
{
	
	// 时钟初始化
	Timer0Init();

	// 系统状态初始化
	OSStatInit();

	// 创建任务栈
   	OSTaskCreate(Task0,(void*)0,(OS_STK *)&Task0Stk[Task0StkLengh - 1],Task0Prio);

   	OSTaskCreate(Task1,(void*)0,(OS_STK *)&Task1Stk[Task1StkLengh - 1],Task1Prio);

    OSTaskCreate(Task2,(void*)0,(OS_STK *)&Task2Stk[Task2StkLengh - 1],Task2Prio);

    OSTaskCreate(Task3,(void*)0,(OS_STK *)&Task3Stk[Task2StkLengh - 1],Task3Prio);

    OSTaskCreate(Task4,(void*)0,(OS_STK *)&Task4Stk[Task2StkLengh - 1],Task4Prio);

    // 创建信号量 零个 有shell唤醒
    SemLedOn = OSSemCreate(0);  
    SemBeepOn = OSSemCreate(0); 
    SemLedOff = OSSemCreate(0); 
    SemBeepOff = OSSemCreate(0); 

	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
		// Maybe you can do something else
	}
}


int main(int argc, char **argv)
{

	uart_init();
	// 打印系统消息
	Uart_Printf("           ------------------*****LXQ-OS*****--------------      \n");
	Uart_Printf("           Author: James Lee                                     \n");
	Uart_Printf("           Email: fjlxq1994@126.com                              \n");
	Uart_Printf("           Create Date: 2017-5-10                                \n");
	Uart_Printf("           Based on uC/OS-II                                     \n");
	Uart_Printf("           source code download: www.xxx.github.com.....         \n");
	Uart_Printf("           Gratitude: Thank you for the opening world            \n");
	// Uart_Printf(sysInfo);

	led_init(); // 初始化LED
	Uart_Printf("LED inited ...\n");
	
	beep_init();// 初始化beep
	Uart_Printf("uart inited ...\n");

	CommInit(COM1); // 初始化命令
	Uart_Printf("command inited ...\n");

	ShellInit(); //初始化shell
	Uart_Printf("shell inited ...\n");

	OSInit();	 
	Uart_Printf("os inited ...\n");
	OSTimeSet(0);

	// 创建系统初始任务
	OSTaskCreate (MainTask,(void *)0, &MainTaskStk[MainTaskStkLengh - 1], MainTaskPrio);

	// 开始任务
	OSStart();

	return 0;
}
void Task0(void *pdata)		//led 打开任务	
{
	INT8U err;
	while (1)
	{
		// Uart_Printf("CPU Usage: %d%%\n",OSCPUUsage);
		// Uart_Printf("\nEnter Task0\n");
		OSSemPend(SemLedOn,0,&err);

		rGPBDAT = rGPBDAT & ~(LED4|LED3|LED2|LED1);	 //四个LED都打开
	}
}

void Task1(void *pdata)		// led 关闭任务	
{
	INT8U err;
	while (1)
	{
		OSSemPend (SemLedOff,0,&err);
		rGPBDAT = rGPBDAT | (LED4|LED3|LED2|LED1);
	}
}

void Task2(void *pdata) // beep 打开
{
	INT8U err;
	while(1)
	{
		OSSemPend(SemBeepOn,0,&err);
		rGPBDAT |= (1<<0);
	}
}

void Task3(void *pdata) // beep 关闭
{
	while(1)
	{
		OSSemPend(SemBeepOff,0,&err);
		rGPBDAT &= ~(1<<0);
	}
}

void Task4(void *pdata) // beep 关闭
{
	Uart_Printf(">");
	while(1)
	{
		EM_CMD(COM1);
	}
}

