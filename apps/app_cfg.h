#ifndef APP_CFG_H
#define APP_CFG_H


//Task Stk lengh

#define	MainTaskStkLengh	512	   	// Define the MainTask stack length 
#define	Task0StkLengh		512     	// Define the Task0 stack length 
#define	Task1StkLengh		512	   	// Define the Task1 stack length 
#define	Task2StkLengh		512	   	// Define the Task2 stack length 
#define	Task3StkLengh		512     	// Define the Task0 stack length 
#define	Task4StkLengh		512	   	// Define the Task1 stack length 
#define	TaskUartStkLengh	512    	// Define the TaskUart stack length 



//Task function
void MainTask(void *pdata);   		//MainTask task
void Task0(void *pdata);			    // Task0 
void Task1(void *pdata);			    // Task1 
void Task2(void *pdata);			    // Task2
void Task3(void *pdata);			    // Task3
void Task4(void *pdata);			    // Task4 
void TaskUart(void *pdata);          //Task Uart


//Task Prio
#define NormalTaskPrio	5
#define MainTaskPrio		NormalTaskPrio
#define Task0Prio		NormalTaskPrio+2
#define Task1Prio		NormalTaskPrio+3
#define Task2Prio		NormalTaskPrio+4
#define Task3Prio		NormalTaskPrio+5
#define Task4Prio		NormalTaskPrio+6
#define TaskUartPrio		NormalTaskPrio+1


#define OS_TASK_NAME_SIZE        16
#define OS_TASK_TMR_PRIO  OS_LOWEST_PRIO-2
#endif

